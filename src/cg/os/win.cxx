#define WINVER 0x0600
#define _WIN32_WINNT 0x0600

#include "win.h"

#include <codecvt>
#include <string>

#include <shlobj.h>
#include <shellapi.h>
#include <windows.h>

namespace cg::os::win {

    static std::vector<std::string> args_storage;
    static std::vector<char *> args;
}

std::vector<char *> cg::os::win::get_command_line_arguments() {
    static bool first = true;
    if (first) {
        // auto logger = make_local_logger();
        int num_command_line_args;
        auto command_line_args = CommandLineToArgvW(GetCommandLineW(), &num_command_line_args);
        if (!command_line_args) return { };
        args_storage.resize(num_command_line_args + 1);
        args.resize(num_command_line_args + 1);
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        for (int i = 0; i < num_command_line_args; i++) {
            args_storage[i] = converter.to_bytes(command_line_args[i]);
            args[i] = args_storage[i].data();
        }
        args[args.size() - 1] = nullptr;
        first = false;
    }
    return args;
}

std::optional<std::filesystem::path> cg::os::win::get_user_desktop_path() {
    CHAR path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(0, CSIDL_DESKTOPDIRECTORY, 0, SHGFP_TYPE_CURRENT, path))) return path;
    return std::nullopt;
}

std::optional<std::filesystem::path> cg::os::win::get_user_startup_path() {
    std::optional<std::filesystem::path> res = std::nullopt;
    PWSTR path;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Startup, 0, NULL, &path))) {
        res = path;
        CoTaskMemFree(path);
    }
    return res;
}

std::optional<std::filesystem::path> cg::os::win::get_module_file_path() {
    TCHAR path[MAX_PATH];
    if (GetModuleFileNameA(NULL, path, sizeof(path)) == 0) return std::nullopt;
    return path;
}

void cg::os::win::show_message_box(const std::string_view &title, const std::string_view &text) {
    MessageBox(NULL, text.data(), title.data(), MB_OK | MB_ICONINFORMATION);
}

bool cg::os::win::com::initialize() {
    // auto logger = make_local_logger();
    auto res = CoInitialize(NULL);
    if (res != S_OK) {
        // logger->warn("Unable to initialize Windows COM library.");
        cleanup();
    } // else logger->debug("Initialized Windows COM library.");
    return res == S_OK;
}

bool cg::os::win::com::create_link(const std::filesystem::path &object_path, const std::filesystem::path &link_path, std::optional<std::string_view> arguments, std::optional<std::string_view> description) {
    bool success = false;
    // auto logger = make_local_logger();
    if (IShellLink *psl; SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, reinterpret_cast<LPVOID *>(&psl)))) {
        psl->SetPath(object_path.string().data());
        if (arguments.has_value()) psl->SetArguments(arguments->data());
        if (description.has_value()) psl->SetDescription(description->data());
        if (IPersistFile *ppf; SUCCEEDED(psl->QueryInterface(IID_IPersistFile, reinterpret_cast<LPVOID *>(&ppf)))) {
            if (WCHAR wsz[MAX_PATH]; MultiByteToWideChar(CP_ACP, 0, link_path.string().data(), -1, wsz, sizeof(wsz)) > 0) {
                if (SUCCEEDED(ppf->Save(wsz, TRUE))) {
                    // logger->debug("Created link: {} -> {}", object_path.string(), link_path.string());
                    success = true;
                } // else logger->warn("Unable to create link: {} -> {}", object_path.string(), link_path.string());
            } // else logger->warn("Unable to convert link path to wide characters: {}", link_path.string());
            ppf->Release();
        }
        psl->Release();
    }
    return success;
}

void cg::os::win::com::cleanup() {
    // auto logger = make_local_logger();
    CoUninitialize();
    // logger->debug("Cleaned up Windows COM library.");
}

std::optional<int> cg::os::win::run(const std::string_view &cmd_line, std::optional<std::filesystem::path> working_directory) {
    // auto logger = make_local_logger(false);
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    if(!CreateProcess(NULL, (char *)cmd_line.data(), NULL, NULL, FALSE, 0, NULL, working_directory.has_value() ? working_directory->string().data() : NULL, &si, &pi)) {
        // logger->error("Unable to create process: {}", cmd_line);
        return std::nullopt;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    int result = -1;
    if (!GetExitCodeProcess(pi.hProcess,(LPDWORD)&result)) {
        // logger->error("Unable to get process exit code: {}", cmd_line);
        return std::nullopt;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return result;
}

std::optional<std::pair<void *, size_t>> cg::os::win::get_resource(int id, const std::string_view &type) {
    auto resource_handle = FindResource(nullptr, MAKEINTRESOURCEA(id), type.data());
    if (resource_handle == NULL) return std::nullopt;
    auto memory_handle = LoadResource(nullptr, resource_handle);
    if (memory_handle == NULL) return std::nullopt;
    auto resource_length = SizeofResource(nullptr, resource_handle);
    if (resource_length == 0) return std::nullopt;
    auto resource_location = LockResource(memory_handle);
    if (resource_location == 0) return std::nullopt;
    return std::make_pair(resource_location, resource_length);
}