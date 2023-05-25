#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class FileSystem {
    public:
        FileSystem() {}

        virtual bool exists_file(std::string file_path) = 0;
        virtual bool create_file(
            std::string file_path) = 0;  // returns true if created. returns
                                         // false if file already exists
        virtual bool write_file(
            std::string file_path,
            std::string contents) = 0;  // returns true if written. returns
                                        // false if file doesn't exist
        virtual bool delete_file(
            std::string file_path) = 0;  // returns true if deleted. returns
                                         // false if file doesn't exist
        virtual std::string read_file(
            std::string
                file_path) = 0;  // returns empty string if no file exists

        virtual bool exists_directory(std::string directory_path) = 0;
        virtual bool create_directory(
            std::string
                directory_path) = 0;  // returns true if created. returns false
                                      // if directory already exists
        virtual bool delete_directory(
            std::string
                directory_path) = 0;  // returns true if deleted. returns false
                                      // if directory never existed
};

#endif