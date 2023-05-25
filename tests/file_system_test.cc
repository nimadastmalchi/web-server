#include "file_system.h"

#include <optional>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

class FileSystemTest : public ::testing::Test {
    protected:
        FileSystem fs;
};

TEST_F(FileSystemTest, nonexistentRead) {
    EXPECT_EQ(fs.read_file(""), std::nullopt);
}

TEST_F(FileSystemTest, nonexistentDelete) { EXPECT_FALSE(fs.delete_file("")); }

TEST_F(FileSystemTest, nonexistentListFiles) {
    EXPECT_EQ(fs.list_files(""), std::nullopt);
}

class FakeFileSystemTest : public ::testing::Test {
    protected:
        FakeFileSystem fs;
};

TEST_F(FakeFileSystemTest, nonexistentRead) {
    EXPECT_EQ(fs.read_file(""), std::nullopt);
}

TEST_F(FakeFileSystemTest, nonexistentDelete) {
    EXPECT_FALSE(fs.delete_file(""));
}

TEST_F(FakeFileSystemTest, nonexistentListFiles) {
    EXPECT_EQ(fs.list_files(""), std::nullopt);
}

TEST_F(FakeFileSystemTest, nonexistentInitializeDirectory) {
    EXPECT_TRUE(fs.initialize_directory("test"));
}

TEST_F(FakeFileSystemTest, writeAndRead) {
    fs.write_file("test", "test");
    EXPECT_EQ(fs.read_file("test").value(), "test");
}

TEST_F(FakeFileSystemTest, existentDelete) {
    fs.write_file("test", "test");
    EXPECT_TRUE(fs.delete_file("test"));
    EXPECT_EQ(fs.read_file("test"), std::nullopt);
}

TEST_F(FakeFileSystemTest, existentListFiles) {
    fs.write_file("test", "test");
    fs.write_file("target/target_file", "test");
    const auto& files = fs.list_files("target");
    EXPECT_EQ(files.value()[0], "target_file");
}

TEST_F(FakeFileSystemTest, existentInitializeDirectory) {
    fs.initialize_directory("test");
    EXPECT_FALSE(fs.initialize_directory("test"));
}
