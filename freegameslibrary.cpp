#include "freegameslibrary.h"

FreeGamesLibrary::FreeGamesLibrary(QObject *parent) : QObject(parent) {
    loadGameCatalog();
}

void FreeGamesLibrary::loadGameCatalog() {

    // GameCube/Wii Games (Dolphin) - .iso, .wbfs, .gcm, .ciso
    games.append(FreeGame(
        "mariokartwii.iso",
        "Mario Kart Wii",
        "2008 Mario Kart game for the Wii.",
        "Dolphin",
        "https://megaemulator.s3.us-east-2.amazonaws.com/games/Mario+Kart+Wii+(USA)+(En%2CFr%2CEs).wbfs",
        2.7 * 1024 * 1024 * 1024,  // 2.7 GB
        "",
        "Wii"
        ));

    games.append(FreeGame(
        "wiimenu.wad",
        "Wii Menu",
        "The wii home screen.",
        "Dolphin",
        "https://megaemulator.s3.us-east-2.amazonaws.com/games/Wii+Menu+(USA)+(v4.3).wad",
        21 * 1024 * 1024,  // 21 MB
        "",
        "Wii"
        ));

    games.append(FreeGame(
        "lozwindwaker.ciso",
        "Legend of Zelda Windwaker",
        "Cartoony Legend of Zelda title.",
        "Dolphin",
        "https://megaemulator.s3.us-east-2.amazonaws.com/games/Legend+of+Zelda%2C+The+-+The+Wind+Waker+(USA%2C+Canada).ciso",
        1 * 1024 * 1024 * 1024,  // 1 GB
        "",
        "Gamecube"
        ));

    // PSP Games (PPSSPP) - .iso, .cso
    games.append(FreeGame(
        "gowchainsofolympus.iso",
        "God of War Chains of Olympus",
        "From 2008, the fourth God of War game.",
        "PPSSPP",
        "https://megaemulator.s3.us-east-2.amazonaws.com/games/God+of+War+-+Chains+of+Olympus+(USA).iso",
        1.5 * 1024 * 1024 * 1024,
        "",
        "PSP"
        ));

    // GBA Games (mGBA) - .gba, .gb, .gbc
    games.append(FreeGame(
        "pokemonemerald.gba",
        "Pokemon Emerald",
        "Generation 3 Pokemon game.",
        "mGBA",
        "https://megaemulator.s3.us-east-2.amazonaws.com/games/Pokemon+-+Emerald+Version+(USA%2C+Europe).gba",
        16 * 1024 * 1024,  // 16 MB
        "",
        "GBA"
        ));

    // Nintendo DS Games - .nds
    games.append(FreeGame(
        "mariods.nds",
        "New Super Mario Bros. DS",
        "2D mario platformer for the DS.",
        "melonDS",
        "https://megaemulator.s3.us-east-2.amazonaws.com/games/New+Super+Mario+Bros.+(USA%2C+Australia).nds",
        33 * 1024 * 1024,  // 33 MB
        "",
        "DS"
        ));

    // PS2 Games (PCSX2) - .iso
    games.append(FreeGame(
        "hni2victoriousroad.iso",
        "Hajime No Ippo 2: Victorious Road",
        "GOATed Hajime No Ippo game.",
        "PCSX2",
        "https://megaemulator.s3.us-east-2.amazonaws.com/games/Hajime+no+Ippo+2+-+Victorious+Road+(Japan).iso",
        2.8 * 1024 * 1024 * 1024,  // 2.8 GB
        "",
        "PS2"
        ));
}

QList<FreeGame> FreeGamesLibrary::getAllGames() const {
    return games;
}

QList<FreeGame> FreeGamesLibrary::getGamesByEmulator(const QString &emulator) const {
    if (emulator == "All Systems") {
        return games;
    }

    QList<FreeGame> filtered;
    for (const auto &game : games) {
        if (game.emulator == emulator) {
            filtered.append(game);
        }
    }
    return filtered;
}

FreeGame FreeGamesLibrary::getGameById(const QString &id) const {
    for (const auto &game : games) {
        if (game.id == id) {
            return game;
        }
    }
    return FreeGame();
}
