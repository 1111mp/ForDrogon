#pragma once

#include <string>
#include <memory>
#include <sodium.h>
#include <trantor/utils/Logger.h>

#include "bcrypt.h"

void encrypt_password(std::shared_ptr<std::string> &password)
{
  auto hashed_password = Bcrypt::genHash(password->data(), 10);
  password.reset(new std::string(hashed_password));
}