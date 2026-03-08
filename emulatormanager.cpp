#include "emulatormanager.h"
#include <QFileInfo>
#include <QDir>

EmulatorManager::EmulatorManager(QSettings *settings, QObject *parent)
    : QObject(parent), settings(settings) {
}

void EmulatorManager::addEmulator(const QString &name, const Emulator &emulator) {
    emulators[name] = emulator;
    emit emulatorsChanged();
}

Emulator* EmulatorManager::getEmulator(const QString &name) {
    if (emulators.contains(name)) {
        return &emulators[name];
    }
    return nullptr;
}

void EmulatorManager::setupDefaultEmulators() {
    QString emulatorsPath = QDir::homePath() + "/.emulauncher/emulators";

    // RPCS3
    Emulator rpcs3("RPCS3", "{game}", QStringList() << "dir");
    QString rpcs3Path = emulatorsPath + "/rpcs3/rpcs3.exe";
    if (QFileInfo::exists(rpcs3Path)) {
        rpcs3.executablePath = rpcs3Path;
    }
    addEmulator("RPCS3", rpcs3);


    // Dolphin
    Emulator dolphin("Dolphin", "-e {game}",
                     QStringList() << "*.iso" << "*.gcm" << "*.wbfs" << "*.ciso" << "*.gcz" << "*.wad");
    QString dolphinPath = emulatorsPath + "/dolphin/Dolphin.exe";
    if (QFileInfo::exists(dolphinPath)) {
        dolphin.executablePath = dolphinPath;
    }
    addEmulator("Dolphin", dolphin);

    // PCSX2
    Emulator pcsx2("PCSX2", "{game}",
                   QStringList() << "*.iso" << "*.bin" << "*.img");
    QString pcsx2Path = emulatorsPath + "/pcsx2/pcsx2-qt.exe";
    if (QFileInfo::exists(pcsx2Path)) {
        pcsx2.executablePath = pcsx2Path;
    }
    addEmulator("PCSX2", pcsx2);

    // Cemu
    Emulator cemu("Cemu", "-g {game}",
                  QStringList() << "*.wud" << "*.wux" << "*.rpx");
    QString cemuPath = emulatorsPath + "/cemu/Cemu.exe";
    if (QFileInfo::exists(cemuPath)) {
        cemu.executablePath = cemuPath;
    }
    addEmulator("Cemu", cemu);

    // PPSSPP
    Emulator ppsspp("PPSSPP", "{game}",
                    QStringList() << "*.iso" << "*.cso" << "*.pbp");
    QString ppssppPath = emulatorsPath + "/ppsspp/PPSSPPWindows64.exe";
    if (QFileInfo::exists(ppssppPath)) {
        ppsspp.executablePath = ppssppPath;
    }
    addEmulator("PPSSPP", ppsspp);

    // mGBA
    Emulator mgba("mGBA", "{game}",
                  QStringList() << "*.gba" << "*.gb" << "*.gbc");
    QString mgbaPath = emulatorsPath + "/mgba/mGBA.exe";
    if (QFileInfo::exists(mgbaPath)) {
        mgba.executablePath = mgbaPath;
    }
    addEmulator("mGBA", mgba);

    // Citra
    Emulator citra("Citra", "{game}",
                  QStringList() << "*.cci" << "*.3ds");
    QString citraPath = emulatorsPath + "/citra/citra.exe";
    if (QFileInfo::exists(citraPath)) {
        mgba.executablePath = citraPath;
    }
    addEmulator("Citra", citra);

    // melonDS
    Emulator melonDS("melonDS", "{game}",
                   QStringList() << "*.cci" << "*.3ds");
    QString melonDSPath = emulatorsPath + "/melonDS/melonDS.exe";
    if (QFileInfo::exists(melonDSPath)) {
        mgba.executablePath = melonDSPath;
    }
    addEmulator("melonDS", melonDS);
}

void EmulatorManager::loadSettings() {
    settings->beginGroup("Emulators");
    for (auto it = emulators.begin(); it != emulators.end(); ++it) {
        QString path = settings->value(it.key() + "_path", "").toString();
        it.value().executablePath = path;
    }
    settings->endGroup();
}

void EmulatorManager::saveSettings() {
    settings->beginGroup("Emulators");
    for (auto it = emulators.begin(); it != emulators.end(); ++it) {
        settings->setValue(it.key() + "_path", it.value().executablePath);
    }
    settings->endGroup();
    settings->sync();
}
