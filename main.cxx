#include <functional>

#include <GLFW/glfw3.h>

#include <iostream>

using std::cout;
using std::clog;
using std::cerr;
using std::endl;

namespace cg::util {

    struct post_scope {
        const std::string label;
        std::function<void()> on_destruct;
        ~post_scope() {
            clog << "Unscope: " << label << " ";
            if (on_destruct) {
                clog << ">> ";
                on_destruct();
            }
            clog << "Done" << endl;
        }
    };

    template<typename T> struct report_res {
        T user_res;
        std::optional<std::string> user_out = std::nullopt;
    };

    template<typename T> std::optional<T> report(std::string_view desc, std::function<report_res<T>()> todo, std::function<bool(const T&)> check = { }) {
        clog << desc << " >> ";
        auto res = todo();
        if (check) {
            if (check(res.user_res)) {
                clog << "Okay" << endl;
                return res.user_res;
            } else {
                clog << "Bad" << endl;
                return std::nullopt;
            }
        } else {
            clog << "Done" << endl;
            return res.user_res;
        }
    }
}

using cg::util::post_scope;
using cg::util::report;

#include <fmt/core.h>
#include <fmt/format.h>

using fmt::format;

int main() {
    post_scope _ps_cleanup_glfw({
        "Cleanup GLFW",
        []() {
            glfwTerminate();
        }
    });
    const auto glfw_okay = report<bool>(
        "Init GLFW",
        []() -> cg::util::report_res<bool> {
            return {
                glfwInit() == GLFW_TRUE,
                format("ver: {}.{}.{}", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION)
            };
        },
        [](bool res) {
            return res;
        }
    );
    if (!glfw_okay) return 1;
    const auto window = report<GLFWwindow *>(
        "Create GLFW window",
        []() -> cg::util::report_res<GLFWwindow *> {
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
            return { glfwCreateWindow(800, 600, "GLFW", 0, 0) };
        },
        [](GLFWwindow *res) {
            clog << "(ptr: " << res << ") ";
            return res != nullptr;
        }
    );
    if (!window) return 2;
    return 0;
}