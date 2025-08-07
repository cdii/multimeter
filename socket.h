#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

class Socket final {
public:
  Socket();
  ~Socket();

  Socket(const Socket&) = delete;
  Socket& operator=(const Socket&) = delete;

  Socket(Socket&& other) noexcept;
  Socket& operator=(Socket&& other) noexcept;

  void Bind(std::string_view path) const;
  void Listen() const;
  Socket Accept() const;
  void Shutdown();
  std::string ReadLine() const;
  void Send(std::string_view message) const;
  int GetSockfd() const;

private:
  explicit Socket(int socket);
  int socket_;
};

class SocketException final : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};
