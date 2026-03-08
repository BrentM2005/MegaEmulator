#include "mainwindow.h"
#include "emulatordownloader.h"
#include "freegameslibrary.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QStatusBar>
#include <QDesktopServices>
#include <QProgressBar>
#include <QScrollArea>
#include <QFrame>

    MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    configPath = QDir::homePath() + "/.megaemulator";
    romsPath = configPath + "/roms";
    emulatorsPath = configPath + "/emulators";

    QDir().mkpath(configPath);
    QDir().mkpath(romsPath);
    QDir().mkpath(emulatorsPath);

    settings = new QSettings(configPath + "/config.ini", QSettings::IniFormat);
    gameManager = new GameManager(configPath, this);
    emulatorManager = new EmulatorManager(settings, this);
    gameLauncher = new GameLauncher(this);

    setupUI();
    setupMenuBar();

    emulatorManager->setupDefaultEmulators();
    loadData();

    connect(gameManager, &GameManager::gamesChanged, this, &MainWindow::refreshGameList);
    connect(gameLauncher, &GameLauncher::gameLaunched, this, [this](const QString &name) {
        statusBar()->showMessage("Launched: " + name, 3000);
    });
    connect(gameLauncher, &GameLauncher::launchFailed, this, [this](const QString &error) {
        QMessageBox::warning(this, "Launch Failed", error);
    });

    refreshGameList();
}

MainWindow::~MainWindow() {
    saveData();
    delete settings;
}

