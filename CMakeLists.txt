cmake_minimum_required (VERSION 3.18)

project (ForDrogon CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(third_party)

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
