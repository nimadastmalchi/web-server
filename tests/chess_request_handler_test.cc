#include "chess_request_handler.h"

#include <boost/regex.hpp>
#include <iostream>
#include <memory>

#include "file_system.h"
#include "gmock/gmock.h"
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

TEST_F(ChessRequestHandlerTest, HandleBadPostPath) {
    http::request<http::string_body> req{http::verb::post, "test/game/new", 11};
    http::response<http::string_body> res;
    handler_.handle_request(req, res);

    EXPECT_EQ(res.result(), http::status::bad_request);
}

TEST_F(ChessRequestHandlerTest, HandleValidPost) {
    http::request<http::string_body> req{http::verb::post, "test/new", 11};
    http::response<http::string_body> res;

    handler_.handle_request(req, res);
    EXPECT_EQ(res.result(), http::status::created);

    boost::smatch game_id;
    EXPECT_TRUE(boost::regex_match(res.body(), game_id,
                                   boost::regex("{\"id\": ([0-9]+)}")));

    std::string file_body = file_system_->read_file("/" + game_id[1]).value();
    EXPECT_EQ(file_body, "\n\nrnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR\n");
}
