#pragma once

#include <string>
#include <node_blf.h>

namespace Utils::Bcrypt
{

  std::string genSalt(uint32_t rounds = 10);

  std::string genHash(const std::string &key, uint32_t rounds = 10);

  bool compare(const std::string &key, const std::string &hash);

  uint32_t getRounds(const std::string &hash);
}
