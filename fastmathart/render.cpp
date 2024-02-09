#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include "utils/pixelUtils.h"
#include "render.h"

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
    fwrite(video_buffer.buffer.get(), 1, width * height * frames * 3, pipe.get());
}

void render_wait_element(Wait *elem, int width, int height, pixel_buffer_t &frame_cache)
{
    std::cout << "Waiting for " << elem->seconds << " seconds" << std::endl;
    int fps = 24;
    int frames = elem->seconds * fps;

    video_buffer_t video_buffer(width, height, frames);

    video_buffer.set_all_frames(frame_cache);

    save_to_video_file(video_buffer, "wait.mp4", fps, width, height,
                       frames);
}

void render_place_element(Place *elem, int width, int height, pixel_buffer_t &frame_cache)
{
    std::cout << "Placing " << elem->obj_count << " objects" << std::endl;
    int fps = 24;
    int frames = 1;
    
    auto framebuffer = std::make_unique<uint8_t[]>(width * height * 3);

    for (int j = 0; j < elem->obj_count; j++)
    {
        switch (elem->obj_types[j])
        {
        case CIRCLE:
            //draw_arc(reinterpret_cast<Circle *>(elem->obj_list[j]));
            break;
        default:
            std::cout << "Unknown object type " << elem->obj_types[j]
                        << std::endl;
        }
    }
}

void cast_then_render_element(SceneElement *elem, int width, int height, pixel_buffer_t &frame_cache)
{
    if (elem->elem == nullptr)
    {
        std::cout << "Null element->elem" << std::endl;
        return;
    }

    switch (elem->type)
    {
    case WAIT:
        render_wait_element(reinterpret_cast<Wait *>(elem->elem), width,
                            height, frame_cache);
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