// #include "Log.h"

// // 定义一个名为LogManager的类，用于管理日志
// class LogManager
// {
// public:
//     // 构造函数，初始化日志管理器
//     LogManager() : logFolder("logs"), maxLogSize(10 * 1024 * 1024), maxBackupFiles(5) {
//         std::filesystem::create_directory(logFolder); // 创建日志文件夹
//     }

//     // 记录一条日志动作，根据日志类型、用户ID、设备ID和日志内容
//     void logAction(Log::LogType logType, int userId, int deviceId, const std::string &content);

//     // 将所有日志保存到指定的文件中
//     void saveLogsToFile(const std::string &filename);

//     // 根据用户ID获取所有相关日志
//     std::vector<Log> getLogsByUser(int userId) const;

//     // 根据设备ID获取所有相关日志
//     std::vector<Log> getLogsByDevice(int deviceId) const;

// private:
//     std::vector<Log> logs;     // 存储所有日志的容器
//     std::mutex logMutex;       // 用于线程安全的日志写入
//     std::string logFolder;     // 日志文件夹名称
//     size_t maxLogSize;         // 每个日志文件的最大大小（字节）
//     int maxBackupFiles;        // 最大备份文件数量

//     // 滚动日志文件
//     void rollLogFiles();

//     // 获取当前日志文件的完整路径
//     std::string getCurrentLogFilePath() const;
// };

// void LogManager::logAction(Log::LogType logType, int userId, int deviceId, const std::string &content) {
//     std::lock_guard<std::mutex> lock(logMutex);
//     logs.emplace_back(logType, userId, deviceId, content);
//     if (shouldRoll()) {
//         rollLogFiles();
//     }
//     saveLogsToFile(getCurrentLogFilePath());
// }

// void LogManager::saveLogsToFile(const std::string &filename) {
//     std::ofstream file(filename, std::ios::app);
//     for (const auto &log : logs) {
//         auto timePoint = log.getTimestamp();
//         auto time = std::chrono::system_clock::to_time_t(timePoint);
//         std::string timeStr(std::ctime(&time));
//         timeStr.pop_back(); // 移除末尾的换行符

//         file << "[" << timeStr << "] ";
//         switch (log.getLogType()) {
//             case Log::LogType::UserAction:
//                 file << "UserAction: ";
//                 break;
//             case Log::LogType::DeviceAction:
//                 file << "DeviceAction: ";
//                 break;
//             case Log::LogType::SystemError:
//                 file << "SystemError: ";
//                 break;
//         }
//         file << "UserID=" << log.getUserId() << ", DeviceID=" << log.getDeviceId() << ", Content=" << log.getContent() << std::endl;
//     }
//     logs.clear(); // 清空内存中的日志
// }

// std::vector<Log> LogManager::getLogsByUser(int userId) const {
//     std::vector<Log> result;
//     for (const auto &log : logs) {
//         if (log.getUserId() == userId) {
//             result.push_back(log);
//         }
//     }
//     return result;
// }

// std::vector<Log> LogManager::getLogsByDevice(int deviceId) const {
//     std::vector<Log> result;
//     for (const auto &log : logs) {
//         if (log.getDeviceId() == deviceId) {
//             result.push_back(log);
//         }
//     }
//     return result;
// }

// bool LogManager::shouldRoll() {
//     std::ifstream file(getCurrentLogFilePath(), std::ios::ate);
//     if (file.is_open()) {
//         return file.tellg() > maxLogSize;
//     }
//     return false;
// }

// void LogManager::rollLogFiles() {
//     std::lock_guard<std::mutex> lock(logMutex);
//     for (int i = maxBackupFiles - 1; i > 0; i--) {
//         std::string oldFilename = logFolder + "/log_" + std::to_string(i) + ".txt";
//         std::string newFilename = logFolder + "/log_" + std::to_string(i + 1) + ".txt";
//         if (std::filesystem::exists(oldFilename)) {
//             std::filesystem::rename(oldFilename, newFilename);
//         }
//     }
//     std::string currentFilename = getCurrentLogFilePath();
//     std::string newFilename = logFolder + "/log_1.txt";
//     if (std::filesystem::exists(currentFilename)) {
//         std::filesystem::rename(currentFilename, newFilename);
//     }
// }

// std::string LogManager::getCurrentLogFilePath() const {
//     return logFolder + "/log.txt";
// }
