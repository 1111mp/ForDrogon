#pragma once

#include <string>
#include <node_blf.h>

namespace Bcrypt
{

  std::string genHash(const std::string &key, unsigned int rounds = 10);

  bool compare(const std::string &key, const std::string &hash);

}
