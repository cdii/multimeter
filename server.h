#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <vector>

#include "socket.h"

class Server final {
public:
  explicit Server(std::string_view path);

  void Run();
  void Stop();

private:
  void HandleClient(int sockfd);

  Socket listener_socket_;
  std::atomic<bool> running_;
  std::unordered_map<int, std::thread> client_threads_;
  std::vector<Socket> connected_clients_;

  std::mutex mutex_;
  std::vector<int> disconnected_clients_;
  std::function<void()> shutdown_;
};
