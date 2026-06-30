#include "crow.h"
#include "DatabaseManager.h"
#include "AuthController.h"
#include "TaskController.h"
#include <memory>

int main() {
    crow::SimpleApp app;

    // Ініціалізація бази даних
    auto db_manager = std::make_shared<DatabaseManager>("database.db");
    db_manager->initializeDatabase();

    // Завантаження головної сторінки (Фронтенд)
    CROW_ROUTE(app, "/") ([] () {
        auto page = crow::mustache::load("index.html");
        return page.render();
    });

    // Підключення роутів із контролерів
    setupAuthRoutes(app, db_manager);
    setupTaskRoutes(app, db_manager);

    // Запуск сервера
    app.port(8080).multithreaded().run();
}