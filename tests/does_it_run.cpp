#include <iostream>
#include <cstdint>
#include "../fastmathart/api_bindings.h"
#include "../fastmathart/render.h"

void wait_animation()
{
    std::cout << "Entering Wait scene element test" << std::endl;
    PyAPI::Wait wait {
        .seconds = 5.0f
    };
    
    PyAPI::SceneElement wait_elem {
        .type = PyAPI::WAIT,
        .elem = &wait,
        .next = nullptr
    };

    PyAPI::Config config {
        .width = 800,
        .height = 600,
        .fps = 60
    };

    render_scene(&wait_elem, config, "wait_scene.mp4");

    std::cout << "Yes it runs!" << std::endl;
}

void morph_animation()
{
    std::cout << "Entering Morph scene element test" << std::endl;

    uint8_t rgb[3] = {255, 0, 0};
    auto red = PyAPI::Color {
        .value = rgb,
        .type = PyAPI::ColorType::RGB_3_BYTES
    };

    PyAPI::Properties prop1 {
        .x = 1.0f,
        .y = 1.0f,
        .z = 0.0f,
        .color = &red,
        .thickness = 1.0f,
        .fill = nullptr,
        .opacity = 1.0f
    };

    PyAPI::Rectangle rect1 {
        .width = 100.0f,
        .height = 100.0f,
        .properties = &prop1
    };

    PyAPI::Morph morph {
        .src = &rect1,
        .dest = &rect1,
        .src_type = PyAPI::ShapeType::RECTANGLE,
        .dest_type = PyAPI::ShapeType::RECTANGLE,
        .seconds = 5.0f
    };
    
    PyAPI::SceneElement morph_elem {
        .type = PyAPI::MORPH,
        .elem = &morph,
        .next = nullptr
    };

    PyAPI::Config config {
        .width = 800,
        .height = 600,
        .fps = 60
    };

    render_scene(&morph_elem, config, "morph_scene.mp4");

    std::cout << "Yes it runs!" << std::endl;
}