//
// Created by Zaid Zamani on 3/17/26.
//

#ifndef APPLE_MUSIC_CLI_APPLE_MUSIC_API_H
#define APPLE_MUSIC_CLI_APPLE_MUSIC_API_H
#include <string>
#include <cstdio>   // popen, pclose, fgets


std::string runAppleScript(const std::string& script) {
    // Write script to a temp file and execute
    std::string cmd = "osascript -e '" + script + "'";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";

    char buffer[256];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        result += buffer;

    pclose(pipe);
    // Remove trailing newline
    if (!result.empty() && result.back() == '\n')
        result.pop_back();
    return result;
}

struct SongInfo {
    std::string title;
    std::string artist;
    std::string album;
    float duration;    // seconds
    float position;    // current position in seconds
    bool isPlaying;
};
inline bool isValidResult(const std::string& s) {
    return !s.empty() && s.find("error") == std::string::npos;
}

SongInfo getMusicAppSong() {
std::cout << "s";
    SongInfo info;

    info.title    = runAppleScript("tell app \"Music\" to get name of current track");
    info.artist   = runAppleScript("tell app \"Music\" to get artist of current track");
    info.album    = runAppleScript("tell app \"Music\" to get album of current track");
    info.title  = isValidResult(info.title)  ? info.title  : "No track playing";
    info.artist = isValidResult(info.artist) ? info.artist : "No artist";
    info.album  = isValidResult(info.album)  ? info.album  : "No album";
    std::string dur = runAppleScript("tell app \"Music\" to get duration of current track");
    std::string pos = runAppleScript("tell app \"Music\" to get player position");
    std::string state = runAppleScript("tell app \"Music\" to get player state");

    info.duration  = dur.empty()   ? 0.0 : std::stod(dur);
    info.position  = pos.empty()   ? 0.0 : std::stod(pos);
    info.isPlaying = (state == "playing");

    return info;
}
SongInfo getSongInfo() {
    return getMusicAppSong();
}
void pauseCurrentSong() {
    runAppleScript("tell app \"Music\" to playpause");
}
void restartSong() {
    runAppleScript("tell app \"Music\" to set player position to 0");
}
void nextTrack() {
    runAppleScript("tell app \"Music\" to next track");
}

void prevTrack() {
    runAppleScript("tell app \"Music\" to previous track");
}
#endif //APPLE_MUSIC_CLI_APPLE_MUSIC_API_H