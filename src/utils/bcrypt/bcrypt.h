#pragma once

#include <string>
#include <node_blf.h>

namespace Utils
{

  class Bcrypt
  {
  public:
    static std::string genSalt(uint32_t rounds = 10);

    static std::string genHash(const std::string &key, uint32_t rounds = 10);

    static bool compare(const std::string &key, const std::string &hash);

    static uint32_t getRounds(const std::string &hash);
  };

}
