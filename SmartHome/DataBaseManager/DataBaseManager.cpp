#include "DataBaseManager.h"

// 构造函数，初始化数据库指针为nullptr，数据库路径为传入的dbPath
DatabaseManager::DatabaseManager(const std::string &dbPath) : db(nullptr), dbPath(dbPath) {}

// 析构函数，调用disconnect方法断开数据库连接
DatabaseManager::~DatabaseManager()
{
    disconnect();
}

// 连接数据库，使用sqlite3_open函数打开数据库文件，如果成功返回true，否则返回false
bool DatabaseManager::connect()
{
    return sqlite3_open(dbPath.c_str(), &db) == SQLITE_OK;
}

// 断开数据库连接，如果数据库指针不为空，则使用sqlite3_close关闭数据库，如果成功返回true，否则返回false
// 如果数据库指针为空，直接返回true
bool DatabaseManager::disconnect()
{
    if (db)
    {
        return sqlite3_close(db) == SQLITE_OK;
    }
    return true;
}

// 执行SQL查询，使用sqlite3_exec函数执行传入的SQL语句，如果成功返回true，否则返回false并将错误信息输出到标准错误
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

// 执行SQL查询并获取结果，使用sqlite3_exec函数执行传入的SQL语句，并通过lambda表达式将结果存储在results向量中
// 如果成功返回结果向量，否则返回空向量并将错误信息输出到标准错误
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

// 回调函数，当前未使用，用于sqlite3_exec函数的回调机制
int DatabaseManager::callback(void *data, int argc, char **argv, char **azColName)
{
    return 0;
}

// 插入角色信息，使用预编译语句防止SQL注入，成功插入返回true，否则返回false并将错误信息输出到标准错误
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

// 插入用户信息，使用预编译语句防止SQL注入，成功插入返回true，否则返回false并将错误信息输出到标准错误
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

// 为用户添加角色信息，使用预编译语句防止SQL注入，成功插入返回true，否则返回false并将错误信息输出到标准错误
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

// 插入角色信息的接口函数，调用insertRoleParametrized函数实现具体功能
bool DatabaseManager::insertRole(const std::string &roleName)
{
    return insertRoleParametrized(roleName);
}

// 根据用户名获取用户ID，使用预编译语句防止SQL注入，成功获取返回用户ID，否则返回-1
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
    return -1; // 用户不存在时返回-1
}

// 根据角色名获取角色ID，使用预编译语句防止SQL注入，成功获取返回角色ID，否则返回-1
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
    return -1; // 角色不存在时返回-1
}

// 为用户添加角色信息的接口函数，调用addRoleToUserParametrized函数实现具体功能
bool DatabaseManager::addRoleToUser(int userId, int roleId)
{
    return addRoleToUserParametrized(userId, roleId);
}

// 注册新用户并为其添加角色，首先连接数据库，然后插入用户信息，获取用户ID
// 检查角色是否存在，如果不存在则插入角色信息并获取角色ID，最后为用户添加角色信息
// 成功完成所有操作返回true，否则返回false并将错误信息输出到标准错误
bool DatabaseManager::registerUser(const std::string &username, const std::string &password, const std::string &roleName)
{
    if (!connect())
    {
        std::cerr << "Failed to connect to the database." << std::endl;
        return false;
    }

    // 插入用户信息
    if (!insertUserParametrized(username, password))
    {
        std::cerr << "Failed to insert user." << std::endl;
        disconnect();
        return false;
    }

    // 获取新插入用户的ID
    int userId = getUserIdByUsername(username);
    if (userId == -1)
    {
        std::cerr << "Failed to get user ID." << std::endl;
        disconnect();
        return false;
    }

    // 插入角色信息（如果角色表中没有对应的角色）
    int roleId = getRoleIdByRoleName(roleName);
    if (roleId == -1)
    {
        if (!insertRoleParametrized(roleName))
        {
            std::cerr << "Failed to insert role." << std::endl;
            disconnect();
            return false;
        }
        roleId = getRoleIdByRoleName(roleName); // 重新获取角色ID
    }

    // 为用户添加角色
    if (!addRoleToUserParametrized(userId, roleId))
    {
        std::cerr << "Failed to add role to user." << std::endl;
        disconnect();
        return false;
    }

    disconnect();
    return true; // 注册成功
}
