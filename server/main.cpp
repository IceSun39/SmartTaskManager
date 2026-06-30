#include "crow.h"
#include "DatabaseManager.h"
#include "../../json.hpp"
#include "PasswordHasher.h"
#include <memory>
#include <random>
#include <string>

using json = nlohmann::json;

// Генерація токена
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

int authenticateUser(const crow::request& req, const std::shared_ptr<DatabaseManager>& db_manager) {
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

crow::response makeJsonMessage(int statusCode, const std::string& status, const std::string& message) {
    json j_error;
    j_error["status"] = status;
    j_error["message"] = message;

    crow::response response(j_error.dump());
    response.code = statusCode;
    return response;
}

int main() {
    crow::SimpleApp app;

    auto db_manager = std::make_shared<DatabaseManager>("database.db");
    db_manager->initializeDatabase();

    // Завантаження головної сторінки
    CROW_ROUTE(app, "/") ([] () {
        auto page = crow::mustache::load("index.html");
        return page.render();
    });

    // Реєстрація
    CROW_ROUTE(app, "/api/v1/auth/register").methods(crow::HTTPMethod::POST)([&db_manager](const crow::request& req ) {
        try {
            json j = json::parse(req.body);
            std::string username = j["username"].get<std::string>();
            std::string password = j["password"].get<std::string>();
            std::string hashedPassword = PasswordHasher::hashPassword(password);

            bool isCreated = db_manager->createUser(username, hashedPassword);

            if (!isCreated) {
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
    CROW_ROUTE(app, "/api/v1/auth/login").methods(crow::HTTPMethod::POST)([&db_manager](const crow::request& req ) {
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

    // Створити таску
    CROW_ROUTE(app, "/api/v1/tasks").methods(crow::HTTPMethod::POST)([&db_manager](const crow::request& req ) {
        try {
            int userId = authenticateUser(req, db_manager);

            json j = json::parse(req.body);
            std::string title = j["title"].get<std::string>();
            std::string description = j["description"].get<std::string>();

            db_manager->createTask(userId, title, description);

            return makeJsonMessage(201, "Success", "Task created");

        } catch (const std::invalid_argument& e) {
            // Помилка парсингу токена
            return makeJsonMessage(401, "Error", e.what());
        } catch (const std::runtime_error& e) {
            // Помилка бази (неіснуючий юзер/токен)
            return makeJsonMessage(401, "Error", e.what());
        } catch (const std::exception& e) {
            // Помилка JSON
            return makeJsonMessage(400, "Error", "Invalid JSON");
        }
    });

    // Отримати масив тасок
    CROW_ROUTE(app, "/api/v1/tasks").methods(crow::HTTPMethod::GET)([&db_manager](const crow::request& req) {
        try {
            // Авторизація
            int userId = authenticateUser(req, db_manager);

            // Отримання даних
            json allTasks = db_manager->getAllTasksForUserId(userId);

            // Успішна відповідь
            crow::response response(allTasks.dump());
            response.code = 200;
            return response;

        } catch (const std::invalid_argument& e) {
            // Ловимо помилки формату токена
            return makeJsonMessage(401, "Error", e.what());

        } catch (const std::runtime_error& e) {
            // Ловимо помилки неіснуючого юзера
            return makeJsonMessage(401, "Error", e.what());

        } catch (const std::exception& e) {
            // Глобальний відлов непередбачуваних помилок (наприклад, краш БД)
            return makeJsonMessage(500, "Error", "Internal Server Error");
        }
    });

    // Видалити таску
    CROW_ROUTE(app, "/api/v1/tasks").methods(crow::HTTPMethod::DELETE)([&db_manager](const crow::request& req) {
        try {
           int userId = authenticateUser(req, db_manager);

            std::string title = json::parse(req.body)["title"].get<std::string>();
            bool isDeleted = db_manager->deleteTask(userId, title);

            if (isDeleted) {
                return makeJsonMessage(200, "Success", "Task deleted");
            }
            else {
                return makeJsonMessage(400, "Error", "Cannot delete task");
            }
        }catch (const std::invalid_argument& e) {
            // Ловимо помилки формату токена
            return makeJsonMessage(401, "Error", e.what());
        }catch (const std::runtime_error& e) {
            // Ловимо помилки неіснуючого юзера
            return makeJsonMessage(401, "Error", e.what());
        }catch (const std::exception& e) {
            // Глобальний відлов непередбачуваних помилок (наприклад, краш БД)
            return makeJsonMessage(500, "Error","Internal Server Error") ;
        }
    });

    // Зміна статусу задачі
    CROW_ROUTE(app, "/api/v1/tasks/status").methods(crow::HTTPMethod::PATCH)([&db_manager](const crow::request& req) {
        try {
            int userId = authenticateUser(req, db_manager);

            json j = json::parse(req.body);
            std::string title = j["title"].get<std::string>();
            std::string description = j["description"].get<std::string>();

            if (db_manager->updateTaskStatus(userId, title, description)) {
                return makeJsonMessage(200, "Success", "Task updated");
            }
            else {
                return makeJsonMessage(400, "Error", e.what());
            }
        } catch (const std::invalid_argument& e) {
            return makeJsonMessage(401, "Error", e.what());
        } catch (const std::runtime_error& e) {
            return makeJsonMessage(401, "Error", e.what());
        } catch (const std::exception& e) {
            return makeJsonMessage(400, "Error", "Invalid JSON");
        }
    });

    app.port(8080).multithreaded().run();
}