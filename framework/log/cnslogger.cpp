#include "cnslogger.h"

#include <log4cplus/fileappender.h>
#include <log4cplus/win32debugappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/ndc.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/property.h>
#include "qt5debugappender.h"
#include <log4cplus/mdc.h>
#include <QDate>
#include <log4cplus/helpers/timehelper.h>

CnsLogger *gCnsLogger = nullptr;
using namespace log4cplus;

CnsLogger::CnsLogger()
{
//    log4cplus::initialize();
    helpers::LogLog::getLogLog()->setInternalDebugging(true);
    helpers::Time now = helpers::truncate_fractions (helpers::now ());
    tchar const * pattern = LOG4CPLUS_TEXT("%Y-%m-%d");
    tstring result (LOG4CPLUS_TEXT("logs/cns.log."));
    result+=helpers::getFormattedTime(pattern, now, false);

    SharedFileAppenderPtr append_1(
        new DailyRollingFileAppender(result,
                               DailyRollingFileSchedule::DAILY, true,1,
                                     true, false));
    append_1->setName(LOG4CPLUS_TEXT("dailyRolling"));
//    log4cplus::getMDC ().put (LOG4CPLUS_TEXT ("key"),
//        LOG4CPLUS_TEXT ("MDC value"));
    pattern = LOG4CPLUS_TEXT("%d{%m/%d/%y %H:%M:%S,%Q} [%t] %-5p %c{2} %%%x%% - %X{key} - %m [%l]%n");
    append_1->setLayout( std::unique_ptr<Layout>(new PatternLayout(pattern)) );
//    append_1->getloc();
//    Logger::getRoot().addAppender(SharedAppenderPtr(append_1.get()));

    _cnsLogger = Logger::getInstance(LOG4CPLUS_TEXT("cnsLogger"));
    _cnsLogger.addAppender(SharedAppenderPtr(append_1.get()));

    log4cplus::SharedAppenderPtr append_2(
        new log4cplus::Win32DebugAppender());
    _cnsLogger.addAppender(append_2);
}

void CnsLogger::addTextEditAppender(QTextEdit *te)
{
    log4cplus::SharedAppenderPtr append_2(
        new log4cplus::Qt5DebugAppender(te));
    append_2->setName(LOG4CPLUS_TEXT("q5Tab"));
//    append_2->setLayout( std::unique_ptr<Layout>(new TTCCLayout()) );
    tchar const * pattern = LOG4CPLUS_TEXT("%d{%m/%d/%y %H:%M:%S,%Q} [%t] %-5p %c{2} %%%x%% - %X{key} - %m [%l]%n");
    append_2->setLayout( std::unique_ptr<Layout>(new PatternLayout(pattern)) );

    _cnsLogger.addAppender(SharedAppenderPtr(append_2.get()));
}

CnsLogger::~CnsLogger()
{
    log4cplus::Logger::shutdown();
}
