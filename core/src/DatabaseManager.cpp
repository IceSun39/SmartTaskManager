#include "../include/DatabaseManager.h"

#include <iostream>
#include <signal.h>

DatabaseManager::DatabaseManager(const std::string& db_path) {
    db = std::make_unique<SQLite::Database>(db_path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
}

DatabaseManager::~DatabaseManager() = default;

// Заповнення бд
void DatabaseManager::initializeDatabase() const {
    db->exec("CREATE TABLE IF NOT EXISTS users ("
             "id INTEGER PRIMARY KEY AUTOINCREMENT, "
             "username VARCHAR(50) UNIQUE NOT NULL, "
             "password_hash TEXT NOT NULL"
             ");");

    db->exec("CREATE TABLE IF NOT EXISTS tasks ("
             "id INTEGER PRIMARY KEY AUTOINCREMENT, "
             "user_id INTEGER NOT NULL, "
             "title TEXT NOT NULL, "
             "description TEXT, "
             "status TEXT DEFAULT 'pending', "
             "CONSTRAINT fk_user_id FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
             ");");
    db->exec("CREATE TABLE IF NOT EXISTS sessions ("
         "id INTEGER PRIMARY KEY AUTOINCREMENT, "
         "user_id INTEGER NOT NULL, "
         "token TEXT UNIQUE NOT NULL, "
         "CONSTRAINT fk_session_user FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
         ");");
}

// Створення користувача
bool DatabaseManager::createUser(std::string& username, const std::string& password) {
    try {
        SQLite::Statement query(*db, "INSERT INTO users(username, password_hash) VALUES (?, ?)");
        query.bind(1, username);
        query.bind(2, password);
        query.exec();
        return true;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

//Видалення користувача
bool DatabaseManager::deleteUser(std::string& username) {
    try {
        SQLite::Statement query(*db, "DELETE FROM users WHERE username = ?");
        query.bind(1, username);
        int rowsDeleted = query.exec();
        return rowsDeleted > 0;         // якщо видалило 1 або більше — true, якщо 0 — false
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

// Створення задачі
bool DatabaseManager::createTask(int user_id, const std::string& title, const std::string& description) {
    try {
        SQLite::Statement query(*db, "INSERT INTO tasks(user_id, title, description) VALUES (?, ?, ?)");
        query.bind(1, user_id);
        query.bind(2, title);
        query.bind(3, description);
        query.exec();
        return true;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

// Видалення задачі
bool DatabaseManager::deleteTask(int user_id, int task_id) {
    try {
        SQLite::Statement query(*db, "DELETE FROM tasks WHERE user_id = ? AND id = ?");
        query.bind(1, user_id);
        query.bind(2, task_id);
        int rowsDeleted = query.exec();
        return rowsDeleted > 0;
    } catch (std::exception& e) {
        return false;
    }
}

// Зміна статусу
bool DatabaseManager::updateTaskStatus(int user_id, int task_id, const std::string& status) {
    try {
        SQLite::Statement query(*db, "UPDATE tasks SET status = ? WHERE user_id = ? AND id = ?");
        query.bind(1, status);
        query.bind(2, user_id);
        query.bind(3, task_id);
        int rowsUpdated = query.exec();
        return rowsUpdated > 0;
    } catch (std::exception& e) {
        return false;
    }
}

// Отримання хешу пароля користувача
std::string DatabaseManager::getPasswordHash(const std::string &username) const {
    try {
        SQLite::Statement query(*db, "SELECT password_hash FROM users WHERE username = ?");
        query.bind(1, username);

        if (query.executeStep()) return query.getColumn(0).getString();
        else return "";

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return "";
    }
}

int DatabaseManager::getUserId(const std::string &username) const {
        try {
            SQLite::Statement query(*db, "SELECT id FROM users WHERE username = ?");
            query.bind(1, username);

            if (query.executeStep()) return query.getColumn(0).getInt();
            else return -1;

        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            return -1;
        }
    }
bool DatabaseManager::createSession(int user_id, const std::string &token) {
        try {
            SQLite::Statement query(*db, "INSERT INTO sessions(user_id, token) VALUES (?, ?)");
            query.bind(1, user_id);
            query.bind(2, token);
            query.exec();
            return true;

        }
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
    }

bool DatabaseManager::deleteSession(const std::string &token) {
    try {
        SQLite::Statement query(*db, "DELETE FROM sessions WHERE token = ?");
        query.bind(1, token);
        int deletedRows = query.exec();
        return deletedRows > 0;
    }catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

int DatabaseManager::getUserIdByToken(const std::string &token) {
    try {
        SQLite::Statement query(*db, "SELECT user_id FROM sessions WHERE token = ?");
        query.bind(1, token);
        if (query.executeStep()) {
            return query.getColumn(0).getInt();
        } else {
            return -1;
        }
    } catch (std::exception& e) {
        std::cerr << "Session Check Error: " << e.what() << std::endl;
        return -1;
    }
}

json DatabaseManager::getAllTasksForUserId(int user_id) const {
    try {
        json result = json::array();
        // Додали id у SELECT
        SQLite::Statement query(*db, "SELECT id, title, description, status FROM tasks WHERE user_id = ?");
        query.bind(1, user_id);

        while (query.executeStep()) {
            json task;
            task["id"] = query.getColumn(0).getInt(); // Віддаємо ID клієнту
            task["title"] = query.getColumn(1).getString();
            task["description"] = query.getColumn(2).getString();
            task["status"] = query.getColumn(3).getString();
            result.push_back(task);
        }
        return result;
    } catch (std::exception& e) {
        return json::array();
    }
}


