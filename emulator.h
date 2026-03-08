#ifndef EMULATOR_H
#define EMULATOR_H

#include <QString>
#include <QStringList>

struct Emulator {
    QString name;
    QString executablePath;
    QString launchArgs;
    QStringList supportedExtensions;

    Emulator() {}
    Emulator(const QString &n, const QString &args, const QStringList &ext)
        : name(n), executablePath(""), launchArgs(args), supportedExtensions(ext) {}
};

#endif // EMULATOR_H
