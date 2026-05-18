# Dolphin

Personal fork of KDE's file manager. See the [upstream project](https://invent.kde.org/system/dolphin) for the original.

## Fork Changes

**Compact view removed.** Only icons and details view modes remain. Existing configs that used compact view are migrated to details view.

**Icons view removed.** Only details view mode is available.

**PackageKit integration removed.** No more automatic package installation via PackageKit. 

**Version control integration removed.** The dolphinvcs library, VCS plugin loading, and all version control overlays/actions in the UI are gone.

**Telemetry (KUserFeedback) removed.** No usage data is collected or sent.

**Separate config file.** Uses `dolphin-nextrc` instead of `dolphinrc`, so settings don't conflict with stock Dolphin.
