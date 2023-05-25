#include "entity_manager.h"
#include "fake_file_system.h"
#include "gtest/gtest.h"

class EntityManagerTest : public ::testing::Test {
    void SetUp() {
        fs = new FakeFileSystem();
    }
    void TearDown() {
        delete fs;
    }
    protected:
        FakeFileSystem* fs;
};

TEST_F(EntityManagerTest, InsertBadPath) {
    EntityManager eman(fs);
    
    bool success = true;
    boost::beast::http::request<boost::beast::http::string_body> request;
    request.target("/shoes");
    boost::beast::http::response<boost::beast::http::string_body> response;
    try {
        eman.insert("bad path", "crud_files", request, response);
    }
    catch (...) {   
        success = false;
    }
    
    EXPECT_FALSE(success);
}

TEST_F(EntityManagerTest, InsertFirstShoe) {
    EntityManager eman(fs);

    boost::beast::http::request<boost::beast::http::string_body> request;
    request.target("/shoes");
    boost::beast::http::response<boost::beast::http::string_body> response;

    bool inserted = eman.insert("", "crud_files", request, response);
    EXPECT_TRUE(inserted);
    EXPECT_EQ(response.result(), boost::beast::http::status::created);
    EXPECT_EQ(response.body(), "{\"id\": 1}");
}

TEST_F(EntityManagerTest, GetEntityFirstShoe) {
    fs->create_file("crud_files/shoes/1");
    fs->write_file("crud_files/shoes/1", "test payload");
    EntityManager eman(fs);

    boost::beast::http::request<boost::beast::http::string_body> request;
    request.target("/shoes/1");
    boost::beast::http::response<boost::beast::http::string_body> response;

    bool inserted = eman.get_entity("", "crud_files", request, response);
    EXPECT_TRUE(inserted);
    EXPECT_EQ(response.result(), boost::beast::http::status::ok);
    EXPECT_EQ(response.body(), "test payload");
}

TEST_F(EntityManagerTest, GetEntityNotFound) {
    EntityManager eman(fs);

    boost::beast::http::request<boost::beast::http::string_body> request;
    request.target("/shoes/1");
    boost::beast::http::response<boost::beast::http::string_body> response;

    bool inserted = eman.get_entity("", "crud_files", request, response);
    EXPECT_EQ(response.result(), boost::beast::http::status::not_found);
}

TEST_F(EntityManagerTest, GetEntityBadPath) {
    fs->create_file("crud_files/shoes/1");
    fs->write_file("crud_files/shoes/1", "test payload");
    EntityManager eman(fs);

    bool success = true;
    boost::beast::http::request<boost::beast::http::string_body> request;
    request.target("/shoes");
    boost::beast::http::response<boost::beast::http::string_body> response;

    try {
        eman.get_entity("bad path", "crud_files", request, response);
    }
    catch (...) {   
        success = false;
    }
    
    EXPECT_FALSE(success);
}

TEST_F(EntityManagerTest, ListTest) {
    EntityManager eman(fs);

    boost::beast::http::request<boost::beast::http::string_body> request;
    request.target("/shoes");
    boost::beast::http::response<boost::beast::http::string_body> response;

    eman.insert("", "crud_files", request, response);
    eman.insert("", "crud_files", request, response);
    eman.insert("", "crud_files", request, response);

    EXPECT_EQ(fs->read_file("crud_files/shoes/list"), "[1,2,3]");
}