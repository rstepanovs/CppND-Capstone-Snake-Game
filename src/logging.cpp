#include "logging.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <map>
#include <chrono>
#include <ctime>
#include <iomanip>

std::mutex Logging::_mutex;
std::atomic<LogLevel> Logging::_level{LogLevel::kDebug};

namespace {
  std::map<LogLevel, std::string> level_names = {
    {LogLevel::kDebug, "DEBUG"},
    {LogLevel::kInfo, "INFO"},
    {LogLevel::kWarning, "WARN"},
    {LogLevel::kError, "ERROR"}
  };
}

void Logging::SetLevel(LogLevel level) {  
  _level.store(level);
}

LogLevel Logging::Level() {  
  return _level.load();
}

const char *Logging::LevelName(LogLevel level) {
  return level_names[level].c_str();
}

void Logging::Log(LogLevel level, const std::string &message) {
  if (level < Level()) {
    return;
  }

const auto now = std::chrono::system_clock::now();
const std::time_t seconds = std::chrono::system_clock::to_time_t(now);
const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now.time_since_epoch()).count() % 1000;

std::tm local_time;
localtime_r(&seconds, &local_time);

  std::ostringstream oss;
  oss << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S") << '.'
      << std::setfill('0') << std::setw(3) << ms      
      << " [" << LevelName(level) << "] "
      << "[thread " << std::this_thread::get_id() << "] "
      << message;

  std::lock_guard<std::mutex> lock(_mutex);      
  std::cout << oss.str() << "\n";
}


Logging::Line Logging::Debug() {
  return Line(LogLevel::kDebug);
}

Logging::Line Logging::Info() {
  return Line(LogLevel::kInfo);
}

Logging::Line Logging::Warning() {
  return Line(LogLevel::kWarning);
}

Logging::Line Logging::Error() {
  return Line(LogLevel::kError);
}
