#include "api_bindings.h"

#include <iostream>

#include "render.h"

#if defined(_WIN32) || defined(_WIN64)
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

extern "C" EXPORT void render(PyAPI::SceneElement *scene, PyAPI::Config *config,
                const char *filename)
{
    if (config == nullptr)
    {
        std::cout << "No config specified" << std::endl;
        return;
    }
    render_scene(scene, *config, filename);
}