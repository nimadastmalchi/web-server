// An nginx config file parser.
//
// See:
//   http://wiki.nginx.org/Configuration
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/
//
// How Nginx does it:
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>

#include "config_parser.h"

std::string NginxConfig::ToString(int depth) {
    std::string serialized_config;
    for (const auto& statement : statements_) {
        serialized_config.append(statement->ToString(depth));
    }
    return serialized_config;
}

std::string NginxConfigStatement::ToString(int depth) {
    std::string serialized_statement;
    for (int i = 0; i < depth; ++i) {
        serialized_statement.append("  ");
    }
    for (unsigned int i = 0; i < tokens_.size(); ++i) {
        if (i != 0) {
            serialized_statement.append(" ");
        }
        serialized_statement.append(tokens_[i]);
    }
    if (child_block_.get() != nullptr) {
        serialized_statement.append(" {\n");
        serialized_statement.append(child_block_->ToString(depth + 1));
        for (int i = 0; i < depth; ++i) {
            serialized_statement.append("  ");
        }
        serialized_statement.append("}");
    } else {
        serialized_statement.append(";");
    }
    serialized_statement.append("\n");
    return serialized_statement;
}

// https://github.com/JonnyKong/UCLA-CS130-Software-Engineering/blob/master/Assignment2_Echo_Server/src/config_parser.cc
int NginxConfig::getPort() {
    // First traverse statements without child blocks
    for (auto pStatement : statements_) {
        if (pStatement->child_block_.get() == nullptr) {
            if (pStatement->tokens_.size() == 2 && pStatement->tokens_[0] == "port") {
                int ret = std::atoi(pStatement->tokens_[1].c_str());
                return (ret >= 0 && ret <= 0xffff) ? ret : -1;    // Valid port range
            }
        }
    }
    // Then traverse statements with child blocks
    for (auto pStatement : statements_) {
        if (pStatement->child_block_.get() != nullptr) {
            int ret;
            if ((ret = pStatement -> child_block_ -> getPort()) != -1)
                return ret;
        }
    }
    return -1;  // Ret type should be int to cover -1
}

// Return a vector of LocationBlock structs, representing the location blocks
// in the server block in the config file.
std::vector<LocationBlock> NginxConfig::getLocationBlocks() {
    std::vector<LocationBlock> blocks;
    for (auto statement : statements_) {
        if (statement->tokens_.size() == 1 &&
            statement->tokens_[0] == "server") {
            for (auto child_statement : statement->child_block_->statements_) {
                if (child_statement->tokens_.size() < 3) {
                    continue;
                }
                if (child_statement->tokens_[0] != "location") {
                    continue;
                }
                LocationBlock block;
                block.uri_ = child_statement->tokens_[1];
                block.handler_ = child_statement->tokens_[2];
                block.root_ = "/";
                for (auto child_child_statement : child_statement->child_block_->statements_) {
                    if (child_child_statement->tokens_.size() == 2 &&
                        child_child_statement->tokens_[0] == "root") {
                        block.root_ = child_child_statement->tokens_[1];
                        break;
                    }
                }
                blocks.push_back(std::move(block));
            }
        }
        break;
    }
    return std::move(blocks);
}

const char* NginxConfigParser::TokenTypeAsString(TokenType type) {
    switch (type) {
        case TOKEN_TYPE_START:         return "TOKEN_TYPE_START";
        case TOKEN_TYPE_NORMAL:        return "TOKEN_TYPE_NORMAL";
        case TOKEN_TYPE_START_BLOCK:   return "TOKEN_TYPE_START_BLOCK";
        case TOKEN_TYPE_END_BLOCK:     return "TOKEN_TYPE_END_BLOCK";
        case TOKEN_TYPE_COMMENT:       return "TOKEN_TYPE_COMMENT";
        case TOKEN_TYPE_STATEMENT_END: return "TOKEN_TYPE_STATEMENT_END";
        case TOKEN_TYPE_EOF:           return "TOKEN_TYPE_EOF";
        case TOKEN_TYPE_ERROR:         return "TOKEN_TYPE_ERROR";
        case TOKEN_TYPE_QUOTED_STRING: return "TOKEN_TYPE_QUOTED_STRING";
        default:                       return "Unknown token type";
    }
}

