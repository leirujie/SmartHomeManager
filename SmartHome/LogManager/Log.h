// #ifndef LOG_H
// #define LOG_H

// #include <string>
// #include <vector>
// #include <mutex>
// #include <chrono> // 添加时间戳支持
// #include <fstream>
// #include <filesystem> // 添加文件系统支持
// #include <algorithm>
// #include <iomanip>
// #include <stdexcept> // 添加错误处理支持

// // 定义一个名为Log的类，用于记录日志信息
// class Log
// {
// public:
//     // 定义一个枚举类LogType，表示日志的不同类型
//     enum class LogType
//     {
//         UserAction,    // 用户操作日志
//         DeviceAction,  // 设备操作日志
//         SystemError    // 系统错误日志
//     };

//     // 构造函数，初始化日志类型、用户ID、设备ID、日志内容以及时间戳
//     Log(LogType logType, int userId, int deviceId, const std::string &content)
//         : logType(logType), userId(userId), deviceId(deviceId),
//           content(content), timestamp(std::chrono::system_clock::now()) {}

//     // 获取日志类型
//     LogType getLogType() const { return logType; }

//     // 获取用户ID
//     int getUserId() const { return userId; }

//     // 获取设备ID
//     int getDeviceId() const { return deviceId; }

//     // 获取日志内容
//     const std::string &getContent() const { return content; }

//     // 获取时间戳
//     std::chrono::system_clock::time_point getTimestamp() const { return timestamp; }

// private:
//     LogType logType;   // 日志类型
//     int userId;        // 用户ID
//     int deviceId;      // 设备ID
//     std::string content; // 日志内容
//     std::chrono::system_clock::time_point timestamp; // 时间戳
// };

// // 定义一个名为LogManager的类，用于管理日志
// class LogManager
// {
// public:
//     // 构造函数，初始化日志管理器
//     LogManager() : logFolder("logs"), maxLogSize(10 * 1024 * 1024), maxBackupFiles(5) {
//         if (!std::filesystem::exists(logFolder)) {
//             std::filesystem::create_directory(logFolder); // 创建日志文件夹
//         } else if (!std::filesystem::is_directory(logFolder)) {
//             throw std::invalid_argument("logFolder is not a directory.");
//         }
//     }

//     // 记录一条日志动作，根据日志类型、用户ID、设备ID和日志内容
//     void logAction(Log::LogType logType, int userId, int deviceId, const std::string &content) {
//         std::lock_guard<std::mutex> lock(logMutex);
//         logs.emplace_back(logType, userId, deviceId, content);
//         rollLogFiles();
//     }

//     // 将所有日志保存到指定的文件中
//     void saveLogsToFile(const std::string &filename) {
//         std::ofstream file(filename);
//         if (!file) {
//             throw std::runtime_error("Unable to open file for writing: " + filename);
//         }

//         for (const auto &log : logs) {
//             auto time = std::chrono::system_clock::to_time_t(log.getTimestamp());
//             file << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") 
//                  << " - User: " << log.getUserId() 
//                  << ", Device: " << log.getDeviceId() 
//                  << ", Type: " << static_cast<int>(log.getLogType()) 
//                  << ", Content: " << log.getContent() 
//                  << std::endl;
//         }
//     }

//     // 根据用户ID获取所有相关日志
//     std::vector<Log> getLogsByUser(int userId) const {
//         std::lock_guard<std::mutex> lock(logMutex);
//         std::vector<Log> result;
//         std::copy_if(logs.begin(), logs.end(), std::back_inserter(result), 
//                      [userId](const Log &log) { return log.getUserId() == userId; });
//         return result;
//     }

//     // 根据设备ID获取所有相关日志
//     std::vector<Log> getLogsByDevice(int deviceId) const {
//         std::lock_guard<std::mutex> lock(logMutex);
//         std::vector<Log> result;
//         std::copy_if(logs.begin(), logs.end(), std::back_inserter(result), 
//                      [deviceId](const Log &log) { return log.getDeviceId() == deviceId; });
//         return result;
//     }

// private:
//     std::vector<Log> logs;     // 存储所有日志的容器
//     mutable std::mutex logMutex;       // 用于线程安全的日志写入
//     const std::string logFolder;     // 日志文件夹名称
//     const size_t maxLogSize;         // 每个日志文件的最大大小（字节）
//     const int maxBackupFiles;        // 最大备份文件数量

//     // 滚动日志文件
//     void rollLogFiles() {
//         auto filePath = getCurrentLogFilePath();
//         if (std::filesystem::file_size(filePath) >= maxLogSize) {
//             for (int i = maxBackupFiles - 1; i > 0; --i) {
//                 if (std::filesystem::exists(logFolder + "/log_" + std::to_string(i) + ".txt")) {
//                     std::filesystem::rename(logFolder + "/log_" + std::to_string(i) + ".txt", 
//                                             logFolder + "/log_" + std::to_string(i + 1) + ".txt");
//                 }
//             }
//             std::filesystem::rename(filePath, logFolder + "/log_1.txt");
//             std::ofstream file(filePath); // 清空当前日志文件
//         }
//     }

//     // 获取当前日志文件的完整路径
//     std::string getCurrentLogFilePath() const {
//         return logFolder + "/log.txt";
//     }
// };

// #endif // LOG_H
