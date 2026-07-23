# vengeance-mc-cheat-1.8.9

Originally this client's purpose was to bring the cheating community of 1.8.9 to it's knees and make a gap between everyday clients and top tier clients like vengeance.

Vengeance wasnt fully custom made, when I started making it back in february I started on an already done base which included a shit aim assist and an even worse esp. Vengeance was made in 1 week, and then tried to sell it, but eventually it didnt work out as my medias abandoned the project after I gaved them a lifetime license. Since then only I and my friends had access to this masterpiece and now im sharing it with anyone who may be struggling with cheat development. The code of vengeance is free to use, but mentioning this repository or me would be appreciated. **For the skids coming to copy paste code, i left a little surprise for you in main.cpp**

the loader is not included, I used a simple bytecode injection from a .hpp file inside the loader itself, so it was just an exe, nothing else, and it injected instantly, didnt had to wait for tens of seconds, since the loader is not included, therefore there wont be any security measures you may need to bypass if you try to make this build instead of copying logic. **the security of vengeance is explained at the bottom if you're interested.**

if you are interested in working with me, my discord: ignrare
enjoy.

Vengeance consists of:
71 modules, extensive customisability, 3 GUIs (2nd and 3rd are not finished but usable), ability to recolor any part of the main GUI, top-grade security

## Module List

### Combat Modules

| Module | Description | Key Features |
|--------|-------------|--------------|
| **Aimbot** | Automatic aiming assistance | Silent/legit aim, lock-on mode, switch mode (distance/angle), vertical aim, activation modes (always/click/mouse/sprint), FOV control, smoothing, GCD-aware |
| **Bow Aimbot** | Automatic bow aiming | Trajectory prediction, motion prediction, smoothing, FOV filter, auto-shoot, charge threshold |
| **AutoClicker** | Automated left-clicking | CPS control (1-25), randomization, exhaust mode, spike mode, activation modes (toggle/hold key/hold LMB), sword-only mode, inventory-aware |
| **Right Clicker** | Automated right-clicking | CPS control, randomization, blatant mode, activation modes |
| **Blockhit** | Automatic block hitting | Timing controls (min/max delay), block chance, hold length, only-while-clicking, sword-only |
| **Hit Select** | Advanced hit timing | Burst mode, criticals mode, pause duration, knockback detection, fake swing, cancel rate control, server attack time sync |
| **Reach** | Extended attack range | Gaussian distribution, critically damped spring interpolation, motion compensation, sprint-only, attack-only, liquid-disable, chance control |
| **Velocity** | Knockback reduction | Horizontal/vertical percentage, chance, only-while-moving, only-on-ground |
| **S-Tap** | Sprint reset (S key) | Adaptive delay, chance, only-while-sprinting, sword-only, anti-bot check |
| **W-Tap** | Sprint reset (W key release) | Duration control, legit mode, adaptive chance, combo tracking, only-forward, sword-only |
| **Auto Tool** | Automatic best tool selection | Activation delay, switch-back, filter by tool type, sneak-only |
| **Auto Sword** | Automatic sword equip | Switches to best sword in hotbar |
| **Fast Place** | Remove block placement delay | Tick delay control (0-3 ticks) |
| **Throwpot** | Automatic potion throwing | Pot count (1-3), switch/throw delay, cooldown, trigger key, sword-only |
| **AutoSoup** | Automatic soup eating | Switch/eat delay, bowl dropping, cooldown, trigger key, sword-only |

### Visuals Modules

