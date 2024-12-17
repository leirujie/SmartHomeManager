#include "DeviceManager.h" // 包含设备管理器的头文件
#include "../MenuManager/ShowMenu.h" // 包含菜单管理器的头文件

DeviceManager::DeviceManager() // 设备管理器类的构造函数
{
    db = nullptr; // 初始化数据库指针为空
    errMsg = nullptr; // 初始化错误消息指针为空
    pthread_mutex_init(&dbMutex, nullptr); // 初始化互斥锁
    initDB(); // 调用初始化数据库的方法
}

void DeviceManager::loadDevicesFromDB() // 从数据库加载设备的方法
{
    pthread_mutex_lock(&dbMutex); // 锁定互斥锁以确保线程安全
    devices.clear(); // 清空当前存储的设备列表

    int rc = sqlite3_open("DBPath/devices.db", &db); // 打开数据库文件
    if (rc) // 如果打开数据库失败
    {
        fprintf(stderr, "数据库打开失败: %s\n", sqlite3_errmsg(db)); // 打印错误信息
        pthread_mutex_unlock(&dbMutex); // 解锁互斥锁
        return; // 返回，不继续执行
    }

    sqlite3_stmt *stmt; // 声明一个sqlite3语句指针
    rc = sqlite3_prepare_v2(db, "SELECT * FROM devices;", -1, &stmt, nullptr); // 准备SQL查询语句
    if (rc != SQLITE_OK) // 如果准备SQL语句失败
    {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db)); // 打印错误信息
        pthread_mutex_unlock(&dbMutex); // 解锁互斥锁
        return; // 返回，不继续执行
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) // 循环遍历查询结果中的每一行
    {
        int id = sqlite3_column_int(stmt, 0); // 获取设备ID
        int typeInt = sqlite3_column_int(stmt, 1); // 获取设备类型（整数形式）
        Device::DeviceType type = static_cast<Device::DeviceType>(typeInt); // 将整数形式的设备类型转换为枚举类型
        const char *status = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2)); // 获取设备状态
        const char *info = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3)); // 获取设备信息

        auto device = DeviceFactory::createDevice(id, type); // 使用设备工厂根据ID和类型创建设备对象
        if (device) // 如果成功创建设备对象
        {
            device->setStatus(status); // 设置设备状态
            device->setInfo(info); // 设置设备信息
            devices[id] = device; // 将设备对象添加到设备列表中
        }
    }

    if (rc != SQLITE_DONE) // 如果SQL执行未完成（即存在错误）
    {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db)); // 打印错误信息
    }

    sqlite3_finalize(stmt); // 释放SQL查询语句的内存
    pthread_mutex_unlock(&dbMutex); // 解锁互斥锁
}

DeviceManager::~DeviceManager() // 设备管理器类的析构函数
{
    pthread_mutex_destroy(&dbMutex); // 销毁互斥锁
    if (db != nullptr) // 如果数据库指针不为空
    {
        sqlite3_close(db); // 关闭数据库连接
    }
}

void DeviceManager::initDB() // 初始化数据库的方法
{
    sqlite3_open("DBPath/devices.db", &db); // 打开数据库文件
    const char *sql = "CREATE TABLE IF NOT EXISTS Devices (" // 定义创建设备表的SQL语句
                      "id INTEGER PRIMARY KEY," // 设备ID作为主键
                      "type INTEGER NOT NULL," // 设备类型，不允许为空
                      "status TEXT NOT NULL," // 设备状态，不允许为空
                      "info TEXT" // 设备信息
                      ");";
    sqlite3_exec(db, sql, nullptr, nullptr, nullptr); // 执行SQL语句以创建设备表
}

void DeviceManager::closeDB() // 关闭数据库的方法
{
    sqlite3_close(db); // 关闭数据库连接
}

void DeviceManager::addDevice(const std::shared_ptr<Device> &device) // 添加设备的方法
{
    loadDevicesFromDB(); // 从数据库加载设备列表
    int deviceId = device->getId(); // 获取要添加的设备的ID
    if (deviceExistsInDB(deviceId)) // 检查设备ID是否已经存在于数据库中
    {
        std::cout << "设备ID " << deviceId << " 已存在，添加失败。" << std::endl; // 如果存在，打印错误信息
        return; // 返回，不继续执行
    }
    if (devices.find(deviceId) != devices.end()) // 检查设备ID是否已经存在于内存中的设备列表中
    {
        std::cout << "设备ID " << deviceId << " 已存在，添加失败。" << std::endl; // 如果存在，打印错误信息
        return; // 返回，不继续执行
    }
    devices[deviceId] = device; // 将设备添加到内存中的设备列表中
    std::cout << "设备添加成功: ID " << deviceId << std::endl; // 打印成功信息
    insertDeviceIntoDB(device); // 调用方法将设备插入数据库
}

