#include "render.h"

#include <cstdio>
#include <fmt/core.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <optional>

#include "api_bindings.h"
#include "math/bezier.h"
#include "math/vec.h"
#include "utils/cWrapper.h"
#include "utils/pixelUtils.h"

using segment_cache = std::unordered_map<void*, std::pair<std::vector<math::fvec3>, PyAPI::Properties>>;
static segment_cache scene_cache;
static std::ofstream concat_file;

void save_to_video_file(video_buffer_t &video_buffer, std::string_view filename,
                        int fps, int width, int height, int frames)
{
    static int num_files = 0;
    std::string actual_filename = fmt::format("{}_{}", num_files, filename);
    std::cout << "Saving to video file " << filename << "\n";
    std::cout << "Frame rate: " << fps << "\n";
    std::cout << "Frame width: " << width << "\n";
    std::cout << "Frame height: " << height << "\n";

    std::string command = fmt::format(
        "ffmpeg -hide_banner -loglevel error -y -f rawvideo -s "
        "{width}x{height} -pix_fmt rgb24 -r {fps} -i - -an "
        "-x264opts opencl -vcodec h264 -pix_fmt yuv420p -q:v 5 -f mp4 "
        "{filename}",
        fmt::arg("width", width), fmt::arg("height", height),
        fmt::arg("fps", fps), fmt::arg("filename", actual_filename));

    auto pipe = popen2(command.c_str(), "w");

    fwrite(video_buffer.buffer.get(), 1, width * height * frames * 3,
           pipe.get());

    concat_file << "file '" << actual_filename << "'\n";

    num_files++;
}



math::BezierPath bezier_curve_approx(const PyAPI::Circle &circle)
{
    auto p = math::circle_bezier(circle.radius);

    std::vector<math::CubicBezier> path(4);

    path[0] = math::CubicBezier(p[0], p[1], p[2], p[3]);
    path[1] = math::CubicBezier(p[3], p[4], p[5], p[6]);
    path[2] = math::CubicBezier(p[6], p[7], p[8], p[9]);
    path[3] = math::CubicBezier(p[9], p[10], p[11], p[0]);

    return path;
}


math::BezierPath bezier_curve_approx(PyAPI::Polyline &polyline)
{
    std::vector<math::CubicBezier> beziers;
    for (int i = 0; i < polyline.point_count - 1; i++)
    {
        auto p1 = math::fvec3(polyline.x[i], polyline.y[i], 0);
        auto p2 = math::fvec3(polyline.x[i + 1], polyline.y[i + 1], 0);
        beziers.push_back(math::CubicBezier::straightLine(p1, p2));
    }
    return math::BezierPath(beziers);
}

math::vec3<int> ndc_to_raster_space(math::fvec3 point, const int width,
                                    const int height)
{
    int bigger_dimension = std::max(width, height);
    int smaller_dimension = std::min(width, height);

    const float screen_ratio =
        float(bigger_dimension) / float(smaller_dimension);

    return math::vec3<int>((point.x + screen_ratio) * smaller_dimension / 2.0f,
                           (-point.y + 1.0f) * smaller_dimension / 2.0f, 0);
}

int ndc_to_raster_space(float quantity, int width, int height)
{
    return quantity * std::min(height, width);
}

void render_disk(math::vec3<int> center, int radius,
                 pixel_buffer_t &frame_cache, color_t<RGB_8> color)
{
    int r = radius * radius;
    for (int x = -radius; x <= radius; x++)
    {
        for (int y = -radius; y <= radius; y++)
        {
            int dist = x * x + y * y;
            if (dist < r)
            {
                frame_cache.set_pixel(center.x + x, center.y + y, color);
            }
            else if (dist == r)
            {
                float alpha = std::sqrt(dist) - radius;
                auto c =
                    blend(frame_cache.get_pixel(center.x + x, center.y + y),
                          color, alpha);
                frame_cache.set_pixel(center.x + x, center.y + y, c);
            }
        }
    }
}

void render_line(math::fvec3 point1, math::fvec3 point2,
                 pixel_buffer_t &frame_cache, PyAPI::Properties &properties)
{
    math::vec3<int> p1 =
        ndc_to_raster_space(point1, frame_cache.width, frame_cache.height);
    math::vec3<int> p2 =
        ndc_to_raster_space(point2, frame_cache.width, frame_cache.height);

    int dx = std::abs(p2.x - p1.x);
    int dy = std::abs(p2.y - p1.y);
    int sx = (p1.x < p2.x) ? 1 : -1;
    int sy = (p1.y < p2.y) ? 1 : -1;
    int err = dx - dy;

    color_t<RGB_8> color = (properties.color != nullptr)
        ? cast_to_color_t_RGB_8(*properties.color)
        : color_t<RGB_8>(255, 255, 255);

    int thickness = ndc_to_raster_space(properties.thickness, frame_cache.width,
                                        frame_cache.height);

    while (true)
    {
        render_disk(p1, thickness / 2, frame_cache, color);
        if (p1.x == p2.x && p1.y == p2.y)
            break;
        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            p1.x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            p1.y += sy;
        }
    }
}

