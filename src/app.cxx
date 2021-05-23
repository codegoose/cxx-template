#include <optional>
#include <string>
#include <functional>

#include <spdlog/spdlog.h>

using spdlog::debug;
using spdlog::info;
using spdlog::warn;
using spdlog::error;
using spdlog::critical;

using fmt::print;
using fmt::format;

#include <curl/curl.h>
#include <curl/easy.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "cg/os/win.h"

enum class err {
    curl_init,
    glfw_init,
    glfw_window_create,
    glew_init
};

// Helper to run some code on scope closure.
struct post_scope {
    const std::string label;
    std::function<void()> on_destruct;
    ~post_scope() { if (on_destruct) { on_destruct(); } }
};

int run(const std::function<std::optional<std::string>()> loop, bool err_notify = true) {
    std::optional<std::string> loop_res;
    const auto init_res = [loop, &loop_res, err_notify]() -> std::optional<err> {
        if (curl_global_init(0) != CURLE_OK) return err::curl_init;
        post_scope _ps_cleanup_curl({ "Cleanup CURL", []() { curl_global_cleanup(); } });
        if (glfwInit() != GLFW_TRUE) return err::glfw_init;
        post_scope _ps_cleanup_glfw({ "Cleanup GLFW", []() { glfwTerminate(); } });
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        const auto window = glfwCreateWindow(800, 600, "GLFW", 0, 0);
        if (!window) return err::glfw_window_create;
        glfwMakeContextCurrent(window);
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) return err::glew_init;
        if (loop) loop_res = loop();
        return std::nullopt;
    }();
    if (err_notify && loop_res.has_value()) cg::os::win::show_message_box("Runtime Error", format("This program encountered an error while it was running:\n\n{}", *loop_res));
    else if (init_res.has_value()) {
        if (err_notify) cg::os::win::show_message_box("Initialization Error", format("This program encountered an error and was unable to start:\n\n{}", [init_res]() {
            switch (*init_res) {
                case err::curl_init: return "Unable to initialize cURL library.";
                case err::glfw_init: return "Unable to initialize GLFW library.";
                case err::glfw_window_create: return "Unable to create GLFW window.";
                case err::glew_init: return "Unable to wrangle OpenGL extensions.";
            }
            return "Someone made a programming mistake.";
        }()));
        return static_cast<int>(*init_res);
    }
    return 0;
}

int main() {
    if (const auto resources = cg::os::win::get_resource(101, "DATA"); resources.has_value()) info("Resources: {} bytes @ {}", resources->second, resources->first);
    else warn("No resources available.");
    return run([]() -> std::optional<std::string> {
        const auto window = glfwGetCurrentContext();
        glfwShowWindow(window);
        post_scope _ps_hide_window({ "Hide Window", [window]() { glfwHideWindow(window); } });
        for (;;) {
            glfwPollEvents();
            if (glfwWindowShouldClose(window)) return std::nullopt;
            glClearColor(.4, .3, .2, 1);
            glClear(GL_COLOR_BUFFER_BIT);
            glfwSwapBuffers(window);
        }
        return std::nullopt;
    }, true);
}