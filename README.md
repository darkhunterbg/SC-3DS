# StarCraft For Nintendo 3DS
## On Assets
Any assets used from StarCraft belongs to Blizzard. You'll still need legal copy of StarCraft to get anything useful out of this repo.
Feel free to use the source code for whatever, as long as you're not making any money out of it.
## Building The Source
### Requirements
For basic (PC only) build:
- x64 Windows 10 machine (others version could work with some tweaks);
- Visual Studio 2019 with Clang, Windows 10 SDK & .NET 5.0;
- Installed copy of StarCraft (Remastered should be OK);
- Ladiks MPQ Editor or any program that can extract MPQ, with listfiles for StarCraft;
- For Remastere with CASC files instead of MPQ, use CascView (untested);

Additional stuff for 3DS build:
- devkitPro installed at C:\devkitPro with 3DS SDK installed;

### Setup
1. Create empty folder **mpq**;
2. From StarCraft's install folder use MPQ extractor on (in order):
- StarCraft.mpq
- StarDat.mpq
- BroodWar.mpq
- BrooDat.mpq
Place extracted files in **mpq** folder. Override if duplicates are found;
2. Open **StarCraft.sln**. If not making 3DS build, unload 3DS project;
3. Build the whole solution;
4. Open **tools\Tools.sln** and start **DataManager**;
5. At tab **Asset Converter** press **Convert All** and wait until it finishes. This will extract sprites and images from the original assets in folder **assets_extracted**;
6. At tab **Atlas Generator**:
- **3DS** For 3DS, press **Build all**. This will generate PC build as well;
- **PC** For PC only press **Build PC**;
This will generate game-ready assets in folder **cooked_assets**. For 3DS this will also generate **SC.cia** with the assets packed inside;

## Running The Game From Source
For PC, just start project **Win32** from **StarCraft.sln**.

For 3DS, you have several options:

**Homebrew Launcher**
Copy everything from **cooked_assets\3ds** to the sd card at **3ds\StarCraft\romfs**.
Put **SC.3dsx** on the sd card (usually folder **3ds**) and start the game from Homebrew Launcher.
Running **SC.3dsx** through 3dslink netloader is also possible. Just edit **debug-3dslink.bat** with your 3DS's IP and run it, while Homebrew Launcher is in netloader mode (Press Y);

**CIA Standalone**
CIA standalone has the advantage of self-contained installation. It is not suitable for distribution as it will contain some of StarCraft's original assets. Just install **SC.cia**.

**CIA without assets**
CIA without assets has the advantage for being very small and containing no original assets from StarCraft. This is suitable for distribution, but the user will have to get the cooked assets on his own.
To install the assets, copy everything from **cooked_assets\3ds** to the sd card at **3ds\StarCraft\romfs**.
Run **makerom-no-romfs.bat**. "**C_no_romfs.cia** should appear in the root folder. Install this CIA on your 3DS.

**Citra Emulator**
Everything that applies for Homebrew Launcher and CIA is valid for Citra. With **Visual Studio Code** you can actually debug the game on the emulator itself.

## Running The Game Without Source
Download the [lastest releases](https://github.com/darkhunterbg/SC-3DS/releases/latest) and follow the **README**.

## 3DS Controls
- CPad: move cursor;
- CStick/ CPad + L: move camera;
- R: toggle zoom out;
- Y: unit select;
- X: attack;
- A: go-to/attack;
- B: stop;
- Select: toggle performance profiler;
