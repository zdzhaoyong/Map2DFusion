#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>

//#define ENABLE_LOGGER

#ifdef ENABLE_LOGGER
#define LOG(C) Logger::instance()<<__FUNCTION__<<" "<<C<<std::endl;\
    Logger::instance().flush();
#else
#define LOG(C)
#endif

class Logger
{
public:
    Logger();
    static std::ofstream& instance()
    {
        static std::ofstream ofs("log.txt");
        return ofs;
    }
};

#endif // LOGGER_H
