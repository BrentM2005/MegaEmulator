#ifndef GAME_H
#define GAME_H

#include <QString>

struct Game {
    QString name;
    QString path;
    QString emulator;
    QString coverArt;
    int playCount;
    QString lastPlayed;

    Game() : playCount(0), lastPlayed("") {}
};

#endif // GAME_H
