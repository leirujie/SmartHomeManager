#include <iostream>
#include "ShowMenu.h"
#include "../DataBaseManager/DataBaseManager.h"
#include "../UserManager/User.h"
#include "../DeviceManager/DeviceManager.h"
#include "../LogManager/Log.h"

int choice;
LogManager logManager;
DeviceManager deviceManager;
UserManager userManager;
std::string username, password, role, newUsername, newPassword, newRole;

void menu()
{
    std::cout << "********************************************" << std::endl;
    std::cout << "*                                          *" << std::endl;
    std::cout << "*                                          *" << std::endl;
    std::cout << "*           请输入您的用户模式：           *" << std::endl;
    std::cout << "*                                          *" << std::endl;
    std::cout << "*    *1.管理员模式  *2.用户模式  *3.退出   *" << std::endl;
    std::cout << "*                                          *" << std::endl;
    std::cout << "*                                          *" << std::endl;
    std::cout << "********************************************" << std::endl;

    std::cin >> choice;
    std::cin.ignore();
    bool isLoggedIn = false;

    switch (choice)
    {
    case 1:
        std::cout << "进入管理员模式：" << std::endl;

        while (!isLoggedIn)
        {
            std::cout << "请输入管理员名称: ";
            std::getline(std::cin, username);
            std::cout << "请输入管理员密码: ";
            std::getline(std::cin, password);

            isLoggedIn = userManager.loginUser(username, password);

            if (isLoggedIn)
            {
                std::cout << "登录成功!" << std::endl;
                logManager.log("User: " + username + " Message: " + "用户登录了系统");
                logManager.saveLogsToFile();
                menu1();
            }
            else
            {
                std::cout << "账号或密码错误，请重新输入." << std::endl;
            }
        }
        break;
    case 2:
        std::cout << "进入用户模式：" << std::endl;
        UserMenu();
        break;
    case 3:
        std::cout << "退出系统..." << std::endl;
        exit(0);
    default:
        std::cout << "无效的选项，请重新选择." << std::endl;
        break;
    }
}

void menu1()
{
    std::cout << "********************************************" << std::endl;
    std::cout << "*                                          *" << std::endl;
    std::cout << "*                                          *" << std::endl;
    std::cout << "*             请选择操作模式：             *" << std::endl;
    std::cout << "*                                          *" << std::endl;
    std::cout << "*    *1.用户操作   *2.设备操作   *3.退出   *" << std::endl;
    std::cout << "*                                          *" << std::endl;
    std::cout << "*                                          *" << std::endl;
    std::cout << "********************************************" << std::endl;

    std::cin >> choice;
    std::cin.ignore();

    switch (choice)
    {
    case 1:
        std::cout << "用户操作模式：" << std::endl;
        AdminMenu();
        break;
    case 2:
        std::cout << "设备操作模式：" << std::endl;
        AdminDeviceManagerMenu(deviceManager);
        break;
    case 3:
        menu();
        break;
    default:
        std::cout << "无效的选项，请重新选择." << std::endl;
        break;
    }
}

void AdminMenu()
{
    do
    {
        std::cout << "**********欢迎使用蜗牛智能家居系统**********" << std::endl;
        std::cout << "1. 添加用户" << std::endl;
        std::cout << "2. 删除用户" << std::endl;
        std::cout << "3. 修改用户" << std::endl;
        std::cout << "4. 退出" << std::endl;
        std::cout << "请选择一个选项: ";

        std::cin >> choice;
        std::cin.ignore();

        switch (choice)
        {
        case 1:
            std::cout << "添加用户：" << std::endl;
            std::cout << "请输入用户名: ";
            std::getline(std::cin, username);
            std::cout << "请输入密码: ";
            std::getline(std::cin, password);
            std::cout << "请输入角色(Admin or User): ";
            std::getline(std::cin, role);

            if (userManager.registerUser(username, password, role))
            {
                std::cout << "添加成功!" << std::endl;
                logManager.log("Message: " + username + "被添加到系统中");
                logManager.saveLogsToFile();
            }
            else
            {
                std::cout << "添加失败!" << std::endl;
            }
            break;

        case 2:
            std::cout << "删除用户：" << std::endl;
            std::cout << "请输入用户名: ";
            std::getline(std::cin, username);
            if (userManager.deleteUser(username))
            {
                std::cout << "删除成功!" << std::endl;
                logManager.log("Message: " + username + "从系统中删除");
                logManager.saveLogsToFile();
            }
            else
            {
                std::cout << "删除失败!" << std::endl;
            }
            break;

        case 3:
            std::cout << "修改用户：" << std::endl;
            modifyUser();
            break;

        case 4:
            menu1();
            break;
        default:
            std::cout << "无效的选项，请重新选择." << std::endl;
            break;
        }

    } while (choice != 4);
}

