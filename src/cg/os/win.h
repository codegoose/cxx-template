#pragma once

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>
#include <filesystem>

namespace cg::os::win {

    std::vector<char *> get_command_line_arguments();
    
    std::optional<std::filesystem::path> get_user_desktop_path();
    std::optional<std::filesystem::path> get_user_startup_path();
    std::optional<std::filesystem::path> get_module_file_path();

    void show_message_box(const std::string_view &title, const std::string_view &text);

    namespace com {

        bool initialize();
        bool create_link(const std::filesystem::path &object_path, const std::filesystem::path &link_path, std::optional<std::string_view> arguments = std::nullopt, std::optional<std::string_view> description = std::nullopt);
        void cleanup();
    }

    std::optional<int> run(const std::string_view &cmd_line, std::optional<std::filesystem::path> working_directory = std::nullopt);

    std::optional<std::pair<void *, size_t>> get_resource(int id, const std::string_view &type);
}