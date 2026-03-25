//
// Created by Zaid Zamani on 3/20/26.
//
#include <iostream>
#include <ostream>
#include <thread>
#include "apple-music-api.h"

bool running = true;
int main() {
while (running) {
    SongInfo info = getMusicAppSong();
    std::cout << info.album << std::endl;
    std::cout << info.artist  << std::endl;;
    std::cout << info.title  << std::endl;;
}
    return 0;
}
