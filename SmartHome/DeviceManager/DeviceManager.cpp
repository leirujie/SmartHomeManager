#include "DeviceManager.h"
#include <iostream>
#include <sqlite3.h>

// 定义 DeviceManager 构造函数，初始化数据库连接
DeviceManager::DeviceManager() {
    initDB();
}

// 定义 DeviceManager 析构函数，关闭数据库连接
DeviceManager::~DeviceManager() {
    closeDB();
}

// 初始化数据库，打开数据库文件并创建 Devices 表（如果表不存在）
void DeviceManager::initDB() {
    sqlite3_open("devices.db", &db); // 打开数据库文件 devices.db
    const char *sql = "CREATE TABLE IF NOT EXISTS Devices ("
                      "id INTEGER PRIMARY KEY," // 设备ID，主键
                      "type INTEGER NOT NULL," // 设备类型，非空
                      "status TEXT NOT NULL," // 设备状态，非空
                      "info TEXT" // 设备信息
                      ");";
    sqlite3_exec(db, sql, nullptr, nullptr, nullptr); // 执行 SQL 语句
}

// 关闭数据库连接
void DeviceManager::closeDB() {
    sqlite3_close(db); // 关闭数据库
}

// 添加设备到内存和数据库中
void DeviceManager::addDevice(const std::shared_ptr<Device> &device) {
    int deviceId = device->getId();
    if (deviceExistsInDB(deviceId)) { // 调用函数检查数据库中是否存在该设备ID
        std::cout << "设备ID " << deviceId << " 已存在，添加失败。" << std::endl; // 输出错误信息
        return; // 返回，不再添加设备
    }
    if (devices.find(deviceId) != devices.end()) {
        std::cout << "设备ID " << deviceId << " 已存在，添加失败。" << std::endl; // 输出错误信息
        return; // 返回，不再添加设备
    }
    devices[deviceId] = device; // 将设备加入内存中的 devices 容器
    std::cout << "设备添加成功: ID " << deviceId << std::endl; // 输出设备添加信息
    insertDeviceIntoDB(device); // 将设备信息插入到数据库中
}

// 检查数据库中是否存在该设备ID：
bool DeviceManager::deviceExistsInDB(int deviceId) {
    std::string sql = "SELECT id FROM Devices WHERE id=" + std::to_string(deviceId) + ";"; // 构建 SQL 查询语句
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr); // 准备 SQL 语句
    int rc = sqlite3_step(stmt); // 执行查询
    bool exists = (rc == SQLITE_ROW); // 查询结果是否存在
    sqlite3_finalize(stmt); // 释放 SQL 语句资源
    return exists; // 返回查询结果
}

// 将设备信息插入到数据库中
void DeviceManager::insertDeviceIntoDB(const std::shared_ptr<Device> &device) {
    std::string sql = "INSERT INTO Devices (id, type, status, info) VALUES ("
                      + std::to_string(device->getId()) + ", " // 设备ID
                      + std::to_string(static_cast<int>(device->getType())) + ", '" // 设备类型，转换为整型
                      + device->getStatus() + "', '" // 设备状态
                      + device->getInfo() + "');"; // 设备信息
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr); // 执行 SQL 插入语句
}

// 从内存和数据库中移除设备
void DeviceManager::removeDevice(int deviceId) {
    auto it = devices.find(deviceId); // 查找设备
    if (it != devices.end()) {
        devices.erase(it); // 从内存中移除设备
        std::cout << "设备删除成功: " << deviceId << std::endl; // 输出设备移除信息
        removeDeviceFromDB(deviceId); // 从数据库中移除设备
    } else {
        std::cout << "没有找到该设备ID: " << deviceId << std::endl; // 输出未找到设备信息
    }
}

// 从数据库中移除设备
void DeviceManager::removeDeviceFromDB(int deviceId) {
    std::string sql = "DELETE FROM Devices WHERE id=" + std::to_string(deviceId) + ";"; // 构建 SQL 删除语句
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr); // 执行 SQL 删除语句
}

// 更新设备状态，并同步到数据库
void DeviceManager::updateDeviceStatus(int deviceId, const std::string &newStatus) {
    auto it = devices.find(deviceId); // 查找设备
    if (it != devices.end()) {
        it->second->setStatus(newStatus); // 更新设备状态
        std::cout << "设备状态更新成功: " << deviceId << ": " << newStatus << std::endl; // 输出状态更新信息
        updateDeviceStatusInDB(deviceId, newStatus); // 同步状态更新到数据库
    } else {
        std::cout << "没有找到该设备ID: " << deviceId << std::endl; // 输出未找到设备信息
    }
}

// 更新数据库中设备的状态
void DeviceManager::updateDeviceStatusInDB(int deviceId, const std::string &newStatus) {
    std::string sql = "UPDATE Devices SET status='" + newStatus + "' WHERE id=" + std::to_string(deviceId) + ";"; // 构建 SQL 更新语句
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr); // 执行 SQL 更新语句
}

// 执行设备控制命令
void DeviceManager::controlDevice(int deviceId, const std::string &command) {
    auto it = devices.find(deviceId); // 查找设备
    if (it != devices.end()) {
        it->second->control(command); // 执行控制命令
        std::cout << "设备控制成功: " << deviceId << ": " << command << std::endl; // 输出控制命令信息
    } else {
        std::cout << "没有找到该设备ID: " << deviceId << std::endl; // 输出未找到设备信息
    }
}

