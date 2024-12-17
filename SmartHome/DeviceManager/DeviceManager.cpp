#include "DeviceManager.h"
#include "../MenuManager/ShowMenu.h"

DeviceManager::DeviceManager()
{
    db = nullptr;
    errMsg = nullptr;
    pthread_mutex_init(&dbMutex, nullptr);
    initDB();
}

void DeviceManager::loadDevicesFromDB()
{
    pthread_mutex_lock(&dbMutex);
    devices.clear();

    int rc = sqlite3_open("DBPath/devices.db", &db);
    if (rc)
    {
        fprintf(stderr, "数据库打开失败: %s\n", sqlite3_errmsg(db));
        pthread_mutex_unlock(&dbMutex);
        return;
    }

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, "SELECT * FROM devices;", -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        pthread_mutex_unlock(&dbMutex);
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        int typeInt = sqlite3_column_int(stmt, 1);
        Device::DeviceType type = static_cast<Device::DeviceType>(typeInt);
        const char *status = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        const char *info = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));

        auto device = DeviceFactory::createDevice(id, type);
        if (device)
        {
            device->setStatus(status);
            device->setInfo(info);
            devices[id] = device; // Add the device to the map
        }
    }

    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    pthread_mutex_unlock(&dbMutex);
}

DeviceManager::~DeviceManager()
{
    pthread_mutex_destroy(&dbMutex);
    if (db != nullptr)
    {
        sqlite3_close(db);
    }
}

void DeviceManager::initDB()
{
    sqlite3_open("DBPath/devices.db", &db);
    const char *sql = "CREATE TABLE IF NOT EXISTS Devices ("
                      "id INTEGER PRIMARY KEY,"
                      "type INTEGER NOT NULL,"
                      "status TEXT NOT NULL,"
                      "info TEXT"
                      ");";
    sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
}

void DeviceManager::closeDB()
{
    sqlite3_close(db);
}

void DeviceManager::addDevice(const std::shared_ptr<Device> &device)
{
    loadDevicesFromDB();
    int deviceId = device->getId();
    if (deviceExistsInDB(deviceId))
    {
        std::cout << "设备ID " << deviceId << " 已存在，添加失败。" << std::endl;
        return;
    }
    if (devices.find(deviceId) != devices.end())
    {
        std::cout << "设备ID " << deviceId << " 已存在，添加失败。" << std::endl;
        return;
    }
    devices[deviceId] = device;
    std::cout << "设备添加成功: ID " << deviceId << std::endl;
    insertDeviceIntoDB(device);
}

