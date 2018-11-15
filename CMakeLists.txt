cmake_minimum_required(VERSION 3.0)
project(Program)

if(NOT MYPROJ_EIGEN3_INCLUDE_DIR)
  find_package(Eigen3 QUIET)
  if(EIGEN3_INCLUDE_DIR)
    set(MYPROJ_EIGEN3_INCLUDE_DIR ${EIGEN3_INCLUDE_DIR})
  else()
    set(MYPROJ_EIGEN3_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/nanogui/ext/eigen)
  endif()
endif()

set(NANOGUI_EIGEN_INCLUDE_DIR ${EIGEN3_INCLUDE_DIR} CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_PYTHON  OFF CACHE BOOL " " FORCE)
set(NANOGUI_INSTALL       OFF CACHE BOOL " " FORCE)
add_subdirectory(Vendor/nanogui/)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(Vendor/assimp)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

include_directories(Headers/
                    Vendor/glm/
                    Vendor/stb/
                    Vendor/assimp/include/
                    Vendor/nanogui/include
                    Vendor/nanogui/ext/glad/include/
                    ${NANOGUI_EXTRA_INCS}
                    ${MYPROJ_EIGEN3_INCLUDE_DIR})

file(GLOB VENDORS_SOURCES Vendor/nanogui/ext/glad/src/glad.c)
file(GLOB PROJECT_HEADERS Headers/*.h)
file(GLOB PROJECT_SOURCES Sources/*.cpp)
file(GLOB PROJECT_SHADERS Shaders/*.comp
                          Shaders/*.frag
                          Shaders/*.geom
                          Shaders/*.vert)
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
target_link_libraries(${PROJECT_NAME}
                      nanogui ${NANOGUI_EXTRA_LIBS}
                      assimp)
set_target_properties(${PROJECT_NAME}
                      PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY
                      ${CMAKE_BINARY_DIR})
