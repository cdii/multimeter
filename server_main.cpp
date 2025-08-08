#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string_view>
#include <thread>

#include "server.h"
#include "utils.h"

constexpr std::string_view kSocketName = "/tmp/leaf/9Lq7BNBnBycd6nxy.socket";

using namespace std::chrono_literals;

namespace {
std::atomic<bool> gShouldStop{false};

void SignalHandler(const int signal) {
  if (signal == SIGINT || signal == SIGTERM) {
    gShouldStop.store(true);
  }
}
}  // namespace

int main() try {
  std::signal(SIGINT, SignalHandler);
  std::signal(SIGTERM, SignalHandler);

  Server server(kSocketName);

  std::thread thread([&server] { server.Run(); });

  while (!gShouldStop.load()) {
    std::this_thread::sleep_for(1s);
  }

  server.Stop();

  if (thread.joinable()) {
    thread.join();
  }

  Println(std::cout, "Goodbye, my friend, goodbye");
  return EXIT_SUCCESS;
} catch (const std::exception& e) {
  Println(std::cerr, e.what());
  return EXIT_FAILURE;
} catch (...) {
  Println(std::cerr, "Oops! Something went wrong.");
  return EXIT_FAILURE;
}
