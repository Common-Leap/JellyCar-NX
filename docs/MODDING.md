# JellyMods — Modding Guide

JellyCar supports mods placed in `sdmc:/switch/JellyCar/JellyMods/` on your SD card.
Each mod is a **folder** containing a `mod.xml` manifest and its asset files.
The game scans this folder at startup and seamlessly adds mod content alongside built-in content.

---

## Folder Structure

```
sdmc:/switch/JellyCar/
  JellyCar.nro
  JellyMods/
    MyCoolSkin/
      mod.xml
      chassis_small.png
      chassis_big.png
      tire_small.png
      tire_big.png
    MyCustomSong/
      mod.xml
      mysong.ogg
    MyLevel/
      mod.xml
      mylevel.scenec
      mylevel_thumb.png
```

---

## Mod Types

### Car Skin

Adds a new car skin to the skin selector.

**mod.xml**
```xml
<?xml version="1.0" encoding="utf-8"?>
<Mod type="skin"
     name="My Cool Skin"
     chassisSmall="chassis_small.png"
     chassisBig="chassis_big.png"
     tireSmall="tire_small.png"
     tireBig="tire_big.png" />
```

**Texture requirements:**
- All 4 files must be PNG
- `chassisSmall` / `tireSmall` — used when the car is in its small/compact form
- `chassisBig` / `tireBig` — used when the car is in its large/expanded form
- Recommended sizes match the built-in skins in `Assets/Jelly/Car_Skins/`

---

### Song

Adds a track to the background music playlist. The game picks songs randomly, so your track will appear in the rotation alongside the built-in songs.

**mod.xml**
```xml
<?xml version="1.0" encoding="utf-8"?>
<Mod type="song"
     name="My Custom Song"
     file="mysong.ogg" />
```

**Audio requirements:**
- File must be OGG Vorbis format
- Stereo or mono, any sample rate SDL2_mixer supports (44100 Hz recommended)

---

### Level

Adds a new level to the level select screen, appearing after all built-in levels.

**mod.xml**
```xml
<?xml version="1.0" encoding="utf-8"?>
<Mod type="level"
     name="My Awesome Level"
     file="mylevel.scenec"
     thumb="mylevel_thumb" />
```

**File requirements:**
- `file` — compiled level binary in `.scenec` format (same format as built-in levels in `Assets/Jelly/Scenes/`)
- `thumb` — thumbnail image filename **without extension** (the loader appends `.png`)
- Thumbnail should be PNG, same dimensions as built-in thumbnails in `Assets/Jelly/Texture/`

---

## Notes for the Companion App

The mod format is intentionally minimal so tools can generate it easily:

- All manifests are plain XML with a single root element and flat attributes — no nesting
- Asset filenames in the manifest are relative to the mod folder — no path prefixes needed
- The `type` attribute on `<Mod>` is the only required discriminator
- Level `.scenec` files are raw binary structs — see the level editor spec for the serialization format
- Skin PNGs are standard RGBA images — any image library can produce them

The companion app workflow for each type:
1. **Skin** — user provides 4 images → app resizes/converts to PNG → writes `mod.xml` → zips folder
2. **Song** — user provides audio file → app converts to OGG → writes `mod.xml` → zips folder  
3. **Level** — user designs level in editor → app serializes to `.scenec` + renders thumbnail → writes `mod.xml` → zips folder

To install a mod, unzip the folder into `sdmc:/switch/JellyCar/JellyMods/` and relaunch the game.