void MainWindow::setupUI() {
    setWindowTitle("Mega Emulator");
    resize(1280, 800);

    QString styleSheet = R"(
        QMainWindow {
            background-color: #1e1e1e;
            color: #f0f0f0;
        }
        QWidget {
            font-family: 'Segoe UI', sans-serif;
            font-size: 14px;
            color: #e0e0e0;
        }
        QMessageBox, QInputDialog {
            background-color: #ffffff;
            color: #333333;
        }
        QMessageBox QLabel, QInputDialog QLabel {
            color: #333333;
        }
        QMessageBox QPushButton, QInputDialog QPushButton {
            background-color: #e1e1e1;
            color: #333333;
            border: 1px solid #adadad;
            border-radius: 4px;
            padding: 6px 12px;
            min-width: 60px;
        }
        QMessageBox QPushButton:hover, QInputDialog QPushButton:hover {
            background-color: #e5f1fb;
            border-color: #0078d7;
        }
        QInputDialog QComboBox {
            background-color: #f9f9f9;
            color: #333333;
            border: 1px solid #cccccc;
            padding: 5px;
            border-radius: 3px;
        }
        QInputDialog QComboBox::drop-down {
            border: none;
            background: transparent;
            width: 20px;
        }
        QInputDialog QComboBox::down-arrow {
            image: none;
            border-left: 1px solid #cccccc;
            width: 0px;
            height: 0px;
        }
        QInputDialog QComboBox QAbstractItemView {
            background-color: #ffffff;
            color: #333333;
            selection-background-color: #007acc;
            selection-color: #ffffff;
            border: 1px solid #cccccc;
            outline: none;
        }
        QStatusBar {
            background-color: #252526;
            color: #888;
        }
        QLineEdit {
            background-color: #2d2d30;
            border: 1px solid #3e3e42;
            border-radius: 4px;
            padding: 8px;
            color: #ffffff;
            selection-background-color: #007acc;
        }
        QLineEdit:focus {
            border: 1px solid #007acc;
        }
        QComboBox {
            background-color: #2d2d30;
            border: 1px solid #3e3e42;
            border-radius: 4px;
            padding: 5px 10px;
            min-height: 25px;
            color: #f0f0f0;
        }
        QComboBox::drop-down {
            border: none;
            background: #2d2d30;
        }
        QListWidget {
            background-color: #252526;
            border: none;
            border-radius: 6px;
            outline: none;
        }
        QListWidget::item {
            padding: 12px;
            border-bottom: 1px solid #2d2d30;
            margin: 2px 5px;
            border-radius: 4px;
        }
        QListWidget::item:selected {
            background-color: #37373d;
            border-left: 3px solid #007acc;
            color: #ffffff;
        }
        QListWidget::item:hover:!selected {
            background-color: #2a2d2e;
        }
        QLabel {
            color: #cccccc;
        }
        QPushButton#LaunchButton {
            background-color: #007acc;
            color: white;
            border: none;
            border-radius: 4px;
            font-weight: bold;
            font-size: 16px;
            padding: 12px;
        }
        QPushButton#LaunchButton:hover {
            background-color: #0062a3;
        }
        QPushButton#LaunchButton:disabled {
            background-color: #333333;
            color: #666666;
        }
        QPushButton#SecondaryButton {
            background-color: transparent;
            border: 1px solid #3e3e42;
            color: #aaaaaa;
            border-radius: 4px;
            padding: 8px 12px;
        }
        QPushButton#SecondaryButton:hover {
            background-color: #3e3e42;
            color: #ffffff;
        }
        QPushButton#SecondaryButton:disabled {
            border-color: #2d2d30;
            color: #444444;
        }
        QPushButton {
            background-color: #3e3e42;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 12px;
        }
        QPushButton:hover {
            background-color: #4e4e52;
        }
        QPushButton:pressed {
            background-color: #007acc;
        }
        QFrame#SidePanel {
            background-color: #252526;
            border-right: 1px solid #1e1e1e;
        }
        QFrame#DetailPanel {
            background-color: #1e1e1e;
            border-left: 1px solid #2d2d30;
        }
        QScrollArea {
            border: none;
            background-color: transparent;
        }
        QScrollBar:vertical {
            border: none;
            background: #1e1e1e;
            width: 10px;
            margin: 0px 0px 0px 0px;
        }
        QScrollBar::handle:vertical {
            background: #3e3e42;
            min-height: 20px;
            border-radius: 5px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
        QGroupBox {
            border: 1px solid #3e3e42;
            border-radius: 6px;
            margin-top: 20px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px;
        }
    )";
    setStyleSheet(styleSheet);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QFrame *sidePanel = new QFrame();
    sidePanel->setObjectName("SidePanel");
    sidePanel->setFixedWidth(240);
    QVBoxLayout *sideLayout = new QVBoxLayout(sidePanel);
    sideLayout->setSpacing(15);
    sideLayout->setContentsMargins(15, 20, 15, 20);

    QLabel *appTitle = new QLabel("MEGA EMULATOR");
    appTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #ffffff; letter-spacing: 1px;");
    appTitle->setAlignment(Qt::AlignCenter);
    sideLayout->addWidget(appTitle);

    sideLayout->addSpacing(10);

    QLabel *libraryHeader = new QLabel("LIBRARY ACTIONS");
    libraryHeader->setStyleSheet("font-size: 11px; font-weight: bold; color: #666;");
    sideLayout->addWidget(libraryHeader);

    addGameBtn = new QPushButton("Add Game File");
    addGameBtn->setCursor(Qt::PointingHandCursor);
    connect(addGameBtn, &QPushButton::clicked, this, &MainWindow::addGame);
    sideLayout->addWidget(addGameBtn);

    scanRomsBtn = new QPushButton("Scan ROMs Folder");
    scanRomsBtn->setCursor(Qt::PointingHandCursor);
    connect(scanRomsBtn, &QPushButton::clicked, this, &MainWindow::scanRomsFolder);
    sideLayout->addWidget(scanRomsBtn);

    QPushButton *freeGamesBtn = new QPushButton("Free Games Store");
    freeGamesBtn->setStyleSheet("background-color: #6a1b9a;"); // Purple for store
    freeGamesBtn->setCursor(Qt::PointingHandCursor);
    connect(freeGamesBtn, &QPushButton::clicked, this, &MainWindow::openFreeGamesLibrary);
    sideLayout->addWidget(freeGamesBtn);

    sideLayout->addSpacing(15);

    QLabel *systemHeader = new QLabel("SYSTEM");
    systemHeader->setStyleSheet("font-size: 11px; font-weight: bold; color: #666;");
    sideLayout->addWidget(systemHeader);

    QPushButton *downloadEmuBtn = new QPushButton("Download Emulators");
    connect(downloadEmuBtn, &QPushButton::clicked, this, &MainWindow::downloadEmulator);
    sideLayout->addWidget(downloadEmuBtn);

    configEmuBtn = new QPushButton("Configure Paths");
    connect(configEmuBtn, &QPushButton::clicked, this, &MainWindow::configureEmulators);
    sideLayout->addWidget(configEmuBtn);

    sideLayout->addStretch();

    statsLabel = new QLabel("Games: 0");
    statsLabel->setStyleSheet("color: #666; font-size: 12px;");
    statsLabel->setAlignment(Qt::AlignCenter);
    sideLayout->addWidget(statsLabel);

    mainLayout->addWidget(sidePanel);

    QWidget *centerWidget = new QWidget();
    QVBoxLayout *centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setContentsMargins(20, 20, 20, 20);
    centerLayout->setSpacing(15);

    QHBoxLayout *topBarLayout = new QHBoxLayout();

    searchBox = new QLineEdit();
    searchBox->setPlaceholderText("Search your library...");
    searchBox->setClearButtonEnabled(true);
    connect(searchBox, &QLineEdit::textChanged, this, &MainWindow::searchGames);

    emulatorFilter = new QComboBox();
    emulatorFilter->setFixedWidth(150);
    emulatorFilter->addItem("All Systems");
    connect(emulatorFilter, &QComboBox::currentTextChanged, this, &MainWindow::filterByEmulator);

    topBarLayout->addWidget(searchBox, 1);
    topBarLayout->addWidget(emulatorFilter);
    centerLayout->addLayout(topBarLayout);

    gameList = new QListWidget();
    gameList->setFocusPolicy(Qt::NoFocus);
    gameList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    connect(gameList, &QListWidget::itemClicked, this, &MainWindow::onGameSelected);
    connect(gameList, &QListWidget::itemDoubleClicked, this, [this]() { launchGame(); });
    centerLayout->addWidget(gameList);

    mainLayout->addWidget(centerWidget, 1);

    QFrame *detailPanel = new QFrame();
    detailPanel->setObjectName("DetailPanel");
    detailPanel->setFixedWidth(350);
    QVBoxLayout *detailPanelLayout = new QVBoxLayout(detailPanel);
    detailPanelLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *inspectorScroll = new QScrollArea();
    inspectorScroll->setWidgetResizable(true);
    inspectorScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *inspectorContent = new QWidget();
    inspectorContent->setObjectName("InspectorContent");
    inspectorContent->setStyleSheet("background-color: transparent;");

    QVBoxLayout *detailLayout = new QVBoxLayout(inspectorContent);
    detailLayout->setContentsMargins(20, 30, 20, 20);
    detailLayout->setSpacing(15);

    coverLabel = new QLabel();
    coverLabel->setFixedSize(290, 290);
    coverLabel->setScaledContents(true);
    coverLabel->setStyleSheet("background-color: #252526; border-radius: 8px; border: 1px solid #3e3e42;");
    coverLabel->setAlignment(Qt::AlignCenter);
    coverLabel->setText("No Selection");
    detailLayout->addWidget(coverLabel, 0, Qt::AlignCenter);

    gameInfoLabel = new QLabel("Select a game to view details");
    gameInfoLabel->setWordWrap(true);
    gameInfoLabel->setStyleSheet("font-size: 14px; color: #aaa; line-height: 1.4;");
    gameInfoLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    detailLayout->addWidget(gameInfoLabel);

    detailLayout->addSpacing(10);

    launchBtn = new QPushButton("PLAY NOW");
    launchBtn->setObjectName("LaunchButton");
    launchBtn->setEnabled(false);
    launchBtn->setCursor(Qt::PointingHandCursor);
    launchBtn->setMinimumHeight(50);
    connect(launchBtn, &QPushButton::clicked, this, &MainWindow::launchGame);
    detailLayout->addWidget(launchBtn);

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #3e3e42;");
    detailLayout->addWidget(line);

    QLabel *manageLabel = new QLabel("Manage");
    manageLabel->setStyleSheet("color: #666; font-size: 11px; font-weight: bold; text-transform: uppercase;");
    detailLayout->addWidget(manageLabel);

    addCoverBtn = new QPushButton("Change Cover Art");
    addCoverBtn->setObjectName("SecondaryButton");
    addCoverBtn->setCursor(Qt::PointingHandCursor);
    addCoverBtn->setEnabled(false);
    connect(addCoverBtn, &QPushButton::clicked, this, &MainWindow::addCoverArt);
    detailLayout->addWidget(addCoverBtn);

    removeGameBtn = new QPushButton("Remove from Library");
    removeGameBtn->setObjectName("SecondaryButton");
    removeGameBtn->setStyleSheet("color: #ff5555; border-color: #552222;");
    removeGameBtn->setCursor(Qt::PointingHandCursor);
    removeGameBtn->setEnabled(false);
    connect(removeGameBtn, &QPushButton::clicked, this, &MainWindow::removeGame);
    detailLayout->addWidget(removeGameBtn);

    detailLayout->addStretch();

    inspectorScroll->setWidget(inspectorContent);
    detailPanelLayout->addWidget(inspectorScroll);
    mainLayout->addWidget(detailPanel);
}

