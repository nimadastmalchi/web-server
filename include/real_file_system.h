#ifndef REAL_FILE_SYSTEM_H
#define REAL_FILE_SYSTEM_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "file_system.h"

class RealFileSystem : public FileSystem {
    public:
        RealFileSystem();

        virtual bool exists_file(std::string file_path);
        virtual bool create_file(
            std::string file_path);  // returns true if created. returns false
                                     // if file already exists
        virtual bool write_file(
            std::string file_path,
            std::string contents);  // returns true if written. returns false if
                                    // file doesn't exist
        virtual bool delete_file(
            std::string file_path);  // returns true if deleted. returns false
                                     // if file doesn't exist
        virtual std::string read_file(
            std::string file_path);  // returns empty string if no file exists

        virtual bool exists_directory(std::string directory_path);
        virtual bool create_directory(
            std::string directory_path);  // returns true if created. returns
                                          // false if directory already exists
        virtual bool delete_directory(
            std::string directory_path);  // returns true if deleted. returns
                                          // false if directory never existed
};

#endif