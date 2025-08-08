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
  void Close();

  static std::string ReadLine(int sockfd);
  static void Send(int sockfd, std::string_view message);
  static void Shutdown(int sockfd);

  int GetSockfd() const;

private:
  explicit Socket(int socket);
  int socket_;
};

class SocketException final : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};
