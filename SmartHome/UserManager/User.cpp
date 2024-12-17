#include "User.h"

UserManager::UserManager() : userIdCounter(1), dbManager("DBPath/users.db")
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

bool UserManager::deleteUser(const std::string &username)
{
    std::string deleteQuery = "DELETE FROM users WHERE username = '" + username + "';";
    if (!dbManager.executeQuery(deleteQuery))
    {
        return false;
    }
    return true;
}

bool UserManager::modifyUserName(const std::string &username, const std::string &new_username) {
    std::string modifyQuery = "UPDATE users SET username = '" + new_username + "' WHERE username = '" + username + "';";
    if (!dbManager.executeQuery(modifyQuery))
    {
        return false;
    }
    return true;
}


bool UserManager::modifyUserPassword(const std::string &username, const std::string &password) {
    std::string new_password;
    std::string passwordHash = hashPassword(new_password);
    std::string modifyQuery = "UPDATE users SET password_hash = '" + passwordHash + "' WHERE username = '" + username + "';";
    if (!dbManager.executeQuery(modifyQuery))
    {
        return false;
    }
    return true;
}

bool UserManager::modifyUserRole(const std::string &username, const std::string &new_role){
    std::string modifyQuery = "UPDATE users SET role = '" + new_role + "' WHERE username = '" + username + "';";
    if (!dbManager.executeQuery(modifyQuery))
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

std::string UserManager::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (mdctx == nullptr) {
        return "";
    }

    if (1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr)) {
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    if (1 != EVP_DigestUpdate(mdctx, password.c_str(), password.size())) {
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    if (1 != EVP_DigestFinal_ex(mdctx, hash, nullptr)) {
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    EVP_MD_CTX_free(mdctx);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}


bool UserManager::checkPasswordHash(const std::string &password, const std::string &hash)
{
    return hashPassword(password) == hash;
}