// 从数据库中获取所有设备的信息，并返回设备列表
std::vector<std::shared_ptr<Device>> DeviceManager::getAllDevices() const {
    std::vector<std::shared_ptr<Device>> deviceList; // 存储设备列表
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, type, status, info FROM Devices;"; // 构建 SQL 查询语句
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr); // 准备 SQL 语句
    int rc;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) { // 执行查询并遍历结果
        int id = sqlite3_column_int(stmt, 0); // 获取设备ID
        int typeInt = sqlite3_column_int(stmt, 1); // 获取设备类型（整型）
        Device::DeviceType type = static_cast<Device::DeviceType>(typeInt); // 转换为设备类型枚举
        const char *status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)); // 获取设备状态
        const char *info = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)); // 获取设备信息
        auto device = DeviceFactory::createDevice(id, type); // 创建设备对象
        if (device) {
            device->setStatus(status); // 设置设备状态
            // 假设你不需要设置 info，只用于显示
            // 如果你需要设置 info，确保 Device 类中有 setInfo 函数
            deviceList.push_back(device); // 将设备添加到列表中
        }
    }
    sqlite3_finalize(stmt); // 释放 SQL 语句资源
    return deviceList; // 返回设备列表
}

// 对所有设备执行批量控制命令
void DeviceManager::batchControl(const std::string &command) {
    for (auto& pair : devices) { // 遍历所有设备
        pair.second->control(command); // 执行控制命令
    }
    std::cout << "批量控制成功: " << command << std::endl; // 输出批量控制命令信息
}

// 根据设备ID获取设备对象
std::shared_ptr<Device> DeviceManager::getDevice(int deviceId) {
    auto it = devices.find(deviceId); // 查找设备
    if (it != devices.end()) {
        return it->second; // 返回设备对象
    } else {
        std::cout << "没有找到该设备ID: " << deviceId << std::endl; // 输出未找到设备信息
        return nullptr; // 返回空指针
    }
}

// 设备管理菜单，提供用户交互功能
void DeviceManagerMenu(DeviceManager &deviceManager) {
    int choice;
    int id;
    std::string command;
    std::shared_ptr<Device> device;
    Device::DeviceType type;
    std::vector<std::shared_ptr<Device>> deviceList;

    while (true) {
        std::cout << "\nDevice Manager Menu:\n" // 输出菜单选项
                  << "1. 添加设备\n" // 添加设备
                  << "2. 删除设备\n" // 移除设备
                  << "3. 获取设备信息\n" // 获取设备信息
                  << "4. 更新设备状态\n" // 更新设备状态
                  << "5. 控制设备\n" // 控制设备
                  << "6. 显示所有设备\n" // 显示所有设备
                  << "7. 批量控制设备\n" // 批量控制设备
                  << "8. 退出\n" // 退出菜单
                  << "请输入选项: ";
        std::cin >> choice; // 获取用户选择

        switch (choice) {
            case 1:
                std::cout << "请输入设备ID: ";
                std::cin >> id; // 获取设备ID
                std::cout << "输入设备类型 (0: Light, 1: Thermostat, 2: Camera, 3: Socket): ";
                int typeInt;
                std::cin >> typeInt; // 获取设备类型
                type = static_cast<Device::DeviceType>(typeInt); // 转换为设备类型枚举
                device = DeviceFactory::createDevice(id, type); // 创建设备对象
                if (device) {
                    deviceManager.addDevice(device); // 添加设备到管理器
                } else {
                    std::cout << "无效的设备类型" << std::endl; // 输出无效设备类型信息
                }
                break;
            case 2:
                std::cout << "请输入设备ID: ";
                std::cin >> id; // 获取设备ID
                deviceManager.removeDevice(id); // 移除设备
                break;
            case 3:
                std::cout << "请输入设备ID: ";
                std::cin >> id; // 获取设备ID
                device = deviceManager.getDevice(id); // 获取设备对象
                if (device) {
                    std::cout << device->getInfo() << std::endl; // 输出设备信息
                }
                break;
            case 4:
                std::cout << "请输入设备ID: ";
                std::cin >> id; // 获取设备ID
                std::cout << "输入新状态 (on/off/adjusted): ";
                std::cin >> command; // 获取新状态
                deviceManager.updateDeviceStatus(id, command); // 更新设备状态
                break;
            case 5:
                std::cout << "请输入设备ID: ";
                std::cin >> id; // 获取设备ID
                std::cout << "输入控制命令 (on/off/brightnessXX/resolutionXX/setXX): ";
                std::cin >> command; // 获取控制命令
                deviceManager.controlDevice(id, command); // 控制设备
                break;
            case 6:
                deviceList = deviceManager.getAllDevices(); // 获取所有设备
                std::cout << "Devices List:\n";
                for (auto& dev : deviceList) {
                    std::cout << "Device ID: " << dev->getId() // 输出设备ID
                              << ", Type: " << static_cast<int>(dev->getType()) // 输出设备类型
                              << ", Status: " << dev->getStatus() // 输出设备状态
                              << ", Info: " << dev->getInfo() << std::endl; // 输出设备信息
                }
                break;
            case 7:
                std::cout << "请输入批量控制命令 (on/off): ";
                std::cin >> command; // 获取批量控制命令
                deviceManager.batchControl(command); // 批量控制设备
                break;
            case 8:
                return; // 退出菜单
            default:
                std::cout << "无效的选择，请重新输入：" << std::endl; // 输出无效选择信息
        }
    }
}