NginxConfigParser::TokenType NginxConfigParser::ParseToken(std::istream* input, std::string* value) {
    TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;
    while (input->good()) {
        const char c = input->get();
        if (!input->good()) {
            break;
        }
        switch (state) {
            case TOKEN_STATE_INITIAL_WHITESPACE:
                switch (c) {
                    case '{':
                        *value = c;
                        return TOKEN_TYPE_START_BLOCK;
                    case '}':
                        *value = c;
                        return TOKEN_TYPE_END_BLOCK;
                    case '#':
                        *value = c;
                        state = TOKEN_STATE_TOKEN_TYPE_COMMENT;
                        continue;
                    case '"':
                        *value = c;
                        state = TOKEN_STATE_DOUBLE_QUOTE;
                        continue;
                    case '\'':
                        *value = c;
                        state = TOKEN_STATE_SINGLE_QUOTE;
                        continue;
                    case ';':
                        *value = c;
                        return TOKEN_TYPE_STATEMENT_END;
                    case ' ':
                    case '\t':
                    case '\n':
                    case '\r':
                        continue;
                    default:
                        *value += c;
                        state = TOKEN_STATE_TOKEN_TYPE_NORMAL;
                        continue;
                }
            case TOKEN_STATE_SINGLE_QUOTE:
                *value += c;
                if (c == '\\') {
                    state = TOKEN_STATE_ESCAPED_SINGLE_QUOTE;
                }
                else if (c == '\'') {
                    // Check for whitespace or semicolon after single quote
                    const char next_c = input->get();
                    switch (next_c) {
                        case ' ':
                        case '\t':
                        case '\n':
                        case '\r':
                        case ';':
                            input->unget();
                            return TOKEN_TYPE_NORMAL;
                        default:
                            return TOKEN_TYPE_ERROR;
                     }
                }
                continue;
            case TOKEN_STATE_DOUBLE_QUOTE:
                *value += c;
                if (c == '\\') {
                    state = TOKEN_STATE_ESCAPED_DOUBLE_QUOTE;
                }
                else if (c == '"') {
                    // Same logic as single quote case
                    const char next_c = input->get();
                    switch (next_c) {
                        case ' ':
                        case '\t':
                        case '\n':
                        case '\r':
                        case ';':
                            input->unget();
                            return TOKEN_TYPE_NORMAL;
                        default:
                            return TOKEN_TYPE_ERROR;
                    }
                }
                continue;
            case TOKEN_STATE_TOKEN_TYPE_COMMENT:
                if (c == '\n' || c == '\r') {
                    return TOKEN_TYPE_COMMENT;
                }
                *value += c;
                continue;
            case TOKEN_STATE_TOKEN_TYPE_NORMAL:
                if (c == ' ' || c == '\t' || c == '\n' || c == '\t' ||
                    c == ';' || c == '{' || c == '}') {
                    input->unget();
                    return TOKEN_TYPE_NORMAL;
                }
                *value += c;
                continue;
            case TOKEN_STATE_ESCAPED_SINGLE_QUOTE:
                *value += c;
                state = TOKEN_STATE_SINGLE_QUOTE;
                continue;
            case TOKEN_STATE_ESCAPED_DOUBLE_QUOTE:
                *value += c;
                state = TOKEN_STATE_DOUBLE_QUOTE;
                continue;
        }
    }

    // If we get here, we reached the end of the file.
    if (state == TOKEN_STATE_SINGLE_QUOTE ||
        state == TOKEN_STATE_DOUBLE_QUOTE) {
        return TOKEN_TYPE_ERROR;
    }

    return TOKEN_TYPE_EOF;
}

bool NginxConfigParser::Parse(std::istream* config_file, NginxConfig* config) {
    std::stack<NginxConfig*> config_stack;
    config_stack.push(config);
    TokenType last_token_type = TOKEN_TYPE_START;
    TokenType token_type;
    while (true) {
        std::string token;
        token_type = ParseToken(config_file, &token);
        printf ("%s: %s\n", TokenTypeAsString(token_type), token.c_str());
        if (token_type == TOKEN_TYPE_ERROR) {
            break;
        }

        if (token_type == TOKEN_TYPE_COMMENT) {
            // Skip comments.
            continue;
        }

        if (token_type == TOKEN_TYPE_START) {
            // Error.
            break;
        } else if (token_type == TOKEN_TYPE_NORMAL ||
                   token_type == TOKEN_TYPE_QUOTED_STRING) {
            if (last_token_type == TOKEN_TYPE_START ||
                last_token_type == TOKEN_TYPE_STATEMENT_END ||
                last_token_type == TOKEN_TYPE_START_BLOCK ||
                last_token_type == TOKEN_TYPE_END_BLOCK ||
                last_token_type == TOKEN_TYPE_NORMAL ||
                last_token_type == TOKEN_TYPE_QUOTED_STRING) {
                if (last_token_type != TOKEN_TYPE_NORMAL) {
                    config_stack.top()->statements_.emplace_back(
                    new NginxConfigStatement);
                }
                config_stack.top()->statements_.back().get()->tokens_.push_back(token);
            } else {
                // Error.
                break;
            }
        } else if (token_type == TOKEN_TYPE_STATEMENT_END) {
            if (last_token_type != TOKEN_TYPE_NORMAL &&
                last_token_type != TOKEN_TYPE_QUOTED_STRING) {
                // Error.
                break;
            }
        } else if (token_type == TOKEN_TYPE_START_BLOCK) {
            if (last_token_type != TOKEN_TYPE_NORMAL) {
                // Error.
                break;
            }
            NginxConfig* const new_config = new NginxConfig;
            config_stack.top()->statements_.back().get()->child_block_.reset(new_config);
            config_stack.push(new_config);
        } else if (token_type == TOKEN_TYPE_END_BLOCK) {
            if (last_token_type != TOKEN_TYPE_STATEMENT_END &&
                last_token_type != TOKEN_TYPE_START_BLOCK &&
                last_token_type != TOKEN_TYPE_END_BLOCK) {
                // Error.
                break;
            }
            if (config_stack.size() <= 1) {
                // Error
                break;
            }
            config_stack.pop();
        } else if (token_type == TOKEN_TYPE_EOF) {
            if ((last_token_type != TOKEN_TYPE_STATEMENT_END &&
                last_token_type != TOKEN_TYPE_END_BLOCK &&
                last_token_type != TOKEN_TYPE_START) ||
                config_stack.size() != 1) {
                // Error.
                break;
            }
            return true;
        } else {
            // Error. Unknown token.
            break;
        }
        last_token_type = token_type;
    }

    printf ("Bad transition from %s to %s\n",
            TokenTypeAsString(last_token_type),
            TokenTypeAsString(token_type));
    return false;
}

bool NginxConfigParser::Parse(const char* file_name, NginxConfig* config) {
    std::ifstream config_file;
    config_file.open(file_name);
    if (!config_file.good()) {
        printf ("Failed to open config file: %s\n", file_name);
        return false;
    }

    const bool return_value = Parse(dynamic_cast<std::istream*>(&config_file), config);
    config_file.close();
    return return_value;
}
