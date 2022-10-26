#include "gui.hpp"

int TextFilters::ProjectName(ImGuiInputTextCallbackData* data) {
    if (data->EventChar < 256 && strchr("abcdefghijklmnopqrstuvwxyz_", (char) data->EventChar)) {
        return 0;
    }

    return 1;
}

void renderGui(entt::registry &registry, Renderer &renderer) {
    ImGui::ShowDemoWindow();

    {
        bool isOpen = false;

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Project")) {
                if (ImGui::MenuItem("Create")) {
                    isOpen = true;
                }

                if (ImGui::MenuItem("Load")) {
                    std::string path = selectFile(0, "project.json");
                    std::cout << path << std::endl;
                    // test.project.isLoaded = true;
                }

                if (ImGui::MenuItem("Save")) {

                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (isOpen) {
            ImGui::OpenPopup("Create Project");
        }

        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Create Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            static char name[32] = "untitled";
            static bool isError = false;
            ImGui::InputText("Name", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_CallbackCharFilter, TextFilters::ProjectName);

            if (isError) {
                ImGui::Text("Project name must no be empty");
            }

            ImGui::Separator();

            if (ImGui::Button("Create")) {
                if (strlen(name)) {
                    // createProject(name);
                    isError = false;
                    strcpy(name, "");
                    ImGui::CloseCurrentPopup();
                } else {
                    isError = true;
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel")) {
                isError = false;
                strcpy(name, "");
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }






















    ImGui::Begin("Explorer");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::ColorEdit4("Clear Color", (float*) &renderer.clearColor);
    ImGui::ColorEdit3("Grid Color", (float*) &renderer.grid.color);
    ImGui::DragFloat3("Camera Position", (float*) &renderer.camera.position, 0.1f);
    ImGui::DragFloat("Camera Speed", (float*) &renderer.camera.speed, 0.1f);

    static auto view = registry.view<Node>();
    static entt::entity selected = entt::null;

    if (ImGui::CollapsingHeader("Nodes")) {
        for(auto entity : view) {
            auto &node = view.get<Node>(entity);

            if (ImGui::Selectable(node.name.c_str(), entity == selected)) {
                selected = entity;
            }
        }
    }

    if (ImGui::CollapsingHeader("Components")) {
        if (registry.valid(selected)) {
            if (registry.any_of<Node>(selected) && ImGui::TreeNode("Node")) {
                auto &node = view.get<Node>(selected);

                ImGui::InputText("Name", node.name.data(), IM_ARRAYSIZE(node.name.data()), ImGuiInputTextFlags_ReadOnly);
                ImGui::TreePop();
            }

            if (registry.any_of<Transform>(selected) && ImGui::TreeNode("Transform")) {
                auto &transform = registry.get<Transform>(selected);

                ImGui::DragFloat3("Translation", (float*) &transform.translation, 0.1f);
                ImGui::DragFloat3("Rotation", (float*) &transform.rotation, 0.1f);
                ImGui::DragFloat3("Scale", (float*) &transform.scale, 0.1f);
                ImGui::TreePop();
            }
        } else {
            ImGui::Text("Select node to see its components");
        }
    }

    if (ImGui::CollapsingHeader("Assets")) {
        static fs::path path = "../projects/";
        static std::size_t selectedHash = fs::hash_value(path);
        static int depth = 0;

        if (ImGui::Button("Import")) {
            std::cout << "Import" << std::endl;
        }

        ImGui::Text(path.c_str());

        if (ImGui::Selectable("..", false, ImGuiSelectableFlags_AllowDoubleClick)) {
            if (ImGui::IsMouseDoubleClicked(0)) {
                if (fs::exists(path.parent_path()) && depth > 0) {
                    path = path.parent_path();
                    depth--;
                }
            }
        }

        for (const auto &entry : fs::directory_iterator(path)) {
            if (ImGui::Selectable(((fs::path) entry).filename().c_str(), selectedHash == fs::hash_value(entry), ImGuiSelectableFlags_AllowDoubleClick)) {
                if (ImGui::IsMouseDoubleClicked(0) && fs::is_directory(entry)) {
                    path = entry;
                    depth++;
                } else {
                    selectedHash = fs::hash_value(entry);
                }
            }
        }
    }

    ImGui::End();
}
