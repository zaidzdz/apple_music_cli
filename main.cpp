#include <iostream>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include "ftxui/component/screen_interactive.hpp"
#include <ftxui/dom/elements.hpp>
#include <unistd.h>
#include "apple-music-api.h"
using namespace ftxui;

SongInfo info;
std::mutex info_mutex;  // ← ADD THIS
bool running = true;
SongInfo getSafeInfo() {
    std::lock_guard<std::mutex> lock(info_mutex);
    return info;
}
inline std::string floatToTime(float seconds) {
    int mins = (int)(seconds / 60);
    int secs = (int)(seconds - mins * 60);

    char buffer[6];
    std::snprintf(buffer, sizeof(buffer), "%d:%02d", mins, secs);
    return std::string(buffer);
}
void threadsdfs() {
    while (running) {
        SongInfo temp = getMusicAppSong();

        {
            std::lock_guard<std::mutex> lock(info_mutex);
            info = temp;  // Safe write
        }


    }
}

int main() {

    ScreenInteractive screen = ScreenInteractive::Fullscreen();

    auto repeatStr = [](const std::string& s, int n) {
        std::string result;
        for (int i = 0; i < n; i++) result += s;
        return result;
    };
    // AFTER (correct) - start with filler() as the base element
    auto vertical_rect = hbox({
    text("") | size(WIDTH, EQUAL, 1)
             | size(HEIGHT, EQUAL, 1)
             | bgcolor(Color::Blue),
    filler()  // pushes rect to the left, takes remaining space
});
    auto customGauge = [&](float progress, int width) {
        int filled = (int)(progress * width);
        int empty  = width - filled;
        return hbox({
            text(repeatStr("█", filled)) | color(Color::Red),
            text(repeatStr("░", empty))  | color(Color::GrayDark),
        });
    };





    auto renderer = Renderer([&] {
        SongInfo current = getSafeInfo();
        auto doc = window(text("APPLE MUSIC CLI") | bold | color(Color::Red),{

        vbox({
            hbox({
                text("Current track:  ") | color(Color::Red),

                text(current.title)
            }),
            hbox({
                text("Current album:  ")  | color(Color::Red),

                text(current.album)
            }),
            hbox({
                text("Current artist: ") | color(Color::Red),

                text(current.artist)
            }),

            hbox({
                text("Progress:  ") | color(Color::Red),
                text(floatToTime(current.position)+"  ") | bold | color(Color::GrayLight),
                customGauge(current.position/current.duration,50),
                text("  "+floatToTime(current.duration)) | bold | color(Color::GrayLight),

            }),
            hbox({
    text(" P ") | bgcolor(Color::Red) | color(Color::Black),
    text(" Pause  "),
    text(" J ") | bgcolor(Color::Red) | color(Color::Black),
    text(" Prev  "),
    text(" K ") | bgcolor(Color::Red) | color(Color::Black),
    text(" Next  "),
    text(" R ") | bgcolor(Color::Red) | color(Color::Black),
    text(" Restart  "),
    text(" Q ") | bgcolor(Color::Red) | color(Color::Black),
    text(" Quit  "),
}) | color(Color::GrayLight),
        separator(),
            vertical_rect
        })
    });




        return doc;



    });

    auto component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q')) {
            //running = false;
            screen.ExitLoopClosure();
            return true;
        }
        if (event == Event::Character('p')) {
           pauseCurrentSong();

        }
        if (event == Event::Character('r')) {
           restartSong();
        }
        if (event == Event::Character('j')) {
           prevTrack();
        }
        if (event == Event::Character('k')) {
           nextTrack();
        }
        return false;
    });

    std::thread updater(threadsdfs);

    screen.Loop(component);
    updater.join();
    return 0;
}

