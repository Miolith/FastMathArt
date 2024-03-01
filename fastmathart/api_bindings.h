#pragma once

#include <iostream>
#include <string>

namespace PyAPI
{
    struct Config
    {
        int width;
        int height;
        int fps;
    };

    enum ElementType
    {
        NOTHING = 0,
        WAIT = 1,
        PLACE = 2,
        DRAW = 3,
        MORPH = 4
    };

    enum ShapeType
    {
        NO_TYPE = 0,
        CIRCLE = 1,
        RECTANGLE = 2
    };

    struct SceneElement
    {
        ElementType type;
        void *elem;
        SceneElement *next;
    };

    struct Wait
    {
        float seconds;
    };

    struct Place
    {
        void **obj_list;
        ShapeType *obj_types;
        int obj_count;
    };

    struct Draw
    {
        void **obj_list;
        ShapeType *obj_types;
        int obj_count;
        float seconds;
    };

    struct Morph
    {
        void *src;
        void *dest;
        ShapeType src_type;
        ShapeType dest_type;
        float seconds;
    };

    template <class F>
    inline constexpr void element_visitor(F &&lambda, SceneElement *elem)
    {
        switch (elem->type)
        {
        case WAIT: return lambda(static_cast<Wait *>(elem->elem));
        case PLACE: return lambda(static_cast<Place *>(elem->elem));
        case DRAW: return lambda(static_cast<Draw *>(elem->elem));
        case MORPH: return lambda(static_cast<Morph *>(elem->elem));
        default: std::cout << "Unknown element type\n";
        }
    }

    enum ColorType
    {
        NOTYPE = 0,
        RGB_3_FLOATS = 1,
        RGB_3_BYTES = 2,
        HEX_STRING = 3,
        HSL = 4
    };

    struct Color
    {
        void *value;
        ColorType type;
    };

    struct Properties
    {
        float x;
        float y;
        float z;
        Color *color;
        float thickness;
        Color *fill;
        float opacity;
    };

    struct Circle
    {
        float radius;
        Properties *properties;
    };

    struct Rectangle
    {
        float width;
        float height;
        Properties *properties;
    };

    template <class F>
    inline constexpr void shape_visitor(F &&lambda, void *shape, ShapeType type)
    {
        switch (type)
        {
        case CIRCLE: return lambda(static_cast<Circle *>(shape));
        case RECTANGLE: return lambda(static_cast<Rectangle *>(shape));
        default: std::cout << "Unknown shape type\n";
        }
    }

} // namespace PyAPI

void render_scene(PyAPI::SceneElement *elem, PyAPI::Config &config,
                  std::string_view filename);