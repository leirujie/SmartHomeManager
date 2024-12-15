#include "DataBaseManager.h"

DatabaseManager::DatabaseManager(const std::string &dbPath) : db(nullptr), dbPath(dbPath) {}

DatabaseManager::~DatabaseManager()
{
    disconnect();
}

bool DatabaseManager::connect()
{
    return sqlite3_open(dbPath.c_str(), &db) == SQLITE_OK;
}

bool DatabaseManager::disconnect()
{
    if (db)
    {
        return sqlite3_close(db) == SQLITE_OK;
    }
    return true;
}

bool DatabaseManager::executeQuery(const std::string &query)
{
    char *zErrMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), callback, nullptr, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        return false;
    }
    return true;
}

std::vector<std::string> DatabaseManager::fetchResults(const std::string &query)
{
    std::vector<std::string> results;
    char *zErrMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), [](void *data, int argc, char **argv, char **azColName) -> int
                          {
                              std::vector<std::string> *results = static_cast<std::vector<std::string> *>(data);
                              for (int i = 0; i < argc; i++)
                              {
                                  results->push_back(argv[i] ? argv[i] : "NULL");
                              }
                              return 0;
                          },
                          &results, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        return {};
    }
    return results;
}

int DatabaseManager::callback(void *data, int argc, char **argv, char **azColName)
{
    return 0;
}

bool DatabaseManager::insertRoleParametrized(const std::string &roleName)
{
    sqlite3_stmt *stmt;
    std::string query = "INSERT INTO roles (role_name) VALUES (?);";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, roleName.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    return true;
}

bool DatabaseManager::insertUserParametrized(const std::string &username, const std::string &password)
{
    sqlite3_stmt *stmt;
    std::string query = "INSERT INTO users (username, password) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    return true;
}

bool DatabaseManager::addRoleToUserParametrized(int userId, int roleId)
{
    sqlite3_stmt *stmt;
    std::string query = "INSERT INTO user_roles (user_id, role_id) VALUES (?, ?);";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, roleId);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    return true;
}

bool DatabaseManager::insertRole(const std::string &roleName)
{
    return insertRoleParametrized(roleName);
}

int DatabaseManager::getUserIdByUsername(const std::string &username)
{
    sqlite3_stmt *stmt;
    std::string query = "SELECT id FROM users WHERE username = ?;";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        int userId = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return userId;
    }

    sqlite3_finalize(stmt);
    return -1;
}

int DatabaseManager::getRoleIdByRoleName(const std::string &roleName)
{
    sqlite3_stmt *stmt;
    std::string query = "SELECT id FROM roles WHERE role_name = ?;";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    sqlite3_bind_text(stmt, 1, roleName.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {
        int roleId = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return roleId;
    }

    sqlite3_finalize(stmt);
    return -1;
}

bool DatabaseManager::addRoleToUser(int userId, int roleId)
{
    return addRoleToUserParametrized(userId, roleId);
}

bool DatabaseManager::registerUser(const std::string &username, const std::string &password, const std::string &roleName)
{
    if (!connect())
    {
        std::cerr << "Failed to connect to the database." << std::endl;
        return false;
    }

    if (!insertUserParametrized(username, password))
    {
        std::cerr << "Failed to insert user." << std::endl;
        disconnect();
        return false;
    }

    int userId = getUserIdByUsername(username);
    if (userId == -1)
    {
        std::cerr << "Failed to get user ID." << std::endl;
        disconnect();
        return false;
    }

    int roleId = getRoleIdByRoleName(roleName);
    if (roleId == -1)
    {
        if (!insertRoleParametrized(roleName))
        {
            std::cerr << "Failed to insert role." << std::endl;
            disconnect();
            return false;
        }
        roleId = getRoleIdByRoleName(roleName);
    }

    if (!addRoleToUserParametrized(userId, roleId))
    {
        std::cerr << "Failed to add role to user." << std::endl;
        disconnect();
        return false;
    }

    disconnect();
    return true;
}
