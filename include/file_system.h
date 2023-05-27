#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <filesystem>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class FileSystem {
    public:
        virtual std::optional<std::string> read_file(const std::string& path);
        virtual void write_file(const std::string& path,
                                const std::string& content);
        virtual bool initialize_directory(const std::string& directory_path);
        virtual bool delete_file(const std::string& path);
        virtual std::optional<std::vector<std::string>> list_files(
            const std::string& path);

    private:
        std::optional<std::filesystem::path> get_filesystem_path(
            const std::string& path, bool is_dir);
        static std::mutex* get_path_lock(const std::string& path);
        static std::unordered_map<std::string, std::mutex> locks;
        static std::mutex file_system_map_lock;
};

/*
FakeFileSystem for unit testing

- This is needed so that the CRUD request handler can be unit tested without
having actual interactions with the local file system

- This functionality is implemented by storing data in an unordered_map to
replicate a fake file system
*/
class FakeFileSystem : public FileSystem {
    public:
        virtual std::optional<std::string> read_file(
            const std::string& path) override;
        virtual void write_file(const std::string& path,
                                const std::string& content) override;
        virtual bool initialize_directory(const std::string& directory_path);
        virtual bool delete_file(const std::string& path) override;
        virtual std::optional<std::vector<std::string>> list_files(
            const std::string& path) override;

    private:
        std::unordered_map<std::string, std::string> files_;
        std::unordered_set<std::string> directories_;
};

#endif