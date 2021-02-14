#ifndef CNSLOGGER_H
#define CNSLOGGER_H

#include <log4cplus/loggingmacros.h>
#include <log4cplus/logger.h>
#include <QTextEdit>
#include <log4cplus/initializer.h>

class CnsLogger
{
public:
    CnsLogger();
    ~CnsLogger();
    log4cplus::Logger _cnsLogger;
    inline log4cplus::Logger& logger()
    {
        return _cnsLogger;
    }
    void addTextEditAppender(QTextEdit* te);
    log4cplus::Initializer initializer;
};
extern CnsLogger *gCnsLogger;

#define CNSLOG_DEBUG(logEvent) \
    LOG4CPLUS_DEBUG (gCnsLogger->logger(), logEvent.toStdString().c_str())
#define CNSLOG_INFO(logEvent) \
    LOG4CPLUS_INFO (gCnsLogger->logger(), logEvent.toStdString().c_str())
#define CNSLOG_WARN(logEvent) \
    LOG4CPLUS_WARN (gCnsLogger->logger(), logEvent.toStdString().c_str())
#define CNSLOG_ERROR(logEvent) \
    LOG4CPLUS_ERROR (gCnsLogger->logger(), logEvent.toStdString().c_str())
#define CNSLOG_FATAL(logEvent) \
    LOG4CPLUS_FATAL (gCnsLogger->logger(), logEvent.toStdString().c_str())

#endif // CNSLOGGER_H