bool DeviceManager::deviceExistsInDB(int deviceId) // 检查设备是否存在于数据库中的方法
{
    std::string sql = "SELECT id FROM Devices WHERE id=" + std::to_string(deviceId) + ";"; // 构建查询设备ID的SQL语句
    sqlite3_stmt *stmt; // 声明一个sqlite3语句指针
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr); // 准备SQL查询语句
    int rc = sqlite3_step(stmt); // 执行SQL查询语句
    bool exists = (rc == SQLITE_ROW); // 如果查询结果为一行，说明设备ID存在
    sqlite3_finalize(stmt); // 释放SQL查询语句的内存
    return exists; // 返回设备是否存在
}

void DeviceManager::insertDeviceIntoDB(const std::shared_ptr<Device> &device)
{
    // 构建SQL插入语句，将设备的id, type, status, info插入到Devices表中
    std::string sql = "INSERT INTO Devices (id, type, status, info) VALUES (" + std::to_string(device->getId()) + ", " + std::to_string(static_cast<int>(device->getType())) + ", '" + device->getStatus() + "', '" + device->getInfo() + "');";
    // 执行SQL语句
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

void DeviceManager::removeDevice(int deviceId)
{
    // 从数据库加载所有设备信息到内存中的devices容器
    loadDevicesFromDB();
    // 查找要删除的设备
    auto it = devices.find(deviceId);
    if (it != devices.end()) // 如果找到了设备
    {
        devices.erase(it); // 从内存中的devices容器删除该设备
        std::cout << "设备删除成功: " << deviceId << std::endl; // 输出删除成功的信息
        removeDeviceFromDB(deviceId); // 从数据库中删除该设备
    }
    else // 如果没有找到设备
    {
        std::cout << "没有找到该设备ID: " << deviceId << std::endl; // 输出未找到设备的信息
    }
}

void DeviceManager::removeDeviceFromDB(int deviceId)
{
    // 构建SQL删除语句，根据设备id删除Devices表中的记录
    std::string sql = "DELETE FROM Devices WHERE id=" + std::to_string(deviceId) + ";";
    // 执行SQL语句
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

void DeviceManager::updateDeviceStatus(int deviceId, const std::string &newStatus)
{
    // 从数据库加载所有设备信息到内存中的devices容器
    loadDevicesFromDB();
    // 查找要更新状态的设备
    auto it = devices.find(deviceId);
    if (it != devices.end()) // 如果找到了设备
    {
        it->second->setStatus(newStatus); // 更新内存中设备的状态
        std::cout << "设备状态更新成功: " << deviceId << ": " << newStatus << std::endl; // 输出状态更新成功的信息
        updateDeviceStatusInDB(deviceId, newStatus); // 更新数据库中设备的状态
    }
    else // 如果没有找到设备
    {
        std::cout << "没有找到该设备ID: " << deviceId << std::endl; // 输出未找到设备的信息
    }
}

void DeviceManager::updateDeviceStatusInDB(int deviceId, const std::string &newStatus)
{
    // 构建SQL更新语句，根据设备id更新Devices表中的status字段
    std::string sql = "UPDATE Devices SET status='" + newStatus + "' WHERE id=" + std::to_string(deviceId) + ";";
    // 执行SQL语句
    sqlite3_exec(db, sql.c_str(), nullptr, nullptr, nullptr);
}

void DeviceManager::controlDevice(int deviceId, const std::string &command)
{
    // 从数据库加载所有设备信息到内存中的devices容器
    loadDevicesFromDB();
    // 查找要控制的设备
    auto it = devices.find(deviceId);
    if (it != devices.end()) // 如果找到了设备
    {
        it->second->control(command); // 执行设备控制命令
        std::string newStatus = it->second->getStatus(); // 获取设备更新后的状态
        std::cout << "设备控制成功: " << deviceId << ": " << command << std::endl; // 输出设备控制成功的信息
        updateDeviceStatusInDB(deviceId, newStatus); // 更新数据库中设备的状态
    }
    else // 如果没有找到设备
    {
        std::cout << "没有找到该设备ID: " << deviceId << std::endl; // 输出未找到设备的信息
    }
}

std::vector<std::shared_ptr<Device>> DeviceManager::getAllDevices()
{
    // 从数据库加载所有设备信息到内存中的devices容器
    loadDevicesFromDB();
    std::vector<std::shared_ptr<Device>> deviceList; // 创建一个设备列表，用于存储所有设备的共享指针
    for (const auto &pair : devices) // 遍历devices容器中的所有设备
    {
        deviceList.push_back(pair.second); // 将每个设备的共享指针添加到设备列表中
    }
    return deviceList; // 返回设备列表
}

std::shared_ptr<Device> DeviceManager::getDevice(int deviceId)
{
    // 从数据库加载所有设备信息到内存中的devices容器
    loadDevicesFromDB();
    // 查找指定id的设备
    auto it = devices.find(deviceId);
    if (it != devices.end()) // 如果找到了设备
    {
        return it->second; // 返回该设备的共享指针
    }
    else // 如果没有找到设备
    {
        std::cout << "没有找到该设备ID: " << deviceId << std::endl; // 输出未找到设备的信息
        return nullptr; // 返回空指针
    }
}

// 管理员设备管理菜单函数定义，提供设备的添加、删除、查看、更新状态、控制、显示所有设备等功能
void AdminDeviceManagerMenu(DeviceManager &deviceManager)
{
    int choice; // 声明一个整数变量choice用于存储用户的选择
    int id; // 声明一个整数变量id用于存储设备的ID
    std::string command; // 声明一个字符串变量command用于存储用户输入的命令
    std::shared_ptr<Device> device; // 声明一个Device类的共享指针变量device用于存储设备对象
    Device::DeviceType type; // 声明一个DeviceType类型的变量type用于存储设备类型
    std::vector<std::shared_ptr<Device>> deviceList; // 声明一个存储设备共享指针的向量deviceList用于存储设备列表

    while (true) // 使用无限循环来持续显示菜单直到用户选择退出
    {
        std::cout << "1. 添加设备\n" // 输出菜单选项1：添加设备
                  << "2. 删除设备\n" // 输出菜单选项2：删除设备
                  << "3. 获取设备信息\n" // 输出菜单选项3：获取设备信息
                  << "4. 更新设备状态\n" // 输出菜单选项4：更新设备状态
                  << "5. 控制设备\n" // 输出菜单选项5：控制设备
                  << "6. 显示所有设备\n" // 输出菜单选项6：显示所有设备
                  << "7. 退出\n" // 输出菜单选项7：退出
                  << "请输入选项: "; // 提示用户输入选项
        std::cin >> choice; // 获取用户输入的选项

        switch (choice) // 使用switch语句根据用户的选择执行不同的操作
        {
        case 1: // 如果用户选择1，执行添加设备的操作
            std::cout << "请输入设备ID: "; // 提示用户输入设备ID
            std::cin >> id; // 获取用户输入的设备ID
            std::cout << "输入设备类型 (0: Light, 1: Thermostat, 2: Camera, 3: Socket): "; // 提示用户输入设备类型
            int typeInt; // 声明一个整数变量typeInt用于存储用户输入的设备类型
            std::cin >> typeInt; // 获取用户输入的设备类型
            type = static_cast<Device::DeviceType>(typeInt); // 将用户输入的设备类型转换为DeviceType枚举类型
            device = DeviceFactory::createDevice(id, type); // 使用DeviceFactory创建设备对象
            if (device) // 如果设备对象创建成功
            {
                deviceManager.addDevice(device); // 将设备对象添加到设备管理器中
            }
            else // 如果设备对象创建失败
            {
                std::cout << "无效的设备类型" << std::endl; // 输出错误信息：无效的设备类型
            }
            break; // 结束case 1

        case 2: // 如果用户选择2，执行删除设备的操作
            std::cout << "请输入设备ID: "; // 提示用户输入设备ID
            std::cin >> id; // 获取用户输入的设备ID
            deviceManager.removeDevice(id); // 从设备管理器中删除指定ID的设备
            break; // 结束case 2

        case 3: // 如果用户选择3，执行获取设备信息的操作
            std::cout << "请输入设备ID: "; // 提示用户输入设备ID
            std::cin >> id; // 获取用户输入的设备ID
            device = deviceManager.getDevice(id); // 从设备管理器中获取指定ID的设备
            if (device) // 如果设备对象获取成功
            {
                std::cout << device->getInfo() << std::endl; // 输出设备的信息
            }
            break; // 结束case 3

        case 4: // 如果用户选择4，执行更新设备状态的操作
            std::cout << "请输入设备ID: "; // 提示用户输入设备ID
            std::cin >> id; // 获取用户输入的设备ID
            std::cout << "输入新状态: "; // 提示用户输入新状态
            std::cin >> command; // 获取用户输入的新状态
            deviceManager.updateDeviceStatus(id, command); // 更新设备管理器中指定ID的设备状态
            break; // 结束case 4

        case 5: // 如果用户选择5，执行控制设备的操作
            std::cout << "请输入设备ID: "; // 提示用户输入设备ID
            std::cin >> id; // 获取用户输入的设备ID
            std::cout << "输入控制命令: "; // 提示用户输入控制命令
            std::cin >> command; // 获取用户输入的控制命令
            deviceManager.controlDevice(id, command); // 控制设备管理器中指定ID的设备
            break; // 结束case 5

        case 6: // 如果用户选择6，执行显示所有设备的操作
            deviceList = deviceManager.getAllDevices(); // 获取设备管理器中的所有设备
            std::cout << "设备列表:\n"; // 输出设备列表标题
            for (auto &dev : deviceList) // 遍历设备列表中的每个设备
            {
                std::cout << "设备ID: " << dev->getId() // 输出设备ID
                          << ", 设备类型: " << static_cast<int>(dev->getType()) // 输出设备类型
                          << ", 设备状态: " << dev->getStatus() // 输出设备状态
                          << ", 设备信息: " << dev->getInfo() << std::endl; // 输出设备信息
            }
            break; // 结束case 6

        case 7: // 如果用户选择7，执行退出操作
            menu1(); // 调用menu1函数返回上一级菜单
            return; // 从AdminDeviceManagerMenu函数返回
        default: // 如果用户输入的选项不在1到7之间，执行默认操作
            std::cout << "无效的选择，请重新输入：" << std::endl; // 输出错误信息：无效的选择，请重新输入
        }
    }
}

// 用户设备管理菜单函数定义，提供设备的查看、控制和退出功能
void UserDeviceManagerMenu(DeviceManager &deviceManager)
{
    int choice; // 声明一个整数变量choice用于存储用户的选择
    int id; // 声明一个整数变量id用于存储设备的ID
    std::string command; // 声明一个字符串变量command用于存储用户输入的命令
    std::shared_ptr<Device> device; // 声明一个Device类的共享指针变量device用于存储设备对象
    Device::DeviceType type; // 声明一个DeviceType类型的变量type用于存储设备类型
    std::vector<std::shared_ptr<Device>> deviceList; // 声明一个存储设备共享指针的向量deviceList用于存储设备列表

    while (true) // 使用无限循环来持续显示菜单直到用户选择退出
    {
        std::cout << "1. 控制设备\n" // 输出菜单选项1：控制设备
                  << "2. 获取设备信息\n" // 输出菜单选项2：获取设备信息
                  << "3. 退出\n" // 输出菜单选项3：退出
                  << "请输入选项: "; // 提示用户输入选项
        std::cin >> choice; // 获取用户输入的选项

        switch (choice) // 使用switch语句根据用户的选择执行不同的操作
        {
        case 1: // 如果用户选择1，执行控制设备的操作
            std::cout << "请输入设备ID: "; // 提示用户输入设备ID
            std::cin >> id; // 获取用户输入的设备ID
            std::cout << "输入控制命令: "; // 提示用户输入控制命令
            std::cin >> command; // 获取用户输入的控制命令
            deviceManager.controlDevice(id, command); // 控制设备管理器中指定ID的设备
            break; // 结束case 1

        case 2: // 如果用户选择2，执行获取设备信息的操作
            std::cout << "请输入设备ID: "; // 提示用户输入设备ID
            std::cin >> id; // 获取用户输入的设备ID
            device = deviceManager.getDevice(id); // 从设备管理器中获取指定ID的设备
            if (device) // 如果设备对象获取成功
            {
                std::cout << device->getInfo() << std::endl; // 输出设备的信息
            }
            break; // 结束case 2

        case 3: // 如果用户选择3，执行退出操作
            UserMenu(); // 调用UserMenu函数返回上一级菜单
            return; // 从UserDeviceManagerMenu函数返回
        default: // 如果用户输入的选项不在1到3之间，执行默认操作
            std::cout << "无效的选择，请重新输入：" << std::endl; // 输出错误信息：无效的选择，请重新输入
        }
    }
}
