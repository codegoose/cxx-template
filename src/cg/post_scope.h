#pragma once

#include <functional>

namespace cg {
    // Helper to run some code on scope closure.
    struct post_scope {
        std::function<void()> on_destruct;
        ~post_scope() { if (on_destruct) { on_destruct(); } }
    };
}