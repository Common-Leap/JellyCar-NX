#include "ModLoader.h"

#include <Andromeda/Utils/Logger.h>

#include "tinyxml.h"

#include <dirent.h>
#include <sys/stat.h>

ModLoader* ModLoader::_instance = 0;

ModLoader::ModLoader()
{
}

ModLoader* ModLoader::Instance()
{
    if (_instance == 0)
        _instance = new ModLoader();
    return _instance;
}

void ModLoader::Shutdown()
{
    if (_instance != 0)
    {
        delete _instance;
        _instance = 0;
    }
}

void ModLoader::Init(const std::string& modsDir)
{
    _modsDir = modsDir;
    Andromeda::Utils::Logger::Instance()->Log("ModLoader::Init: scanning %s\n", _modsDir.c_str());
    LoadDisableList();
    ScanMods();
    Andromeda::Utils::Logger::Instance()->Log("ModLoader::Init: found %d skins, %d songs, %d levels\n",
        (int)_skins.size(), (int)_songs.size(), (int)_levels.size());
}

void ModLoader::LoadDisableList()
{
    std::string path = _modsDir + "/disable.xml";
    FILE* f = fopen(path.c_str(), "rb");
    if (f == 0)
        return; // no disable list — all built-ins enabled

    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size <= 0) { fclose(f); return; }

    unsigned char* buf = new unsigned char[size];
    fread(buf, 1, size, f);
    fclose(f);

    TiXmlDocument doc;
    if (!doc.LoadContent(buf, size))
    {
        Andromeda::Utils::Logger::Instance()->Log("ModLoader: failed to parse disable.xml\n");
        return;
    }

    TiXmlElement* root = doc.FirstChildElement("Disable");
    if (root == 0) return;

    // <Songs> block — <Song index="1"/> (1-based)
    TiXmlElement* songs = root->FirstChildElement("Songs");
    if (songs)
    {
        for (TiXmlElement* e = songs->FirstChildElement("Song"); e; e = e->NextSiblingElement("Song"))
        {
            const char* idx = e->Attribute("index");
            if (idx)
            {
                int i = atoi(idx);
                _disabledSongIndices.insert(i);
                Andromeda::Utils::Logger::Instance()->Log("ModLoader: disabling built-in song %d\n", i);
            }
        }
    }

    // <Skins> block — <Skin name="Classic"/>
    TiXmlElement* skins = root->FirstChildElement("Skins");
    if (skins)
    {
        for (TiXmlElement* e = skins->FirstChildElement("Skin"); e; e = e->NextSiblingElement("Skin"))
        {
            const char* name = e->Attribute("name");
            if (name)
            {
                _disabledSkinNames.insert(std::string(name));
                Andromeda::Utils::Logger::Instance()->Log("ModLoader: disabling built-in skin '%s'\n", name);
            }
        }
    }

    // <Levels> block — <Level name="hill"/>
    TiXmlElement* levels = root->FirstChildElement("Levels");
    if (levels)
    {
        for (TiXmlElement* e = levels->FirstChildElement("Level"); e; e = e->NextSiblingElement("Level"))
        {
            const char* name = e->Attribute("name");
            if (name)
            {
                _disabledLevelNames.insert(std::string(name));
                Andromeda::Utils::Logger::Instance()->Log("ModLoader: disabling built-in level '%s'\n", name);
            }
        }
    }
}

void ModLoader::ScanMods()
{
    DIR* dir = opendir(_modsDir.c_str());
    if (dir == 0)
    {
        Andromeda::Utils::Logger::Instance()->Log("ModLoader::ScanMods: can't open %s\n", _modsDir.c_str());
        return;
    }

    struct dirent* ent;
    while ((ent = readdir(dir)) != 0)
    {
        std::string name(ent->d_name);
        if (name == "." || name == "..")
            continue;

        std::string fullPath = _modsDir + "/" + name;

        // d_type is DT_UNKNOWN on Switch sdmc — use stat() instead
        struct stat st;
        if (stat(fullPath.c_str(), &st) != 0 || !S_ISDIR(st.st_mode))
            continue;

        LoadMod(fullPath);
    }

    closedir(dir);
}