| Module | Description | Key Features |
|--------|-------------|--------------|
| **ESP** | Entity highlighting | Box styles (full/corners), outline/fill colors, health bar, armor bar, name display, distance display, info scale, image overlay |
| **Chest ESP** | Chest/container highlighting | Outline/fill colors, tracers, width control |
| **Tracer** | Line tracing to entities | Origin options (bottom/center/top), width, distance display, color |
| **XRay** | See-through blocks | Opacity control, show ores/chests/spawners |
| **Nametags** | Custom player nametags | Health, armor, weapon display, durability info, size control |
| **Target HUD** | Combat target display | Health bar (smooth lerp), name, armor, weapon, display timeout |
| **Chams** | Player model coloring | OpenGL state manipulation for colored models |
| **Fullbright** | Maximum brightness | Gamma mode, night vision mode |
| **No Hurt Cam** | Remove damage screen effect | — |
| **Zoom** | Camera zoom | FOV control, key binding |
| **Freecam** | Detached camera movement | Speed control |
| **Armor ESP** | Display armor on players | — |
| **Item ESP** | Highlight dropped items | Color customization |
| **Storage ESP** | Highlight storage containers | — |
| **Hole ESP** | Highlight safe/unsafe holes | Safe (green) / unsafe (red) colors |
| **Trajectories** | Projectile trajectory prediction | Line width, max ticks, landing cross |
| **Bread Crumbs** | Player trail rendering | Color customization |

### Movement Modules

| Module | Description | Key Features |
|--------|-------------|--------------|
| **Speed** | Movement speed multiplier | Multiplier control |
| **Fly** | Creative-like flight | Speed control |
| **Flight** | Survival flight | Vanilla/smooth modes, speed control |
| **No Fall** | Fall damage prevention | JNI-based packet manipulation |
| **Sprint** | Auto-sprint | — |
| **Sprint Reset** | Sprint reset on attack | W-Tap/sneak/nostop modes, delay, duration, randomization, damage wait |
| **Inv Walk** | Walk while inventory open | Vanilla/spoof modes |
| **Step** | Auto-step up blocks | Vanilla/motion modes, height control, reverse step |
| **Spider** | Wall climbing | Speed control |
| **Safe Walk** | Prevent edge falling | — |
| **Reverse Step** | Pull down from ledges | Height control |
| **Bouncy Slime** | Enhanced slime block bounce | — |
| **Fast Stop** | Instant stop on key release | — |
| **Glide** | Slow falling | Speed control |
| **Air Jump** | Jump in mid-air | — |
| **High Jump** | Increased jump height | Motion control |
| **Jump Reset** | Auto-jump reset | Chance, only-when-hurt |
| **No Jump Delay** | Remove jump cooldown | — |
| **Scaffold** | Auto-block placement | Tower mode, legit mode (head angle), auto-block, delay, min distance |
| **Bridge Assist** | Bridge placement aid | RMB-only, backward-only, pitch check, blocks-only, delay |

### Network Modules

| Module | Description | Key Features |
|--------|-------------|--------------|
| **Blink** | Delay packet sending | — |
| **Timer** | Game speed modification | Speed control |
| **Fake Lag** | Simulated lag spikes | Delay control |
| **No Packet** | Block packet sending | — |
| **Ping Spoof** | Fake ping display | Amount control |
| **Anti Bot** | Bot player filtering | Advanced/simple modes |

### Utilities Modules

| Module | Description | Key Features |
|--------|-------------|--------------|
| **Refill** | Auto-refill hotbar from inventory | Soup/potion support, smart speed, random slots, auto-close, trigger key |
| **Teams** | Team color/armor detection | Color/armor modes |
| **Friends** | Friend list management | — |
| **Anti Void** | Void safety | Distance threshold |
| **Middle Click Friend** | Add friends via middle click | — |
| **Auto Armor** | Automatic armor equipping | — |
| **Inventory Manager** | Inventory organization | Delay control |
| **Anti AFK** | Anti-idle kick | Spin/jump/walk modes |
| **Anti Bad Effects** | Remove negative potion effects | — |
| **Panic** | Disable all modules | Key binding (F12) |
| **Staff Notifier** | Alert on staff join | — |
| **No Rotate** | Block server rotation packets | — |
| **Anti Cactus** | Prevent cactus damage | — |
| **Reconnect** | Auto-reconnect on disconnect | Delay control |
| **Ping Spoof Mod** | Modified ping display | Amount control |

### Settings & UI

| Feature | Description |
|---------|-------------|
| **ArrayList** | On-screen enabled modules list with customizable colors, position, background, accent bar, rainbow mode, watermark (name/version/FPS/keybinds) |
| **Keybinds** | Configurable hotkeys for all modules |
| **UI Presets** | Built-in themes: Vengeance, V4-inspired, Slinky-inspired|
| **Theme Customization** | Full color scheme control via UITheme namespace |
| **Safe Unload** | Clean DLL ejection with thread cleanup |


