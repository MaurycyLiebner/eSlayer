#ifndef EEXCEPTIONS_H
#define EEXCEPTIONS_H

#include "eSlayerHelpers/eslayerhelpersexport.h"

#include <functional>
#include <string>

#define eFILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define eRuntimeThrow(msg) \
{ \
    std::throw_with_nested( \
        std::runtime_error( \
            std::to_string(__LINE__) + "  :  " + \
            eFILENAME + "  :  " + __func__ + "()\n  " + msg \
        ) \
    ); \
}

using eLogger = std::function<void(const std::string& msg)>;

class ESLAYERHELPERS_API eExceptions {
public:
    static std::string fullMsg(const std::exception& e);
    static void logError(const std::string& msg);
    static void logError(const std::string& msg,
                         const std::string& err);
    static void setLogger(const eLogger& logger);
private:
    static eLogger mLogger;
};

#endif // EEXCEPTIONS_H
