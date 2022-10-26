#pragma once
#define SOL_ALL_SAFETIES_ON 1
#include "renderer.hpp"
#include <entt/entt.hpp>
#include <sol/sol.hpp>

struct lua {
    static entt::registry* registry;

    static entt::entity createNode(const std::string &name);

    template <typename T>
    static void addComponent(entt::entity entity, T component);
};

void loadScript(const std::string &path);
