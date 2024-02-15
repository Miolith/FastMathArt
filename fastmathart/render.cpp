#include "render.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include "math/bezier.h"
#include "math/vec.h"
#include "utils/pixelUtils.h"

void save_to_video_file(video_buffer_t &video_buffer, std::string_view filename,
                        int fps, int width, int height, int frames)
{
    std::cout << "Saving to video file " << filename << std::endl;
    std::cout << "Frame rate: " << fps << std::endl;
    std::cout << "Frame width: " << width << std::endl;
    std::cout << "Frame height: " << height << std::endl;

    // Save the framebuffer to a video file
    // execute ffmpeg
    std::string command = "ffmpeg  -y -f rawvideo -s " + std::to_string(width)
        + "x" + std::to_string(height) + " -pix_fmt rgb24 -r "
        + std::to_string(fps)
        + " -i - -an -x264opts opencl -vcodec h264 -q:v 5 -f mp4 "
        + std::string(filename);

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "w"),
                                                  pclose);
    if (!pipe)
    {
        std::cerr << "popen() failed!" << std::endl;
        return;
    }
    fwrite(video_buffer.buffer.get(), 1, width * height * frames * 3,
           pipe.get());
}

void render_wait_element(PyAPI::Wait *elem, PyAPI::Config &config,
                         pixel_buffer_t &frame_cache)
{
    std::cout << "Waiting for " << elem->seconds << " seconds" << std::endl;
    int frames = elem->seconds * config.fps;

    video_buffer_t video_buffer(config.width, config.height, frames);

    video_buffer.set_all_frames(frame_cache);

    save_to_video_file(video_buffer, "wait.mp4", config.fps, config.width, config.height, frames);
}

math::fvec3 rotate(math::fvec3 point, float angle)
{
    float radians = angle * (M_PI / 180.0);
    float x = point.x * cos(radians) - point.y * sin(radians);
    float y = point.x * sin(radians) + point.y * cos(radians);
    return math::fvec3(x, y, 0);
}

std::vector<math::CubicBezier> bezier_curve_approx(PyAPI::Circle &circle)
{
    constexpr float a = 1.00005519;
    constexpr float b = 0.55342686;
    constexpr float c = 0.99873585;

    auto p1 = math::fvec3(0, a, 0) * circle.radius;
    auto p2 = math::fvec3(b, c, 0) * circle.radius;
    auto p3 = math::fvec3(c, b, 0) * circle.radius;
    auto p4 = math::fvec3(a, 0, 0) * circle.radius;

    std::vector<math::CubicBezier> path(4);
    path[0] = math::CubicBezier(p1, p2, p3, p4);
    path[1] = math::CubicBezier(rotate(p1, 90), rotate(p2, 90), rotate(p3, 90),
                                rotate(p4, 90));
    path[2] = math::CubicBezier(rotate(p1, 180), rotate(p2, 180),
                                rotate(p3, 180), rotate(p4, 180));
    path[3] = math::CubicBezier(rotate(p1, 270), rotate(p2, 270),
                                rotate(p3, 270), rotate(p4, 270));

    return path;
}

math::vec3<int> view_space_to_raster_space(math::fvec3 point, int width,
                                           int height)
{
    return math::vec3<int>((point.x + float(width) / float(height)) * height
                               / 2.0f,
                           (point.y + 1.0f) * height / 2.0f, 0);
}

int view_space_to_raster_space(float quantity, int width, int height)
{
    return (quantity)*std::min(height, width);
}

void render_disk(math::vec3<int> center, int radius, pixel_buffer_t &frame_cache,
                    color_t<RGB_8> color)
{
    for (int x = -radius; x < radius; x++)
    {
        for (int y = -radius; y < radius; y++)
        {
            if (x * x + y * y < radius * radius)
            {
                frame_cache.set_pixel(center.x + x, center.y + y, color);
            }
        }
    }
}


