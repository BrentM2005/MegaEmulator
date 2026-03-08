#ifndef EMULATORMANAGER_H
#define EMULATORMANAGER_H

#include <QObject>
#include <QMap>
#include <QSettings>
#include "emulator.h"

class EmulatorManager : public QObject {
    Q_OBJECT

public:
    explicit EmulatorManager(QSettings *settings, QObject *parent = nullptr);

    void addEmulator(const QString &name, const Emulator &emulator);
    Emulator* getEmulator(const QString &name);
    const QMap<QString, Emulator>& getAllEmulators() const { return emulators; }
    QStringList getEmulatorNames() const { return emulators.keys(); }

    void loadSettings();
    void saveSettings();
    void setupDefaultEmulators();

signals:
    void emulatorsChanged();

private:
    QMap<QString, Emulator> emulators;
    QSettings *settings;
};

#endif // EMULATORMANAGER_H
