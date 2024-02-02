#pragma once

#include <string>

enum ElementType
{
    NOTHING = 0,
    WAIT = 1,
};

struct SceneElement
{
    int type;
    void* elem;
    SceneElement* next;
};

struct Wait
{
    float seconds;
};

void render_scene(SceneElement* elem, int width, int height, std::string_view filename);