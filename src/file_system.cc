#include "file_system.h"

#include <filesystem>
#include <fstream>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

/*
Path based map of locks.
A single mutex locks access to the map itself.
*/
std::unordered_map<std::string, std::mutex> FileSystem::locks;
std::mutex FileSystem::file_system_map_lock;

std::mutex* FileSystem::get_path_lock(const std::string& path) {
    std::lock_guard<std::mutex> guard(file_system_map_lock);
    auto& path_lock = locks[path];
    return &path_lock;
}

std::optional<std::filesystem::path> FileSystem::get_filesystem_path(
    const std::string& path, bool is_dir) {
    if (!std::filesystem::exists(path)) {
        return std::nullopt;
    }
    if (!is_dir && !std::filesystem::is_regular_file(path)) {
        return std::nullopt;
    }

    return std::filesystem::path(path);
}

std::optional<std::string> FileSystem::read_file(const std::string& path) {
    const auto file_path = get_filesystem_path(path, false);
    if (!file_path.has_value()) return std::nullopt;
    std::ifstream file;
    file.open(path);

    std::stringstream strStream;
    strStream << file.rdbuf();
    std::string contents = strStream.str();
    return std::optional<std::string>{contents};
}

void FileSystem::write_file(const std::string& path,
                            const std::string& content) {
    std::lock_guard<std::mutex> guard(*get_path_lock(path));
    std::ofstream file;
    file.open(path);
    file << content;
    file.close();
}

bool FileSystem::initialize_directory(const std::string& directory_path) {
    if (!std::filesystem::exists(directory_path) &&
        !std::filesystem::is_regular_file(directory_path)) {
        std::lock_guard<std::mutex> guard(*get_path_lock(directory_path));
        std::filesystem::create_directories(directory_path);
        return true;
    }
    return false;
}

bool FileSystem::delete_file(const std::string& path) {
    std::lock_guard<std::mutex> guard(*get_path_lock(path));
    const auto file_path = get_filesystem_path(path, false);
    if (!file_path.has_value()) {
        return false;
    }
    std::filesystem::remove(file_path.value());
    return true;
}

std::optional<std::vector<std::string>> FileSystem::list_files(
    const std::string& path) {
    const auto file_path = get_filesystem_path(path, true);
    if (!file_path.has_value()) return std::nullopt;

    std::vector<std::string> files;

    // Reference:
    // https://en.cppreference.com/w/cpp/filesystem/directory_iterator
    for (auto const& dir_entry :
         std::filesystem::directory_iterator{file_path.value()}) {
        files.emplace_back(dir_entry.path());
    }

    return std::optional<std::vector<std::string>>{files};
}

/*
Fake File System Implementation
*/

std::optional<std::string> FakeFileSystem::read_file(const std::string& path) {
    if (files_.find(path) == files_.end()) return std::nullopt;
    return files_[path];
}

void FakeFileSystem::write_file(const std::string& path,
                                const std::string& content) {
    files_[path] = content;
}

bool FakeFileSystem::delete_file(const std::string& path) {
    if (files_.find(path) == files_.end()) return false;
    files_.erase(path);
    return true;
}

bool FakeFileSystem::initialize_directory(const std::string& directory_path) {
    if (directories_.find(directory_path) == directories_.end()) {
        directories_.emplace(directory_path);
        return true;
    }

    return false;
}

std::optional<std::vector<std::string>> FakeFileSystem::list_files(
    const std::string& path) {
    std::string prefix = path;
    std::vector<std::string> file_list;
    for (const auto& [key, val] : files_) {
        if (key.size() >= prefix.size() &&
            key.compare(0, prefix.size(), prefix) == 0) {
            std::string file_name = key.substr(prefix.length());
            if (file_name[0] == '/') file_name = file_name.substr(1);
            file_list.emplace_back(file_name);
        }
    }
    if (file_list.empty()) return std::nullopt;
    return file_list;
}
