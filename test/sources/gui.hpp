#pragma once
#include <imgui/imgui.h>
#include <entt/entt.hpp>
#include "renderer.hpp"
#include "utility.hpp"
#include <filesystem>

namespace fs = std::filesystem;

struct TextFilters {
    static int ProjectName(ImGuiInputTextCallbackData* data);
};

void renderGui(entt::registry &registry, Renderer &renderer);
