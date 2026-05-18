{
    description = "Dolphin file manager development environment";

    inputs = {
        nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    };

    outputs = { self, nixpkgs }: let
        system = "x86_64-linux";
        pkgs = nixpkgs.legacyPackages.${system};

        # Dev tools
        devTools = with pkgs; [
            llvmPackages_latest.clang-tools
            neocmakelsp
        ];

        # KDE Plasma 6 / kdePackages development dependencies
        buildDeps = with pkgs; [
            # Build system
            cmake
            kdePackages.extra-cmake-modules
            pkg-config
            ninja

            # Qt6
            qt6.qtbase
            qt6.qttools # for Qt6::LinguistTools (optional but useful)
            qt6.qtmultimedia # for Baloo info panel
            qt6.qtwayland # Wayland support
            qt6.qtsvg # SVG icon rendering — REQUIRED for breeze icons
            qt6.wrapQtAppsHook

            # kdePackages core frameworks (matching root CMakeLists.txt)
            kdePackages.kcoreaddons
            kdePackages.ki18n
            kdePackages.kconfig
            kdePackages.kdbusaddons
            kdePackages.kio # pulls in KIOCore, KIOWidgets, KIOFileWidgets
            kdePackages.kcmutils
            kdePackages.knewstuff
            kdePackages.kparts
            kdePackages.solid
            kdePackages.kiconthemes
            kdePackages.kcompletion
            kdePackages.ktextwidgets
            kdePackages.knotifications
            kdePackages.kcrash
            kdePackages.kwindowsystem
            kdePackages.kwidgetsaddons
            kdePackages.kcodecs
            kdePackages.kguiaddons
            kdePackages.kcolorscheme
            kdePackages.kbookmarks
            kdePackages.kfilemetadata
            kdePackages.baloo
            kdePackages.baloo-widgets
        ];

        runtimeDeps = with pkgs; [
            # Things needed at runtime in the devshell
            kdePackages.konsole # terminal panel embedding
            kdePackages.kio # KIO slaves (sftp, smb, trash, etc.)
            kdePackages.kio-extras # additional protocol handlers

            # Icon/theme support on non-KDE WMs (niri, sway, etc.)
            kdePackages.breeze # Qt/GTK widget theme
            kdePackages.breeze-icons # KDE app icons (including Dolphin's)
            kdePackages.oxygen-icons # fallback icon theme
            hicolor-icon-theme # base icon theme spec + index.theme
            gtk3 # provides gtk-update-icon-cache
        ];

        # All packages whose share/ should be in XDG_DATA_DIRS
        # This is what wrapQtAppsHook does for installed NixOS apps
        xdgDataPackages = with pkgs; [
            kdePackages.breeze
            kdePackages.breeze-icons
            kdePackages.oxygen-icons
            hicolor-icon-theme
            kdePackages.kiconthemes
            kdePackages.kio
            kdePackages.kio-extras
            kdePackages.kfilemetadata
            kdePackages.baloo
            kdePackages.baloo-widgets
            kdePackages.kcoreaddons
        ];

        # All packages whose lib/qt-6/plugins/ should be in QT_PLUGIN_PATH
        qtPluginPackages = with pkgs; [
            qt6.qtbase
            qt6.qtsvg
            qt6.qtmultimedia
            qt6.qtwayland
            kdePackages.kiconthemes
        ];

    in {
        devShells.${system}.default = pkgs.mkShellNoCC {
            name = "dolphin-dev";

            packages = buildDeps ++ runtimeDeps ++ devTools;

            # Qt plugin paths — SVG engine, icon engines, platform plugins, etc.
            QT_PLUGIN_PATH =
                pkgs.lib.makeSearchPath "lib/qt-6/plugins" qtPluginPackages;

            QML2_IMPORT_PATH =
                pkgs.lib.makeSearchPath "lib/qt-6/qml" [ pkgs.qt6.qtbase ];

            # Static XDG data dirs from Nix store paths (evaluated at build time)
            _XDG_DATA_DIRS_STATIC = pkgs.lib.concatStringsSep ":" (
                map (p: "${p}/share") xdgDataPackages
            );

            # Make CMake able to find kdePackages/Qt6 configs
            CMAKE_PREFIX_PATH = pkgs.lib.concatStringsSep ";" (with pkgs; [
                qt6.qtbase
                qt6.qtmultimedia
                kdePackages.kcoreaddons
                kdePackages.kio
                kdePackages.kcmutils
                kdePackages.kparts
                kdePackages.solid
                kdePackages.baloo
                kdePackages.baloo-widgets
                kdePackages.kfilemetadata
                kdePackages.kbookmarks
                kdePackages.knotifications
                kdePackages.kcrash
                kdePackages.extra-cmake-modules
            ]);

            shellHook = ''
                # Shell-agnostic helper scripts (works in bash, fish, zsh...)
                export PATH="$PWD/.bin''${PATH:+:}''${PATH}"

                # Local install prefix
                export DOLPHIN_DEV_PREFIX="$PWD/install"

                # XDG data dirs — icon themes, .desktop files, mime types, etc.
                # Local install prefix first, then Nix store paths, then system defaults
                export XDG_DATA_DIRS="$PWD/install/share:''${_XDG_DATA_DIRS_STATIC}''${XDG_DATA_DIRS:+:}''${XDG_DATA_DIRS}"

                echo "🐬 Dolphin dev shell ready"
                echo "   .configure  # cmake -G Ninja -B build ... (first time)"
                echo "   .run        # build + install + run (daily driver)"
            '';
        };
    };
}