Some tech used:

#### Gaussian Reach Distribution
Uses the **Box-Muller transform** to generate normally-distributed reach values, creating natural-looking variation that avoids pattern detection.

#### Critically Damped Spring Interpolation
Reach values are smoothed using a critically damped spring system, providing smooth transitions between target values without overshoot.

#### GCD-Aware Aim Smoothing
Aimbot calculations account for Minecraft's **GCD (Gravity-Centric Division)** to ensure smooth, detectable rotations that match legitimate mouse input.

### JNI Field Access

The client resolves and caches JNI field/method IDs for performance:

**Entity Fields:** posX, posY, posZ, rotationYaw, rotationPitch, prevRotationYaw, prevRotationPitch, rotationYawHead, isDead, onGround, motionX, motionY, motionZ, fallDistance, hurtResistantTime, hurtTime, health, isSprinting, isSneaking, jumpTicks, stepHeight

**Player Fields:** inventory, currentItem, sendQueue, gameSettings, objectMouseOver

**World Fields:** entityList

**Methods:** attackEntity, swingItem, getEyeHeight, getCurrentItem, getStackInSlot, sendUseItem, dropOneItem, displayGuiScreen, closeScreen, rightClickMouse, getHealth, getMaxHealth, getName, getTotalArmorValue, getEquipmentInSlot, getStrVsBlock, isSneaking, setSprinting

### Prerequisites

- Visual Studio 2022 (or later) with C++ desktop development
- CMake 3.10+
- Windows SDK
- Minecraft Java Edition (for testing)


## Project Structure


vengeance/
├── SDK/                          # Main source code
│   ├── main.cpp                  # Entry point, hooks, global state
│   ├── includes.h                # Global includes
│   ├── combat_modules.h          # S-Tap, W-Tap, Bow Aimbot modules
│   ├── CMakeLists.txt            # CMake build config
│   │
│   ├── Core/                     # Core system
│   │   ├── Core.cpp/.h           # Hook setup, initialization
│   │   └── Keybinds.cpp/.h       # Keybind system
│   │
│   ├── Modules/                  # All cheat modules
│   │   ├── Combat/               # Combat modules
│   │   │   ├── Aimbot.cpp/.h
│   │   │   ├── AutoClicker.cpp/.h
│   │   │   ├── Blockhit.cpp/.h
│   │   │   └── HitSelect.cpp/.h
│   │   ├── Visuals/              # Visual modules
│   │   │   ├── ESP.cpp/.h
│   │   │   ├── ChestESP.cpp/.h
│   │   │   ├── Tracer.cpp/.h
│   │   │   └── XRay.cpp/.h
│   │   ├── Utility/              # Utility modules
│   │   │   └── Refill.h
│   │   └── Appearance/           # UI presets
│   │       └── UIPresets/
│   │           ├── UI_VapeV4.h
│   │           └── UI_Slinky.h
│   │
│   ├── UI/                       # User interface
│   │   ├── UI.cpp/.h             # UI rendering, theme, components
│   │   └── ModernMenu.cpp        # Modern menu implementation
│   │
│   ├── Esp/                      # ESP rendering
│   │   ├── Position.cpp/.h       # Entity position tracking
│   │   ├── Radius.cpp/.h         # ESP radius calculations
│   │   ├── Color.cpp/.h          # Color management
│   │   └── img/stb_image.h       # Image loading (stb)
│   │
│   ├── GlDraw/                   # OpenGL drawing
│   │   └── glDraw.cpp/.h
│   │
│   ├── GlText/                   # OpenGL text rendering
│   │   └── gltext.cpp/.h
│   │
│   ├── Globals/                  # Global state
│   │   └── Globals.cpp/.h
│   │
│   ├── JNI/                      # Java Native Interface
│   │   └── JNIHelpers.cpp/.h     # JNI field/method resolution
│   │
│   └── Utils/                    # Utilities
│       ├── KeyUtils.cpp/.h       # Key handling utilities
│       └── Math.cpp/.h           # Math helpers
│
├── imgui/                        # Dear ImGui library
├── backends/                     # ImGui backend implementations
├── Detours/                      # Microsoft Detours library
├── OpenGL/                       # OpenGL headers/libs
├── InternalMC/                   # Build output
├── openglhook.vcxproj            # Visual Studio project
└── InternalMC.sln                # Visual Studio solution

