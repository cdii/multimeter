#pragma once

#include <atomic>
#include <string_view>
#include <thread>
#include <vector>

#include "socket.h"

class Server final {
public:
  explicit Server(std::string_view path);

  void Run();
  void Stop();

private:
  void HandleClient(Socket client) const;

  Socket listener_socket_;
  std::atomic<bool> running_;
  std::vector<std::thread> client_threads_;
  int sockfd_;
};
