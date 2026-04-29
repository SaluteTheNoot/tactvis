#include "hud.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

HUD::HUD(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}

HUD::~HUD() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void HUD::begin_frame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void HUD::draw(const std::vector<Entity>& entities, const Camera& cam,
               double /*now*/, float fps)
{
    draw_status_bar(cam, fps);
    draw_entity_list(entities);
}

void HUD::end_frame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void HUD::draw_status_bar(const Camera& cam, float fps) {
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({320, 60});
    ImGui::Begin("##status", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove     | ImGuiWindowFlags_NoBackground);

    ImGui::TextColored({0.3f, 1.f, 0.5f, 1.f}, "TactVis");
    ImGui::SameLine(80);
    ImGui::Text("%.0f FPS", fps);
    ImGui::SameLine(140);
    ImGui::Text("Mode: %s", cam.mode() == CameraMode::TopDown ? "TOP-DOWN" : "3D");
    ImGui::SameLine(260);
    ImGui::TextDisabled("[V] toggle");

    ImGui::End();
}

void HUD::draw_entity_list(const std::vector<Entity>& entities) {
    ImGui::SetNextWindowPos({0, 70});
    ImGui::SetNextWindowSize({280, 400});
    ImGui::Begin("Entities", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    int friendly = 0, hostile = 0, unknown = 0;
    for (const auto& e : entities) {
        switch (e.iff) {
            case IFF::Friendly: friendly++; break;
            case IFF::Hostile:  hostile++;  break;
            case IFF::Unknown:  unknown++;  break;
        }
    }

    ImGui::TextColored({0.f, 0.9f, 0.3f, 1.f}, "FRD: %d", friendly);
    ImGui::SameLine();
    ImGui::TextColored({0.9f, 0.1f, 0.1f, 1.f}, "HST: %d", hostile);
    ImGui::SameLine();
    ImGui::TextColored({0.9f, 0.7f, 0.1f, 1.f}, "UNK: %d", unknown);
    ImGui::Separator();

    if (ImGui::BeginTable("entity_table", 4,
            ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInner | ImGuiTableFlags_ScrollY,
            {0, 0}))
    {
        ImGui::TableSetupColumn("Call");
        ImGui::TableSetupColumn("IFF");
        ImGui::TableSetupColumn("Hdg");
        ImGui::TableSetupColumn("Spd");
        ImGui::TableHeadersRow();

        for (const auto& e : entities) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", e.callsign.empty() ? std::to_string(e.id).c_str() : e.callsign.c_str());

            ImGui::TableSetColumnIndex(1);
            ImVec4 c = (e.iff == IFF::Friendly) ? ImVec4{0, .9f, .3f, 1}
                     : (e.iff == IFF::Hostile)  ? ImVec4{.9f, .1f, .1f, 1}
                                                : ImVec4{.9f, .7f, .1f, 1};
            const char* label = (e.iff == IFF::Friendly) ? "FRD"
                              : (e.iff == IFF::Hostile)  ? "HST" : "UNK";
            ImGui::TextColored(c, "%s", label);

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.0f°", e.heading);

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%.0f", e.speed);
        }
        ImGui::EndTable();
    }
    ImGui::End();
}
