cmake_minimum_required(VERSION 3.22)
project(tse VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
  enable_language(OBJCXX)
endif()

find_package(SDL2 CONFIG REQUIRED)

if(NOT DEFINED VCPKG_INSTALLED_DIR OR NOT DEFINED VCPKG_TARGET_TRIPLET)
  message(FATAL_ERROR "Configure with the vcpkg toolchain so OGRE-Next can be located")
endif()

set(TSE_VCPKG_PREFIX "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")

# FindOGRE needs the linkage choice before it searches for library names. Read
# it from the package rather than making every static vcpkg triplet pass a flag.
set(TSE_OGRE_BUILD_SETTINGS
    "${TSE_VCPKG_PREFIX}/include/OGRE-Next/OgreBuildSettings.h")
if(EXISTS "${TSE_OGRE_BUILD_SETTINGS}")
  file(STRINGS "${TSE_OGRE_BUILD_SETTINGS}" TSE_OGRE_STATIC_DEFINE
       REGEX "^#define[ \t]+OGRE_STATIC_LIB")
  if(TSE_OGRE_STATIC_DEFINE)
    set(OGRE_STATIC ON CACHE BOOL "Use the vcpkg OGRE-Next linkage" FORCE)
  else()
    set(OGRE_STATIC OFF CACHE BOOL "Use the vcpkg OGRE-Next linkage" FORCE)
  endif()
endif()

list(PREPEND CMAKE_MODULE_PATH "${TSE_VCPKG_PREFIX}/share/ogre-next")
find_package(OGRE REQUIRED)

if(NOT OGRE_HlmsPbs_FOUND OR NOT OGRE_HlmsUnlit_FOUND)
  message(FATAL_ERROR "The OGRE-Next vcpkg package is missing the PBS or Unlit HLMS component")
endif()

# vcpkg deliberately omits OGRE-Next's runtime shader templates. Fetch the
# source at the exact package version, but do not add OGRE-Next as a subproject.
include(FetchContent)
FetchContent_Declare(
  ogre_next_media
  URL https://github.com/OGRECave/ogre-next/archive/refs/tags/v3.0.0.tar.gz
  URL_HASH SHA256=7818085243018178730bdbe85a84d358e49b3b19e3ff65e02d33b008e778210d
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SOURCE_SUBDIR TSE-Media-Only
)
FetchContent_MakeAvailable(ogre_next_media)

set(TSE_SOURCES
  source/main.cpp
  source/camera.cpp
  source/camera.hpp
  source/cube.cpp
  source/cube.hpp
  source/gameobject.hpp
  source/level.cpp
  source/level.hpp
  source/light.cpp
  source/light.hpp
  source/model.cpp
  source/model.hpp
)

if(APPLE)
  list(APPEND TSE_SOURCES
    source/platform/osx_window.hpp
    source/platform/osx_window.mm
  )
endif()

add_executable(tse ${TSE_SOURCES})

target_include_directories(tse PRIVATE
  source
  ${OGRE_INCLUDE_DIRS}
  "${TSE_VCPKG_PREFIX}/include/OGRE-Next/Hlms/Common"
  ${OGRE_HlmsPbs_INCLUDE_DIRS}
  ${OGRE_HlmsUnlit_INCLUDE_DIRS}
)

target_link_libraries(tse PRIVATE
  ${OGRE_LIBRARIES}
  ${OGRE_HlmsPbs_LIBRARIES}
  ${OGRE_HlmsUnlit_LIBRARIES}
  $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
  $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

if(OGRE_STATIC)
  if(APPLE)
    find_path(TSE_RENDERER_INCLUDE_DIR OgreMetalPlugin.h
      HINTS "${TSE_VCPKG_PREFIX}/include/OGRE-Next/RenderSystems/Metal"
      PATH_SUFFIXES include
      REQUIRED
    )
    find_library(TSE_RENDERER_RELEASE RenderSystem_MetalStatic
      PATHS "${TSE_VCPKG_PREFIX}/lib" NO_DEFAULT_PATH REQUIRED)
    find_library(TSE_RENDERER_DEBUG RenderSystem_MetalStatic_d
      PATHS "${TSE_VCPKG_PREFIX}/debug/lib" NO_DEFAULT_PATH REQUIRED)
    set(TSE_RENDERER_LIBRARIES
      optimized ${TSE_RENDERER_RELEASE}
      debug ${TSE_RENDERER_DEBUG}
      "-framework Metal"
      "-framework AppKit"
      "-framework QuartzCore"
      "-framework IOKit"
      "-framework Carbon"
      "-framework CoreVideo"
      "-framework OpenGL"
    )
  elseif(WIN32)
    set(TSE_RENDERER_INCLUDE_DIR ${OGRE_RenderSystem_Direct3D11_INCLUDE_DIRS})
    set(TSE_RENDERER_LIBRARIES ${OGRE_RenderSystem_Direct3D11_LIBRARIES})
  else()
    set(TSE_RENDERER_INCLUDE_DIR ${OGRE_RenderSystem_GL3Plus_INCLUDE_DIRS})
    set(TSE_RENDERER_LIBRARIES ${OGRE_RenderSystem_GL3Plus_LIBRARIES})
  endif()

  target_include_directories(tse PRIVATE ${TSE_RENDERER_INCLUDE_DIR})
  target_link_libraries(tse PRIVATE
    ${TSE_RENDERER_LIBRARIES}
    ${OGRE_LIBRARIES}
  )
else()
  if(APPLE)
    set(TSE_RENDER_PLUGIN RenderSystem_Metal)
  elseif(WIN32)
    set(TSE_RENDER_PLUGIN RenderSystem_Direct3D11)
  else()
    set(TSE_RENDER_PLUGIN RenderSystem_GL3Plus)
  endif()

  if(WIN32)
    set(TSE_PLUGIN_SUBDIR bin)
  else()
    set(TSE_PLUGIN_SUBDIR lib)
  endif()

  file(GENERATE
    OUTPUT "$<TARGET_FILE_DIR:tse>/plugins.cfg"
    CONTENT "PluginFolder=$<IF:$<CONFIG:Debug>,${TSE_VCPKG_PREFIX}/debug/${TSE_PLUGIN_SUBDIR},${TSE_VCPKG_PREFIX}/${TSE_PLUGIN_SUBDIR}>\nPlugin=${TSE_RENDER_PLUGIN}$<$<CONFIG:Debug>:_d>\n"
  )
endif()

add_custom_command(TARGET tse POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_SOURCE_DIR}/assets"
          "$<TARGET_FILE_DIR:tse>/assets"
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${ogre_next_media_SOURCE_DIR}/Samples/Media/Hlms"
          "$<TARGET_FILE_DIR:tse>/assets/Hlms"
  VERBATIM
)
