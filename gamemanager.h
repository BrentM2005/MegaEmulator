#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QList>
#include <QString>
#include "game.h"

class GameManager : public QObject {
    Q_OBJECT

public:
    explicit GameManager(const QString &configPath, QObject *parent = nullptr);

    void addGame(const Game &game);
    void removeGame(const QString &gamePath);
    void updateGame(const Game &game);
    Game* findGame(const QString &gamePath);
    const QList<Game>& getAllGames() const { return games; }
    QList<Game> getGamesByEmulator(const QString &emulator) const;

    void loadGames();
    void saveGames();

signals:
    void gamesChanged();

private:
    QList<Game> games;
    QString gamesFilePath;
};

#endif // GAMEMANAGER_H