void render_cubic_bezier(math::CubicBezier &bezier, pixel_buffer_t &frame_cache,
                        PyAPI::Properties props)
{
    math::fvec3 point1 = bezier.valueAt(0);

    for (float t = 0.01; t < 1.0; t += 0.01)
    {
        math::fvec3 point2 = bezier.valueAt(t);

        render_line(point1, point2, frame_cache, props);

        point1 = point2;
    }
}

void draw_path(math::BezierPath &beziers, video_buffer_t &video,
               PyAPI::Properties &props, void* obj)
{
    std::cout << "Drawing path"
              << "\n";

    const int total_frames = video.frames;
    int current_frame = 0;

    const int steps_per_bezier = 100;
    const float draw_per_frame = 1.0 / (total_frames - 1);

    float accumulated_length = 0.0f;
    const float total_length = beziers.length(100);
    const float length_ratio = 1.0f / total_length;

    std::vector<math::fvec3> segments;
    segments.reserve(100 * beziers.size());
    segments.push_back(beziers.valueAt(0));

    for (auto &bezier : beziers)
    {
        math::fvec3 point1 = bezier.valueAt(0);
        for (int i = 1; i <= steps_per_bezier; i++)
        {
            float t = float(i) / float(steps_per_bezier);
            math::fvec3 point2 = bezier.valueAt(t);

            segments.push_back(point2);

            const float length = (point2 - point1).length();
            accumulated_length += length * length_ratio;

            point1 = point2;

            if (accumulated_length - draw_per_frame * (current_frame + 1.0f)
                >= -0.01f)
            {
                auto frame = video.get_frame(current_frame);

                for (std::size_t j = 0; j < segments.size() - 1; j++)
                    render_line(segments[j], segments[j + 1], frame, props);

                current_frame++;
            }
        }
    }
    scene_cache[obj] = {segments, props};
}

void render_element(PyAPI::Wait *elem, PyAPI::Config &config,
                    pixel_buffer_t &frame_cache, std::optional<video_buffer_t> &video_buffer)
{
    std::cout << "Waiting for " << elem->seconds << " seconds"
              << "\n";
    int frames = elem->seconds * config.fps;

    if (!video_buffer.has_value())
        video_buffer.emplace(config.width, config.height, frames);

    auto &video = video_buffer.value();

    video.set_all_frames(frame_cache);
}


void render_element(PyAPI::Place *elem, PyAPI::Config &config,
                    pixel_buffer_t &frame_cache, std::optional<video_buffer_t> &video_buffer)
{
    std::cout << "Placing " << elem->obj_count << " objects"
              << "\n";

    for (int j = 0; j < elem->obj_count; j++)
    {
        PyAPI::shape_visitor(
            [&](auto *shape) {
                auto beziers = bezier_curve_approx(*shape);
                for (auto &bez : beziers)
                    render_cubic_bezier(bez, frame_cache, *shape->properties);
            },
            elem->obj_list[j], elem->obj_types[j]);
    }
}


void render_element(PyAPI::Draw *elem, PyAPI::Config &config,
                    pixel_buffer_t &frame_cache, std::optional<video_buffer_t> &video_buffer)
{
    std::cout << "Drawing " << elem->obj_count << " objects"
              << "\n";

    int frames = elem->seconds * config.fps;
    std::cout << "Frames: " << frames << "\n";
    std::cout << "Seconds: " << elem->seconds << "\n";

    if (!video_buffer.has_value())
        video_buffer.emplace(config.width, config.height, frames);

    auto &video = video_buffer.value();
    video.set_frame(frame_cache, 0);

    for (int j = 0; j < elem->obj_count; j++)
    {
        PyAPI::shape_visitor(
            [&](auto *shape) {
                auto beziers = bezier_curve_approx(*shape);
                draw_path(beziers, video, *shape->properties, shape);
            },
            elem->obj_list[j], elem->obj_types[j]);
    }
    frame_cache.copy_from(video.get_frame(frames - 1));
}

void render_cached_scene(segment_cache &segments, pixel_buffer_t &frame_cache)
{
    for (auto &[pointer, pair] : segments)
    {
        auto& segments = pair.first;
        auto& properties = pair.second;
        std::cout << "Rendering from cache"
                  << "\n";
        for (std::size_t i = 0; i < segments.size() - 1; i++)
            render_line(segments[i], segments[i + 1], frame_cache, properties);
    }
}

void prepare_cache(segment_cache &segments, auto &elem)
{
    (void)segments;
    (void)elem;
    std::puts("Doing nothing");
}

