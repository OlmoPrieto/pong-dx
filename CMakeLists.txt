cmake_minimum_required(VERSION 3.10)
project(pong-dx LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE PONG_DX_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(pong-dx ${PONG_DX_SOURCES})

target_include_directories(pong-dx PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(APPLE)
  target_link_directories(pong-dx PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/macos/raylib"
  )
  target_link_libraries(pong-dx PRIVATE
    raylib
    "-framework CoreVideo"
    "-framework IOKit"
    "-framework Cocoa"
    "-framework GLUT"
    "-framework OpenGL"
  )
elseif(WIN32)
  target_link_directories(pong-dx PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/win/raylib"
  )
  target_link_libraries(pong-dx PRIVATE raylib)
endif()
