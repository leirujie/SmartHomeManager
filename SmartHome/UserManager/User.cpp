#include "User.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>

UserManager::UserManager() : userIdCounter(1), dbManager("users.db")
{
    if (!dbManager.connect())
    {
        std::cerr << "Failed to connect to database." << std::endl;
        return;
    }

    std::string createTableQuery = "CREATE TABLE IF NOT EXISTS users ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                   "username TEXT NOT NULL UNIQUE,"
                                   "password_hash TEXT NOT NULL,"
                                   "role TEXT NOT NULL"
                                   ");";
    if (!dbManager.executeQuery(createTableQuery))
    {
        std::cerr << "用户表创建失败." << std::endl;
    }
}

bool UserManager::registerUser(const std::string &username, const std::string &password, const std::string &role)
{
    std::string passwordHash = hashPassword(password);
    std::string insertQuery = "INSERT INTO users (username, password_hash, role) VALUES ('" +
                              username + "', '" + passwordHash + "', '" + role + "');";
    if (!dbManager.executeQuery(insertQuery))
    {
        return false;
    }
    return true;
}

bool UserManager::loginUser(const std::string &username, const std::string &password)
{
    std::string selectQuery = "SELECT id, password_hash FROM users WHERE username = '" + username + "';";
    std::vector<std::string> results = dbManager.fetchResults(selectQuery);
    if (results.size() != 2)
    {
        return false;
    }
    int userId = std::stoi(results[0]);
    std::string storedPasswordHash = results[1];

    if (checkPasswordHash(password, storedPasswordHash))
    {
        sessions[userId] = "Logged in";
        return true;
    }
    return false;
}

void UserManager::logoutUser(int userId)
{
    sessions.erase(userId);
}

User* UserManager::getUser(int userId)
{
    auto it = users.find(userId);
    if (it != users.end())
    {
        return &it->second;
    }
    return nullptr;
}

std::string UserManager::hashPassword(const std::string &password)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

bool UserManager::checkPasswordHash(const std::string &password, const std::string &hash)
{
    return hashPassword(password) == hash;
}
