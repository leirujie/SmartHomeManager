#include "DataBaseManager.h"  // 引入数据库管理头文件

// 构造函数，初始化数据库指针为空，设置数据库路径
DatabaseManager::DatabaseManager(const std::string &dbPath) : db(nullptr), dbPath(dbPath) {}

// 析构函数，确保在对象销毁时断开数据库连接
DatabaseManager::~DatabaseManager()
{
    disconnect();
}

// 连接数据库函数，返回连接是否成功的布尔值
bool DatabaseManager::connect()
{
    // 使用sqlite3_open函数打开数据库，如果返回SQLITE_OK则连接成功
    return sqlite3_open(dbPath.c_str(), &db) == SQLITE_OK;
}

// 断开数据库连接函数，返回操作是否成功的布尔值
bool DatabaseManager::disconnect()
{
    // 检查数据库指针是否为空，如果不为空则尝试关闭数据库
    if (db)
    {
        // 使用sqlite3_close函数关闭数据库，如果返回SQLITE_OK则操作成功
        return sqlite3_close(db) == SQLITE_OK;
    }
    // 如果数据库指针为空，直接返回true表示操作成功
    return true;
}

// 执行SQL查询函数，返回查询是否成功的布尔值
bool DatabaseManager::executeQuery(const std::string &query)
{
    char *zErrMsg = nullptr;  // 初始化错误消息指针为空
    // 使用sqlite3_exec函数执行查询，如果返回值不是SQLITE_OK则表示查询失败
    int rc = sqlite3_exec(db, query.c_str(), callback, nullptr, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << zErrMsg << std::endl;  // 输出错误消息
        sqlite3_free(zErrMsg);  // 释放错误消息内存
        return false;  // 返回false表示查询失败
    }
    return true;  // 返回true表示查询成功
}

// 获取SQL查询结果函数，返回结果集向量
std::vector<std::string> DatabaseManager::fetchResults(const std::string &query)
{
    std::vector<std::string> results;  // 初始化结果集向量
    char *zErrMsg = nullptr;  // 初始化错误消息指针为空
    // 使用sqlite3_exec函数执行查询，并传递一个lambda函数作为回调以收集结果
    int rc = sqlite3_exec(db, query.c_str(), [](void *data, int argc, char **argv, char **azColName) -> int
                          {
                              // 将data指针转换为std::vector<std::string>指针
                              std::vector<std::string> *results = static_cast<std::vector<std::string> *>(data);
                              // 遍历查询结果中的每一列，将列值添加到结果集中
                              for (int i = 0; i < argc; i++)
                              {
                                  // 如果列值不为空则添加，否则添加"NULL"
                                  results->push_back(argv[i] ? argv[i] : "NULL");
                              }
                              return 0;  // 返回0表示回调执行成功
                          },
                          &results, &zErrMsg);  // 传递结果集指针和错误消息指针
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << zErrMsg << std::endl;  // 输出错误消息
        sqlite3_free(zErrMsg);  // 释放错误消息内存
        return {};  // 返回空结果集
    }
    return results;  // 返回填充好的结果集
}

// 回调函数，用于sqlite3_exec的回调，这里未被使用
int DatabaseManager::callback(void *data, int argc, char **argv, char **azColName)
{
    return 0;  // 返回0表示回调执行成功
}

// 使用参数化查询插入角色函数，返回操作是否成功的布尔值
bool DatabaseManager::insertRoleParametrized(const std::string &roleName)
{
    sqlite3_stmt *stmt;  // 声明sqlite3_stmt指针用于保存预编译的SQL语句
    std::string query = "INSERT INTO roles (role_name) VALUES (?);";  // 定义插入角色的SQL语句
    // 使用sqlite3_prepare_v2函数预编译SQL语句，如果返回值不是SQLITE_OK则表示预编译失败
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;  // 输出错误消息
        return false;  // 返回false表示操作失败
    }

    // 使用sqlite3_bind_text函数绑定角色名称到预编译的SQL语句中的第一个占位符
    sqlite3_bind_text(stmt, 1, roleName.c_str(), -1, SQLITE_STATIC);

    // 使用sqlite3_step函数执行预编译的SQL语句
    int rc = sqlite3_step(stmt);
    // 使用sqlite3_finalize函数释放预编译的SQL语句
    sqlite3_finalize(stmt);

    // 检查执行结果是否为SQLITE_DONE，如果不是则表示执行失败
    if (rc != SQLITE_DONE)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;  // 输出错误消息
        return false;  // 返回false表示操作失败
    }
    return true;  // 返回true表示操作成功
}

