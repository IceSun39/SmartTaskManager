#pragma once
#include "crow.h"
#include "DatabaseManager.h"
#include "Utils.h"

inline void setupTaskRoutes(crow::SimpleApp& app, std::shared_ptr<DatabaseManager> db_manager) {
    // Створити таску
    CROW_ROUTE(app, "/api/v1/tasks").methods(crow::HTTPMethod::POST)([db_manager](const crow::request& req ) {
        try {
            int userId = authenticateUser(req, db_manager);
            json j = json::parse(req.body);
            std::string title = j["title"].get<std::string>();
            std::string description = j["description"].get<std::string>();

            db_manager->createTask(userId, title, description);
            return makeJsonMessage(201, "Success", "Task created");
        } catch (const std::exception& e) {
            return makeJsonMessage(400, "Error", e.what());
        }
    });

    // Отримати всі таски
    CROW_ROUTE(app, "/api/v1/tasks").methods(crow::HTTPMethod::GET)([db_manager](const crow::request& req) {
        try {
            int userId = authenticateUser(req, db_manager);
            json allTasks = db_manager->getAllTasksForUserId(userId);

            crow::response response(allTasks.dump());
            response.code = 200;
            return response;
        } catch (const std::exception& e) {
            return makeJsonMessage(401, "Error", e.what());
        }
    });

    // Видалити таску
    CROW_ROUTE(app, "/api/v1/tasks").methods(crow::HTTPMethod::DELETE)([db_manager](const crow::request& req) {
            try {
                int userId = authenticateUser(req, db_manager);
                int taskId = json::parse(req.body)["id"].get<int>(); // Читаємо ID

                if (db_manager->deleteTask(userId, taskId)) {
                    return makeJsonMessage(200, "Success", "Task deleted");
                } else {
                    return makeJsonMessage(400, "Error", "Cannot delete task");
                }
            } catch (const std::exception& e) {
                return makeJsonMessage(400, "Error", e.what());
            }
        });

    // Оновити статус таски
    CROW_ROUTE(app, "/api/v1/tasks/status").methods(crow::HTTPMethod::PATCH)([db_manager](const crow::request& req) {
            try {
                int userId = authenticateUser(req, db_manager);
                json j = json::parse(req.body);
                int taskId = j["id"].get<int>(); // Читаємо ID
                std::string status = j["status"].get<std::string>();

                if (db_manager->updateTaskStatus(userId, taskId, status)) {
                    return makeJsonMessage(200, "Success", "Task updated");
                } else {
                    return makeJsonMessage(400, "Error", "Cannot update task");
                }
            } catch (const std::exception& e) {
                return makeJsonMessage(400, "Error", e.what());
            }
        });
}