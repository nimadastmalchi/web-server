#include "crud_request_handler.h"
#include "entity_manager.h"
#include "fake_file_system.h"
#include "gtest/gtest.h"

using namespace boost::beast;

class CRUDRequestHandlerTest : public ::testing::Test {
    void SetUp() {
        fs = new FakeFileSystem();
    }
    void TearDown() {
        delete fs;
    }
    protected:
        FakeFileSystem* fs;
};

TEST_F(CRUDRequestHandlerTest, BadMethod) {
    EntityManager eman(fs);
    CRUDRequestHandler handler_ = CRUDRequestHandler("", "crud_files", eman);

    http::request<http::string_body> req;
    req.method(http::verb::head);
    req.target("test");
    http::response<http::string_body> res;
    handler_.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::method_not_allowed);
}

TEST_F(CRUDRequestHandlerTest, BadRequest) {
    EntityManager eman(fs);
    CRUDRequestHandler handler_ = CRUDRequestHandler("", "crud_files", eman);

    http::request<http::string_body> req;
    req.method(http::verb::post);
    req.target("crud_files/hats");
    http::response<http::string_body> res;
    handler_.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::bad_request);
}

TEST_F(CRUDRequestHandlerTest, HandlePost) {
    EntityManager eman(fs);
    CRUDRequestHandler handler_ = CRUDRequestHandler("", "crud_files", eman);

    http::request<http::string_body> req;
    req.method(http::verb::post);
    req.target("/hats");
    http::response<http::string_body> res;
    handler_.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::created);
}

TEST_F(CRUDRequestHandlerTest, HandleGetNotFound) {
    EntityManager eman(fs);
    CRUDRequestHandler handler_ = CRUDRequestHandler("", "crud_files", eman);

    http::request<http::string_body> req;
    req.method(http::verb::get);
    req.target("/hats/1");
    http::response<http::string_body> res;
    handler_.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::not_found);
}

TEST_F(CRUDRequestHandlerTest, HandleGet) {
    fs->create_file("crud_files/hats/1");
    fs->write_file("crud_files/hats/1", "test payload");
    EntityManager eman(fs);
    CRUDRequestHandler handler_ = CRUDRequestHandler("", "crud_files", eman);

    boost::beast::http::request<boost::beast::http::string_body> req;
    req.method(http::verb::get);
    req.target("/hats/1");
    boost::beast::http::response<boost::beast::http::string_body> res;
    handler_.handle_request(req, res);

    EXPECT_EQ(res.result(), boost::beast::http::status::ok);
    EXPECT_EQ(res.body(), "test payload");
}

TEST_F(CRUDRequestHandlerTest, ListTest) {
    EntityManager eman(fs);
    CRUDRequestHandler handler_ = CRUDRequestHandler("", "crud_files", eman);

    boost::beast::http::request<boost::beast::http::string_body> req;
    req.method(http::verb::post);
    req.target("/hats");
    boost::beast::http::response<boost::beast::http::string_body> res;
    handler_.handle_request(req, res);
    handler_.handle_request(req, res);
    handler_.handle_request(req, res);

    EXPECT_EQ(fs->read_file("crud_files/hats/list"), "[1,2,3]");
}