#ifndef EXCEPTIONHANDLER_H
#define EXCEPTIONHANDLER_H

#include <exception>
#include <iostream>

class ExceptionHandler
{
public:
    static void handleException(const std::exception &e);
    static void logException(const std::exception &e);
};

#endif // EXCEPTIONHANDLER_H