void prepare_cache(segment_cache &segments, PyAPI::Morph &elem)
{
    segments.erase(elem.src);
    std::puts(fmt::format("Deleted {}", elem.src).c_str());
}

void prepare_cache(segment_cache &segments, PyAPI::Simultaneous &elem)
{
    for (int i = 0; i < elem.obj_count; i++)
    {
        PyAPI::element_visitor(
            [&](auto *element) {
                prepare_cache(segments, *element);
            },
            elem.obj_list[i], elem.obj_types[i]);
    }
}

void render_element(PyAPI::Morph *elem, PyAPI::Config &config,
                    pixel_buffer_t &frame_cache, std::optional<video_buffer_t> &video_buffer)
{
    std::cout << "Morphing " << elem->seconds << " seconds"
              << "\n";

    int frames = elem->seconds * config.fps;

    math::BezierPath src_beziers, dest_beziers;
    PyAPI::Properties *src_props = nullptr, *dest_props = nullptr;
    scene_cache.erase(elem->src);

    PyAPI::shape_visitor(
        [&](auto *shape) {
            src_beziers = bezier_curve_approx(*shape);
            src_props = shape->properties;
        },
        elem->src, elem->src_type);

    PyAPI::shape_visitor(
        [&](auto *shape) {
            dest_beziers = bezier_curve_approx(*shape);
            dest_props = shape->properties;
        },
        elem->dest, elem->dest_type);

    if (!video_buffer.has_value())
        video_buffer.emplace(config.width, config.height, frames);
    
    auto &video = video_buffer.value();

    for(int i =0; i < frames; i++)
    {
        auto fr = video.get_frame(i);
        render_cached_scene(scene_cache, fr);
    }
    math::alignPaths(src_beziers, dest_beziers);

    auto props = *src_props;

    color_t<RGB_f32> src_color = cast_to_color_t_RGB_f32(*src_props->color);
    color_t<RGB_f32> dest_color = cast_to_color_t_RGB_f32(*dest_props->color);

    for (int i = 0; i < frames; i++)
    {
        float t = float(i) / float(frames - 1);
        auto color = blend(src_color, dest_color, t);

        props.color->r = color.r;
        props.color->g = color.g;
        props.color->b = color.b;

        auto morphed_beziers =
            math::interpolatePaths(src_beziers, dest_beziers, t);
        
        auto frame = video.get_frame(i);

        for (auto &bezier : morphed_beziers)
            render_cubic_bezier(bezier, frame, props);
    }
}


float get_seconds(auto* element)
{
    return element->seconds;
}

float get_seconds(PyAPI::Place *elem)
{
    return 0.0f;
}

float get_seconds(PyAPI::Simultaneous *elem)
{
    float seconds = 0.0f;

    for (int i = 0; i < elem->obj_count; i++)
    {
        PyAPI::element_visitor(
            [&](auto *element) {
                seconds = std::max(seconds, get_seconds(element));
            },
            elem->obj_list[i], elem->obj_types[i]);
    }

    return seconds;
}

void render_element(PyAPI::Simultaneous *elem, PyAPI::Config &config,
                    pixel_buffer_t &frame_cache, std::optional<video_buffer_t> &video_buffer)
{
    std::cout << "Simultaneous " << '\n';

    float seconds = get_seconds(elem);

    int frames = seconds * config.fps;
    if (!video_buffer.has_value())
        video_buffer.emplace(config.width, config.height, frames);
    
    auto &video = video_buffer.value();

    for (int i = 0; i < elem->obj_count; i++)
    {
        PyAPI::element_visitor(
            [&](auto *element) {
                render_element(element, config, frame_cache, video_buffer);
            },
            elem->obj_list[i], elem->obj_types[i]);
    }

    frame_cache.copy_from(video.get_frame(frames - 1));
}

void concat_animation_files(std::string_view filename)
{
    const std::string command = fmt::format("ffmpeg -y -hide_banner -loglevel error "
                                      "-f concat -i concat.txt -c copy {name}",
                                      fmt::arg("name", filename));

    auto pipe = popen2(command.c_str(), "w");
}

void render_scene(PyAPI::Scene &scene, PyAPI::Config &config,
                  std::string_view filename)
{
    std::cout << "Rendering scene to " << filename << "\n";

    pixel_buffer_t frame_cache(config.width, config.height);
    concat_file = std::ofstream("concat.txt");


    for(int i = 0; i < scene.element_count; i++)
    {
        auto elem = scene.elements[i];
        PyAPI::element_visitor(
            [&](auto *element) {
                auto opt = std::optional<video_buffer_t>();
                prepare_cache(scene_cache, *element);
                render_element(element, config, frame_cache, opt);
                if (opt.has_value())
                {
                    auto &video = opt.value();
                    save_to_video_file(video, "temp.mp4", config.fps, config.width, config.height, video.frames);
                }
            },
            elem.elem, elem.type);
    }

    concat_file.close();

    concat_animation_files(filename);
}