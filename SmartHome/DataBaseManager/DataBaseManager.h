#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include <iostream>

class DatabaseManager {
public:
    // 构造函数，初始化数据库路径
    DatabaseManager(const std::string &dbPath);

    // 析构函数，确保数据库连接被正确关闭
    ~DatabaseManager();

    // 连接到数据库，返回连接是否成功
    bool connect();

    // 断开数据库连接，返回操作是否成功
    bool disconnect();

    // 执行给定的 SQL 查询，返回查询是否成功
    bool executeQuery(const std::string &query);

    // 执行给定的 SQL 查询并获取结果，返回结果字符串向量
    std::vector<std::string> fetchResults(const std::string &query);

    // 新增方法：插入一个新角色到数据库中，返回操作是否成功
    bool insertRole(const std::string &roleName);

    // 新增方法：根据用户名获取用户ID，未找到时返回-1
    int getUserIdByUsername(const std::string &username);

    // 新增方法：根据角色名获取角色ID，未找到时返回-1
    int getRoleIdByRoleName(const std::string &roleName);

    // 新增方法：将指定角色添加到指定用户，返回操作是否成功
    bool addRoleToUser(int userId, int roleId);

    // 新增方法：注册一个新用户并分配角色，返回操作是否成功
    bool registerUser(const std::string &username, const std::string &password, const std::string &roleName);

private:
    // SQLite 数据库连接指针
    sqlite3 *db;

    // 数据库文件路径
    std::string dbPath;

    // 静态回调函数，用于 sqlite3_exec 处理查询结果
    static int callback(void *data, int argc, char **argv, char **azColName);

    // 新增的私有方法：使用参数化查询插入角色
    bool insertRoleParametrized(const std::string &roleName);

    // 新增的私有方法：使用参数化查询插入用户
    bool insertUserParametrized(const std::string &username, const std::string &password);

    // 新增的私有方法：使用参数化查询将角色添加到用户
    bool addRoleToUserParametrized(int userId, int roleId);
};

#endif // DATABASEMANAGER_H
