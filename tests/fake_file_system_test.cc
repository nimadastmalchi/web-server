#include "fake_file_system.h"
#include "gtest/gtest.h"

class FakeFileSystemTest : public ::testing::Test {
    protected:
        void SetUp() override {}
};

TEST_F(FakeFileSystemTest, UncreatedFile) {
    FakeFileSystem fs;
    bool exists = fs.exists_file("file");
    EXPECT_FALSE(exists);
}

TEST_F(FakeFileSystemTest, CreatedFile) {
    FakeFileSystem fs;
    bool created = fs.create_file("file");
    bool exists = fs.exists_file("file");
    EXPECT_TRUE(exists);
    EXPECT_TRUE(created);
}

TEST_F(FakeFileSystemTest, CreatedFileFails) {
    FakeFileSystem fs;
    fs.create_file("file");
    bool created = fs.create_file("file");
    EXPECT_FALSE(created);
}

TEST_F(FakeFileSystemTest, CreateDirectoriesFromPath) {
    FakeFileSystem fs;
    fs.create_file("root/clothing/hats/beanie");
    EXPECT_TRUE(fs.exists_directory(""));
    EXPECT_TRUE(fs.exists_directory("root"));
    EXPECT_TRUE(fs.exists_directory("root/clothing"));
    EXPECT_TRUE(fs.exists_directory("root/clothing/hats"));
}

TEST_F(FakeFileSystemTest, FileInitiallyEmpty) {
    FakeFileSystem fs;
    fs.create_file("test");
    std::string contents = fs.read_file("test");
    EXPECT_EQ(contents, "");
}

TEST_F(FakeFileSystemTest, WriteFileNotFound) {
    FakeFileSystem fs;
    bool written = fs.write_file("file", "payload");
    EXPECT_FALSE(written);
}

TEST_F(FakeFileSystemTest, WriteFile) {
    FakeFileSystem fs;
    fs.create_file("file");
    bool written = fs.write_file("file", "payload");
    std::string contents = fs.read_file("file");
    EXPECT_TRUE(written);
    EXPECT_EQ(contents, "payload");
}

TEST_F(FakeFileSystemTest, ExistsDirectory) {
    FakeFileSystem fs;
    bool created = fs.create_directory("directory");
    bool exists = fs.exists_directory("directory");
    EXPECT_TRUE(created);
    EXPECT_TRUE(exists);
}

TEST_F(FakeFileSystemTest, CreateDirectoryFails) {
    FakeFileSystem fs;
    fs.create_directory("directory");
    bool created = fs.create_directory("directory");
    EXPECT_FALSE(created);
}

TEST_F(FakeFileSystemTest, ExistsDirectoryFails) {
    FakeFileSystem fs;
    bool exists = fs.exists_directory("directory");
    EXPECT_FALSE(exists);
}