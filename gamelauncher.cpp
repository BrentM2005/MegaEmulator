#include "gamelauncher.h"
#include <QFileInfo>

GameLauncher::GameLauncher(QObject *parent) : QObject(parent) {
}

bool GameLauncher::launchGame(const Game &game, const Emulator &emulator) {
    if (emulator.executablePath.isEmpty()) {
        emit launchFailed("Emulator path not configured");
        return false;
    }

    if (!QFileInfo::exists(emulator.executablePath)) {
        emit launchFailed("Emulator executable not found: " + emulator.executablePath);
        return false;
    }

    if (!QFileInfo::exists(game.path)) {
        emit launchFailed("Game file not found: " + game.path);
        return false;
    }

    QString args = buildCommandLine(emulator.launchArgs, game.path);

    QProcess *process = new QProcess(this);

    QStringList argList;
    if (!args.isEmpty()) {
        argList = args.split(" ", Qt::SkipEmptyParts);
    }

    bool success = process->startDetached(emulator.executablePath, argList);

    if (success) {
        emit gameLaunched(game.name);
    } else {
        emit launchFailed("Failed to start emulator process");
    }

    return success;
}

QString GameLauncher::buildCommandLine(const QString &launchArgs, const QString &gamePath) {
    QString args = launchArgs;

    QString quotedPath = gamePath;
    if (gamePath.contains(" ")) {
        quotedPath = "\"" + gamePath + "\"";
    }

    args.replace("{game}", quotedPath);
    return args;
}
