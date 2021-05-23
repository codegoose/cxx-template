#pragma once

#include <functional>
#include <optional>
#include <string>

namespace cg::init {

    int run(const std::function<std::optional<std::string>()> loop, bool err_notify = true);
}