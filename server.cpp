#include "server.h"

#include <sys/socket.h>

#include <algorithm>
#include <exception>
#include <iostream>
#include <sstream>
#include <thread>
#include <utility>

using namespace std::chrono_literals;

Server::Server(std::string_view path) : running_{false}, sockfd_{-1} {
  listener_socket_.Bind(path);
  listener_socket_.Listen();
  sockfd_ = listener_socket_.GetSockfd();

  std::cout << "Server is listening on " << path << '\n';
}

void Server::Run() {
  running_.store(true);

  while (running_.load()) {
    try {
      auto client = listener_socket_.Accept();

      std::stringstream hex;
      hex << std::hex << client.GetSockfd();
      std::cout << '#' << hex.str() << " client connected\n";

      client_threads_.emplace_back(
        &Server::HandleClient, this, std::move(client));

      std::this_thread::yield();
    } catch (const std::exception& e) {
      if (running_.load()) {
        std::cerr << e.what() << '\n';
      }
    }
  }

  try {
    for (auto&& thread : client_threads_) {
      if (thread.joinable()) {
        thread.join();
      }
    }

    client_threads_.clear();

    std::cout << "Server stopped\n";
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}

void Server::Stop() {
  running_.store(false);

  ::shutdown(sockfd_, SHUT_RDWR);
}

void Server::HandleClient(const Socket client) const {
  while (running_.load()) {
    try {
      auto message = client.ReadLine();
      if (message.empty()) {
        std::stringstream hex;
        hex << std::hex << client.GetSockfd();
        std::cout << '#' << hex.str() << " client disconnected\n";
        break;
      }
      std::cout << message;
      std::reverse(message.begin(), message.end());
      client.Send(message);

      std::this_thread::yield();
    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
      break;
    }
  }
}
