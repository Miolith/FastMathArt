#include <iostream>
#include "render.h"

extern "C"
{
    void render(PyAPI::SceneElement *scene, PyAPI::Config *config,
                const char *filename)
    {
        if (config == nullptr)
        {
            std::cout << "No config specified" << std::endl;
            return;
        }
        render_scene(scene, *config, filename);
    }
}