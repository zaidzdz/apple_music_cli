//
// Created by Zaid Zamani on 3/17/26.
//

#ifndef APPLE_MUSIC_CLI_APPLE_MUSIC_API_H
#define APPLE_MUSIC_CLI_APPLE_MUSIC_API_H
#include <string>
#include <vector>
#include <sstream>
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
    SongInfo info;

    std::string result = runAppleScript(
        "tell application \"Music\" to return "
        "(get name of current track) & \"|\" & "
        "(get artist of current track) & \"|\" & "
        "(get album of current track) & \"|\" & "
        "(get duration of current track) & \"|\" & "
        "(get player position) & \"|\" & "
        "(get player state as string)"
    );

    // parse |-delimited result
    auto parts = std::vector<std::string>{};
    std::istringstream ss(result);
    std::string token;
    while (std::getline(ss, token, '|')) parts.push_back(token);

    info.title    = (parts.size() > 0 && isValidResult(parts[0])) ? parts[0] : "No track playing";
    info.artist   = (parts.size() > 1 && isValidResult(parts[1])) ? parts[1] : "No artist";
    info.album    = (parts.size() > 2 && isValidResult(parts[2])) ? parts[2] : "No album";
    info.duration = (parts.size() > 3 && !parts[3].empty()) ? std::stod(parts[3]) : 0.0;
    info.position = (parts.size() > 4 && !parts[4].empty()) ? std::stod(parts[4]) : 0.0;
    info.isPlaying = (parts.size() > 5 && parts[5] == "playing");

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