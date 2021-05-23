#include <spdlog/spdlog.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "cg/init.h"
#include "cg/post_scope.h"
#include "cg/os/win.h"

int main() {
    if (const auto resources = cg::os::win::get_resource(101, "DATA"); resources.has_value()) spdlog::info("Resources: {} bytes @ {}", resources->second, resources->first);
    else spdlog::warn("No resources available.");
    return cg::init::run([]() -> std::optional<std::string> {
        const auto window = glfwGetCurrentContext();
        glfwShowWindow(window);
        cg::post_scope _ps_hide_window({ [window]() { glfwHideWindow(window); } });
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