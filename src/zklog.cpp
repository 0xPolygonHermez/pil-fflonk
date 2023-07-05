#include "zklog.hpp"
#include "utils.hpp"

zkLog zklog;

#include "logger.hpp"
using namespace CPlusPlusLogging;

zkLog::zkLog()
{
    Logger::getInstance()->enableConsoleLogging();
    Logger::getInstance()->updateLogLevel(LOG_LEVEL_DEBUG);
}

void zkLog::setPrefix ( const string &_prefix)
{
    prefix = _prefix;
}

void zkLog::info (const string &message)
{
    //cout << getTimestamp() << " " << prefix << message << endl;
    string tmp = prefix + " " + message;
    LOG_INFO(tmp);
}

void zkLog::warning (const string &message)
{
    // cout << getTimestamp() << " " << prefix << "zkWarning: " << message << endl;
    string tmp = prefix + " " + message;
    LOG_ALARM(tmp);
}

void zkLog::error (const string &message)
{
    // cerr << getTimestamp() << " " << prefix << "zkError: " << message << endl << flush;
    string tmp = prefix + " " + message;
    LOG_ERROR(tmp);
}