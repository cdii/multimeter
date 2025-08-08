#include "server.h"

#include <algorithm>
#include <exception>
#include <iostream>
#include <sstream>
#include <thread>
#include <utility>

#include "utils.h"

using namespace std::chrono_literals;

Server::Server(std::string_view path) : running_{false} {
  listener_socket_.Bind(path);
  listener_socket_.Listen();
  shutdown_ = [sockfd = listener_socket_.GetSockfd()] {
    Socket::Shutdown(sockfd);
  };

  Println(std::cout, "Server is listening on ", path);
}

void Server::Run() {
  running_.store(true);

  while (running_.load()) {
    try {
      {
        auto client = listener_socket_.Accept();
        auto sockfd = client.GetSockfd();

        std::stringstream hex;
        hex << std::hex << sockfd;
        Println(std::cout, '#', hex.str(), " client connected");

        client_threads_.emplace(
          sockfd, std::thread{&Server::HandleClient, this, sockfd});
        connected_clients_.emplace_back(std::move(client));
      }

      {
        const std::lock_guard lock(mutex_);
        for (const auto sockfd : disconnected_clients_) {
          auto node = client_threads_.extract(sockfd);
          if (node.mapped().joinable()) {
            node.mapped().join();
          }

          connected_clients_.erase(
            std::remove_if(
              connected_clients_.begin(),
              connected_clients_.end(),
              [sockfd](const Socket& socket) {
                return socket.GetSockfd() == sockfd;
              }),
            connected_clients_.end());
        }
        disconnected_clients_.clear();
      }

      Println(std::cout, "total: ", client_threads_.size());

      std::this_thread::yield();
    } catch (const std::exception& e) {
      if (running_.load()) {
        Println(std::cerr, e.what());
      }
    }
  }

  try {
    for (auto& [sockfd, thread] : client_threads_) {
      Socket::Shutdown(sockfd);
      if (thread.joinable()) {
        thread.join();
      }
    }

    client_threads_.clear();
    client_threads_.clear();

    Println(std::cout, "Server stopped");
  } catch (const std::exception& e) {
    Println(std::cerr, e.what());
  }
}

void Server::Stop() {
  running_.store(false);
  shutdown_();
}

void Server::HandleClient(const int sockfd) {
  while (running_.load()) {
    try {
      auto message = Socket::ReadLine(sockfd);
      if (message.empty()) {
        std::stringstream hex;
        hex << std::hex << sockfd;
        Println(std::cout, '#', hex.str(), " client disconnected");

        {
          const std::lock_guard lock(mutex_);
          disconnected_clients_.emplace_back(sockfd);
        }
        break;
      }
      Println(std::cout, message);

      Socket::Send(sockfd, message);

      std::this_thread::yield();
    } catch (const std::exception& e) {
      Println(std::cerr, e.what());
      break;
    }
  }
}
