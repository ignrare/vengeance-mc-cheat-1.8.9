# Minecraft Cheat - Modern Organized Structure

## 📁 Project Structure

This project has been completely reorganized for better maintainability and clarity.

### 🏗️ Core Architecture

```
SDK/
├── 📁 Core/                    # Core functionality and hooks
│   ├── Core.h                  # Core system declarations
│   └── Core.cpp                # Core system implementation
├── 📁 Modules/                  # All cheat modules
│   ├── 📁 Combat/             # Combat-related features
│   │   ├── Combat.h
│   │   └── Combat.cpp
│   ├── 📁 Visuals/            # Visual-related features
│   │   ├── Visuals.h
│   │   └── Visuals.cpp
│   ├── 📁 Movement/           # Movement-related features
│   │   ├── Movement.h
│   │   └── Movement.cpp
│   ├── 📁 Network/            # Network-related features
│   │   ├── Network.h
│   │   └── Network.cpp
│   ├── 📁 Utilities/          # Utility features
│   │   ├── Utilities.h
│   │   └── Utilities.cpp
│   └── 📁 Settings/           # Settings and configuration
│       ├── Settings.h
│       └── Settings.cpp
├── 📁 UI/                       # User interface system
│   ├── UI.h                    # UI system declarations
│   ├── UI.cpp                  # UI system implementation
│   └── ModernMenu.cpp          # Modern menu rendering
├── 📁 Esp/                      # ESP rendering system
│   ├── Position.h
│   ├── Position.cpp
│   ├── Radius.h
│   ├── Radius.cpp
│   ├── Color.h
│   └── Color.cpp
├── 📁 GlDraw/                   # OpenGL drawing utilities
│   ├── glDraw.h
│   └── glDraw.cpp
├── 📁 GlText/                   # OpenGL text rendering
│   ├── glText.h
│   └── glText.cpp
├── 📄 main_new.cpp              # New organized main file
├── 📄 main.cpp                 # Original main file (backup)
├── 📄 includes.h               # Global includes and definitions
├── 📄 CMakeLists.txt           # CMake build configuration
└── 📄 README.md                # This file
```

## 🎯 Module Categories

### ⚔️ Combat Module
- **Aimbot**: Advanced aiming assistance with customizable settings
- **AutoClicker**: Automated clicking with CPS control and randomization
- **Blockhit**: Block hitting automation with timing controls

### 👁️ Visuals Module
- **ESP**: Entity highlighting with customizable colors
- **Tracer**: Line tracing to entities
- **XRay**: See through blocks
- **Chest ESP**: Highlight chests and containers

### 🏃 Movement Module
- **Speed**: Movement speed multiplier
- **Fly**: Flight mode with speed control
- **No Fall**: Prevent fall damage

### 🌐 Network Module
- **No Packet**: Block packet sending
- **Ping Spoof**: Fake ping values

### 🛠️ Utilities Module
- **Fullbright**: Maximum brightness
- **No Weather**: Disable weather effects
- **Time Changer**: Control world time

### ⚙️ Settings Module
- **Keybinds**: Configure hotkeys for all features
- **Theme**: UI customization options
- **Unload**: Safe DLL unloading

## 🎨 UI Features

### Modern Dark Theme
- Dark gray (#121212) background
- Light gray (#E0E0E0) text
- Electric blue (#007AFF) accents
- Smooth rounded corners and shadows

### Layout
- **140px Sidebar**: Category navigation
- **Main Content**: Module tabs and settings
- **Custom Elements**: Modern checkboxes, sliders, toggles
- **Footer**: Version and attribution

### Custom UI Components
- `CustomCheckbox()`: Rounded checkboxes with smooth animations
- `CustomSlider()`: Modern sliders with custom styling
- `ModernToggle()`: Animated toggle switches
- `CategoryButton()`: Sidebar navigation buttons
- `ModuleTabButton()`: Horizontal module tabs
- `SectionHeader()`: Styled section dividers

## 🔧 Building

### Using CMake
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Dependencies
- **ImGui**: Immediate Mode GUI library
- **OpenGL 2**: Graphics rendering
- **Win32 API**: Windows integration
- **C++17**: Modern C++ features

## 📋 Key Features

### Safety & Detection
- Clean code organization for easier maintenance
- Modular architecture for better debugging
- Modern UI with professional appearance
- Configurable keybinds for quick access

### Performance
- Optimized rendering loops
- Efficient entity tracking
- Minimal CPU overhead
- Smooth animations and transitions

### Customization
- Full theme control
- Adjustable module settings
- Custom keybinds
- Flexible configuration options

## 🚀 Usage

1. **Compile** the project using CMake
2. **Inject** the DLL into Minecraft
3. **Press INSERT** to toggle the menu
4. **Navigate** categories using the sidebar
5. **Configure** modules using the tabs
6. **Set keybinds** for quick toggling

## 📝 Development Notes

### Adding New Modules
1. Create folder in `Modules/`
2. Create `.h` and `.cpp` files
3. Implement required functions:
   - `Initialize[Module]()`
   - `Update[Module]()`
   - `Render[Module]UI()`
   - `Cleanup[Module]()`
4. Add to `UI.h` includes
5. Update `g_categories` in `UI.cpp`

### UI Customization
- Modify `UITheme` namespace in `UI.h`
- Update `UILayout` constants for sizing
- Add custom elements in `UI.cpp`
- Modify `ModernMenu.cpp` for layout changes

### Core Modifications
- Hook new functions in `Core.cpp`
- Update entity tracking systems
- Add new OpenGL integrations
- Implement additional safety checks

## 📄 License

This project is for educational purposes only.
