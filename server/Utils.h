#pragma once
#include "crow.h"
#include "DatabaseManager.h"
#include "../../json.hpp"
#include <memory>
#include <random>
#include <string>

using json = nlohmann::json;

inline std::string generateBearerToken(size_t length = 32) {
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::string token;
    for (size_t i = 0; i < length; ++i) {
        token += characters[distribution(generator)];
    }
    return token;
}

inline int authenticateUser(const crow::request& req, const std::shared_ptr<DatabaseManager>& db_manager) {
    std::string token = req.get_header_value("Authorization");

    if (token.empty() || !token.starts_with("Bearer ")) {
        throw std::invalid_argument("Bearer token not found or invalid format");
    }

    int userId = db_manager->getUserIdByToken(token.substr(7));
    if (userId == -1) {
        throw std::runtime_error("User not logged in or session expired");
    }

    return userId;
}

inline crow::response makeJsonMessage(int statusCode, const std::string& status, const std::string& message) {
    json j_error;
    j_error["status"] = status;
    j_error["message"] = message;

    crow::response response(j_error.dump());
    response.code = statusCode;
    return response;
}