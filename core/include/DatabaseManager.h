#ifndef SMARTTASKMANAGER_DATABASEMANAGER_H
#define SMARTTASKMANAGER_DATABASEMANAGER_H

#include "SQLiteCpp/SQLiteCpp.h"
#include "../../json.hpp"

using json = nlohmann::json;

class DatabaseManager {
private:
    std::unique_ptr<SQLite::Database> db;
public:
    DatabaseManager(const std::string& db_path);
    ~DatabaseManager();

    void initializeDatabase() const;
    bool createUser(std::string& username, const std::string& password_hash);
    bool deleteUser(std::string& username);
    bool createTask(int user_id, const std::string& title, const std::string& description);
    bool deleteTask(int user_id, const std::string& title);
    bool updateTaskStatus(int user_id, const std::string& title, const std::string& status);
    std::string getPasswordHash(const std::string& username) const;
    int getUserId(const std::string& username) const;
    bool createSession(int user_id, const std::string& token);
    bool deleteSession(int user_id, const std::string& token);
    int getUserIdByToken(const std::string& token);
    json getAllTasksForUserId(int user_id) const;
};


#endif //SMARTTASKMANAGER_DATABASEMANAGER_H
