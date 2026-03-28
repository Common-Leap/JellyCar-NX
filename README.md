## JellyCar is great 2d game with soft body physics created by Walaber https://twitter.com/walaber.

This is an updated homebrew version of the original project for Switch, with all other consoles deprecated and unsupported.

Current implementation uses [Andromeda-Lib](https://github.com/DrakonPL/Andromeda-Lib), patched during setup below.

## Build instructions

-Note: all the other platform builds from this fork are unsupported and are there because I'm too lazy to remove them. If you want the Vita or Windows builds, check the original project

### Nintendo Switch

1. Clone both repositories as siblings:

```sh
git clone https://github.com/DrakonPL/Andromeda-Lib
git clone https://github.com/Common-Leap/JellyCar.git JellyCar
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

4. Apply the required `Andromeda-Lib` compatibility patch for latest `libnx` (Switch exit/audio/font/shader fixes):

```sh
git -C Andromeda-Lib apply ../JellyCar/Build/Switch/andromeda-lib-libnx.patch
```

If this reports the patch is already applied, continue.

5. Build:

```sh
cd JellyCar/Build/Switch
make clean
make -j$(nproc)
```

By default, file logging is disabled (`FILE_LOG=0`), so no `Info*.txt` / `Error*.txt` debug files are written.
To enable file logging, build with:

```sh
make -j$(nproc) FILE_LOG=1
```

If `Andromeda-Lib` is not in the default sibling location:

```sh
make -j$(nproc) ANDROMEDA_LIB=/absolute/or/relative/path/to/Andromeda-Lib
```

The build now auto-stages `JellyCar/Assets` into `Build/Switch/romfs/Assets` and embeds it in `JellyCar.nro`, so no separate asset copy is required for normal use.

6. Deploy and run:

- Copy `JellyCar/Build/Switch/JellyCar.nro` to `sdmc:/switch/JellyCar/JellyCar.nro`.
- Launch from Homebrew Menu with title override (hold `R` while opening a game) to avoid applet-memory crashes.
- Optional override: if you want filesystem assets instead of embedded romfs, place `Assets` at `sdmc:/switch/JellyCar/Assets`.

For a release package, `JellyCar.nro` is the required runtime file.


### Mod Support

If you would like to customize the games asset/music or want to make custom levels, head on over to https://github.com/Common-Leap/JellyModTool/, where you can find a simple editor for making mods. Mods go in the same folder as the .nro and are in a folder named JellyMods. 
