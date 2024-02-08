#pragma once

#include <string>

enum ElementType
{
    NOTHING = 0,
    WAIT = 1,
    PLACE = 2
};

enum ShapeType
{
    CIRCLE = 0,
    RECTANGLE = 1
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

struct Circle
{
    float radius;
    float x;
    float y;
    float r;
    float g;
    float b;
    float thickness;
    float *fill;
};

void render_scene(SceneElement *elem, int width, int height,
                  std::string_view filename);