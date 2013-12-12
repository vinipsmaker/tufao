#ifndef MAIN_HPP_
#define MAIN_HPP_

#include <QDebug>
#include <stdlib.h>

// A macro that prints a message and exits the application.
#define printErrorAndExit(x, ...) qCritical(x, ##__VA_ARGS__);exit(1);

// The MondoDB namespaces
extern const char * NS_SERVER_SETTINGS;

#endif // MAIN_HPP_
