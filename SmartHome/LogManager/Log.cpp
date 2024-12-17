#include "Log.h"

std::chrono::system_clock::time_point getTimestamp() { 
    auto timestamp = std::chrono::system_clock::now();
    return timestamp;
}

// 格式化时间戳为字符串
std::string formatTimestamp(std::chrono::system_clock::time_point timestamp) {
    std::time_t time_t_timestamp = std::chrono::system_clock::to_time_t(timestamp);
    std::tm* tm_time = std::localtime(&time_t_timestamp);
    std::ostringstream oss;
    oss << std::put_time(tm_time, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void LogManager::log(const std::string& message) {
    logs.push_back(message);
    std::cout << message << std::endl;
}

// 输出到控制台的日志消息格式为记录到.txt文件
void LogManager::saveLogsToFile() {
    std::ofstream ofs("LogPath/UserAction.txt", std::ios::app);
    for (auto& log : logs) {
        auto timestamp = getTimestamp();
        std::string formattedTime = formatTimestamp(timestamp);
        ofs << "[" << formattedTime <<"]" << " " << log << std::endl;
    }

    ofs.close();
}
