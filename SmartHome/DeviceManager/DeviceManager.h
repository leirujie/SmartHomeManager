#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <sqlite3.h>

// 定义一个设备基类
class Device
{
public:
    // 定义设备类型枚举类
    enum class DeviceType
    {
        Light,      // 灯光
        Thermostat, // 温控器
        Camera,     // 摄像头
        Socket      // 插座
    };
    // 构造函数，初始化设备ID、类型和状态
    Device(int id, DeviceType type) : id(id), type(type), status("off") {}
    // 虚析构函数，确保派生类的析构函数被正确调用
    virtual ~Device() = default;
    // 获取设备ID
    int getId() const { return id; }
    // 获取设备类型
    DeviceType getType() const { return type; }
    // 获取设备状态
    const std::string &getStatus() const { return status; }
    // 设置设备状态
    void setStatus(const std::string &newStatus) { status = newStatus; }
    // 控制设备的虚函数，需要在派生类中实现
    virtual void control(const std::string &command) = 0;
    // 获取设备信息的虚函数，需要在派生类中实现
    virtual std::string getInfo() const = 0;

protected:
    int id;             // 设备ID
    DeviceType type;    // 设备类型
    std::string status; // 设备状态
};

// 定义灯光类，继承自Device基类
class Light : public Device
{
public:
    // 构造函数，初始化灯光的ID和亮度
    Light(int id) : Device(id, DeviceType::Light), brightness(0) {}
    // 重写控制函数，处理灯光的开关和亮度调整
    void control(const std::string &command) override {
        if (command == "on") {
            setStatus("on");
        } else if (command == "off") {
            setStatus("off");
        } else if (command.find("brightness") != std::string::npos) {
            brightness = std::stoi(command.substr(9));
            setStatus("adjusted");
        }
    }
    // 重写获取信息函数，返回灯光的详细信息
    std::string getInfo() const override {
        return "Light, Brightness: " + std::to_string(brightness);
    }

private:
    int brightness; // 灯光亮度
};

// 定义温控器类，继承自Device基类
class Thermostat : public Device
{
public:
    // 构造函数，初始化温控器的ID、当前温度和目标温度
    Thermostat(int id) : Device(id, DeviceType::Thermostat), currentTemperature(20), targetTemperature(20) {}
    // 重写控制函数，处理温控器的开关和目标温度设置
    void control(const std::string &command) override {
        if (command == "on") {
            setStatus("on");
        } else if (command == "off") {
            setStatus("off");
        } else if (command.find("set") != std::string::npos) {
            targetTemperature = std::stoi(command.substr(4));
            setStatus("adjusted");
        }
    }
    // 重写获取信息函数，返回温控器的详细信息
    std::string getInfo() const override {
        return "Thermostat, Current Temperature: " + std::to_string(currentTemperature) + 
               ", Target Temperature: " + std::to_string(targetTemperature);
    }

private:
    int currentTemperature; // 当前温度
    int targetTemperature;  // 目标温度
};

// 定义摄像头类，继承自Device基类
class Camera : public Device
{
public:
    // 构造函数，初始化摄像头的ID和分辨率
    Camera(int id) : Device(id, DeviceType::Camera), resolution("1080p") {}
    // 重写控制函数，处理摄像头的开关和分辨率设置
    void control(const std::string &command) override {
        if (command == "on") {
            setStatus("on");
        } else if (command == "off") {
            setStatus("off");
        } else if (command.find("resolution") != std::string::npos) {
            resolution = command.substr(11);
            setStatus("adjusted");
        }
    }
    // 重写获取信息函数，返回摄像头的详细信息
    std::string getInfo() const override {
        return "Camera, Resolution: " + resolution;
    }

private:
    std::string resolution; // 摄像头分辨率
};

// 定义插座类，继承自Device基类
class Socket : public Device
{
public:
    // 构造函数，初始化插座的ID
    Socket(int id) : Device(id, DeviceType::Socket) {}
    // 重写控制函数，处理插座的开关
    void control(const std::string &command) override {
        if (command == "on") {
            setStatus("on");
        } else if (command == "off") {
            setStatus("off");
        }
    }
    // 重写获取信息函数，返回插座的详细信息
    std::string getInfo() const override {
        return "Socket";
    }
};

// 定义设备工厂类，用于创建不同类型的设备
class DeviceFactory
{
public:
    // 静态方法，根据设备类型创建设备实例
    static std::shared_ptr<Device> createDevice(int id, Device::DeviceType type) {
        switch (type) {
            case Device::DeviceType::Light:
                return std::make_shared<Light>(id);
            case Device::DeviceType::Thermostat:
                return std::make_shared<Thermostat>(id);
            case Device::DeviceType::Camera:
                return std::make_shared<Camera>(id);
            case Device::DeviceType::Socket:
                return std::make_shared<Socket>(id);
            default:
                return nullptr;
        }
    }
};

// 定义设备管理类，用于管理所有设备
class DeviceManager {
public:
    // 构造函数，初始化设备管理器
    DeviceManager();
    // 析构函数，关闭数据库连接
    ~DeviceManager();
    // 添加设备到管理器
    void addDevice(const std::shared_ptr<Device> &device);
    // 根据设备ID移除设备
    void removeDevice(int deviceId);
    // 根据设备ID获取设备
    std::shared_ptr<Device> getDevice(int deviceId);
    // 更新设备状态
    void updateDeviceStatus(int deviceId, const std::string &newStatus);
    // 控制设备
    void controlDevice(int deviceId, const std::string &command);
    // 获取所有设备
    std::vector<std::shared_ptr<Device>> getAllDevices() const;
    // 批量控制设备
    void batchControl(const std::string &command);

private:
    std::map<int, std::shared_ptr<Device>> devices; // 存储所有设备的映射
    sqlite3 *db;                                    // 数据库连接指针

    // 初始化数据库连接
    void initDB();
    // 关闭数据库连接
    void closeDB();
    bool deviceExistsInDB(int deviceId);
    // 将设备插入到数据库
    void insertDeviceIntoDB(const std::shared_ptr<Device> &device);
    // 从数据库中移除设备
    void removeDeviceFromDB(int deviceId);
    // 更新数据库中的设备状态
    void updateDeviceStatusInDB(int deviceId, const std::string &newStatus);
    // 在数据库中控制设备
    void controlDeviceInDB(int deviceId, const std::string &command);
};

// 设备管理器菜单函数，用于与用户交互
void DeviceManagerMenu(DeviceManager &deviceManager);

#endif // DEVICEMANAGER_H
