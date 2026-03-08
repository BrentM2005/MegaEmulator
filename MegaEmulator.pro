QT       += core gui widgets network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
SOURCES += \
    emulatordownloader.cpp \
    emulatormanager.cpp \
    freegameslibrary.cpp \
    gamelauncher.cpp \
    gamemanager.cpp \
    main.cpp \
    mainwindow.cpp
HEADERS += \
    emulator.h \
    emulatordownloader.h \
    emulatormanager.h \
    freegameslibrary.h \
    game.h \
    gamelauncher.h \
    gamemanager.h \
    mainwindow.h
FORMS +=
TRANSLATIONS += \
    MegaEmulator_en_CA.ts
CONFIG += lrelease
CONFIG += embed_translations

RESOURCES += \
    resources.qrc

# Embeds the icon into the .exe and removes the console window
RC_FILE = app.rc
win32: CONFIG += windows

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
