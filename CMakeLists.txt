cmake_minimum_required(VERSION 3.6)
project(MagnusEffect)

if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  include_directories(vendor/nanogui/ext/glad/include/)
  file(GLOB VENDORS_SOURCES vendor/nanogui/ext/glad/src/glad.c)
endif()

if(NOT MYPROJ_EIGEN3_INCLUDE_DIR)
  find_package(Eigen3 QUIET)
  if(EIGEN3_INCLUDE_DIR)
    set(MYPROJ_EIGEN3_INCLUDE_DIR ${EIGEN3_INCLUDE_DIR})
  else()
    set(MYPROJ_EIGEN3_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/nanogui/ext/eigen)
  endif()
endif()

set(NANOGUI_EIGEN_INCLUDE_DIR ${EIGEN3_INCLUDE_DIR} CACHE BOOL " " FORCE)
option(NANOGUI_BUILD_EXAMPLE OFF)
option(NANOGUI_BUILD_PYTHON OFF)
option(NANOGUI_BUILD_SHARED OFF)
option(NANOGUI_INSTALL OFF)
add_subdirectory(vendor/nanogui/)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(vendor/assimp)

include_directories(vendor/glm/
                    vendor/stb/
                    vendor/assimp/include/
                    vendor/nanogui/include
                    ${NANOGUI_EXTRA_INCS}
                    ${MYPROJ_EIGEN3_INCLUDE_DIR})

file(GLOB PROJECT_HEADERS src/*.h)
file(GLOB PROJECT_SOURCES src/*.cpp)
file(GLOB PROJECT_SHADERS shaders/*.vert
                          shaders/*.frag)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          README.md
                          .gitattributes
                          .gitignore
                          .gitmodules)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\"
                ${NANOGUI_EXTRA_DEFS})

add_executable(${PROJECT_NAME}
               ${PROJECT_SOURCES}
               ${PROJECT_HEADERS}
               ${PROJECT_SHADERS}
               ${PROJECT_CONFIGS}
               ${VENDORS_SOURCES})

target_link_libraries(${PROJECT_NAME} assimp nanogui ${NANOGUI_EXTRA_LIBS})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})