// 使用参数化查询插入用户函数，返回操作是否成功的布尔值
bool DatabaseManager::insertUserParametrized(const std::string &username, const std::string &password)
{
    sqlite3_stmt *stmt;  // 声明sqlite3_stmt指针用于保存预编译的SQL语句
    std::string query = "INSERT INTO users (username, password) VALUES (?, ?);";  // 定义插入用户的SQL语句
    // 使用sqlite3_prepare_v2函数预编译SQL语句，如果返回值不是SQLITE_OK则表示预编译失败
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;  // 输出错误消息
        return false;  // 返回false表示操作失败
    }

    // 使用sqlite3_bind_text函数绑定用户名到预编译的SQL语句中的第一个占位符
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    // 使用sqlite3_bind_text函数绑定密码到预编译的SQL语句中的第二个占位符
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    // 使用sqlite3_step函数执行预编译的SQL语句
    int rc = sqlite3_step(stmt);
    // 使用sqlite3_finalize函数释放预编译的SQL语句
    sqlite3_finalize(stmt);

    // 检查执行结果是否为SQLITE_DONE，如果不是则表示执行失败
    if (rc != SQLITE_DONE)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;  // 输出错误消息
        return false;  // 返回false表示操作失败
    }
    return true;  // 返回true表示操作成功
}

// 使用参数化查询将角色添加到用户函数，返回操作是否成功的布尔值
bool DatabaseManager::addRoleToUserParametrized(int userId, int roleId)
{
    sqlite3_stmt *stmt;  // 声明sqlite3_stmt指针用于保存预编译的SQL语句
    std::string query = "INSERT INTO user_roles (user_id, role_id) VALUES (?, ?);";  // 定义插入用户角色的SQL语句
    // 使用sqlite3_prepare_v2函数预编译SQL语句，如果返回值不是SQLITE_OK则表示预编译失败
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;  // 输出错误消息
        return false;  // 返回false表示操作失败
    }

    // 使用sqlite3_bind_int函数绑定用户ID到预编译的SQL语句中的第一个占位符
    sqlite3_bind_int(stmt, 1, userId);
    // 使用sqlite3_bind_int函数绑定角色ID到预编译的SQL语句中的第二个占位符
    sqlite3_bind_int(stmt, 2, roleId);

    // 使用sqlite3_step函数执行预编译的SQL语句
    int rc = sqlite3_step(stmt);
    // 使用sqlite3_finalize函数释放预编译的SQL语句
    sqlite3_finalize(stmt);

    // 检查执行结果是否为SQLITE_DONE，如果不是则表示执行失败
    if (rc != SQLITE_DONE)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;  // 输出错误消息
        return false;  // 返回false表示操作失败
    }
    return true;  // 返回true表示操作成功
}

// 插入角色函数，调用参数化版本
bool DatabaseManager::insertRole(const std::string &roleName)
{
    return insertRoleParametrized(roleName);
}

// 根据用户名获取用户ID函数，返回用户ID或-1（未找到）
int DatabaseManager::getUserIdByUsername(const std::string &username)
{
    sqlite3_stmt *stmt;  // 声明sqlite3_stmt指针用于保存预编译的SQL语句
    std::string query = "SELECT id FROM users WHERE username = ?;";  // 定义查询用户ID的SQL语句
    // 使用sqlite3_prepare_v2函数预编译SQL语句，如果返回值不是SQLITE_OK则表示预编译失败
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;  // 输出错误消息
        return -1;  // 返回-1表示未找到用户
    }

    // 使用sqlite3_bind_text函数绑定用户名到预编译的SQL语句中的第一个占位符
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    // 使用sqlite3_step函数执行预编译的SQL语句
    int rc = sqlite3_step(stmt);
    // 检查执行结果是否为SQLITE_ROW，如果是则表示查询到一行数据
    if (rc == SQLITE_ROW)
    {
        // 使用sqlite3_column_int函数获取第一列的整数值（即用户ID）
        int userId = sqlite3_column_int(stmt, 0);
        // 使用sqlite3_finalize函数释放预编译的SQL语句
        sqlite3_finalize(stmt);
        return userId;  // 返回用户ID
    }

    // 使用sqlite3_finalize函数释放预编译的SQL语句
    sqlite3_finalize(stmt);
    return -1;  // 返回-1表示未找到用户
}