void modifyUser()
{
    std::cout << "1. 修改用户名" << std::endl;
    std::cout << "2. 修改用户密码" << std::endl;
    std::cout << "3. 修改用户角色" << std::endl;
    std::cout << "4. 退出" << std::endl;
    std::cout << "请选择一个选项: ";

    std::cin >> choice;
    std::cin.ignore();

    switch (choice)
    {
    case 1:
        std::cout << "修改用户名：" << std::endl;
        std::cout << "请输入原用户名: ";
        std::getline(std::cin, username);
        
        std::cout << "请输入新用户名: ";
        std::getline(std::cin, newUsername);

        if (userManager.modifyUserName(username, newUsername))
        {
            std::cout << "修改成功!" << std::endl;

            logManager.log("User: " + username + " Message: " + "该用户名被修改为" + newUsername);
            logManager.saveLogsToFile();
        }
        else
        {
            std::cout << "修改失败! 用户不存在或新用户名已存在。" << std::endl;
        }
        break;

    case 2:
        std::cout << "修改用户密码：" << std::endl;
        std::cout << "请输入用户名: ";
        std::getline(std::cin, username);
        std::cout << "请输入新密码: ";
        std::getline(std::cin, newPassword);

        if (userManager.modifyUserPassword(username, newPassword)){
            std::cout << "修改成功!" << std::endl;
            logManager.log("User: " + username + " Message: " + "该用户密码被修改为" + newPassword);
            logManager.saveLogsToFile();
        }
        else
        {
            std::cout << "修改失败! 用户不存在。" << std::endl;
        }
        break;

    case 3:
        std::cout << "修改用户角色：" << std::endl;
        std::cout << "请输入用户名: ";
        std::getline(std::cin, username);
        std::cout << "请输入新角色: ";
        std::getline(std::cin, newRole);

        if (userManager.modifyUserRole(username, newRole))
        {
            std::cout << "修改成功!" << std::endl;
            logManager.log("User: " + username + " Message: " + "该用户角色被修改为" + newRole);
            logManager.saveLogsToFile();
        }
        else
        {
            std::cout << "修改失败! 用户不存在或新角色无效。" << std::endl;
        }
        break;


    case 4:
        AdminMenu();
        break;

    default:
        std::cout << "无效的选项，请重新选择." << std::endl;
        break;
    }
}

void UserMenu()
{
    do {
        std::cout << "**********欢迎使用蜗牛智能家居系统**********" << std::endl;
        std::cout << "1. 注册用户" << std::endl;
        std::cout << "2. 登录用户" << std::endl;
        std::cout << "3. 退出系统" << std::endl;
        std::cout << "请选择一个选项: ";

        std::cin >> choice;
        std::cin.ignore();

        switch (choice) {
        case 1:
            std::cout << "请输入用户名: ";
            std::getline(std::cin, username);
            std::cout << "请输入密码: ";
            std::getline(std::cin, password);
            role = "User";

            if (userManager.registerUser(username, password, role)) {
                std::cout << "注册成功!" << std::endl;

                logManager.log("Message: " + username + "被添加到系统中");
                logManager.saveLogsToFile();
            } else {
                std::cout << "注册失败!" << std::endl;
            }
            break;

        case 2:
            std::cout << "请输入用户名: ";
            std::getline(std::cin, username);
            std::cout << "请输入密码: ";
            std::getline(std::cin, password);

            if (userManager.loginUser(username, password)) {
                std::cout << "登录成功!" << std::endl;

                logManager.log("User: " + username + " Message: " + "用户登录了系统");
                logManager.saveLogsToFile();

                DeviceManager deviceManager;
                UserDeviceManagerMenu(deviceManager);
            } else {
                std::cout << "登录失败!" << std::endl;
            }
            break;

        case 3:
            menu();
            break;

        default:
            std::cout << "无效的选项，请重新选择." << std::endl;
            break;
        }
    } while (choice != 3);
}
