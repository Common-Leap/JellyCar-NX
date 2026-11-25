## JellyCar is great 2d game with soft body physics created by Walaber https://twitter.com/walaber.

This is an updated Switch and Switch 2 homebrew version of the [original project](https://github.com/DrakonPL/JellyCar), which also has the Vita and Windows builds.

It uses a [Switch fork of Andromeda-Lib](https://github.com/Common-Leap/Andromeda-Lib).

## Controls

Car controls can be changed in the options.

| Action | Default | Also |
| --- | --- | --- |
| Drive | Left stick | D-pad |
| Tilt | L / R | ZL / ZR, right stick |
| Transform | B | |
| Balloon | A | Touch |
| Sticky tire | Y | Touch |
| Map | X | |
| Pause | + / - | |

On the level select, the D-pad picks a level, L / R change the page, ZL / ZR change the car skin and the left stick drives the car. The other menus use the left stick or the D-pad. Everything in the menus can be touched.

## Build instructions

1. Clone both repositories as siblings:

```sh
git clone https://github.com/Common-Leap/Andromeda-Lib.git
git clone https://github.com/Common-Leap/JellyCar-NX.git JellyCar
```

Expected layout:

```txt
Workspace/
  Andromeda-Lib/
  JellyCar/
```

Run the remaining commands from the `Workspace/` directory shown above.

2. Install devkitPro + Switch toolchain and libs:

```sh
sudo pacman -Syu devkitA64 libnx switch-tools \
  switch-pkg-config switch-mesa switch-glad switch-glm \
  switch-sdl2 switch-sdl2_mixer switch-freetype switch-harfbuzz
```

3. Set environment variables:

```sh
export DEVKITPRO=/opt/devkitpro
export DEVKITA64=$DEVKITPRO/devkitA64
export PATH=$DEVKITA64/bin:$DEVKITPRO/tools/bin:$DEVKITPRO/portlibs/switch/bin:$PATH
```

4. Build:

```sh
cd JellyCar/Build/Switch
make -j2
```

| Command | Menus | Output |
|---|---|---|
| `make` or `make MENUS=new` | Redesigned menus (v1.3.0 and later) | `JellyCar.nro` |
| `make MENUS=old` | Menus from before the v1.3.0 redesign | `JellyCar-OldMenus.nro` |
| `make both` | One build of each | Both |

### Nintendo Switch 2

Switch 2 builds run under [kaiHEN](https://github.com/ouncebrew/kaihen). Install its devkit, [ounce-devkit](https://github.com/ouncebrew/ounce-devkit), into devkitPro with the `install.sh` from that repository, then add `PLATFORM=switch2` to any of the commands above:

| Command | Menus | Output |
|---|---|---|
| `make PLATFORM=switch2` | Redesigned menus | `JellyCar-NS2.nro` |
| `make PLATFORM=switch2 MENUS=old` | Menus from before the v1.3.0 redesign | `JellyCar-OldMenus-NS2.nro` |
| `make PLATFORM=switch2 both` | One build of each | Both |
| `make everything` | Both menus for Switch and Switch 2 | All four |

### Other options

- `FILE_LOG=1` writes `Info*.txt` / `Error*.txt` debug logs. Run `make clean` first when changing it.
- `ANDROMEDA_LIB=/path/to/Andromeda-Lib` if it is not in the default sibling location. Use an absolute path, or one relative to `Build/Switch`.

The build embeds `Assets` in the `.nro`, so no separate asset copy is required for normal use.

## Deploy and run

- Copy the `.nro` you built to `sdmc:/switch/JellyCar/`.
- Launch from Homebrew Menu with title override (hold `R` while opening a game) to avoid applet-memory crashes. On Switch 2, holding `R` while opening a native game is how kaiHEN opens hbmenu.
- Optional override: if you want filesystem assets instead of embedded romfs, place `Assets` at `sdmc:/switch/JellyCar/Assets`.

### Mod Support

If you would like to customize the games asset/music or want to make custom levels, head on over to https://github.com/Common-Leap/JellyModTool/, where you can find a simple editor for making mods. Mods go in the same folder as the .nro and are in a folder named JellyMods. The mod format is described in [docs/MODDING.md](docs/MODDING.md).
