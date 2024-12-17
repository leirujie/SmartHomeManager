#include "User.h"

<<<<<<< HEAD
// UserManager的构造函数，初始化userIdCounter为1，并初始化dbManager，指定数据库路径为"DBPath/users.db"
UserManager::UserManager() : userIdCounter(1), dbManager("DBPath/users.db")
{
    // 尝试连接数据库，如果失败则输出错误信息并返回
=======
UserManager::UserManager() : userIdCounter(1), dbManager("DBPath/users.db")
{
>>>>>>> 68b4d859287c0d8352d66a2d05d473e976d49280
    if (!dbManager.connect())
    {
        std::cerr << "Failed to connect to database." << std::endl;
        return;
    }

<<<<<<< HEAD
    // 定义创建用户表的SQL语句，如果表不存在则创建，包含id、username、password_hash和role字段
=======
>>>>>>> 68b4d859287c0d8352d66a2d05d473e976d49280
    std::string createTableQuery = "CREATE TABLE IF NOT EXISTS users ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "username TEXT NOT NULL UNIQUE,"
                                   "password_hash TEXT NOT NULL,"
                                   "role TEXT NOT NULL"
                                   ");";
<<<<<<< HEAD
    // 执行创建表的SQL语句，如果执行失败则输出错误信息
=======
>>>>>>> 68b4d859287c0d8352d66a2d05d473e976d49280
    if (!dbManager.executeQuery(createTableQuery))
    {
        std::cerr << "用户表创建失败." << std::endl;
    }
}

<<<<<<< HEAD
// 注册新用户的函数，接收用户名、密码和角色作为参数
bool UserManager::registerUser(const std::string &username, const std::string &password, const std::string &role)
{
    // 对密码进行哈希处理，得到密码的哈希值
    std::string passwordHash = hashPassword(password);
    // 定义插入新用户的SQL语句，使用用户名、密码哈希值和角色
    std::string insertQuery = "INSERT INTO users (username, password_hash, role) VALUES ('" +
                              username + "', '" + passwordHash + "', '" + role + "');";
    // 执行插入用户的SQL语句，如果执行失败则返回false
=======
bool UserManager::registerUser(const std::string &username, const std::string &password, const std::string &role)
{
    std::string passwordHash = hashPassword(password);
    std::string insertQuery = "INSERT INTO users (username, password_hash, role) VALUES ('" +
                              username + "', '" + passwordHash + "', '" + role + "');";
>>>>>>> 68b4d859287c0d8352d66a2d05d473e976d49280
    if (!dbManager.executeQuery(insertQuery))
    {
        return false;
    }
<<<<<<< HEAD
    // 插入成功则返回true
    return true;
}

// 删除用户的函数，接收用户名作为参数
bool UserManager::deleteUser(const std::string &username)
{
    // 定义删除用户的SQL语句，根据用户名删除记录
    std::string deleteQuery = "DELETE FROM users WHERE username = '" + username + "';";
    // 执行删除用户的SQL语句，如果执行失败则返回false
=======
    return true;
}

bool UserManager::deleteUser(const std::string &username)
{
    std::string deleteQuery = "DELETE FROM users WHERE username = '" + username + "';";
>>>>>>> 68b4d859287c0d8352d66a2d05d473e976d49280
    if (!dbManager.executeQuery(deleteQuery))
    {
        return false;
    }
<<<<<<< HEAD
    // 删除成功则返回true
    return true;
}

// 修改用户名的函数，接收旧用户名和新用户名作为参数
bool UserManager::modifyUserName(const std::string &username, const std::string &new_username) {
    // 定义更新用户名的SQL语句，根据旧用户名更新为新用户名
    std::string modifyQuery = "UPDATE users SET username = '" + new_username + "' WHERE username = '" + username + "';";
    // 执行更新用户名的SQL语句，如果执行失败则返回false
=======
    return true;
}

bool UserManager::modifyUserName(const std::string &username, const std::string &new_username) {
    std::string modifyQuery = "UPDATE users SET username = '" + new_username + "' WHERE username = '" + username + "';";
>>>>>>> 68b4d859287c0d8352d66a2d05d473e976d49280
    if (!dbManager.executeQuery(modifyQuery))
    {
        return false;
    }
<<<<<<< HEAD
    // 更新成功则返回true
    return true;
}

// 修改用户密码的函数，接收用户名和新密码作为参数
bool UserManager::modifyUserPassword(const std::string &username, const std::string &password) {
    // 声明一个新的密码字符串变量，这里应该是password而不是new_password
    std::string new_password = password;
    // 对新密码进行哈希处理，得到新密码的哈希值
    std::string passwordHash = hashPassword(new_password);
    // 定义更新用户密码哈希的SQL语句，根据用户名更新密码哈希值
    std::string modifyQuery = "UPDATE users SET password_hash = '" + passwordHash + "' WHERE username = '" + username + "';";
    // 执行更新密码哈希的SQL语句，如果执行失败则返回false
=======
    return true;
}


bool UserManager::modifyUserPassword(const std::string &username, const std::string &password) {
    std::string new_password;
    std::string passwordHash = hashPassword(new_password);
    std::string modifyQuery = "UPDATE users SET password_hash = '" + passwordHash + "' WHERE username = '" + username + "';";
>>>>>>> 68b4d859287c0d8352d66a2d05d473e976d49280
    if (!dbManager.executeQuery(modifyQuery))
    {
        return false;
    }
<<<<<<< HEAD
    // 更新成功则返回true
    return true;
}

// 修改用户角色的函数，接收用户名和新角色作为参数
bool UserManager::modifyUserRole(const std::string &username, const std::string &new_role){
    // 定义更新用户角色的SQL语句，根据用户名更新角色
    std::string modifyQuery = "UPDATE users SET role = '" + new_role + "' WHERE username = '" + username + "';";
    // 执行更新角色的SQL语句，如果执行失败则返回false
=======
    return true;
}

bool UserManager::modifyUserRole(const std::string &username, const std::string &new_role){
    std::string modifyQuery = "UPDATE users SET role = '" + new_role + "' WHERE username = '" + username + "';";
>>>>>>> 68b4d859287c0d8352d66a2d05d473e976d49280
    if (!dbManager.executeQuery(modifyQuery))
    {
        return false;
    }
<<<<<<< HEAD
    // 更新成功则返回true
    return true;
}

// 用户登录函数，接收用户名和密码作为参数
bool UserManager::loginUser(const std::string &username, const std::string &password)
{
    // 定义查询用户ID和密码哈希的SQL语句，根据用户名查询
    std::string selectQuery = "SELECT id, password_hash FROM users WHERE username = '" + username + "';";
    // 执行查询语句，获取结果
    std::vector<std::string> results = dbManager.fetchResults(selectQuery);
    // 如果结果不包含两个元素（ID和密码哈希），则返回false
=======
    return true;
}

bool UserManager::loginUser(const std::string &username, const std::string &password)
{
    std::string selectQuery = "SELECT id, password_hash FROM users WHERE username = '" + username + "';";
    std::vector<std::string> results = dbManager.fetchResults(selectQuery);
>>>>>>> 68b4d859287c0d8352d66a2d05d473e976d49280
    if (results.size() != 2)
    {
        return false;
    }
<<<<<<< HEAD
    // 将查询结果的第一项转换为整数，得到用户ID
    int userId = std::stoi(results[0]);
    // 获取查询结果的第二项，得到存储的密码哈希
    std::string storedPasswordHash = results[1];

    // 检查输入的密码与存储的密码哈希是否匹配，如果匹配则将用户ID添加到sessions中，并返回true
=======
    int userId = std::stoi(results[0]);
    std::string storedPasswordHash = results[1];

>>>>>>> 68b4d859287c0d8352d66a2d05d473e976d49280
    if (checkPasswordHash(password, storedPasswordHash))
    {
        sessions[userId] = "Logged in";
        return true;
    }
<<<<<<< HEAD
    // 如果不匹配则返回false
    return false;
}

// 用户登出函数，接收用户ID作为参数
void UserManager::logoutUser(int userId)
{
    // 从sessions中移除指定用户ID的记录
    sessions.erase(userId);
}

// 获取用户对象的函数，接收用户ID作为参数
User* UserManager::getUser(int userId)
{
    // 查找users容器中是否存在指定用户ID的记录
    auto it = users.find(userId);
    // 如果存在则返回该用户对象的指针，否则返回nullptr
=======
    return false;
}

void UserManager::logoutUser(int userId)
{
    sessions.erase(userId);
}

User* UserManager::getUser(int userId)
{
    auto it = users.find(userId);
>>>>>>> 68b4d859287c0d8352d66a2d05d473e976d49280
    if (it != users.end())
    {
        return &it->second;
    }
    return nullptr;
}

<<<<<<< HEAD
// 对密码进行哈希处理的函数，接收密码字符串作为参数
std::string UserManager::hashPassword(const std::string& password) {
    // 定义一个长度为SHA256_DIGEST_LENGTH的数组存储哈希结果
    unsigned char hash[SHA256_DIGEST_LENGTH];
    // 创建一个EVP_MD_CTX对象，用于进行哈希计算
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    // 如果创建失败则返回空字符串
=======
std::string UserManager::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
>>>>>>> 68b4d859287c0d8352d66a2d05d473e976d49280
    if (mdctx == nullptr) {
        return "";
    }

<<<<<<< HEAD
    // 初始化哈希计算，指定使用SHA256算法
    if (1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr)) {
        // 如果初始化失败则释放EVP_MD_CTX对象并返回空字符串
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    // 更新哈希计算，使用传入的密码字符串
    if (1 != EVP_DigestUpdate(mdctx, password.c_str(), password.size())) {
        // 如果更新失败则释放EVP_MD_CTX对象并返回空字符串
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    // 结束哈希计算，获取最终的哈希值
    if (1 != EVP_DigestFinal_ex(mdctx, hash, nullptr)) {
        // 如果结束计算失败则释放EVP_MD_CTX对象并返回空字符串
=======
    if (1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr)) {
>>>>>>> 68b4d859287c0d8352d66a2d05d473e976d49280
        EVP_MD_CTX_free(mdctx);
        return "";
    }

<<<<<<< HEAD
    // 释放EVP_MD_CTX对象
    EVP_MD_CTX_free(mdctx);

    // 将哈希值转换为十六进制字符串表示
=======
    if (1 != EVP_DigestUpdate(mdctx, password.c_str(), password.size())) {
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    if (1 != EVP_DigestFinal_ex(mdctx, hash, nullptr)) {
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    EVP_MD_CTX_free(mdctx);

>>>>>>> 68b4d859287c0d8352d66a2d05d473e976d49280
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

<<<<<<< HEAD
    // 返回十六进制字符串形式的哈希值
    return ss.str();
}

// 检查密码哈希是否匹配的函数，接收密码和存储的哈希值作为参数
bool UserManager::checkPasswordHash(const std::string &password, const std::string &hash)
{
    // 对输入的密码进行哈希处理，并与存储的哈希值进行比较，如果相同则返回true，否则返回false
=======
    return ss.str();
}


bool UserManager::checkPasswordHash(const std::string &password, const std::string &hash)
{
>>>>>>> 68b4d859287c0d8352d66a2d05d473e976d49280
    return hashPassword(password) == hash;
}
