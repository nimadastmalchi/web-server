#include "crud_request_handler.h"

#include <memory>

#include "file_system.h"
#include "gtest/gtest.h"

using namespace boost::beast;

class CRUDRequestHandlerTest : public ::testing::Test {
    protected:
        void SetUp() override {}

        std::shared_ptr<FakeFileSystem> file_system_ =
            std::make_shared<FakeFileSystem>();
        CRUDRequestHandler handler_ =
            CRUDRequestHandler("test", "", file_system_);
};

TEST_F(CRUDRequestHandlerTest, BadMethod) {
    http::request<http::string_body> req{http::verb::head, "test/entity", 11};
    http::response<http::string_body> res;
    handler_.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::method_not_allowed);
}

TEST_F(CRUDRequestHandlerTest, BadRequest) {
    http::request<http::string_body> req{http::verb::get, "test", 11};
    http::response<http::string_body> res;
    handler_.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::bad_request);
}

TEST_F(CRUDRequestHandlerTest, HandlePost) {
    http::request<http::string_body> req{http::verb::post, "test/entity", 11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.body(), "{\"id\": 1}");

    handler_.handle_request(req, res);
    EXPECT_EQ(res.body(), "{\"id\": 2}");

    EXPECT_EQ(res.result(), http::status::created);
}

TEST_F(CRUDRequestHandlerTest, HandleGetNotFound) {
    http::request<http::string_body> req{http::verb::get, "test/entity/1", 11};
    http::response<http::string_body> res;
    handler_.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::not_found);
}

TEST_F(CRUDRequestHandlerTest, HandleGet) {
    file_system_->write_file("/entity/1", "test");

    http::request<http::string_body> req{http::verb::get, "test/entity/1", 11};
    http::response<http::string_body> res;
    handler_.handle_request(req, res);

    EXPECT_EQ(res.body(), "test");
    EXPECT_EQ(res.result(), http::status::ok);
}

TEST_F(CRUDRequestHandlerTest, ListTest) {
    file_system_->write_file("/entity/1", "");
    file_system_->write_file("/entity/2", "");

    http::request<http::string_body> req{http::verb::get, "test/entity", 11};
    http::response<http::string_body> res;
    handler_.handle_request(req, res);

    EXPECT_TRUE(res.body() == "[1, 2]" || res.body() == "[2, 1]");
    EXPECT_EQ(res.result(), http::status::ok);
}

TEST_F(CRUDRequestHandlerTest, PutMethodExistingFile) {
    file_system_->write_file("/entity/1", "");

    http::request<http::string_body> req{http::verb::put, "test/entity/1", 11};
    req.body() = "test";
    http::response<http::string_body> res;

    std::string before_content = file_system_->read_file("/entity/1").value();
    handler_.handle_request(req, res);
    std::string after_content = file_system_->read_file("/entity/1").value();

    EXPECT_EQ(before_content, "");
    EXPECT_EQ(after_content, "test");
    EXPECT_EQ(res.result(), http::status::created);
}

TEST_F(CRUDRequestHandlerTest, PutMethodNonexistentFile) {
    http::request<http::string_body> req{http::verb::put, "test/entity/50", 11};
    req.body() = "test";
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::created);

    req.method(http::verb::get);
    handler_.handle_request(req, res);

    EXPECT_EQ(res.body(), "test");
    EXPECT_EQ(res.result(), http::status::ok);
}

TEST_F(CRUDRequestHandlerTest, DeleteMethodNotFound) {
    http::request<http::string_body> req{http::verb::delete_, "entity/100", 11};
    http::response<http::string_body> res;
    handler_.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::not_found);
}

TEST_F(CRUDRequestHandlerTest, DeleteMethodSuccess) {
    file_system_->write_file("/entity/1", "");

    http::request<http::string_body> req{http::verb::delete_, "test/entity/1",
                                         11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::no_content);

    handler_.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::not_found);
}
