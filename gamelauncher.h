#ifndef GAMELAUNCHER_H
#define GAMELAUNCHER_H

#include <QObject>
#include <QProcess>
#include "game.h"
#include "emulator.h"

class GameLauncher : public QObject {
    Q_OBJECT

public:
    explicit GameLauncher(QObject *parent = nullptr);

    bool launchGame(const Game &game, const Emulator &emulator);

signals:
    void gameLaunched(const QString &gameName);
    void launchFailed(const QString &error);

private:
    QString buildCommandLine(const QString &launchArgs, const QString &gamePath);
};

#endif // GAMELAUNCHER_H
