message(STATUS "################Add Module Thgirdparty################")

add_subdirectory(drogon)

# set(UUID_USING_CXX20_SPAN ON)
set(UUID_BUILD_TESTS OFF)
add_subdirectory(stduuid)

set(JWT_BUILD_EXAMPLES OFF)
set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl@1.1")
add_subdirectory(jwt-cpp)

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
