#ifndef SMARTTASKMANAGER_DATABASEMANAGER_H
#define SMARTTASKMANAGER_DATABASEMANAGER_H

#include "SQLiteCpp/SQLiteCpp.h"

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
    std::string getPasswordHash(const std::string& username) const;
};


#endif //SMARTTASKMANAGER_DATABASEMANAGER_H
