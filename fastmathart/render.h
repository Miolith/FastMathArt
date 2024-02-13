#pragma once

#include <string>

enum ElementType
{
    NOTHING = 0,
    WAIT = 1,
    PLACE = 2,
    DRAW = 3
};

enum ShapeType
{
    UKNOWN = 0,
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

struct Draw
{
    void **obj_list;
    ShapeType *obj_types;
    int obj_count;
    float seconds;
};

void render_scene(SceneElement *elem, int width, int height,
                  std::string_view filename);