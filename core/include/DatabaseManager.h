#ifndef SMARTTASKMANAGER_DATABASEMANAGER_H
#define SMARTTASKMANAGER_DATABASEMANAGER_H

#include "SQLiteCpp/SQLiteCpp.h"

class DatabaseManager {
private:
    std::unique_ptr<SQLite::Database> db;
public:
    DatabaseManager();
    ~DatabaseManager();

    void initializeDatabase();
    bool createUser(std::string& username, const std::string& password_hash);
    bool create_task(int user_id, const std::string& title, const std::string& description);
};


#endif //SMARTTASKMANAGER_DATABASEMANAGER_H
