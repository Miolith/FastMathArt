#include "render.h"

extern "C"
{
    void render(PyAPI::SceneElement *scene, int width, int height,
                const char *filename)
    {
        render_scene(scene, width, height, filename);
    }
}