#include "fake_file_system.h"
#include "gtest/gtest.h"

class FakeFileSystemTest : public ::testing::Test {
    protected:
        void SetUp() override {}
        FakeFileSystem fs;
};

TEST_F(FakeFileSystemTest, UncreatedFile) {
    bool exists = fs.exists_file("file");
    EXPECT_FALSE(exists);
}

TEST_F(FakeFileSystemTest, CreatedFile) {
    bool created = fs.create_file("file");
    bool exists = fs.exists_file("file");
    EXPECT_TRUE(exists);
    EXPECT_TRUE(created);
}

TEST_F(FakeFileSystemTest, CreatedFileFails) {
    fs.create_file("file");
    bool created = fs.create_file("file");
    EXPECT_FALSE(created);
}

TEST_F(FakeFileSystemTest, CreateDirectoriesFromPath) {
    fs.create_file("root/clothing/hats/beanie");
    EXPECT_TRUE(fs.exists_directory(""));
    EXPECT_TRUE(fs.exists_directory("root"));
    EXPECT_TRUE(fs.exists_directory("root/clothing"));
    EXPECT_TRUE(fs.exists_directory("root/clothing/hats"));
}

TEST_F(FakeFileSystemTest, FileInitiallyEmpty) {
    fs.create_file("test");
    std::string contents = fs.read_file("test");
    EXPECT_EQ(contents, "");
}

TEST_F(FakeFileSystemTest, WriteFileNotFound) {
    bool written = fs.write_file("file", "payload");
    EXPECT_FALSE(written);
}

TEST_F(FakeFileSystemTest, WriteFile) {
    fs.create_file("file");
    bool written = fs.write_file("file", "payload");
    std::string contents = fs.read_file("file");
    EXPECT_TRUE(written);
    EXPECT_EQ(contents, "payload");
}

TEST_F(FakeFileSystemTest, ExistsDirectory) {
    bool created = fs.create_directory("directory");
    bool exists = fs.exists_directory("directory");
    EXPECT_TRUE(created);
    EXPECT_TRUE(exists);
}

TEST_F(FakeFileSystemTest, CreateDirectoryFails) {
    fs.create_directory("directory");
    bool created = fs.create_directory("directory");
    EXPECT_FALSE(created);
}

TEST_F(FakeFileSystemTest, ExistsDirectoryFails) {
    bool exists = fs.exists_directory("directory");
    EXPECT_FALSE(exists);
}

TEST_F(FakeFileSystemTest, DeleteDirectorySuccess) {
    fs.create_directory("test");
    bool deleted = fs.delete_directory("test");
    EXPECT_TRUE(deleted);
}

TEST_F(FakeFileSystemTest, DeleteDirectoryFailure) {
    bool deleted = fs.delete_directory("test");
    EXPECT_FALSE(deleted);
}