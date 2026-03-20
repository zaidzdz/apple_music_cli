#include <iostream>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include "ftxui/component/screen_interactive.hpp"
#include <ftxui/dom/elements.hpp>
#include <unistd.h>
#include "apple-music-api.h"
using namespace ftxui;

SongInfo info;  // shared state
bool running = true;


void threadsdfs() {
    while (running) {
        info = getMusicAppSong();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "s";
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
    auto doc = window(text("APPLE MUSIC CLI") | bold | color(Color::Red),{

        vbox({
            hbox({
                text("Current track:  ") | color(Color::Red),

                text("Neon Gravestones")
            }),
            hbox({
                text("Current album:  ")  | color(Color::Red),

                text("Trench")
            }),
            hbox({
                text("Current artist: ") | color(Color::Red),

                text(info.artist)
            }),

            hbox({
                text("Progress:  ") | color(Color::Red),
                text("0:23  ") | bold | color(Color::GrayLight),
                customGauge(info.position,50),
                text("  3:33") | bold | color(Color::GrayLight),

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




    auto renderer = Renderer([&] { return doc; });

    auto component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q')) {
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

