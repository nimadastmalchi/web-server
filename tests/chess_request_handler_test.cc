#include "chess_request_handler.h"

#include <boost/regex.hpp>
#include <iostream>
#include <memory>

#include "file_system.h"
#include "gtest/gtest.h"

using namespace boost::beast;

class ChessRequestHandlerTest : public ::testing::Test {
    protected:
        void SetUp() override {}

        std::shared_ptr<FakeFileSystem> file_system_ =
            std::make_shared<FakeFileSystem>();
        ChessRequestHandler handler_ =
            ChessRequestHandler("test", "", "0.0.0.0", file_system_);
};

TEST_F(ChessRequestHandlerTest, BadPostPath) {
    http::request<http::string_body> req{http::verb::post, "test/game/new", 11};
    http::response<http::string_body> res;
    handler_.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::bad_request);
}

TEST_F(ChessRequestHandlerTest, ValidPost) {
    http::request<http::string_body> req{http::verb::post, "test/new", 11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::created);

    boost::smatch game_id;
    EXPECT_TRUE(boost::regex_match(res.body(), game_id,
                                   boost::regex("{\"id\": ([0-9]+)}")));

    std::string file_body = file_system_->read_file("/" + game_id[1]).value();
    EXPECT_EQ(file_body, "\n\nrnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR\n\n");
}

TEST_F(ChessRequestHandlerTest, BadGetPath) {
    http::request<http::string_body> req{http::verb::get,
                                         "test/too/many/directories", 11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::bad_request);
}

TEST_F(ChessRequestHandlerTest, InvalidGetID) {
    http::request<http::string_body> req{http::verb::get, "test/0", 11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::not_found);
}

TEST_F(ChessRequestHandlerTest, ValidGetID) {
    file_system_->write_file("/0", "test\ntest\ntest\ntest");
    file_system_->write_file("/chess130/chessboard.html", "test");
    http::request<http::string_body> req{http::verb::get, "test/0", 11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.body(), "test");
    EXPECT_EQ(res.result(), http::status::ok);
}

TEST_F(ChessRequestHandlerTest, UnsupportedGetPath) {
    http::request<http::string_body> req{http::verb::get, "test/unsupported/0",
                                         11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::bad_request);
}

TEST_F(ChessRequestHandlerTest, InvalidGetGamesID) {
    http::request<http::string_body> req{http::verb::get, "test/games/0", 11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::not_found);
}

TEST_F(ChessRequestHandlerTest, InvalidFileContent) {
    file_system_->write_file("/0", "test");
    http::request<http::string_body> req{http::verb::get, "test/games/0", 11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_FALSE(file_system_->read_file("/0"));
    EXPECT_EQ(res.result(), http::status::not_found);
}

TEST_F(ChessRequestHandlerTest, AddNewPlayer) {
    file_system_->write_file("/0", "\n\ntest\n\n");
    http::request<http::string_body> req{http::verb::get, "test/games/0", 11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.body(),
              "{\"role\": \"w\", \"fen\": \"test\", \"last_move\": \"\"}");
    EXPECT_EQ(res.result(), http::status::ok);

    std::string file_body = file_system_->read_file("/0").value();
    EXPECT_EQ(file_body, "0.0.0.0\n\ntest\n\n");
}

TEST_F(ChessRequestHandlerTest, GetExistingPlayer) {
    file_system_->write_file("/0", "0.0.0.0\n1.1.1.1\ntest\n\n");
    http::request<http::string_body> req{http::verb::get, "test/games/0", 11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.body(),
              "{\"role\": \"w\", \"fen\": \"test\", \"last_move\": \"\"}");
    EXPECT_EQ(res.result(), http::status::ok);

    std::string file_body = file_system_->read_file("/0").value();
    EXPECT_EQ(file_body, "0.0.0.0\n1.1.1.1\ntest\n\n");
}

TEST_F(ChessRequestHandlerTest, GetViewer) {
    file_system_->write_file("/0", "1.1.1.1\n2.2.2.2\ntest\n\n");
    http::request<http::string_body> req{http::verb::get, "test/games/0", 11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.body(),
              "{\"role\": \"v\", \"fen\": \"test\", \"last_move\": \"\"}");
    EXPECT_EQ(res.result(), http::status::ok);

    std::string file_body = file_system_->read_file("/0").value();
    EXPECT_EQ(file_body, "1.1.1.1\n2.2.2.2\ntest\n\n");
}

TEST_F(ChessRequestHandlerTest, GetHomePage) {
    file_system_->write_file("/chess130/home.html", "test");

    http::request<http::string_body> req{http::verb::get, "test", 11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.body(), "test");
    EXPECT_EQ(res.result(), http::status::ok);
}

TEST_F(ChessRequestHandlerTest, HomePageNotFound) {
    http::request<http::string_body> req{http::verb::get, "test", 11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::not_found);
}

TEST_F(ChessRequestHandlerTest, PutNewFenString) {
    file_system_->write_file("/0", "0.0.0.0\n2.2.2.2\ntest\n\n");

    // Update fen string:
    http::request<http::string_body> req{http::verb::put, "test/games/0", 11};
    req.body() = "new_test\ntest";
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::ok);

    // Ensure fen string is updated:
    std::string file_body = file_system_->read_file("/0").value();
    EXPECT_EQ(file_body, "0.0.0.0\n2.2.2.2\nnew_test\ntest\n");
}

TEST_F(ChessRequestHandlerTest, InvalidPutRequestURI) {
    file_system_->write_file("/0", "0.0.0.0\n2.2.2.2\ntest\n");

    http::request<http::string_body> req{http::verb::put, "test/0", 11};
    req.body() = "new_test";
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::not_found);
}

TEST_F(ChessRequestHandlerTest, PutRequestToNonExistentFile) {
    http::request<http::string_body> req{http::verb::put, "test/games/0", 11};
    req.body() = "new_test";
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::not_found);
}
