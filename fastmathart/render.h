#pragma once

#include <string_view>

#include "api_bindings.h"

void render_scene(PyAPI::SceneElement *elem, PyAPI::Config &config,
                  std::string_view filename);