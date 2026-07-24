cmake_minimum_required(VERSION 3.22)
project(tes VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_definitions(GL_SILENCE_DEPRECATION)

find_package(OGRE CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(Bullet CONFIG REQUIRED)

file(GLOB_RECURSE SOURCE_FILES source/*.cpp source/*.h)
add_executable(tes ${SOURCE_FILES})

target_link_libraries(tes PRIVATE OgreMain OgreBullet OgreBites Codec_STBI  
        ${BULLET_LIBRARIES}
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)
target_include_directories(tes PRIVATE ${BULLET_INCLUDE_DIRS})
