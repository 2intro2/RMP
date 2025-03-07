#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>
#include <ctime>
using namespace std;

class Logger
{
public:
    enum Level
    {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    static string getCurrentTime()
    {
        // 获取当前时间
        auto now = time(nullptr);
        // 将时间转换为本地时间
        auto localTime = localtime(&now);
        // 格式化时间
        char timeBuffer[80];
        strftime(timeBuffer, sizeof(timeBuffer), "%Y/%m/%d %H:%M:%S", localTime);

        return string(timeBuffer);
    }

    static string levelToString(Level level)
    {
        switch (level)
        {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        case FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
        }
    }

    static void output(const string &message, Level level)
    {
        string levelStr = levelToString(level);
        string colorCode = "";
        string resetCode = "\033[0m";

        if (level == WARNING)
            colorCode = "\033[33m"; // 黄色
        else if (level == ERROR)
            colorCode = "\033[31m"; // 红色

        cerr << colorCode << "[EMP][" << getCurrentTime() << "][" << levelStr << "]: " << message << resetCode << endl;
    }
};

#endif
