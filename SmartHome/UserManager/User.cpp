// User.cpp
#include "User.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>

// UserManager类的构造函数，初始化用户ID计数器和数据库管理器
UserManager::UserManager() : userIdCounter(1), dbManager("users.db")
{
    // 尝试连接到数据库，如果连接失败则输出错误信息并返回
    if (!dbManager.connect())
    {
        std::cerr << "Failed to connect to database." << std::endl;
        return;
    }

    // 创建users表的SQL查询语句
    std::string createTableQuery = "CREATE TABLE IF NOT EXISTS users ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"  // 用户ID，自动递增的主键
                                   "username TEXT NOT NULL UNIQUE,"         // 用户名，不能为空且唯一
                                   "password_hash TEXT NOT NULL,"           // 密码哈希值，不能为空
                                   "role TEXT NOT NULL"                    // 角色，不能为空
                                   ");";
    // 执行创建表的查询，如果失败则输出错误信息
    if (!dbManager.executeQuery(createTableQuery))
    {
        std::cerr << "用户表创建失败." << std::endl;
    }
}

// 注册新用户的函数
bool UserManager::registerUser(const std::string &username, const std::string &password, const std::string &role)
{
    // 对密码进行哈希处理
    std::string passwordHash = hashPassword(password);
    // 构建插入新用户记录的SQL查询语句
    std::string insertQuery = "INSERT INTO users (username, password_hash, role) VALUES ('" +
                              username + "', '" + passwordHash + "', '" + role + "');";
    // 执行插入查询，如果失败则返回false
    if (!dbManager.executeQuery(insertQuery))
    {
        return false;
    }
    // 插入成功返回true
    return true;
}

// 用户登录验证函数
bool UserManager::loginUser(const std::string &username, const std::string &password)
{
    // 构建查询用户记录的SQL语句
    std::string selectQuery = "SELECT id, password_hash FROM users WHERE username = '" + username + "';";
    // 执行查询并获取结果
    std::vector<std::string> results = dbManager.fetchResults(selectQuery);
    // 如果查询结果不包含两个元素（用户ID和密码哈希），则认为登录失败
    if (results.size() != 2)
    {
        return false;
    }
    // 转换用户ID为整数
    int userId = std::stoi(results[0]);
    // 获取存储的密码哈希
    std::string storedPasswordHash = results[1];

    // 验证输入的密码与存储的密码哈希是否匹配
    if (checkPasswordHash(password, storedPasswordHash))
    {
        // 如果匹配，将该用户状态设置为登录
        sessions[userId] = "Logged in";
        return true;
    }
    // 如果不匹配，登录失败
    return false;
}

// 用户登出函数
void UserManager::logoutUser(int userId)
{
    // 从会话列表中移除指定用户的登录状态
    sessions.erase(userId);
}

// 获取指定用户对象的函数
User* UserManager::getUser(int userId)
{
    // 在users映射中查找指定ID的用户
    auto it = users.find(userId);
    // 如果找到，返回该用户的指针
    if (it != users.end())
    {
        return &it->second;
    }
    // 如果未找到，返回nullptr
    return nullptr;
}

// 对密码进行SHA256哈希处理的函数
std::string UserManager::hashPassword(const std::string &password)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];  // 存储哈希结果的数组
    SHA256_CTX sha256;                        // SHA256上下文结构
    SHA256_Init(&sha256);                     // 初始化SHA256上下文
    SHA256_Update(&sha256, password.c_str(), password.size());  // 更新上下文以包含密码数据
    SHA256_Final(hash, &sha256);              // 完成哈希计算并将结果存储
    std::stringstream ss;                     // 创建stringstream对象用于格式化哈希输出
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        // 将哈希值转换为十六进制字符串
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    // 返回格式化后的哈希字符串
    return ss.str();
}

// 验证密码哈希是否匹配的函数
bool UserManager::checkPasswordHash(const std::string &password, const std::string &hash)
{
    // 对输入的密码进行哈希处理并与存储的哈希进行比较
    return hashPassword(password) == hash;
}
