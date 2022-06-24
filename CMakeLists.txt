cmake_minimum_required (VERSION 3.18)

project (ForDrogon CXX)

# include(CheckIncludeFileCXX)

# check_include_file_cxx(any HAS_ANY)
# check_include_file_cxx(string_view HAS_STRING_VIEW)
# check_include_file_cxx(coroutine HAS_COROUTINE)
# if (NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
#   # Do nothing
# elseif (HAS_ANY AND HAS_STRING_VIEW AND HAS_COROUTINE)
#   set(CMAKE_CXX_STANDARD 20)
# elseif (HAS_ANY AND HAS_STRING_VIEW)
#   set(CMAKE_CXX_STANDARD 17)
# else ()
#   set(CMAKE_CXX_STANDARD 14)
# endif ()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fcoroutines")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_subdirectory(third_party)

if (CMAKE_CXX_STANDARD LESS 17)
  # With C++14, use boost to support any, string_view and filesystem
  message(STATUS "use c++14")
  find_package(Boost 1.61.0 REQUIRED)
  target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost)
elseif (CMAKE_CXX_STANDARD LESS 20)
  message(STATUS "use c++17")
else ()
  message(STATUS "use c++20")
endif ()

file(GLOB_RECURSE ${PROJECT_NAME}_HEADER_FILES src/*.h src/*.hpp)
file(GLOB_RECURSE ${PROJECT_NAME}_SOURCE_FILES src/*.cpp src/*.cc)

# https://github.com/drogonframework/drogon/wiki/CHN-06-%E8%A7%86%E5%9B%BE#csp%E6%96%87%E4%BB%B6%E7%9A%84%E8%87%AA%E5%8A%A8%E5%8C%96%E5%A4%84%E7%90%86
file(GLOB SCP_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/views/*.csp)
foreach(cspFile ${SCP_LIST})
  message(STATUS "cspFile:" ${cspFile})
  EXEC_PROGRAM(basename ARGS "-s .csp ${cspFile}" OUTPUT_VARIABLE classname)
  message(STATUS "view classname:" ${classname})
  add_custom_command(OUTPUT ${classname}.h ${classname}.cc
    COMMAND drogon_ctl
    ARGS create view ${cspFile}
    DEPENDS ${cspFile}
    VERBATIM
  )
  set(VIEWSRC ${VIEWSRC} ${classname}.cc)
endforeach()

add_executable(${PROJECT_NAME}
  ${${PROJECT_NAME}_HEADER_FILES}
  ${${PROJECT_NAME}_SOURCE_FILES}
  ${VIEWSRC}
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/models
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  drogon
  jwt-cpp
  bcrypt
  stduuid
  sodium
)