void MainWindow::setupMenuBar() {
    menuBar()->setStyleSheet(
        "QMenuBar { background-color: #1e1e1e; color: #f0f0f0; border-bottom: 1px solid #2d2d30; }"
        "QMenuBar::item { background: transparent; spacing: 3px; padding: 4px 8px; }"
        "QMenuBar::item:selected { background: #3e3e42; }"
        "QMenu { background-color: #252526; color: #f0f0f0; border: 1px solid #3e3e42; }"
        "QMenu::item { padding: 6px 24px; }"
        "QMenu::item:selected { background-color: #007acc; }"
        );

    QMenu *fileMenu = menuBar()->addMenu("&File");

    QAction *freeGamesAction = new QAction("Games Store", this);
    connect(freeGamesAction, &QAction::triggered, this, &MainWindow::openFreeGamesLibrary);
    fileMenu->addAction(freeGamesAction);

    fileMenu->addSeparator();

    QAction *downloadAction = new QAction("Download Emulators", this);
    connect(downloadAction, &QAction::triggered, this, &MainWindow::downloadEmulator);
    fileMenu->addAction(downloadAction);

    fileMenu->addSeparator();

    QAction *scanAction = new QAction("Scan ROMs Folder", this);
    connect(scanAction, &QAction::triggered, this, &MainWindow::scanRomsFolder);
    fileMenu->addAction(scanAction);

    QAction *openRomsFolder = new QAction("Open ROMs Folder", this);
    connect(openRomsFolder, &QAction::triggered, this, [this]() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(romsPath));
    });
    fileMenu->addAction(openRomsFolder);

    fileMenu->addSeparator();

    QAction *exitAction = new QAction("Exit", this);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);

    QMenu *helpMenu = menuBar()->addMenu("&Help");
    QAction *aboutAction = new QAction("About", this);
    connect(aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "Mega Emulator",
                           "Mega Emulator v1.0\n\n"
                           "A modern, multi-system game launcher.\n\n"
                           "ROMs Folder: " + romsPath);
    });
    helpMenu->addAction(aboutAction);
}