void ModLoader::LoadMod(const std::string& modFolder)
{
    std::string manifestPath = modFolder + "/mod.xml";

    // Read manifest via raw fopen — FileManager prepends mainDirPath which we don't want here
    FILE* f = fopen(manifestPath.c_str(), "rb");
    if (f == 0)
    {
        Andromeda::Utils::Logger::Instance()->Log("ModLoader: no mod.xml in %s\n", modFolder.c_str());
        return;
    }

    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size <= 0)
    {
        fclose(f);
        return;
    }

    unsigned char* buf = new unsigned char[size];
    fread(buf, 1, size, f);
    fclose(f);

    // NOTE: TiXmlDocument::LoadContent takes ownership of buf and frees it internally.
    // Do NOT delete[] buf after this call.
    TiXmlDocument doc;
    if (!doc.LoadContent(buf, size))
    {
        Andromeda::Utils::Logger::Instance()->Log("ModLoader: failed to parse %s\n", manifestPath.c_str());
        return;
    }

    TiXmlElement* root = doc.FirstChildElement("Mod");
    if (root == 0)
        return;

    const char* type = root->Attribute("type");
    if (type == 0)
        return;

    std::string modType(type);

    if (modType == "skin")
    {
        ModSkinInfo skin;
        skin.modPath = modFolder;

        const char* name         = root->Attribute("name");
        const char* chassisSmall = root->Attribute("chassisSmall");
        const char* chassisBig   = root->Attribute("chassisBig");
        const char* tireSmall    = root->Attribute("tireSmall");
        const char* tireBig      = root->Attribute("tireBig");

        if (!name || !chassisSmall || !chassisBig || !tireSmall || !tireBig)
        {
            Andromeda::Utils::Logger::Instance()->Log("ModLoader: skin mod missing attributes in %s\n", modFolder.c_str());
            return;
        }

        skin.name         = name;
        skin.chassisSmall = chassisSmall;
        skin.chassisBig   = chassisBig;
        skin.tireSmall    = tireSmall;
        skin.tireBig      = tireBig;

        _skins.push_back(skin);
        Andromeda::Utils::Logger::Instance()->Log("ModLoader: loaded skin '%s'\n", skin.name.c_str());
    }
    else if (modType == "song")
    {
        ModSongInfo song;
        song.modPath = modFolder;

        const char* name = root->Attribute("name");
        const char* file = root->Attribute("file");

        if (!name || !file)
        {
            Andromeda::Utils::Logger::Instance()->Log("ModLoader: song mod missing attributes in %s\n", modFolder.c_str());
            return;
        }

        song.name = name;
        song.file = file;

        _songs.push_back(song);
        Andromeda::Utils::Logger::Instance()->Log("ModLoader: loaded song '%s'\n", song.name.c_str());
    }
    else if (modType == "level")
    {
        ModLevelInfo level;
        level.modPath = modFolder;

        const char* name  = root->Attribute("name");
        const char* file  = root->Attribute("file");
        const char* thumb = root->Attribute("thumb");

        if (!name || !file || !thumb)
        {
            Andromeda::Utils::Logger::Instance()->Log("ModLoader: level mod missing attributes in %s\n", modFolder.c_str());
            return;
        }

        level.name  = name;
        level.file  = file;
        level.thumb = thumb;

        _levels.push_back(level);
        Andromeda::Utils::Logger::Instance()->Log("ModLoader: loaded level '%s'\n", level.name.c_str());
    }
    else
    {
        Andromeda::Utils::Logger::Instance()->Log("ModLoader: unknown mod type '%s' in %s\n", type, modFolder.c_str());
    }
}
