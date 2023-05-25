#include "real_file_system.h"

#include <boost/log/trivial.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

RealFileSystem::RealFileSystem() : FileSystem() {}

bool RealFileSystem::exists_file(std::string file_path) {
    return std::filesystem::exists(file_path) &&
           !std::filesystem::is_directory(file_path);
}

bool RealFileSystem::create_file(std::string file_path) {
    std::ofstream file(file_path);
    if (file.bad()) {
        return false;
    }
    file << "";
    file.close();
    return true;
}

bool RealFileSystem::write_file(std::string file_path, std::string contents) {
    if (!exists_file(file_path)) {
        return false;
    }
    std::ofstream file(file_path);
    if (file.bad()) {
        return false;
    }
    file << contents;
    file.close();
    return true;
}

bool RealFileSystem::delete_file(std::string file_path) {
    try {
        std::filesystem::remove(file_path);
    } catch (const std::filesystem::filesystem_error& error) {
        return false;
    }
    return true;
}

std::string RealFileSystem::read_file(std::string file_path) {
    if (!exists_file(file_path)) {
        return "";
    }

    std::ifstream file_in(file_path);
    std::stringstream buffer;
    buffer << file_in.rdbuf();
    std::string contents = buffer.str();
    file_in.close();
    return contents;
}

bool RealFileSystem::exists_directory(std::string directory_path) {
    return std::filesystem::exists(directory_path) &&
           std::filesystem::is_directory(directory_path);
}

bool RealFileSystem::create_directory(std::string directory_path) {
    return std::filesystem::create_directory(directory_path);
}

bool RealFileSystem::delete_directory(std::string directory_path) {
    try {
        std::filesystem::remove(directory_path);
    } catch (...) {
        return false;
    }
    return true;
}