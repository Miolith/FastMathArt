#pragma once

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

    struct Draw
    {
        void **obj_list;
        ShapeType *obj_types;
        int obj_count;
        float seconds;
    };

    struct Morph
    {
        void* src;
        void* dest;
        ShapeType src_type;
        ShapeType dest_type;
        float seconds;
    };
} // namespace PyAPI

void render_scene(PyAPI::SceneElement *elem, PyAPI::Config &config,
                  std::string_view filename);