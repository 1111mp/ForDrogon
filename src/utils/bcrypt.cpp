#include "bcrypt.h"

#include <sodium.h>

namespace Bcrypt
{

  std::string genHash(const std::string &key, unsigned int rounds)
  {
    char salt[_SALT_LEN];

    unsigned char seed[16];
    randombytes_buf(seed, sizeof(seed));

    bcrypt_gensalt('b', rounds, seed, salt);

    char bcrypted[_PASSWORD_LEN];

    bcrypt(key.c_str(), key.size(), salt, bcrypted);

    return bcrypted;
  }

  bool compare(const std::string &key, const std::string &hash)
  {
    char bcrypted[_PASSWORD_LEN];
    bcrypt(key.c_str(), key.size(), hash.c_str(), bcrypted);

    return hash == bcrypted;
  }
}