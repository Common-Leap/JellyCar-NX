#ifndef ModLoader_H
#define ModLoader_H

// JellyMods Mod Loader
// ---------------------
// Scans sdmc:/switch/JellyCar/JellyMods/ for mod folders at startup.
// Each mod folder contains a mod.xml manifest and its assets.
//
// Mod types:
//   skin  - adds a car skin (4 PNGs + mod.xml)
//   song  - adds a background music track (1 OGG + mod.xml)
//   level - adds a playable level (.scenec + thumbnail PNG + mod.xml)
//
// A special disable.xml file in the JellyMods root can suppress built-in
// songs (by 1-based index), skins (by name), and levels (by name).
//
// See docs/MODDING.md for the full format specification.

#include <string>
#include <vector>
#include <set>

struct ModSkinInfo
{
    std::string modPath;       // full path to mod folder
    std::string name;
    std::string chassisSmall;  // filename only, resolved from modPath
    std::string chassisBig;
    std::string tireSmall;
    std::string tireBig;
};

struct ModSongInfo
{
    std::string modPath;
    std::string name;
    std::string file;          // filename only, resolved from modPath
};

struct ModLevelInfo
{
    std::string modPath;
    std::string name;
    std::string file;          // .scenec filename
    std::string thumb;         // thumbnail PNG filename (no extension)
};

class ModLoader
{
private:
    static ModLoader* _instance;

    std::string _modsDir;

    std::vector<ModSkinInfo>  _skins;
    std::vector<ModSongInfo>  _songs;
    std::vector<ModLevelInfo> _levels;

    // Disabled built-in content (loaded from disable.xml)
    std::set<int>         _disabledSongIndices;  // 1-based song numbers
    std::set<std::string> _disabledSkinNames;
    std::set<std::string> _disabledLevelNames;

    ModLoader();

    void ScanMods();
    void LoadMod(const std::string& modFolder);
    void LoadDisableList();

public:
    static ModLoader* Instance();
    static void Shutdown();

    // Called once at startup before LevelManager/AudioHelper init
    void Init(const std::string& modsDir);

    const std::vector<ModSkinInfo>&  GetSkins()  const { return _skins;  }
    const std::vector<ModSongInfo>&  GetSongs()  const { return _songs;  }
    const std::vector<ModLevelInfo>& GetLevels() const { return _levels; }

    // Disable list queries — used by AudioHelper and LevelManager
    bool IsSongDisabled(int oneBased)        const { return _disabledSongIndices.count(oneBased) > 0; }
    bool IsSkinDisabled(const std::string& n) const { return _disabledSkinNames.count(n) > 0; }
    bool IsLevelDisabled(const std::string& n) const { return _disabledLevelNames.count(n) > 0; }
};

#endif