void MainWindow::addGame() {
    QStringList emulators = emulatorManager->getEmulatorNames();

    bool ok;
    QString emulatorName = QInputDialog::getItem(this, "Select Emulator",
                                                 "Choose which emulator to use:",
                                                 emulators, 0, false, &ok);
    if (!ok || emulatorName.isEmpty()) return;

    Emulator *emu = emulatorManager->getEmulator(emulatorName);
    if (!emu) return;

    QString gamePath;

    if (emu->supportedExtensions.contains("dir")) {
        gamePath = QFileDialog::getExistingDirectory(
            this,
            "Select Game Folder",
            romsPath,
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
            );
    } else {
        QString filters =
            QString("%1 Games (%2);;All Files (*)")
                .arg(emulatorName)
                .arg(emu->supportedExtensions.join(" "));
        gamePath = QFileDialog::getOpenFileName(
            this, "Select Game File", romsPath, filters
            );
    }

    if (gamePath.isEmpty()) return;

    Game game;
    game.path = gamePath;
    game.emulator = emulatorName;
    game.name = QFileInfo(gamePath).completeBaseName();
    game.coverArt = "";
    game.playCount = 0;
    game.lastPlayed = "";

    gameManager->addGame(game);
    gameManager->saveGames();

    statusBar()->showMessage("Added: " + game.name, 3000);
}

void MainWindow::addCoverArt() {
    if (currentSelectedGame.isEmpty()) return;

    QString fileName = QFileDialog::getOpenFileName(this, "Select Cover Art",
                                                    QDir::homePath(),
                                                    "Images (*.png *.jpg *.jpeg *.bmp *.gif)");

    if (fileName.isEmpty()) return;

    Game *game = gameManager->findGame(currentSelectedGame);
    if (game) {
        game->coverArt = fileName;
        gameManager->updateGame(*game);
        gameManager->saveGames();
        updateGameInfo(game);
        statusBar()->showMessage("Cover art updated", 3000);
    }
}

void MainWindow::removeGame() {
    if (currentSelectedGame.isEmpty()) return;

    Game *game = gameManager->findGame(currentSelectedGame);
    if (!game) return;

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Remove Game",
                                                              QString("Remove '%1' from library?\n\nThe game file will not be deleted.").arg(game->name),
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        gameManager->removeGame(currentSelectedGame);
        gameManager->saveGames();

        currentSelectedGame.clear();
        gameInfoLabel->setText("Select a game to view details");
        coverLabel->setText("No Selection");
        launchBtn->setEnabled(false);
        addCoverBtn->setEnabled(false);
        removeGameBtn->setEnabled(false);

        statusBar()->showMessage("Game removed from library", 3000);
    }
}

void MainWindow::launchGame() {
    if (currentSelectedGame.isEmpty()) return;

    Game *game = gameManager->findGame(currentSelectedGame);
    if (!game) return;

    Emulator *emu = emulatorManager->getEmulator(game->emulator);
    if (!emu) return;

    if (emu->executablePath.isEmpty() || !QFileInfo::exists(emu->executablePath)) {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                  "Emulator Not Configured",
                                                                  QString("The %1 emulator path is not set or invalid.\n\n"
                                                                          "Would you like to configure it now?").arg(emu->name),
                                                                  QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            configureEmulators();
        }
        return;
    }

    if (gameLauncher->launchGame(*game, *emu)) {
        game->playCount++;
        game->lastPlayed = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm");
        gameManager->updateGame(*game);
        gameManager->saveGames();
        updateGameInfo(game);
    }
}

void MainWindow::configureEmulators() {
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Configure Emulators");
    dialog->resize(750, 600);
    dialog->setStyleSheet("background-color: #1e1e1e; color: #f0f0f0;");

    QVBoxLayout *layout = new QVBoxLayout(dialog);

    QLabel *infoLabel = new QLabel(
        "Configure the executable path for each emulator.\n"
        "Only configured emulators will be able to launch games.");
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("QLabel { padding: 15px; background-color: #252526; border-radius: 5px; color: #ccc; }");
    layout->addWidget(infoLabel);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; } QWidget { background: transparent; }");

    QWidget *scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setSpacing(15);

    const QMap<QString, Emulator> &emulators = emulatorManager->getAllEmulators();
    for (auto it = emulators.begin(); it != emulators.end(); ++it) {
        QGroupBox *box = new QGroupBox(it.key());
        box->setStyleSheet("QGroupBox { border: 1px solid #3e3e42; border-radius: 6px; margin-top: 20px; font-weight: bold; color: #007acc; } QGroupBox::title { subcontrol-origin: margin; left: 10px; }");

        QVBoxLayout *boxLayout = new QVBoxLayout(box);
        boxLayout->setContentsMargins(15, 25, 15, 15);

        QHBoxLayout *pathLayout = new QHBoxLayout();
        QLabel *pathLabel = new QLabel("Executable:");
        pathLabel->setStyleSheet("color: #ccc;");

        QLineEdit *pathEdit = new QLineEdit(it.value().executablePath);
        pathEdit->setReadOnly(true);
        pathEdit->setStyleSheet("background-color: #2d2d30; color: #fff; padding: 6px; border: 1px solid #3e3e42; border-radius: 4px;");

        QPushButton *browseBtn = new QPushButton("Browse");
        browseBtn->setStyleSheet("background-color: #3e3e42; color: white; padding: 6px 12px; border-radius: 4px;");

        connect(browseBtn, &QPushButton::clicked, [this, pathEdit, it]() {
            QString path = QFileDialog::getOpenFileName(this,
                                                        "Select " + it.key() + " Executable",
                                                        QDir::homePath() + "/.megaemulator/emulators",
                                                        "Executables (*.exe *.app *);;All Files (*)");
            if (!path.isEmpty()) {
                pathEdit->setText(path);
                Emulator *emu = emulatorManager->getEmulator(it.key());
                if (emu) {
                    emu->executablePath = path;
                }
            }
        });

        pathLayout->addWidget(pathLabel);
        pathLayout->addWidget(pathEdit, 1);
        pathLayout->addWidget(browseBtn);
        boxLayout->addLayout(pathLayout);

        QLabel *argsLabel = new QLabel(QString("Launch Arguments: %1").arg(it.value().launchArgs));
        argsLabel->setStyleSheet("color: #777; font-size: 11px;");
        boxLayout->addWidget(argsLabel);

        scrollLayout->addWidget(box);
    }

    scrollContent->setLayout(scrollLayout);
    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    QPushButton *saveBtn = new QPushButton("Save and Close");
    saveBtn->setStyleSheet("QPushButton { padding: 10px 20px; background-color: #007acc; color: white; border-radius: 4px; font-weight: bold; } QPushButton:hover { background-color: #0062a3; }");
    connect(saveBtn, &QPushButton::clicked, [this, dialog]() {
        emulatorManager->saveSettings();
        dialog->accept();
        statusBar()->showMessage("Emulator settings saved", 3000);
    });
    btnLayout->addWidget(saveBtn);

    layout->addLayout(btnLayout);

    dialog->exec();
}

