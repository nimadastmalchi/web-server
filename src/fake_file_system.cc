#include "fake_file_system.h"

FakeFileSystem::FakeFileSystem() : FileSystem() {

}

bool FakeFileSystem::exists_file(std::string file_path) {
    return files.find(file_path) != files.end();
}

bool FakeFileSystem::create_file(std::string file_path) {
    if (exists_file(file_path)) {
        return false;
    }

    std::vector<std::string> directories = parse_directories(file_path);
    for (int i = 0; i < directories.size(); i++) {
        create_directory(directories[i]);
    }

    files[file_path] = "";
    return true;
}

bool FakeFileSystem::write_file(std::string file_path, std::string contents) {
    if (!exists_file(file_path)) {
        return false;
    }

    files[file_path] = contents;
    return true;
}

bool FakeFileSystem::delete_file(std::string file_path) {
    if (!exists_file(file_path)) {
        return false;
    }

    files.erase(file_path);
    return true;
}

std::string FakeFileSystem::read_file(std::string file_path) {
    if (!exists_file(file_path)) {
        return "";
    }
    return files.find(file_path)->second;
}

bool FakeFileSystem::exists_directory(std::string directory_path) {
    return directories.find(directory_path) != directories.end();
}

bool FakeFileSystem::create_directory(std::string directory_path) {
    if (exists_directory(directory_path)) {
        return false;
    }

    directories.insert(directory_path);
    return true;
}

std::vector<std::string> FakeFileSystem::parse_directories(std::string directory_path) {
    std::vector<std::string> directories = {""};
    for (int i = 0; i < directory_path.size(); i++) {
        if (directory_path[i] == '/') {
            directories.push_back(directory_path.substr(0, i));
        }
    }
    return directories;
}