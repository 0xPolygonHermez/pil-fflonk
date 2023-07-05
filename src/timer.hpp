#ifndef TIMER_HPP
#define TIMER_HPP

#include <cstdint>
#include <sys/time.h>
#include <string>
#include "zklog.hpp"
// #include "definitions.hpp"

// Returns the time difference in us
uint64_t TimeDiff(const struct timeval &startTime, const struct timeval &endTime);
uint64_t TimeDiff(const struct timeval &startTime); // End time is now

// Returns date and time in a string
std::string DateAndTime(struct timeval &tv);

// #ifdef LOG_TIME
#define TimerStart(name) struct timeval name##_start; gettimeofday(&name##_start,NULL); zklog.info("--> " + string(#name));
#define TimerStop(name) struct timeval name##_stop; gettimeofday(&name##_stop,NULL); zklog.info("<-- " + string(#name) + " done");
#define TimerLog(name) zklog.info(DateAndTime(name##_stop) + " " + string(#name) + ": " + double(TimeDiff(name##_start, name##_stop))/1000000 + " s");
#define TimerStopAndLog(name) struct timeval name##_stop; gettimeofday(&name##_stop,NULL);zklog.info("<-- " + string(#name) + string(" done: ") + to_string(double(TimeDiff(name##_start, name##_stop))/1000000) + " s");
// #else
// #define TimerStart(name)
// #define TimerStop(name)
// #define TimerLog(name)
// #define TimerStopAndLog(name)
// #endif

#endif