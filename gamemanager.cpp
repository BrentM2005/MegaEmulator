#include "gamemanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDateTime>

GameManager::GameManager(const QString &configPath, QObject *parent)
    : QObject(parent), gamesFilePath(configPath + "/games.json") {
}

void GameManager::addGame(const Game &game) {
    games.append(game);
    emit gamesChanged();
}

void GameManager::removeGame(const QString &gamePath) {
    for (int i = 0; i < games.size(); ++i) {
        if (games[i].path == gamePath) {
            games.removeAt(i);
            emit gamesChanged();
            return;
        }
    }
}

void GameManager::updateGame(const Game &game) {
    for (int i = 0; i < games.size(); ++i) {
        if (games[i].path == game.path) {
            games[i] = game;
            emit gamesChanged();
            return;
        }
    }
}

Game* GameManager::findGame(const QString &gamePath) {
    for (auto &game : games) {
        if (game.path == gamePath) {
            return &game;
        }
    }
    return nullptr;
}

QList<Game> GameManager::getGamesByEmulator(const QString &emulator) const {
    if (emulator == "All Systems") {
        return games;
    }

    QList<Game> filtered;
    for (const auto &game : games) {
        if (game.emulator == emulator) {
            filtered.append(game);
        }
    }
    return filtered;
}

void GameManager::loadGames() {
    QFile file(gamesFilePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray arr = doc.array();

    games.clear();
    for (const auto &val : arr) {
        QJsonObject obj = val.toObject();
        Game game;
        game.name = obj["name"].toString();
        game.path = obj["path"].toString();
        game.emulator = obj["emulator"].toString();
        game.coverArt = obj["coverArt"].toString();
        game.playCount = obj["playCount"].toInt();
        game.lastPlayed = obj["lastPlayed"].toString();
        games.append(game);
    }

    file.close();
    emit gamesChanged();
}

void GameManager::saveGames() {
    QFile file(gamesFilePath);
    if (!file.open(QIODevice::WriteOnly)) return;

    QJsonArray arr;
    for (const auto &game : games) {
        QJsonObject obj;
        obj["name"] = game.name;
        obj["path"] = game.path;
        obj["emulator"] = game.emulator;
        obj["coverArt"] = game.coverArt;
        obj["playCount"] = game.playCount;
        obj["lastPlayed"] = game.lastPlayed;
        arr.append(obj);
    }

    QJsonDocument doc(arr);
    file.write(doc.toJson());
    file.close();
}
