# SmartHomeManager

#### 介绍

C++/智能家居管理系统

#### 软件架构

软件架构说明

```
SmartHomeManager：
├──
    ├── ConfigManager  // 配置管理模块
        ├── ConfigManager.h
        ├── ConfigManager.cpp
    ├── DataBaseManager  // 数据库管理模块
        ├── DataBaseManager.h
        ├── DataBaseManager.cpp
    ├── DBPath  // 数据库路径
        ├── devices.db
        ├── users.db
    ├── DeviceManager  // 设备管理模块
        ├── DeviceManager.h
        ├── DeviceManager.cpp
    ├── ExpectionManager  // 异常处理模块
        ├── ExceptionHandler.h
        ├── ExceptionHandler.cpp
    ├── LogManager  // 日志管理模块
        ├── Log.h
        ├── Log.cpp
    ├── LogPath  // 日志路径
        ├── UserAction.txt
    ├── MenuManager  // 菜单管理模块
        ├── ShowMenu.h
        ├── ShowMenu.cpp
    ├── ThreadManager  // 线程池管理模块
        ├── ThreadPool.h
        ├── ThreadPool.cpp
    ├── UserManager  // 用户管理模块
        ├── User.h
        ├── User.cpp
    ├── main.cpp // 程序运行主函数
    ├── makefile
    ├── README.md
```

#### 使用说明

1.  admin为管理员账号，密码为<root123>，登录后可以进行设备管理、用户管理、日志管理等操作.
2.  普通用户账号密码需要自己注册，登录后可以查看设备状态、控制设备.
3.  设备控制命令格式：on/off/bright/20.

#### 完成进度

1.  配置管理模块：未完成
2.  数据库管理模块：完成
3.  设备管理模块：完成2/3
4.  异常处理模块：未完成
5.  日志管理模块：完成1/3
6.  菜单管理模块：完成
7.  线程池管理模块：未完成
8.  用户管理模块：完成，部分细节待完善
9.  程序运行主函数：完成
