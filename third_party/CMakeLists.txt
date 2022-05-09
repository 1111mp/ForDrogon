message(STATUS "################Add Module Thgirdparty################")

add_subdirectory(drogon)

add_library(bcrypt
  bcrypt/src/node_blf.h
  bcrypt/src/bcrypt.cc
  bcrypt/src/blowfish.cc
)

target_include_directories(bcrypt
  PUBLIC
  bcrypt/src
)

include(FetchContent)

FetchContent_Declare(Sodium
  GIT_REPOSITORY https://github.com/robinlinden/libsodium-cmake.git
)
set(SODIUM_DISABLE_TESTS OFF)
set(SODIUM_MINIMAL ON)
FetchContent_MakeAvailable(Sodium)
