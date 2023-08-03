#ifndef ZKLOG_HPP
#define ZKLOG_HPP

#include <string>

using namespace std;

class zkLog
{
private:
    // Log prefix
    string prefix;

public:
    zkLog();
    void setPrefix(const string &_prefix);
    void info(const string &message);
    void warning(const string &message);
    void error(const string &message);
};

extern zkLog zklog;

#endif