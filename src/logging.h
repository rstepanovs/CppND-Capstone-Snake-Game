#ifndef LOGGING_H
#define LOGGING_H

#include <atomic>
#include <mutex>
#include <sstream>
#include <string>

enum class LogLevel { kDebug, kInfo, kWarning, kError };

// Thread-safe debug output. All members are static, so any thread can call
// Logging::Debug() << "x=" << x without passing an object around. One mutex
// guarantees that lines written by different threads are never mixed together.
class Logging {
 public:
  Logging() = delete;

  // Messages below this level are dropped. Default: LogLevel::kDebug.
  static void SetLevel(LogLevel level);
  static LogLevel Level();

  // A temporary that collects one log line. The line is written when the
  // temporary is destroyed, i.e. at the end of the full expression:
  //   Logging::Debug() << "head=" << x << "," << y;
  class Line {
   public:
    explicit Line(LogLevel level) : _level(level) {}

    ~Line() {
      Logging::Log(_level, _stream.str());
    }

    // Movable (the factory functions return it by value), not copyable:
    // a copy would print the line twice.
    Line(Line &&) = default;
    Line(const Line &) = delete;
    Line &operator=(const Line &) = delete;

    template <typename T>
    Line &operator<<(const T &value) {
      _stream << value;
      return *this;
    }

   private:
    LogLevel _level;
    std::ostringstream _stream;
  };

  static Line Debug();
  static Line Info();
  static Line Warning();
  static Line Error();

 private:
  // Writes "[<time>] [<level>] [thread <id>] <message>" to std::cerr. Called
  // by Line only.
  static void Log(LogLevel level, const std::string &message);
  static const char *LevelName(LogLevel level);

  static std::mutex _mutex;
  static std::atomic<LogLevel> _level;
};

#endif
