#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sqlite3.h> // 用于数据库操作
#include <openssl/sha.h> // 用于密码哈希
#include <sstream>        // 用于字符串流操作
#include <iomanip>        // 用于格式化字符串输出
#include "../DataBaseManager/DataBaseManager.h"
#include <openssl/evp.h>

// 定义User类，用于表示用户信息
class User
{
public:
    // 定义用户角色枚举类
    enum class Role
    {
        Admin, // 管理员角色
        User   // 普通用户角色
    };

    // 构造函数，初始化用户ID、用户名、密码哈希和角色
    User(int id, const std::string &username, const std::string &password_hash, Role role) : id(id), username(username), password_hash(password_hash), role(role) {}

    // 获取用户ID
    int getId() const { return id; }

    // 获取用户名
    const std::string &getUsername() const { return username; }

    // 获取密码哈希
    const std::string &getPasswordHash() const { return password_hash; }

    // 获取用户角色
    Role getRole() const { return role; }

private:
    int id; // 用户ID
    std::string username; // 用户名
    std::string password_hash; // 密码哈希
    Role role; // 用户角色
};

// 定义UserManager类，用于管理用户注册、登录和登出等操作
class UserManager
{
public:
    // 构造函数，初始化用户ID计数器和其他成员变量
    UserManager();

    // 注册新用户，传入用户名、密码和角色，返回注册是否成功
    bool registerUser(const std::string &username, const std::string &password, const std::string &role);

    // 删除用户，传入用户名，返回删除是否成功
    bool deleteUser(const std::string &username);

    // 修改用户信息，传入用户名返回修改是否成功
    bool modifyUserName(const std::string &username, const std::string &new_username);

    // 修改用户信息，传入用户密码返回修改是否成功
    bool modifyUserPassword(const std::string &username, const std::string &new_password);

    // 修改用户信息，传入用户角色返回修改是否成功
    bool modifyUserRole(const std::string &username, const std::string &new_role);

    // 用户登录验证，传入用户名和密码，返回登录是否成功
    bool loginUser(const std::string &username, const std::string &password);

    // 用户登出，传入用户ID
    void logoutUser(int userId);

    // 获取指定用户ID的用户对象指针
    User* getUser(int userId);

private:
    int userIdCounter; // 用户ID计数器，用于生成新的用户ID
    std::map<int, User> users; // 存储用户信息的映射表
    std::map<int, std::string> sessions; // 记录会话信息，如登录时间等
    DatabaseManager dbManager; // 数据库管理对象，用于与数据库交互

    // 将密码哈希化，传入密码字符串，返回哈希后的密码字符串
    std::string hashPassword(const std::string &password);

    // 验证输入的密码是否与存储的密码哈希匹配，传入密码字符串和密码哈希，返回验证结果
    bool checkPasswordHash(const std::string &password, const std::string &hash);
};

#endif // USER_H
