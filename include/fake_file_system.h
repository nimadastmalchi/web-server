#ifndef FAKE_FILE_SYSTEM_H
#define FAKE_FILE_SYSTEM_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "file_system.h"

class FakeFileSystem : public FileSystem {
    public:
        FakeFileSystem();

        virtual bool exists_file(std::string file_path);
        virtual bool create_file(std::string file_path);                        //returns true if created. returns false if file already exists
        virtual bool write_file(std::string file_path, std::string contents);   //returns true if written. returns false if file doesn't exist
        virtual bool delete_file(std::string file_path);                        //returns true if deleted. returns false if file doesn't exist
        virtual std::string read_file(std::string file_path);                   //returns empty string if no file exists

        bool exists_directory(std::string directory_path);
        bool create_directory(std::string directory_path);              //returns true if created. returns false if directory already exists

    private:
        std::vector<std::string> parse_directories(std::string directory_path);
        std::unordered_map<std::string, std::string> files;
        std::unordered_set<std::string> directories;
};

#endif