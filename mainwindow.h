#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QSettings>
#include <QLineEdit>
#include "gamemanager.h"
#include "gamelauncher.h"
#include "emulatormanager.h"
#include "freegameslibrary.h"

    class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addGame();
    void removeGame();
    void launchGame();
    void addCoverArt();
    void configureEmulators();
    void onGameSelected(QListWidgetItem *item);
    void filterByEmulator(const QString &emulator);
    void searchGames(const QString &text);
    void refreshGameList();
    void scanRomsFolder();
    void downloadEmulator();
    void openFreeGamesLibrary();

private:
    void setupUI();
    void setupMenuBar();
    void loadData();
    void saveData();
    void updateGameInfo(Game *game);

    // UI Elements
    QListWidget *gameList;
    QPushButton *addGameBtn;
    QPushButton *removeGameBtn;
    QPushButton *launchBtn;
    QPushButton *addCoverBtn;
    QPushButton *configEmuBtn;
    QPushButton *scanRomsBtn;
    QComboBox *emulatorFilter;
    QLineEdit *searchBox;
    QLabel *gameInfoLabel;
    QLabel *coverLabel;
    QLabel *statsLabel;

    // Managers
    GameManager *gameManager;
    EmulatorManager *emulatorManager;
    GameLauncher *gameLauncher;
    QSettings *settings;

    QString currentSelectedGame;
    QString configPath;
    QString romsPath;
    QString emulatorsPath;
};

#endif // MAINWINDOW_H
