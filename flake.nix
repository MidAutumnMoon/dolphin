{
  description = "Dolphin file manager development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  };

  outputs =
    { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};

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

        # Optional but nice
        kdePackages.kdoctools # doc generation
        kdePackages.kuserfeedback # telemetry (optional)

        # Misc
        kdePackages.packagekit-qt # service menu installer (optional)
      ];

      runtimeDeps = with pkgs; [
        # Things needed at runtime in the devshell
        kdePackages.konsole # terminal panel embedding
        kdePackages.kio # KIO slaves (sftp, smb, trash, etc.)
        kdePackages.kio-extras # additional protocol handlers
      ];
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        name = "dolphin-dev";

        packages = buildDeps ++ runtimeDeps;

        # Qt plugin paths — critical for QML/Widgets to find platform plugins
        QT_PLUGIN_PATH = pkgs.lib.makeSearchPath "lib/qt-6/plugins" (
          with pkgs; [
            qt6.qtbase
            qt6.qtmultimedia
            qt6.qtwayland
          ]
        );

        QML2_IMPORT_PATH = pkgs.lib.makeSearchPath "lib/qt-6/qml" [ pkgs.qt6.qtbase ];

        # Make CMake able to find kdePackages/Qt6 configs
        CMAKE_PREFIX_PATH = pkgs.lib.concatStringsSep ";" (
          with pkgs;
          [
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
          ]
        );

        shellHook = ''
          # Local install prefix for icons, .desktop, etc.
          export DOLPHIN_DEV_PREFIX="$PWD/install"

          # XDG paths so the locally-installed app icon and desktop file are found
          export XDG_DATA_DIRS="''${DOLPHIN_DEV_PREFIX}/share''${XDG_DATA_DIRS:+:}''${XDG_DATA_DIRS:-/usr/local/share:/usr/share}"

          # Convenience aliases
          dolphin-configure() {
            cmake -G Ninja -B build -DCMAKE_INSTALL_PREFIX="''${DOLPHIN_DEV_PREFIX}" .
          }
          dolphin-build() {
            cmake --build build -j$(nproc)
          }
          dolphin-install() {
            cmake --install build
          }
          dolphin-run() {
            "''${DOLPHIN_DEV_PREFIX}/bin/dolphin" "$@"
          }
          dolphin-rebuild() {
            dolphin-build && dolphin-install && dolphin-run "$@"
          }

          echo "🐬 Dolphin dev shell ready"
          echo "   dolphin-configure  # cmake -G Ninja -B build -DCMAKE_INSTALL_PREFIX=... ."
          echo "   dolphin-build      # incremental build"
          echo "   dolphin-install    # install to $PWD/install"
          echo "   dolphin-run        # run the installed binary"
          echo "   dolphin-rebuild    # build + install + run (your daily driver)"
        '';
      };
    };
}