void render_line(math::fvec3 point1, math::fvec3 point2,
                 pixel_buffer_t &frame_cache, PyAPI::Properties &properties)
{
    std::cout << "Placing line" << std::endl;

    math::vec3<int> p1 = view_space_to_raster_space(point1, frame_cache.width,
                                                    frame_cache.height);
    math::vec3<int> p2 = view_space_to_raster_space(point2, frame_cache.width,
                                                    frame_cache.height);

    int dx = std::abs(p2.x - p1.x);
    int dy = std::abs(p2.y - p1.y);
    int sx = (p1.x < p2.x) ? 1 : -1;
    int sy = (p1.y < p2.y) ? 1 : -1;
    int err = dx - dy;

    color_t<RGB_8> color = (properties.color != nullptr)
        ? cast_to_color_t<RGB_8>(*properties.color)
        : color_t<RGB_8>(255, 255, 255);

    int thickness = view_space_to_raster_space(
        properties.thickness, frame_cache.width, frame_cache.height);

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

void place_cubic_bezier(math::CubicBezier &bezier, pixel_buffer_t &frame_cache,
                        PyAPI::Properties props)
{
    std::cout << "Placing cubic bezier" << std::endl;

    // Divide the cubic bezier into segments
    // Draw each segment

    math::fvec3 point1 = bezier.valueAt(0);

    for (float t = 0.01; t < 1.0; t += 0.01)
    {
        math::fvec3 point2 = bezier.valueAt(t);

        render_line(point1, point2, frame_cache, props);

        point1 = point2;
    }
}

void place_circle(PyAPI::Circle &circle, pixel_buffer_t &frame_cache)
{
    auto beziers = bezier_curve_approx(circle);
    for (auto &bez : beziers)
        place_cubic_bezier(bez, frame_cache, *circle.properties);
}

void render_place_element(PyAPI::Place *elem, PyAPI::Config &config,
                          pixel_buffer_t &frame_cache)
{
    std::cout << "Placing " << elem->obj_count << " objects" << std::endl;

    for (int j = 0; j < elem->obj_count; j++)
    {
        switch (elem->obj_types[j])
        {
        case PyAPI::CIRCLE:
            place_circle(*reinterpret_cast<PyAPI::Circle *>(elem->obj_list[j]),
                         frame_cache);
            break;
        default:
            std::cout << "Unknown object type " << elem->obj_types[j]
                      << std::endl;
        }
    }
}

void draw_path(std::vector<math::CubicBezier> &beziers,
               pixel_buffer_t &frame_cache, video_buffer_t &video,
                PyAPI::Properties &props)
{
    std::cout << "Drawing path" << std::endl;

    int frames = video.frames;
    int current_frame = 0;
    int bezier_amount = beziers.size();
    math::fvec3 point1 = beziers[0].valueAt(0);

    int steps_per_bezier = 100;
    int u = 0;
    int draw_per_frame = steps_per_bezier * bezier_amount / frames;

    for (auto &bezier : beziers)
    {
        for (int i = 1; i <= steps_per_bezier; i++, u++)
        {
            float t = float(i) / float(steps_per_bezier);
            math::fvec3 point2 = bezier.valueAt(t);
            render_line(point1, point2, frame_cache, props);
            point1 = point2;

            if (u >= draw_per_frame)
            {
                std::cout << "Frame " << current_frame << std::endl;
                video.set_frame(frame_cache, current_frame);
                current_frame++;
                u = 0;
            }
        }
    }

    // Save the last frame
    video.set_frame(frame_cache, frames - 1);
}


void draw_circle(PyAPI::Circle &circle, pixel_buffer_t &frame_cache, video_buffer_t &video)
{
    auto beziers = bezier_curve_approx(circle);
    draw_path(beziers, frame_cache, video, *circle.properties);
}


void render_draw_element(PyAPI::Draw *elem, PyAPI::Config &config,
                         pixel_buffer_t &frame_cache)
{
    std::cout << "Drawing " << elem->obj_count << " objects" << std::endl;

    int frames = elem->seconds * config.fps;
    std::cout << "Frames: " << frames << std::endl;
    std::cout << "Seconds: " << elem->seconds << std::endl;
    
    auto video = video_buffer_t(frame_cache.width, frame_cache.height, frames);

    for (int j = 0; j < elem->obj_count; j++)
    {
        switch (elem->obj_types[j])
        {
        case PyAPI::CIRCLE:
            draw_circle(*reinterpret_cast<PyAPI::Circle *>(elem->obj_list[j]),
                        frame_cache, video);
            break;
        default:
            std::cout << "Unknown object type " << elem->obj_types[j]
                      << std::endl;
        }
    }

    save_to_video_file(video, "draw.mp4", config.fps, config.width, config.height, frames);
}

void cast_then_render_element(PyAPI::SceneElement *elem, PyAPI::Config &config,
                              pixel_buffer_t &frame_cache)
{
    if (elem->elem == nullptr)
    {
        std::cout << "Null element->elem" << std::endl;
        return;
    }

    switch (elem->type)
    {
    case PyAPI::WAIT:
        render_wait_element(reinterpret_cast<PyAPI::Wait *>(elem->elem), config, frame_cache);
        break;
    case PyAPI::PLACE:
        render_place_element(reinterpret_cast<PyAPI::Place *>(elem->elem),
                             config, frame_cache);
        break;
    case PyAPI::DRAW:
        render_draw_element(reinterpret_cast<PyAPI::Draw *>(elem->elem), config, frame_cache);
        break;
    default:
        std::cout << "Unknown element type " << elem->type << std::endl;
    }
}

void render_scene(PyAPI::SceneElement *elem, PyAPI::Config &config,
                  std::string_view filename)
{
    std::cout << "Rendering scene to " << filename << std::endl;

    pixel_buffer_t frame_cache(config.width, config.height);

    while (elem != nullptr)
    {
        cast_then_render_element(elem, config, frame_cache);
        elem = elem->next;
    }
}