## Dependencies

| Library | Purpose | License |
|---------|---------|---------|
| [Dear ImGui](https://github.com/ocornut/imgui) | Immediate mode GUI | MIT |
| [Detours](https://github.com/microsoft/Detours) | API hooking library | MIT |
| [stb_image](https://github.com/nothings/stb) | Image loading | Public Domain |
| OpenGL 2.x | Graphics rendering | — |
| Win32 API | Windows integration | — |
| JNI | Java Native Interface | — |


The client includes several features designed to reduce detection risk:

- **Gaussian randomization** for reach and timing values
- **GCD-aware aim smoothing** that mimics legitimate mouse input
- **Adaptive delays** that vary naturally
- **Chance-based activation** for probabilistic behavior
- **Thread-safe architecture** with proper cleanup on unload
- **Panic key** (F12) to instantly disable all modules


In the current state which i uploaded it in its probably not going to compile.
Future discontinued ideas:
- More GUIs
- More customisability for the modules like more "rules" for the modules to follow such as sword only for example
- Migrating to webgui
- About 200 more planned modules


### FOR THE WEIRDOS THAT LIKE READING AND WANNA KNOW THE SECURITY OF VENGEANCE AND PERHAPS LEARN


user runs the loader, the first thing the loader does is it calls the server and checks its own file hash, the server connection is verified using certificate pinning against a hardcoded public key so a fake or proxied server gets rejected immediately, if it's been modified it deletes itself and bans the hwid (hwid here was already multi-factor from the start, hardware identifiers, MAC address, drive serials, etc, pulled together rather than relying on one single value, so spoofing one isn't enough on its own, i also considered ip but i backed out of it)

if it's clean it prompts for a login, the username and password are sent to the server which verifies them first if it's a real account, then the discord bot checks for a customer role, on the first successful login the account gets locked to the user hwid
throughout this the loader is running anti-debug including RDTSC timing checks and TLS callbacks, these checks do not run in a dedicated thread, they are scattered inline across execution flow so killing a single thread does nothing, since RDTSC alone is unreliable on VMs and gets fooled by hypervisor traps, it's backed up by separate hypervisor-presence checks and behavioral checks rather than relying on timing alone, it does a check every once in a while if all threads are there, if it detects anything it doesn't just self-delete and ban instantly, it feeds back fake/decoy behavior first so an attacker can't easily tell which specific check tripped, then bans on confirmed pattern

the loader binary itself is obfuscated so reversing it isn't straightforward even if someone gets their hands on it, and since obfuscation only slows static analysis, the server side also rate-limits and flags abnormal auth patterns (same account hammering login, impossible jumps between hwids, etc) so a leaked/cracked loader still gets caught from the backend even if the client-side checks get bypassed
once authenticated the loader requests the main cheat payload from the server, this payload isn't raw bytecode, it's an encrypted blob, the loader decrypts it in memory right before injection and the decryption is timed as close to the injection moment as possible to minimize the window where the key and decrypted bytes exist together in memory
the decrypted payload is then injected into minecraft, but the cheat isn't standalone, the moment it loads it initiates its own communication with the server using a session key provided by the loader, the session key is derived in part from handshake data that only the real server can produce meaning a fake server cannot generate a valid one even if it knows the protocol, and the injected module itself carries lightweight integrity checks on its own critical jumps (like the session-validity check) so patching out the "no session = crash" logic in memory breaks a checksum and crashes anyway instead of silently succeeding
if there's no active valid session the cheat's first and only job is to crash the game instantly, making a dumped payload useless without a live authenticated session backed by the real server, and even the server side doesn't fully trust the panel/bot layer either, since that's historically the easier target than the client, so the auth backend and discord bot run on separate hardened access with logging on every login/role-check call

if you are interested in working with me, my discord: ignrare
enjoy.
