#include <iostream>
#include "DataBaseManager/DataBaseManager.h"
#include "UserManager/User.h"
#include "DeviceManager/DeviceManager.h"

// 创建系统菜单
void showMenu()
{
    std::cout << "**********欢迎使用蜗牛智能家居系统**********" << std::endl;
    std::cout << "1. 注册用户" << std::endl;
    std::cout << "2. 登录用户" << std::endl;
    std::cout << "3. 退出系统" << std::endl;
    std::cout << "请选择一个选项: ";
}

int main()
{
    UserManager userManager;
    int choice;
    std::string username, password, role;

    do
    {
        showMenu();
        std::cin >> choice;
        std::cin.ignore(); // 忽略换行符

        switch (choice)
        {
        case 1:
            std::cout << "请输入用户名: ";
            std::getline(std::cin, username);
            std::cout << "请输入密码: ";
            std::getline(std::cin, password);
            std::cout << "请输入角色: ";
            std::getline(std::cin, role);
            if (userManager.registerUser(username, password, role))
            {
                std::cout << "注册成功!" << std::endl;
            }
            else
            {
                std::cout << "注册失败!" << std::endl;
            }
            break;
        case 2:
            std::cout << "请输入用户名: ";
            std::getline(std::cin, username);
            std::cout << "请输入密码: ";
            std::getline(std::cin, password);
            if (userManager.loginUser(username, password))
            {
                std::cout << "登录成功!" << std::endl;
                DeviceManager deviceManager;
                DeviceManagerMenu(deviceManager);
            }
            else
            {
                std::cout << "登录失败!" << std::endl;
            }
            break;
        case 3:
            std::cout << "退出系统!" << std::endl;
            break;
        default:
            std::cout << "无效的选项，请重新选择." << std::endl;
            break;
        }
    } while (choice != 3);

    return 0;
}
