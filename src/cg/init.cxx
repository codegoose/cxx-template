#include "init.h"
#include "post_scope.h"
#include "os/win.h"

#include <spdlog/fmt/fmt.h>

using fmt::format;

#include <curl/curl.h>
#include <curl/easy.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

enum class err {
    curl_init,
    glfw_init,
    glfw_window_create,
    glew_init
};

int cg::init::run(const std::function<std::optional<std::string>()> loop, bool err_notify) {
    std::optional<std::string> loop_res;
    const auto init_res = [loop, &loop_res, err_notify]() -> std::optional<err> {
        if (curl_global_init(0) != CURLE_OK) return err::curl_init;
        post_scope _ps_cleanup_curl({ []() { curl_global_cleanup(); } });
        if (glfwInit() != GLFW_TRUE) return err::glfw_init;
        post_scope _ps_cleanup_glfw({ []() { glfwTerminate(); } });
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