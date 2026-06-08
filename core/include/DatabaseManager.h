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
    bool create_task(int user_id, const std::string& title, const std::string& description);
    bool delete_task(int user_id, const std::string& title);
};


#endif //SMARTTASKMANAGER_DATABASEMANAGER_H
