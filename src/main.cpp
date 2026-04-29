#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>

#include "entity_manager.h"
#include "sensor_receiver.h"
#include "renderer.h"
#include "hud.h"

static EntityManager g_em;
static Renderer*     g_renderer = nullptr;

static void on_key(GLFWwindow* win, int key, int /*scancode*/, int action, int /*mods*/) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(win, true);
        if (key == GLFW_KEY_V)      g_renderer->camera().toggle_mode();
    }
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        g_renderer->camera().process_keyboard(key, 0.016f);
    }
}

static void on_scroll(GLFWwindow* /*win*/, double /*dx*/, double dy) {
    g_renderer->camera().process_scroll(static_cast<float>(dy));
}

static double g_last_x = 0, g_last_y = 0;
static bool   g_dragging = false;

static void on_mouse_button(GLFWwindow* win, int btn, int action, int /*mods*/) {
    if (btn == GLFW_MOUSE_BUTTON_LEFT) {
        g_dragging = (action == GLFW_PRESS);
        glfwGetCursorPos(win, &g_last_x, &g_last_y);
    }
}

static void on_cursor(GLFWwindow* /*win*/, double x, double y) {
    if (!g_dragging) return;
    float dx = static_cast<float>(x - g_last_x);
    float dy = static_cast<float>(y - g_last_y);
    g_last_x = x; g_last_y = y;
    g_renderer->camera().process_mouse_drag(dx, dy);
}

int main() {
    if (!glfwInit()) { std::cerr << "GLFW init failed\n"; return 1; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* win = glfwCreateWindow(1280, 720, "TactVis — Tactical Situational Awareness", nullptr, nullptr);
    if (!win) { std::cerr << "Window creation failed\n"; glfwTerminate(); return 1; }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "GLAD init failed\n"; return 1;
    }

    glfwSetKeyCallback(win, on_key);
    glfwSetScrollCallback(win, on_scroll);
    glfwSetMouseButtonCallback(win, on_mouse_button);
    glfwSetCursorPosCallback(win, on_cursor);

    Renderer renderer(win, g_em);
    g_renderer = &renderer;

    HUD hud(win);

    SensorReceiver receiver(g_em, 5005);
    receiver.start();

    double prev_time = glfwGetTime();
    double fps_accum = 0;
    int    fps_frames = 0;
    float  fps = 0;

    while (!glfwWindowShouldClose(win)) {
        double now = glfwGetTime();
        double dt  = now - prev_time;
        prev_time  = now;

        fps_accum += dt;
        fps_frames++;
        if (fps_accum >= 0.5) {
            fps = static_cast<float>(fps_frames / fps_accum);
            fps_accum = 0; fps_frames = 0;
        }

        int w, h;
        glfwGetFramebufferSize(win, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.05f, 0.07f, 0.10f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        renderer.draw(now, static_cast<float>(w) / static_cast<float>(h));

        auto entities = g_em.snapshot();
        hud.begin_frame();
        hud.draw(entities, renderer.camera(), now, fps);
        hud.end_frame();

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    receiver.stop();
    g_renderer = nullptr;
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
