#include "../include/eSlayerHelpers/eexceptions.h"

#include <exception>
#include <stdarg.h>

eLogger eExceptions::mLogger;
eDialogShower eExceptions::mDialogShower;

bool isExceptionNested(const std::exception& e) {
    if(auto ne = dynamic_cast<const std::nested_exception*>(std::addressof(e))) {
        if(ne->nested_ptr()) return true;
    }
    return false;
}

std::string eExceptions::fullMsg(const std::exception& e) {
    std::string result;
    try {
        if(isExceptionNested(e)) {
            std::rethrow_if_nested(e);
        }
    } catch(const std::exception& nested) {
        result += fullMsg(nested);
    } catch(...) {}
    return result + e.what() + "\n";
}

void eExceptions::logError(const std::string& msg) {
    if(mLogger) mLogger(msg);
}

void eExceptions::logError(const std::string& msg,
                           const std::string& err) {
    logError(msg + "\n" + err);
}

void eExceptions::showDialog(const std::string& msg) {
    if(mDialogShower) mDialogShower(msg);
}

void eExceptions::showDialog(const std::exception& e) {
    const auto msg = eExceptions::fullMsg(e);
    showDialog(msg);
}

void eExceptions::setLogger(const eLogger& logger) {
    mLogger = logger;
}

void eExceptions::setDialogShower(const eDialogShower& dialogShower) {
    mDialogShower = dialogShower;
}
