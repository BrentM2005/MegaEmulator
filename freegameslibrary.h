#ifndef FREEGAMESLIBRARY_H
#define FREEGAMESLIBRARY_H

#include <QObject>
#include <QString>
#include <QList>

struct FreeGame {
    QString id;
    QString name;
    QString description;
    QString emulator;
    QString downloadUrl;
    QString coverImageUrl;
    qint64 sizeBytes;
    QString license;

    FreeGame() : sizeBytes(0) {}
    FreeGame(const QString &id, const QString &name, const QString &desc,
             const QString &emu, const QString &url, qint64 size,
             const QString &coverUrl = "", const QString &lic = "Open Source")
        : id(id), name(name), description(desc), emulator(emu),
        downloadUrl(url), coverImageUrl(coverUrl), sizeBytes(size), license(lic) {}
};

class FreeGamesLibrary : public QObject {
    Q_OBJECT

public:
    explicit FreeGamesLibrary(QObject *parent = nullptr);

    QList<FreeGame> getAllGames() const;
    QList<FreeGame> getGamesByEmulator(const QString &emulator) const;
    FreeGame getGameById(const QString &id) const;

private:
    void loadGameCatalog();
    QList<FreeGame> games;
};

#endif // FREEGAMESLIBRARY_H
