#ifndef LOG_H
#define LOG_H

#include <string>
#include <vector>
#include <mutex>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <iostream>
#include <sqlite3.h>

// 定义一个名为Log的类，用于记录日志信息
class Log
{
public:
    // 定义一个枚举类LogType，表示日志的不同类型
    enum class LogType
    {
        UserAction,    // 用户操作日志
        DeviceAction,  // 设备操作日志
        SystemError    // 系统错误日志
    };

    // 构造函数，初始化日志类型、用户ID、设备ID、日志内容以及时间戳
    Log(LogType logType, int userId, int deviceId, const std::string &content) : logType(logType), userId(userId), deviceId(deviceId), content(content), timestamp(std::chrono::system_clock::now()) {}

    // 获取日志类型
    LogType getLogType() const { return logType; }

    // 获取用户ID
    int getUserId() const { return userId; }

    // 获取设备ID
    int getDeviceId() const { return deviceId; }

    // 获取日志内容
    const std::string &getContent() const { return content; }

    // 获取时间戳
    std::chrono::system_clock::time_point getTimestamp() const { return timestamp; }

private:
    LogType logType;   // 日志类型
    int userId;        // 用户ID
    int deviceId;      // 设备ID
    std::string content; // 日志内容
    std::chrono::system_clock::time_point timestamp; // 时间戳
};

// 定义一个名为LogManager的类，用于管理日志
class LogManager {
public:
    void saveLogsToFile();
    void log(const std::string& message);
private:
    std::vector<std::string> logs; // 存储所有日志的容器
};

#endif // LOG_H
