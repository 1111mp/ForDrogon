cmake_minimum_required (VERSION 3.18)

project (ForDrogon CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(third_party)

file(GLOB_RECURSE ${PROJECT_NAME}_HEADER_FILES src/*.h *.hpp)
file(GLOB_RECURSE ${PROJECT_NAME}_SOURCE_FILES src/*.cpp)

add_executable(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADER_FILES}
  ${${PROJECT_NAME}_SOURCE_FILES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  drogon
)
