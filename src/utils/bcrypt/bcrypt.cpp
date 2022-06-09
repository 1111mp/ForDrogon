#include "bcrypt.h"

#include <sodium.h>

namespace Utils
{

  std::string Bcrypt::genSalt(uint32_t rounds)
  {
    unsigned char seed[16];
    randombytes_buf(seed, sizeof(seed));

    char salt[_SALT_LEN];
    bcrypt_gensalt('b', rounds, seed, salt);

    return salt;
  }

  std::string Bcrypt::genHash(const std::string &key, uint32_t rounds)
  {
    std::string salt = genSalt(rounds);

    char bcrypted[_PASSWORD_LEN];

    bcrypt(key.c_str(), key.size(), salt.c_str(), bcrypted);

    return bcrypted;
  }

  bool Bcrypt::compare(const std::string &key, const std::string &hash)
  {
    char bcrypted[_PASSWORD_LEN];
    bcrypt(key.c_str(), key.size(), hash.c_str(), bcrypted);

    return hash == bcrypted;
  }

  uint32_t Bcrypt::getRounds(const std::string &hash)
  {
    uint32_t rounds = bcrypt_get_rounds(hash.c_str());

    return rounds;
  }
}