bool DeviceManager::deviceExistsInDB(int deviceId)
{
    std::string sql = "SELECT id FROM Devices WHERE id=" + std::to_string(deviceId) + ";";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    int rc = sqlite3_step(stmt);
    bool exists = (rc == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
}

void DeviceManager::insertDeviceIntoDB(const std::shared_ptr<Device> &device)
{
    std::string sql = "INSERT INTO Devices (id, type, status, info) VALUES (" + std::to_string(device->getId()) + ", " + std::to_string(static_cast<int>(device->getType())) + ", '" + device->getStatus() + "', '" + device->getInfo() + "');";
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

void DeviceManager::removeDevice(int deviceId)
{
    loadDevicesFromDB();
    auto it = devices.find(deviceId);
    if (it != devices.end())
    {
        devices.erase(it);
        std::cout << "设备删除成功: " << deviceId << std::endl;
        removeDeviceFromDB(deviceId);
    }
    else
    {
        std::cout << "没有找到该设备ID: " << deviceId << std::endl;
    }
}

void DeviceManager::removeDeviceFromDB(int deviceId)
{
    std::string sql = "DELETE FROM Devices WHERE id=" + std::to_string(deviceId) + ";";
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

void DeviceManager::updateDeviceStatus(int deviceId, const std::string &newStatus)
{
    loadDevicesFromDB();
    auto it = devices.find(deviceId);
    if (it != devices.end())
    {
        it->second->setStatus(newStatus);
        std::cout << "设备状态更新成功: " << deviceId << ": " << newStatus << std::endl;
        updateDeviceStatusInDB(deviceId, newStatus);
    }
    else
    {
        std::cout << "没有找到该设备ID: " << deviceId << std::endl;
    }
}

void DeviceManager::updateDeviceStatusInDB(int deviceId, const std::string &newStatus)
{
    std::string sql = "UPDATE Devices SET status='" + newStatus + "' WHERE id=" + std::to_string(deviceId) + ";";
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

void DeviceManager::controlDevice(int deviceId, const std::string &command)
{
    loadDevicesFromDB();
    auto it = devices.find(deviceId);
    if (it != devices.end())
    {
        it->second->control(command);
        std::string newStatus = it->second->getStatus();
        std::cout << "设备控制成功: " << deviceId << ": " << command << std::endl;
        updateDeviceStatusInDB(deviceId, newStatus);
    }
    else
    {
        std::cout << "没有找到该设备ID: " << deviceId << std::endl;
    }
}

std::vector<std::shared_ptr<Device>> DeviceManager::getAllDevices()
{
    loadDevicesFromDB();
    std::vector<std::shared_ptr<Device>> deviceList;
    for (const auto &pair : devices)
    {
        deviceList.push_back(pair.second);
    }
    return deviceList;
}

std::shared_ptr<Device> DeviceManager::getDevice(int deviceId)
{
    loadDevicesFromDB();
    auto it = devices.find(deviceId);
    if (it != devices.end())
    {
        return it->second;
    }
    else
    {
        std::cout << "没有找到该设备ID: " << deviceId << std::endl;
        return nullptr;
    }
}

// 管理员设备管理菜单，提供设备的添加、删除、查看、更新状态、控制、显示所有设备等功能
void AdminDeviceManagerMenu(DeviceManager &deviceManager)
{
    int choice;
    int id;
    std::string command;
    std::shared_ptr<Device> device;
    Device::DeviceType type;
    std::vector<std::shared_ptr<Device>> deviceList;

    while (true)
    {
        std::cout << "1. 添加设备\n"
                  << "2. 删除设备\n"
                  << "3. 获取设备信息\n"
                  << "4. 更新设备状态\n"
                  << "5. 控制设备\n"
                  << "6. 显示所有设备\n"
                  << "7. 退出\n"
                  << "请输入选项: ";
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            std::cout << "请输入设备ID: ";
            std::cin >> id;
            std::cout << "输入设备类型 (0: Light, 1: Thermostat, 2: Camera, 3: Socket): ";
            int typeInt;
            std::cin >> typeInt;
            type = static_cast<Device::DeviceType>(typeInt);
            device = DeviceFactory::createDevice(id, type);
            if (device)
            {
                deviceManager.addDevice(device);
            }
            else
            {
                std::cout << "无效的设备类型" << std::endl;
            }
            break;

        case 2:
            std::cout << "请输入设备ID: ";
            std::cin >> id;
            deviceManager.removeDevice(id);
            break;
        case 3:
            std::cout << "请输入设备ID: ";
            std::cin >> id;
            device = deviceManager.getDevice(id);
            if (device)
            {
                std::cout << device->getInfo() << std::endl;
            }
            break;
        case 4:
            std::cout << "请输入设备ID: ";
            std::cin >> id;
            std::cout << "输入新状态: ";
            std::cin >> command;
            deviceManager.updateDeviceStatus(id, command);
            break;
        case 5:
            std::cout << "请输入设备ID: ";
            std::cin >> id;
            std::cout << "输入控制命令: ";
            std::cin >> command;
            deviceManager.controlDevice(id, command);
            break;
        case 6:
            deviceList = deviceManager.getAllDevices();
            std::cout << "设备列表:\n";
            for (auto &dev : deviceList)
            {
                std::cout << "设备ID: " << dev->getId()
                          << ", 设备类型: " << static_cast<int>(dev->getType())
                          << ", 设备状态: " << dev->getStatus()
                          << ", 设备信息: " << dev->getInfo() << std::endl;
            }
            break;
        case 7:
            menu1();
            return;
        default:
            std::cout << "无效的选择，请重新输入：" << std::endl;
        }
    }
}

// 用户设备管理菜单，提供设备的查看、控制和退出功能
void UserDeviceManagerMenu(DeviceManager &deviceManager)
{
    int choice;
    int id;
    std::string command;
    std::shared_ptr<Device> device;
    Device::DeviceType type;
    std::vector<std::shared_ptr<Device>> deviceList;

    while (true)
    {
        std::cout << "1. 控制设备\n"
                  << "2. 获取设备信息\n"
                  << "3. 退出\n"
                  << "请输入选项: ";
        std::cin >> choice;

        switch (choice)
        {
        case 1:
            std::cout << "请输入设备ID: ";
            std::cin >> id;
            std::cout << "输入控制命令: ";
            std::cin >> command;
            deviceManager.controlDevice(id, command);
            break;

        case 2:
            std::cout << "请输入设备ID: ";
            std::cin >> id;
            device = deviceManager.getDevice(id);
            if (device)
            {
                std::cout << device->getInfo() << std::endl;
            }
            break;

        case 3:
            UserMenu();
            return;
        default:
            std::cout << "无效的选择，请重新输入：" << std::endl;
        }
    }
}