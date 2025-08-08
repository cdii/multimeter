#pragma once

#include <ostream>
#include <sstream>

template <typename... Args>
void Println(std::ostream& os, Args&&... args) {
  std::stringstream buf;
  (buf << ... << std::forward<Args>(args));
  buf << '\n';
  os << buf.str();
}
