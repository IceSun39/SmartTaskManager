#pragma once
#include "crow.h"
#include "DatabaseManager.h"
#include "PasswordHasher.h"
#include "Utils.h"

inline void setupAuthRoutes(crow::SimpleApp& app, std::shared_ptr<DatabaseManager> db_manager) {
    // Реєстрація
    CROW_ROUTE(app, "/api/v1/auth/register").methods(crow::HTTPMethod::POST)([db_manager](const crow::request& req ) {
        try {
            json j = json::parse(req.body);
            std::string username = j["username"].get<std::string>();
            std::string password = j["password"].get<std::string>();
            std::string hashedPassword = PasswordHasher::hashPassword(password);

            if (!db_manager->createUser(username, hashedPassword)) {
                return makeJsonMessage(400, "Error","User already exists");
            }

            json j_success;
            j_success["status"] = "Success";
            j_success["message"] = "User created successfully";
            crow::response response(j_success.dump());
            response.code = 201;
            return response;
        } catch (const std::exception& e) {
            return makeJsonMessage(400, "Error", "Invalid JSON");
        }
    });

    // Логін
    CROW_ROUTE(app, "/api/v1/auth/login").methods(crow::HTTPMethod::POST)([db_manager](const crow::request& req ) {
        try {
            json j = json::parse(req.body);
            std::string username = j["username"].get<std::string>();
            std::string password = j["password"].get<std::string>();
            std::string hashedPassword = db_manager->getPasswordHash(username);

            if (hashedPassword.empty() || !PasswordHasher::verifyPassword(password, hashedPassword)) {
                return makeJsonMessage(401, "Error", "Invalid username or password");
            }

            int userId = db_manager->getUserId(username);
            std::string token = generateBearerToken();

            db_manager->createSession(userId, token);

            json j_success;
            j_success["status"] = "Success";
            j_success["message"] = "User logged in";
            j_success["token"] = token;

            crow::response response(j_success.dump());
            response.code = 200;
            return response;
        } catch (const std::exception& e) {
            return makeJsonMessage(400, "Error", "Invalid JSON");
        }
    });

    // Логаут
    CROW_ROUTE(app, "/api/v1/auth/logout").methods(crow::HTTPMethod::POST)([db_manager](const crow::request& req ) {
        try {
            std::string auth_header = req.get_header_value("Authorization");
            if (auth_header.empty() || !auth_header.starts_with("Bearer ")) {
                return makeJsonMessage(401, "Error", "Invalid or missing token");
            }

            std::string token = auth_header.substr(7);
            if (db_manager->deleteSession(token)) {
                return makeJsonMessage(200, "Success", "Session closed");
            } else {
                return makeJsonMessage(400, "Error", "Cannot close session");
            }
        } catch (const std::exception& e) {
            return makeJsonMessage(400, "Error", "Invalid JSON or server error");
        }
    });
}