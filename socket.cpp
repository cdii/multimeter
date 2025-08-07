#include "socket.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <iostream>

using namespace std::string_literals;

Socket::Socket() : socket_(::socket(AF_UNIX, SOCK_STREAM, 0)) {
  if (socket_ == -1) {
    throw SocketException("Failed to create socket: "s + std::strerror(errno));
  }
}
Socket::~Socket() { Shutdown(); }

Socket::Socket(const int socket) : socket_(socket) {}

Socket::Socket(Socket&& other) noexcept : socket_(other.socket_) {
  other.socket_ = -1;
}

Socket& Socket::operator=(Socket&& other) noexcept {
  if (this != &other) {
    socket_ = other.socket_;
    other.socket_ = -1;
  }
  return *this;
}

void Socket::Bind(std::string_view path) const {
  if (std::filesystem::exists(path)) {
    std::filesystem::remove(path);
  }

  std::filesystem::create_directories(
    std::filesystem::path{path}.parent_path());

  sockaddr_un addr{};
  addr.sun_family = AF_UNIX;

  std::copy(path.begin(), path.end(), std::begin(addr.sun_path));

  if (::bind(socket_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
    throw SocketException(
      "Failed to bind socket to "s + path.data() + ": " + std::strerror(errno));
  }
}

void Socket::Listen() const {
  if (::listen(socket_, SOMAXCONN) == -1) {
    throw SocketException(
      "Failed to listen on socket: "s + std::strerror(errno));
  }
}

Socket Socket::Accept() const {
  const auto socket = ::accept(socket_, nullptr, nullptr);

  if (socket == -1) {
    throw SocketException(
      "Failed to accept new connection: "s + std::strerror(errno));
  }

  return Socket(socket);
}

void Socket::Shutdown() {
  if (socket_ != -1) {
    ::close(socket_);
    socket_ = -1;
  }
}

std::string Socket::ReadLine() const {
  std::string buffer;

  while (true) {
    char byte = 0;
    const auto num_bytes = ::read(socket_, &byte, 1);

    if (num_bytes == -1) {
      if (errno == EINTR) {
        continue;
      }

      if (errno == ECONNRESET) {
        return {};
      }

      throw SocketException(
        "Failed to receive message: "s + std::strerror(errno));
    }

    if (num_bytes == 0) {
      return buffer;
    }

    buffer.push_back(byte);

    if (byte == '\n') {
      break;
    }
  }

  return buffer;
}

void Socket::Send(const std::string_view message) const {
  if (::send(socket_, message.data(), message.size(), 0) == -1) {
    throw SocketException("Failed to send message: "s + std::strerror(errno));
  }
}

int Socket::GetSockfd() const { return socket_; }