// 根据角色名称获取角色ID函数，返回角色ID或-1（未找到）
int DatabaseManager::getRoleIdByRoleName(const std::string &roleName)
{
    sqlite3_stmt *stmt;  // 声明sqlite3_stmt指针用于保存预编译的SQL语句
    std::string query = "SELECT id FROM roles WHERE role_name = ?;";  // 定义查询角色ID的SQL语句
    // 使用sqlite3_prepare_v2函数预编译SQL语句，如果返回值不是SQLITE_OK则表示预编译失败
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;  // 输出错误消息
        return -1;  // 返回-1表示未找到角色
    }

    // 使用sqlite3_bind_text函数绑定角色名称到预编译的SQL语句中的第一个占位符
    sqlite3_bind_text(stmt, 1, roleName.c_str(), -1, SQLITE_STATIC);

    // 使用sqlite3_step函数执行预编译的SQL语句
    int rc = sqlite3_step(stmt);
    // 检查执行结果是否为SQLITE_ROW，如果是则表示查询到一行数据
    if (rc == SQLITE_ROW)
    {
        // 使用sqlite3_column_int函数获取第一列的整数值（即角色ID）
        int roleId = sqlite3_column_int(stmt, 0);
        // 使用sqlite3_finalize函数释放预编译的SQL语句
        sqlite3_finalize(stmt);
        return roleId;  // 返回角色ID
    }

    // 使用sqlite3_finalize函数释放预编译的SQL语句
    sqlite3_finalize(stmt);
    return -1;  // 返回-1表示未找到角色
}

// 将角色添加到用户函数，调用参数化版本
bool DatabaseManager::addRoleToUser(int userId, int roleId)
{
    return addRoleToUserParametrized(userId, roleId); // 调用参数化的方法来将角色添加到用户
}

// 注册用户的函数，包括插入用户、查找或插入角色以及将角色添加到用户
bool DatabaseManager::registerUser(const std::string &username, const std::string &password, const std::string &roleName)
{
    // 尝试连接数据库，如果失败则输出错误消息并返回false
    if (!connect()) // 尝试连接数据库
    {
        std::cerr << "Failed to connect to the database." << std::endl; // 如果连接失败，输出错误消息
        return false; // 返回false表示注册失败
    }

    // 尝试插入用户，如果失败则输出错误消息，断开数据库连接并返回false
    if (!insertUserParametrized(username, password)) // 尝试插入用户
    {
        std::cerr << "Failed to insert user." << std::endl; // 如果插入失败，输出错误消息
        disconnect(); // 断开数据库连接
        return false; // 返回false表示注册失败
    }

    // 根据用户名查找用户ID，如果未找到则输出错误消息，断开数据库连接并返回false
    int userId = getUserIdByUsername(username); // 根据用户名查找用户ID
    if (userId == -1) // 如果用户ID未找到
    {
        std::cerr << "Failed to get user ID." << std::endl; // 输出错误消息
        disconnect(); // 断开数据库连接
        return false; // 返回false表示注册失败
    }

    // 根据角色名称查找角色ID，如果未找到则尝试插入角色并再次查找角色ID
    int roleId = getRoleIdByRoleName(roleName); // 根据角色名称查找角色ID
    if (roleId == -1) // 如果角色ID未找到
    {
        // 尝试插入角色，如果失败则输出错误消息，断开数据库连接并返回false
        if (!insertRoleParametrized(roleName)) // 尝试插入角色
        {
            std::cerr << "Failed to insert role." << std::endl; // 如果插入失败，输出错误消息
            disconnect(); // 断开数据库连接
            return false; // 返回false表示注册失败
        }
        // 再次查找角色ID，确保角色已插入
        roleId = getRoleIdByRoleName(roleName); // 再次查找角色ID，确保角色已成功插入
    }

    // 尝试将角色添加到用户，如果失败则输出错误消息，断开数据库连接并返回false
    if (!addRoleToUserParametrized(userId, roleId)) // 尝试将角色添加到用户
    {
        std::cerr << "Failed to add role to user." << std::endl; // 如果添加失败，输出错误消息
        disconnect(); // 断开数据库连接
        return false; // 返回false表示注册失败
    }

    // 断开数据库连接
    disconnect(); // 注册成功后，断开数据库连接
    return true;  // 返回true表示用户注册成功
}
