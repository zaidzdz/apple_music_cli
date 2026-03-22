#include <iostream>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include "ftxui/component/screen_interactive.hpp"
#include <ftxui/dom/elements.hpp>
#include <unistd.h>
#include <thread>
#include <mutex>
#include "apple-music-api.h"

using namespace ftxui;

SongInfo info;
bool running = true;
bool loading = false;
std::mutex info_mutex;  // ← ADD THIS

inline std::string floatToTime(float seconds) {
    int mins = (int)(seconds / 60);
    int secs = (int)(seconds - mins * 60);
    char buffer[6];
    std::snprintf(buffer, sizeof(buffer), "%d:%02d", mins, secs);
    return std::string(buffer);
}


int main() {
    ScreenInteractive screen = ScreenInteractive::Fullscreen();

    auto repeatStr = [](const std::string& s, int n) {
        std::string result;
        for (int i = 0; i < n; i++) result += s;
        return result;
    };

    auto vertical_rect = hbox({
        text("") | size(WIDTH, EQUAL, 1)
                 | size(HEIGHT, EQUAL, 1)
                 | bgcolor(Color::Blue),
        filler()
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
        SongInfo current;
        {
            std::lock_guard<std::mutex> lock(info_mutex);  // ← LOCK HERE TOO
            current = info;
        }  // ← AUTO UNLOCK

        auto doc = window(text("APPLE MUSIC CLI") | bold | color(Color::Red), {
            vbox({
                hbox({
                    text("Current track:  ") | color(Color::Red),
                    text(loading ? "Loading...":current.title)
                }),
                hbox({
                    text("Current album:  ")  | color(Color::Red),
                    text(loading ? "Loading...":current.album)
                }),
                hbox({
                    text("Current artist: ") | color(Color::Red),
                    text(loading ? "Loading...":current.artist)
                }),
                hbox({
                    text("Progress:  ") | color(Color::Red),
                    text(floatToTime(current.position)+"  ") | bold | color(Color::GrayLight),
                    customGauge(current.position/current.duration, 50),
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


            })
        });
        return doc;
    });

    auto component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q')) {
            screen.ExitLoopClosure()();
            return true;
        }
        if (event == Event::Character('p')) {
            pauseCurrentSong();
        }
        if (event == Event::Character('r')) {
            restartSong();
        }
        if (event == Event::Character('j')) {
            loading = true;
            prevTrack();
        }
        if (event == Event::Character('k')) {
            loading = true;
            nextTrack();
        }
        return false;
    });

    std::thread updater([&] {
        while (running) {
            SongInfo new_info = getMusicAppSong();
            {
                std::lock_guard<std::mutex> lock(info_mutex);
                info = new_info;
            }
            loading = false;
            screen.PostEvent(Event::Custom);
        }
    });
    screen.Loop(component);
    running = false;
    updater.join();
    return 0;
}