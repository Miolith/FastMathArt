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

void render_wait_element(Wait *elem, int width, int height,
                         pixel_buffer_t &frame_cache)
{
    std::cout << "Waiting for " << elem->seconds << " seconds" << std::endl;
    int fps = 24;
    int frames = elem->seconds * fps;

    video_buffer_t video_buffer(width, height, frames);

    video_buffer.set_all_frames(frame_cache);

    save_to_video_file(video_buffer, "wait.mp4", fps, width, height, frames);
}

math::fvec3 rotate(math::fvec3 point, float angle)
{
    float radians = angle * (M_PI / 180.0);
    float x = point.x * cos(radians) - point.y * sin(radians);
    float y = point.x * sin(radians) + point.y * cos(radians);
    return math::fvec3(x, y, 0);
}

std::vector<math::CubicBezier> bezier_curve_approx(Circle &circle)
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
    return math::vec3<int>((point.x + float(width)/float(height)) * height / 2.0f,
                           (point.y + 1.0f) * height / 2.0f, 0);
}

int view_space_to_raster_space(float quantity, int width, int height)
{
    return (quantity) * std::min(height, width);
}

void place_line(math::fvec3 point1, math::fvec3 point2,
                pixel_buffer_t &frame_cache, Properties &properties)
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
    
    std::cout << "float thickness: " << properties.thickness << std::endl;

    int thickness = view_space_to_raster_space(properties.thickness, frame_cache.width, frame_cache.height);

    std::cout << "Thickness: " << thickness << std::endl;

    while (true)
    {
        frame_cache.set_pixel(p1.x, p1.y, color);
        for (int i = 0; i < thickness / 2; i++)
        {
            frame_cache.set_pixel(p1.x + i, p1.y, color);
            frame_cache.set_pixel(p1.x - i, p1.y, color);
            frame_cache.set_pixel(p1.x, p1.y + i, color);
            frame_cache.set_pixel(p1.x, p1.y - i, color);
        }
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
                        Properties props)
{
    std::cout << "Placing cubic bezier" << std::endl;

    // Divide the cubic bezier into segments
    // Draw each segment

    math::fvec3 point1 = bezier.valueAt(0);

    for (float t = 0.01; t < 1.0; t += 0.01)
    {
        math::fvec3 point2 = bezier.valueAt(t);

        place_line(point1, point2, frame_cache, props);

        point1 = point2;
    }
}

void place_circle(Circle &circle, pixel_buffer_t &frame_cache)
{
    auto beziers = bezier_curve_approx(circle);
    for (auto &bez : beziers)
        place_cubic_bezier(bez, frame_cache, *circle.properties);
}

void render_place_element(Place *elem, int width, int height,
                          pixel_buffer_t &frame_cache)
{
    std::cout << "Placing " << elem->obj_count << " objects" << std::endl;

    for (int j = 0; j < elem->obj_count; j++)
    {
        switch (elem->obj_types[j])
        {
        case CIRCLE:
            place_circle(*reinterpret_cast<Circle *>(elem->obj_list[j]),
                         frame_cache);
            break;
        default:
            std::cout << "Unknown object type " << elem->obj_types[j]
                      << std::endl;
        }
    }
}

void cast_then_render_element(SceneElement *elem, int width, int height,
                              pixel_buffer_t &frame_cache)
{
    if (elem->elem == nullptr)
    {
        std::cout << "Null element->elem" << std::endl;
        return;
    }

    switch (elem->type)
    {
    case WAIT:
        render_wait_element(reinterpret_cast<Wait *>(elem->elem), width, height,
                            frame_cache);
        break;
    case PLACE:
        render_place_element(reinterpret_cast<Place *>(elem->elem), width,
                             height, frame_cache);
        break;
    default:
        std::cout << "Unknown element type " << elem->type << std::endl;
    }
}

void render_scene(SceneElement *elem, int width, int height,
                  std::string_view filename)
{
    std::cout << "Rendering scene to " << filename << std::endl;

    pixel_buffer_t frame_cache(width, height);

    while (elem != nullptr)
    {
        cast_then_render_element(elem, width, height, frame_cache);
        elem = elem->next;
    }
}