void MainWindow::scanRomsFolder() {
    QMessageBox::information(this, "Scan ROMs Folder",
                             QString("This will scan the ROMs folder for supported game files:\n%1\n\n"
                                     "Files will be automatically categorized by extension.").arg(romsPath));

    int addedCount = 0;
    QDir romsDir(romsPath);

    const QMap<QString, Emulator> &emulators = emulatorManager->getAllEmulators();
    for (auto it = emulators.begin(); it != emulators.end(); ++it) {
        const Emulator &emu = it.value();

        for (const QString &ext : emu.supportedExtensions) {
            QString pattern = ext;
            pattern.replace("*.", "");

            QStringList files = romsDir.entryList(
                QStringList() << "*." + pattern,
                QDir::Files
                );

            for (const QString &file : files) {
                QString fullPath = romsDir.filePath(file);

                // Check if already in library
                if (gameManager->findGame(fullPath)) {
                    continue;
                }

                Game game;
                game.path = fullPath;
                game.emulator = it.key();
                game.name = QFileInfo(file).completeBaseName();
                game.coverArt = "";
                game.playCount = 0;

                gameManager->addGame(game);
                addedCount++;
            }
        }
    }

    if (addedCount > 0) {
        gameManager->saveGames();
        QMessageBox::information(this, "Scan Complete",
                                 QString("Added %1 new game(s) to the library.").arg(addedCount));
    } else {
        QMessageBox::information(this, "Scan Complete",
                                 "No new games found in the ROMs folder.");
    }
}

void MainWindow::onGameSelected(QListWidgetItem *item) {
    if (!item) return;

    currentSelectedGame = item->data(Qt::UserRole).toString();
    Game *game = gameManager->findGame(currentSelectedGame);

    if (game) {
        updateGameInfo(game);
        launchBtn->setEnabled(true);
        addCoverBtn->setEnabled(true);
        removeGameBtn->setEnabled(true);
    }
}

void MainWindow::updateGameInfo(Game *game) {
    if (!game) return;

    QString info = QString(
                       "<h1 style='margin-bottom: 5px; color: #ffffff;'>%1</h1>"
                       "<p style='color: #007acc; font-weight: bold; margin-top: 0;'>%2</p>"
                       "<hr style='border: 1px solid #3e3e42;'>"
                       "<p style='color: #ccc;'><b>Location:</b><br>%3</p>"
                       "<p style='color: #ccc;'><b>Stats:</b><br>Played %4 times<br>Last played: %5</p>"
                       ).arg(game->name)
                       .arg(game->emulator)
                       .arg(game->path)
                       .arg(game->playCount)
                       .arg(game->lastPlayed.isEmpty() ? "Never" : game->lastPlayed);

    gameInfoLabel->setText(info);

    if (!game->coverArt.isEmpty() && QFileInfo::exists(game->coverArt)) {
        coverLabel->setPixmap(QPixmap(game->coverArt));
    } else {
        coverLabel->setText(game->name);
        coverLabel->setStyleSheet("QLabel { background-color: #252526; color: #666; font-size: 18px; border-radius: 8px; border: 1px solid #3e3e42; }");
    }
}

void MainWindow::filterByEmulator(const QString &emulator) {
    refreshGameList();
}

void MainWindow::searchGames(const QString &text) {
    refreshGameList();
}

