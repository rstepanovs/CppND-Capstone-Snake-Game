#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <mutex>
#include <utility>
#include <vector>

template <typename T>
class MessageQueue {
 public:
  void Send(T &&message) {
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.push_back(std::move(message));
  }

  std::vector<T> TakeAll() {
    std::vector<T> msgs;
    std::lock_guard<std::mutex> lock(_mutex);
    msgs.swap(_queue);
    return msgs;}

 private:
  std::mutex _mutex;
  std::vector<T> _queue;
};

#endif
