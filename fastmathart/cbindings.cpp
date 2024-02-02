#include "render.h"

extern "C"
{
    void render(SceneElement *scene, int width, int height,
                const char *filename)
    {
        render_scene(scene, width, height, filename);
    }
}