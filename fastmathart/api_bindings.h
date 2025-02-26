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
        MORPH = 4,
        SIMULTANEOUS = 5
    };

    enum ShapeType
    {
        NO_TYPE = 0,
        CIRCLE = 1,
        POLYLINES = 2
    };

    struct SceneElement
    {
        ElementType type;
        void *elem;
    };

    struct Scene
    {
        SceneElement *elements;
        int element_count;
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

    struct Simultaneous
    {
        void **obj_list;
        ElementType *obj_types;
        int obj_count;
    };

    template <class F>
    inline constexpr void element_visitor(F &&lambda, void* elem, ElementType type)
    {
        switch (type)
        {
        case WAIT: return lambda(static_cast<Wait *>(elem));
        case PLACE: return lambda(static_cast<Place *>(elem));
        case DRAW: return lambda(static_cast<Draw *>(elem));
        case MORPH: return lambda(static_cast<Morph *>(elem));
        case SIMULTANEOUS: return lambda(static_cast<Simultaneous *>(elem));
        default: std::cout << "Unknown element type\n";
        }
    }

    struct Color
    {
        float r;
        float g;
        float b;
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

    struct Polyline
    {
        float *x;
        float *y;
        int point_count;
        Properties *properties;
    };

    template <class F>
    inline constexpr void shape_visitor(F &&lambda, void *shape, ShapeType type)
    {
        switch (type)
        {
        case CIRCLE: return lambda(static_cast<Circle *>(shape));
        case POLYLINES: return lambda(static_cast<Polyline *>(shape));
        default: std::cout << "Unknown shape type\n";
        }
    }

} // namespace PyAPI

void render_scene(PyAPI::SceneElement *elem, PyAPI::Config &config,
                  std::string_view filename);