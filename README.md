# Dangerous Dave in C

A faithful recreation of the classic 1990 DOS platformer "Dangerous Dave" by John Romero, implemented in pure C using SDL2 for cross-platform compatibility.

![Game Preview](preview.png)

## Quick Start (Windows PowerShell)

If you have **CMake** and **SDL2** already installed:

```powershell
# Navigate to project directory
cd "C:\Users\PC\Desktop\Dangerous Dave"

# Build the game
cmake -B build -S .
cmake --build build

# Run the game (PowerShell requires .\ prefix for local executables)
.\build\dave.exe
```

**Note**: In PowerShell, you must use `.\build\dave.exe` (with the `dot-backslash` prefix) to run local executables. In Command Prompt (`cmd.exe`), you can use just `build\dave.exe`.

### Alternative: Using build.bat

```powershell
cd "C:\Users\PC\Desktop\Dangerous Dave"
.\build.bat
.\build\dave.exe
```

## Overview

This project recreates the core gameplay mechanics of the original Dangerous Dave: a fast-paced 2D platformer where you navigate through levels, collect treasures, avoid enemies, and reach the exit door. The game uses a retro 320x200 resolution scaled up for modern displays, maintaining the classic pixel-perfect aesthetic.

### Why This Project?

- **Educational**: Demonstrates game development fundamentals in C without complex frameworks
- **Portable**: SDL2 ensures the game runs on Windows, Linux, and macOS
- **Lightweight**: Entire game is under 500 lines of code, easy to understand and modify
- **Extensible**: Simple ASCII-based level format allows anyone to create new levels

## Features

### Core Gameplay
- **3 Complete Levels** with increasing difficulty and platforming challenges
- **Classic Platformer Physics** with momentum-based movement and gravity
- **Collectibles System**:
  - Gems (cyan): 100 points each, optional collection for high scores
  - Trophy (gold): 1000 points, required to unlock the exit door
- **Enemy AI** with simple patrolling behavior between waypoints
- **Life System**: 3 lives per game, lose one when touching enemies or falling
- **Progressive Difficulty**: Each level introduces more enemies and complex layouts

### Technical Features
- 60 FPS game loop with delta-time independent physics
- AABB (Axis-Aligned Bounding Box) collision detection
- Tile-based level system (20x12 grid, 16x16 pixel tiles)
- Scalable rendering (320x200 logical resolution, scales to any window size)
- Pause functionality and level reset capability

## Controls

| Key | Action | Description |
|-----|--------|-------------|
| `←` / `→` | Move | Run left or right with momentum |
| `↑` | Jump | Jump only when on ground; variable height not implemented |
| `R` | Reset | Restart current level (loses a life) |
| `P` | Pause | Toggle pause menu |
| `Esc` | Quit | Exit the game |

### Gameplay Tips
- You must collect the **Trophy** before the door will open
- Enemies patrol back and forth - time your movements carefully
- You can fall through the bottom of the screen - you'll lose a life
- Plan your route to maximize gem collection before getting the trophy

## Game Mechanics

### Physics System
The game uses a simple but effective physics simulation:

```
Gravity: 0.4 pixels/frame²
Jump Force: 8 pixels/frame (upward)
Run Speed: 2 pixels/frame
Terminal Velocity: Limited by collision detection
```

**Collision Detection**: The game checks tile-based collisions by converting the player's bounding box to tile coordinates and testing against solid tiles (walls and platforms).

### Enemy Behavior
Enemies use a simple state-less patrol AI:
- Each enemy has a `startX` and `endX` position defining their patrol route
- They move at 1 pixel/frame, reversing direction at boundaries
- Collision with player is distance-based (Euclidean distance < 10 pixels)
- Enemies ignore gravity and platforms - they move in straight lines

### Scoring System
- **Gem**: +100 points (optional collectible)
- **Trophy**: +1000 points (required for level completion)
- **Level Complete**: Bonus based on remaining lives (not implemented in current version)

### Win/Lose Conditions
- **Level Complete**: Touch the door after collecting the trophy
- **Death**: Touch an enemy or fall off the screen
- **Game Over**: Lose all 3 lives
- **Victory**: Complete all 3 levels

## Building

### Prerequisites