void MainWindow::refreshGameList() {
    gameList->clear();
    QString filter = emulatorFilter->currentText();
    QString search = searchBox->text().toLower();

    QList<Game> games = gameManager->getGamesByEmulator(filter);
    int count = 0;

    for (const auto &game : games) {
        // Apply search filter
        if (!search.isEmpty() && !game.name.toLower().contains(search)) {
            continue;
        }

        QString displayName = QString("%1  |  %2").arg(game.name).arg(game.emulator);

        QListWidgetItem *item = new QListWidgetItem(displayName);
        item->setData(Qt::UserRole, game.path);

        if (game.playCount > 10) {
            item->setForeground(QColor("#ffd700"));
        } else {
            item->setForeground(QColor("#e0e0e0"));
        }

        QFont font = item->font();
        font.setPointSize(11);
        item->setFont(font);

        gameList->addItem(item);
        count++;
    }

    statsLabel->setText(QString("Total Games: %1").arg(gameManager->getAllGames().size()));
}

void MainWindow::loadData() {
    emulatorManager->loadSettings();
    gameManager->loadGames();

    emulatorFilter->clear();
    emulatorFilter->addItem("All Systems");
    emulatorFilter->addItems(emulatorManager->getEmulatorNames());
}

void MainWindow::saveData() {
    emulatorManager->saveSettings();
    gameManager->saveGames();
}

void MainWindow::downloadEmulator() {
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Download Emulators");
    dialog->resize(600, 600);
    dialog->setStyleSheet("background-color: #1e1e1e; color: #f0f0f0;");

    QVBoxLayout *layout = new QVBoxLayout(dialog);

    QLabel *infoLabel = new QLabel(
        "Install emulators directly to: " + emulatorsPath + "\n\n"
                                                            "Click 'Download' to automatically download and extract."
        );
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("QLabel { padding: 15px; background-color: #007acc; border-radius: 5px; color: white; margin-bottom: 10px; }");
    layout->addWidget(infoLabel);

    QGroupBox *progressBox = new QGroupBox("Download Progress");
    progressBox->setStyleSheet("QGroupBox { border: 1px solid #3e3e42; margin-top: 10px; color: #ccc; }");
    QVBoxLayout *progressLayout = new QVBoxLayout(progressBox);

    QLabel *statusLabel = new QLabel("Ready to download");
    statusLabel->setStyleSheet("color: #aaa;");
    progressLayout->addWidget(statusLabel);

    QProgressBar *progressBar = new QProgressBar();
    progressBar->setStyleSheet("QProgressBar { border: 1px solid #3e3e42; border-radius: 4px; text-align: center; } QProgressBar::chunk { background-color: #007acc; }");
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressLayout->addWidget(progressBar);

    layout->addWidget(progressBox);
    progressBox->setVisible(false);

    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("background: transparent; border: none;");
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);

    struct EmulatorDownload {
        QString name;
        QString displayName;
        QString url;
        QString description;
    };

    QList<EmulatorDownload> emulators = {
        {"rpcs3", "RPCS3 (PS3)",
         "https://megaemulator.s3.us-east-2.amazonaws.com/rpcs3.zip",
         "PlayStation 3 emulator"},
        {"dolphin", "Dolphin (GameCube/Wii)",
         "https://megaemulator.s3.us-east-2.amazonaws.com/dolphin.zip",
         "GameCube and Wii emulator"},
        {"pcsx2", "PCSX2 (PS2)",
         "https://megaemulator.s3.us-east-2.amazonaws.com/pcsx2.zip",
         "PlayStation 2 emulator"},
        {"cemu", "Cemu (Wii U)",
         "https://megaemulator.s3.us-east-2.amazonaws.com/cemu.zip",
         "Wii U emulator"},
        {"ppsspp", "PPSSPP (PSP)",
         "https://megaemulator.s3.us-east-2.amazonaws.com/ppsspp.zip",
         "PlayStation Portable emulator"},
        {"mgba", "mGBA (GBA/GB/GBC)",
         "https://megaemulator.s3.us-east-2.amazonaws.com/mgba.zip",
         "Game Boy Advance emulator"},
        {"citra", "citra (3DS)",
         "https://megaemulator.s3.us-east-2.amazonaws.com/citra-windows-msvc-20240303-0ff3440_nightly.zip",
         "3DS emulator"},
        {"melonDS", "melonDS (DS)",
         "https://megaemulator.s3.us-east-2.amazonaws.com/melonDS-1.1-windows-x86_64.zip",
         "DS emulator"}
    };

    EmulatorDownloader *downloader = new EmulatorDownloader(emulatorsPath, this);

    for (const auto &emu : emulators) {
        QGroupBox *box = new QGroupBox(emu.displayName);
        box->setStyleSheet("QGroupBox { border: 1px solid #3e3e42; border-radius: 4px; font-weight: bold; color: #fff; background-color: #252526; }");
        QVBoxLayout *boxLayout = new QVBoxLayout(box);

        QLabel *description = new QLabel(emu.description);
        description->setStyleSheet("QLabel { font-size: 12px; color: #aaa; }");
        boxLayout->addWidget(description);

        QHBoxLayout *btnLayout = new QHBoxLayout();
        btnLayout->addStretch();

        QPushButton *downloadBtn = new QPushButton("Download");
        downloadBtn->setStyleSheet("QPushButton { padding: 6px 15px; background-color: #2d2d30; color: white; border: 1px solid #3e3e42; border-radius: 3px; } QPushButton:hover { background-color: #007acc; border-color: #007acc; }");

        connect(downloadBtn, &QPushButton::clicked, [this, emu, progressBox, statusLabel, progressBar, dialog, downloader]() {
            progressBox->setVisible(true);
            statusLabel->setText("Downloading " + emu.displayName + "...");
            progressBar->setValue(0);

            for (auto child : dialog->findChildren<QPushButton*>()) {
                if (child->text() == "Download") {
                    child->setEnabled(false);
                }
            }

            downloader->downloadEmulator(emu.name, emu.url);
        });

        btnLayout->addWidget(downloadBtn);
        boxLayout->addLayout(btnLayout);

        scrollLayout->addWidget(box);
    }

    scrollContent->setLayout(scrollLayout);
    scroll->setWidget(scrollContent);
    layout->addWidget(scroll);

    connect(downloader, &EmulatorDownloader::downloadProgress,
            [progressBar](qint64 received, qint64 total) {
                if (total > 0) {
                    int percentage = (received * 100) / total;
                    progressBar->setValue(percentage);
                }
            });

    connect(downloader, &EmulatorDownloader::statusMessage,
            [statusLabel](const QString &msg) {
                statusLabel->setText(msg);
            });

    connect(downloader, &EmulatorDownloader::extractionFinished,
            [this, dialog, statusLabel, progressBar](bool success) {
                if (success) {
                    statusLabel->setText("Installation complete!");
                    progressBar->setValue(100);

                    for (auto child : dialog->findChildren<QPushButton*>()) {
                        if (child->text() == "Download") {
                            child->setEnabled(true);
                        }
                    }

                    QMessageBox::information(dialog, "Success",
                                             "Emulator installed successfully!\n\n"
                                             "Go to configure emulators, browse, find the correct folder, and add the .exe");
                } else {
                    statusLabel->setText("Installation failed. Please try again.");
                    progressBar->setValue(0);

                    for (auto child : dialog->findChildren<QPushButton*>()) {
                        if (child->text() == "Download") {
                            child->setEnabled(true);
                        }
                    }
                }
            });

    QPushButton *openFolderBtn = new QPushButton("Open Emulators Folder");
    openFolderBtn->setStyleSheet("QPushButton { padding: 10px; background-color: #3e3e42; color: white; }");
    connect(openFolderBtn, &QPushButton::clicked, [this]() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(emulatorsPath));
    });
    layout->addWidget(openFolderBtn);

    QPushButton *closeBtn = new QPushButton("Close");
    closeBtn->setStyleSheet("padding: 10px;");
    connect(closeBtn, &QPushButton::clicked, dialog, &QDialog::accept);
    layout->addWidget(closeBtn);

    dialog->exec();

    delete downloader;
}

