#include <iostream>
#include <string_view>

#include "render.h"


void render_element(Wait* elem, int width, int height)
{
    std::cout << "Waiting for " << elem->seconds << " seconds" << std::endl;
}

void cast_then_render_element(SceneElement* elem, int width, int height)
{
    if (elem == nullptr)
    {
        std::cout << "Null element" << std::endl;
        return;
    }

    if (elem->elem == nullptr)
    {
        std::cout << "Null element->elem" << std::endl;
        return;
    }

    switch ((ElementType)elem->type)
    {
    case WAIT: render_element(reinterpret_cast<Wait*>(elem->elem), width, height);
        break;
    default:
        std::cout << "Unknown element type " << elem->type << std::endl;
    }
}

void render_scene(SceneElement* elem, int width, int height, std::string_view filename)
{
    cast_then_render_element(elem, width, height);
}