| Component | Minimum Version | Download |
|-----------|----------------|----------|
| C Compiler | C99 support | [gcc](https://gcc.gnu.org/), [clang](https://clang.llvm.org/), or [MSVC](https://visualstudio.microsoft.com/) |
| SDL2 | 2.0.12+ | [libsdl.org](https://libsdl.org/download-2.0.php) |
| CMake | 3.16+ | [cmake.org](https://cmake.org/download/) |

#### SDL2 Installation by Platform

**Windows (vcpkg)**:
```bash
vcpkg install sdl2:x64-windows
```

**macOS (Homebrew)**:
```bash
brew install sdl2
```

**Linux (apt)**:
```bash
sudo apt-get install libsdl2-dev
```

**Linux (pacman)**:
```bash
sudo pacman -S sdl2
```

### Build Instructions

#### Generic (Unix/Linux/macOS)
```bash
# Clone or extract the project
cd "Dangerous Dave"

# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Run
./dave
```

#### Windows (MinGW)
```bash
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```

#### Windows (Visual Studio 2022)
```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```
Or open the generated `DangerousDave.sln` in Visual Studio.

#### Windows (build.bat helper - PowerShell)
In **PowerShell**, run the batch file with the dot-backslash prefix:
```powershell
.\build.bat
```

In **Command Prompt (cmd.exe)**, you can run it directly:
```cmd
build.bat
```

This will auto-detect your compiler and build accordingly.

### Running the Game

After building, the executable will be at `build\dave.exe`.

**PowerShell** (requires `.\` prefix):
```powershell
.\build\dave.exe
```

**Command Prompt**:
```cmd
build\dave.exe
```

Or double-click `build\dave.exe` in File Explorer to run it.

### Troubleshooting Build Issues

| Issue | Solution |
|-------|----------|
| "cmake is not recognized" | CMake is not installed or not in PATH. Download from [cmake.org](https://cmake.org/download/) and add to PATH, or use the installer with "Add to PATH" option |
| "gcc/cl is not recognized" | No C compiler installed. Install MinGW-w64 (Windows), Xcode Command Line Tools (macOS), or `build-essential` (Linux) |
| "SDL2 not found" | Install SDL2 development libraries and ensure `SDL2_DIR` environment variable points to the SDL2 cmake directory |
| "Cannot find SDL2.dll" | Copy `SDL2.dll` from your SDL2 installation to the build directory, or add SDL2 to your PATH |
| "build.bat is not recognized" (PowerShell) | Use `.\build.bat` instead of `build.bat` in PowerShell |
| "build\dave.exe is not recognized" (PowerShell) | Use `.\build\dave.exe` instead of `build\dave.exe` in PowerShell |
| Linker errors on Linux | Install `libsdl2-dev` package, not just `libsdl2` |
| CMake version too old | Update CMake to 3.16+ or modify `CMakeLists.txt` to lower minimum version (not recommended) |

## Level Format

Levels are defined as ASCII strings in `main.c`. The grid is 20 columns × 12 rows (320×192 pixels), with each character representing a 16×16 pixel tile.

### Tile Types

| Character | Tile | Behavior |
|-----------|------|----------|
| `#` | Wall | Solid on all sides, blocks movement |
| `=` | Platform | One-way collision (can jump through from below) |
| `P` | Player Spawn | Starting position for the player |
| `D` | Door/Exit | Level exit, opens only after trophy collected |
| `T` | Trophy | Golden trophy, required for level completion |
| `G` | Gem | Blue gem, 100 points, optional |
| `E` | Enemy | Red patrolling enemy, kills on contact |
| `.` | Empty | Walkable empty space |

### Level Structure

```c
const char* LEVEL_DATA[3] = {
    "####################"  // Row 0 (top)
    "#..................#"  // Row 1
    "#...T..............#"  // Row 2
    // ... etc
    "####################", // Row 11 (bottom)
    // Level 2, Level 3...
};
```

### Creating New Levels

1. Open `main.c` and find the `LEVEL_DATA` array
2. Add a new string following the 20×12 format
3. Update the array size (e.g., `[4]` instead of `[3]`)
4. In `loadLevel()`, change the level check: `if (levelNum >= 4)`
5. Rebuild and test

**Level Design Tips**:
- Always surround your level with walls `#` to prevent player escaping
- Place the trophy somewhere challenging but reachable
- Add enemies on platforms for interesting encounters
- Test jump distances - max jump is about 3-4 tiles horizontally

## Architecture

### File Structure

```
Dangerous Dave/
├── dave.h              # Game structures, constants, function declarations
├── main.c              # Game implementation (~400 lines)
├── CMakeLists.txt      # CMake build configuration
├── build.bat           # Windows quick-build script
└── README.md           # This file
```

### Code Architecture

The game follows a simple modular design:

#### Core Data Structures (`dave.h`)

```c
typedef struct {
    float x, y;          // Position
    float vx, vy;        // Velocity
    int width, height;   // Bounding box
    bool onGround;       // Ground state for jumping
    bool alive;
    int score, lives;
    bool hasTrophy;      // Required for exit
} Player;

typedef struct {
    TileType tiles[LEVEL_HEIGHT][LEVEL_WIDTH];
    Enemy enemies[10];
    Collectible collectibles[20];
    int doorX, doorY;
} Level;
```

#### Game Loop (`main.c`)

```
main()
  └── while (running)
        ├── processInput()    - Handle keyboard input
        ├── updateGame()      - Update physics, enemies, collisions
        │   ├── movePlayer()
        │   ├── updateEnemies()
        │   ├── checkCollectibles()
        │   └── checkLevelComplete()
        └── renderGame()      - Draw tiles, entities, HUD
```

#### Key Functions

| Function | Purpose | Line Count |
|----------|---------|------------|
| `initGame()` | SDL initialization, window creation | ~40 lines |
| `loadLevel()` | Parse ASCII level data, spawn entities | ~60 lines |
| `movePlayer()` | Physics, input handling, collision | ~30 lines |
| `checkCollision()` | AABB tile collision detection | ~15 lines |
| `updateEnemies()` | Enemy AI and player death check | ~25 lines |
| `renderGame()` | All drawing operations | ~50 lines |

### Rendering System

The game uses a retro rendering approach:
- **Logical Resolution**: 320×200 (classic DOS resolution)
- **Tile Size**: 16×16 pixels
- **Scaling**: SDL2 handles automatic scaling to window size
- **Color Palette**: Simple RGB colors without textures

```c
SDL_RenderSetLogicalSize(renderer, 320, 200);
// All drawing happens at 320x200, SDL2 scales to window
```

### Physics Details

**Collision Detection Algorithm**:
1. Convert player position to tile coordinates (divide by 16)
2. Check all tiles overlapping player bounding box
3. If any tile is solid, collision = true
4. Resolve collision by preventing movement in that direction

**Gravity Implementation**:
```c
player->vy += GRAVITY;  // 0.4 per frame
if (collision) {
    if (player->vy > 0) player->onGround = true;
    player->vy = 0;
}
```

## System Requirements

### Minimum
- OS: Windows 7+, macOS 10.12+, Linux kernel 3.0+
- CPU: 1 GHz processor (any modern CPU)
- RAM: 64 MB
- GPU: Any GPU with OpenGL 2.0 or Direct3D 9 support
- Storage: 1 MB

### Recommended
- OS: Windows 10/11, macOS 12+, Ubuntu 20.04+
- CPU: 2 GHz dual-core
- RAM: 256 MB
- GPU: Modern integrated graphics

The game is extremely lightweight and will run on virtually any hardware from the last 20 years.

## Future Enhancements

Planned features for future versions:

- [ ] Sound effects and background music (SDL2_mixer)
- [ ] Sprite/texture support instead of colored rectangles
- [ ] More enemy types (flying, jumping, shooting)
- [ ] Power-ups (speed boost, invincibility, extra life)
- [ ] Save/load game state
- [ ] High score table with persistent storage
- [ ] Level editor GUI
- [ ] Boss battles at end of level sets
- [ ] Parallax scrolling backgrounds
- [ ] Particle effects (explosions, sparkles)

## Contributing

Contributions are welcome! Areas that need help:

1. **Graphics**: Create 16×16 pixel art sprites for Dave, enemies, tiles
2. **Audio**: Add sound effects and chiptune music
3. **Levels**: Design challenging new level layouts
4. **Bug Fixes**: Report or fix any issues found
5. **Documentation**: Improve this README or add code comments

### Coding Standards
- Follow existing C99 style
- Keep functions under 50 lines when possible
- Add comments for complex algorithms
- Test on multiple platforms before submitting

## Acknowledgments

- **John Romero** - Original creator of Dangerous Dave (1990)
- **SDL2 Team** - Simple DirectMedia Layer library
- **id Software** - For inspiring a generation of game developers

## License

MIT License - See LICENSE file for details.

This project is a fan recreation and is not affiliated with or endorsed by the original creators of Dangerous Dave.

## Version History

- **v1.0** (Current) - Initial release with 3 levels, basic physics, enemies, and collectibles

---

**Happy Gaming!** Jump carefully and collect them all! 🏆