void MainWindow::openFreeGamesLibrary() {
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Free Games Library");
    dialog->resize(900, 700);
    dialog->setStyleSheet("background-color: #1e1e1e; color: #f0f0f0;");

    QVBoxLayout *layout = new QVBoxLayout(dialog);

    QLabel *infoLabel = new QLabel(
        "Games you can download right from the app\n\n"
        "These are some of my personal favourites. "
        "All games are free to play."
        );
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet(
        "QLabel { padding: 20px; background-color: #6a1b9a; color: white; "
        "margin-bottom: 10px; font-size: 13px; border-radius: 6px; }"
        );
    layout->addWidget(infoLabel);

    QHBoxLayout *filterLayout = new QHBoxLayout();
    QLabel *filterLabel = new QLabel("Filter by System:");
    filterLabel->setStyleSheet("color: #ccc; font-weight: bold;");

    QComboBox *systemFilter = new QComboBox();
    systemFilter->setStyleSheet("background-color: #2d2d30; color: white; padding: 5px; border: 1px solid #3e3e42;");
    systemFilter->addItem("All Systems");
    systemFilter->addItems(emulatorManager->getEmulatorNames());

    filterLayout->addWidget(filterLabel);
    filterLayout->addWidget(systemFilter);
    filterLayout->addStretch();
    layout->addLayout(filterLayout);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    QWidget *gamesWidget = new QWidget();
    gamesWidget->setStyleSheet("background: transparent;");
    QVBoxLayout *gamesLayout = new QVBoxLayout(gamesWidget);

    QGroupBox *progressBox = new QGroupBox("Download Progress");
    progressBox->setStyleSheet("QGroupBox { border: 1px solid #3e3e42; margin-top: 10px; color: #ccc; }");
    QVBoxLayout *progressLayout = new QVBoxLayout(progressBox);

    QLabel *statusLabel = new QLabel("Ready to download");
    statusLabel->setStyleSheet("color: #aaa;");
    progressLayout->addWidget(statusLabel);

    QProgressBar *progressBar = new QProgressBar();
    progressBar->setStyleSheet("QProgressBar { border: 1px solid #3e3e42; border-radius: 4px; text-align: center; } QProgressBar::chunk { background-color: #6a1b9a; }");
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressLayout->addWidget(progressBar);

    layout->addWidget(progressBox);
    progressBox->setVisible(false);

    FreeGamesLibrary *library = new FreeGamesLibrary(this);
    EmulatorDownloader *downloader = new EmulatorDownloader(romsPath, this);

    auto populateGames = [=, this](const QString &filter) {
        QLayoutItem *item;
        while ((item = gamesLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }

        QList<FreeGame> games = library->getGamesByEmulator(filter);

        if (games.isEmpty()) {
            QLabel *noGames = new QLabel("No games available for this system yet.");
            noGames->setStyleSheet("QLabel { padding: 20px; color: #888; font-style: italic; }");
            gamesLayout->addWidget(noGames);
        }

        for (const auto &game : games) {
            QGroupBox *gameBox = new QGroupBox();
            gameBox->setStyleSheet("QGroupBox { background-color: #252526; border: 1px solid #3e3e42; border-radius: 6px; }");
            QVBoxLayout *gameLayout = new QVBoxLayout(gameBox);

            QLabel *titleLabel = new QLabel(game.name);
            titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #fff;");
            gameLayout->addWidget(titleLabel);

            QString sizeStr = QString::number(game.sizeBytes / (1024.0 * 1024.0), 'f', 1) + " MB";
            QLabel *infoText = new QLabel(
                game.description + "\n\n"
                                   "System: " + game.emulator + " | "
                                  "Size: " + sizeStr + " | "
                            "License: " + game.license
                );
            infoText->setWordWrap(true);
            infoText->setStyleSheet("QLabel { color: #aaa; font-size: 12px; margin-top: 5px; }");
            gameLayout->addWidget(infoText);

            QHBoxLayout *btnLayout = new QHBoxLayout();

            QPushButton *downloadBtn = new QPushButton("Download Game");
            downloadBtn->setStyleSheet(
                "QPushButton { padding: 8px 20px; background-color: #6a1b9a; color: white; border-radius: 4px; font-weight: bold; } QPushButton:hover { background-color: #8e24aa; }"
                );

            connect(downloadBtn, &QPushButton::clicked,
                    [this, game, progressBox, statusLabel, progressBar, gamesWidget, downloader]() {
                        progressBox->setVisible(true);
                        statusLabel->setText("Downloading " + game.name + "...");
                        progressBar->setValue(0);

                        for (auto child : gamesWidget->findChildren<QPushButton*>()) {
                            child->setEnabled(false);
                        }

                        downloader->downloadEmulator(game.id, game.downloadUrl);
                    });

            btnLayout->addWidget(downloadBtn);
            btnLayout->addStretch();
            gameLayout->addLayout(btnLayout);

            gamesLayout->addWidget(gameBox);
        }

        gamesLayout->addStretch();
    };

    populateGames("All Systems");

    connect(systemFilter, &QComboBox::currentTextChanged, populateGames);

    scrollArea->setWidget(gamesWidget);
    layout->addWidget(scrollArea);

    connect(downloader, &EmulatorDownloader::downloadProgress,
            [progressBar](qint64 received, qint64 total) {
                if (total > 0) {
                    int percentage = (received * 100) / total;
                    progressBar->setValue(percentage);
                }
            });

    connect(downloader, &EmulatorDownloader::statusMessage,
            [statusLabel](const QString &msg) {
                statusLabel->setText(msg);
            });

    connect(downloader, &EmulatorDownloader::downloadFinished,
            [this, dialog, statusLabel, progressBar, gamesWidget, library](bool success, const QString &filePath) {
                if (success) {
                    statusLabel->setText("Download complete! Adding to library...");
                    progressBar->setValue(100);

                    for (auto child : gamesWidget->findChildren<QPushButton*>()) {
                        child->setEnabled(true);
                    }

                    QFileInfo fileInfo(filePath);
                    QString fileName = fileInfo.completeBaseName();

                    QString emulator = "";
                    for (const auto &freeGame : library->getAllGames()) {
                        if (filePath.contains(freeGame.id)) {
                            emulator = freeGame.emulator;
                            fileName = freeGame.name;
                            break;
                        }
                    }

                    if (!emulator.isEmpty()) {
                        Game newGame;
                        newGame.name = fileName;
                        newGame.path = filePath;
                        newGame.emulator = emulator;
                        newGame.coverArt = "";
                        newGame.playCount = 0;
                        newGame.lastPlayed = "";

                        gameManager->addGame(newGame);
                        gameManager->saveGames();

                        statusLabel->setText("Game added to library: " + fileName);

                        QMessageBox::information(dialog, "Success",
                                                 "Game downloaded and added to your library!\n\n"
                                                 "Name: " + fileName + "\n"
                                                                  "System: " + emulator + "\n"
                                                                  "File: " + fileInfo.fileName());
                    } else {
                        statusLabel->setText("Downloaded: " + fileInfo.fileName());
                    }

                } else {
                    statusLabel->setText("Download failed. Please try again.");
                    progressBar->setValue(0);

                    for (auto child : gamesWidget->findChildren<QPushButton*>()) {
                        child->setEnabled(true);
                    }
                }
            });

    QPushButton *closeBtn = new QPushButton("Close");
    closeBtn->setStyleSheet("padding: 10px;");
    connect(closeBtn, &QPushButton::clicked, dialog, &QDialog::accept);
    layout->addWidget(closeBtn);

    dialog->exec();

    delete downloader;
    delete library;
}
