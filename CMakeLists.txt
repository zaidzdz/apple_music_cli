cmake_minimum_required(VERSION 3.14)
project(apple_music_cli)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
        GIT_TAG        v5.0.0
)
FetchContent_MakeAvailable(ftxui)

add_executable(apple_music_cli
        apple-music-api.h
        main.cpp
)

# Name must match your executable above
target_link_libraries(apple_music_cli
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
)