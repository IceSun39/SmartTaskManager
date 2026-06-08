#include "crow.h"
#include "DatabaseManager.h"
#include "/home/vlad/CLionProjects/SmartTaskManager/json.hpp"
#include "PasswordHasher.h"
#include <memory>
#include <random>
#include <string>

using json = nlohmann::json;

std::string generateBearerToken(size_t length = 32) {
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

int main() {
    crow::SimpleApp app;

    auto db_manager = std::make_shared<DatabaseManager>("database.db");
    db_manager->initializeDatabase();

    CROW_ROUTE(app, "/") ([] () {
        auto page = crow::mustache::load("index.html");
        return page.render();
    });

    CROW_ROUTE(app, "/api/v1/auth/register")([&db_manager](const crow::request& req ) {
        try {
            json j = json::parse(req.body);
            std::string username = j["username"].get<std::string>();
            std::string password = j["password"].get<std::string>();
            std::string hashedPassword = PasswordHasher::hashPassword(password);

            bool isCreated = db_manager->createUser(username, hashedPassword);

            if (!isCreated) {
                json j_error;
                j_error["status"] = "Error";
                j_error["message"] = "User already exists";
                crow::response response(j_error.dump());
                response.code = 400;
                return response;
            }
            else {
                json j_success;
                j_success["status"] = "Success";
                j_success["message"] = "User created successfully";
                crow::response response(j_success.dump());
                response.code = 201;
                return response;
            }
        } catch (std::exception& e) {
            json j_error;
            j_error["status"] = "Error";
            j_error["message"] = "Invalid JSON";
            crow::response response(j_error.dump());
            response.code = 400;
            return response;
        }
    });


    app.port(8080).multithreaded().run();
}