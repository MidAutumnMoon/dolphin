/*
 * SPDX-FileCopyrightText: 2011 Peter Penz <peter.penz19@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef VIEWMODESETTINGS_H
#define VIEWMODESETTINGS_H

#include "kitemviews/kstandarditemlistview.h"
#include "views/dolphinview.h"
#include "viewsettingstab.h"

#include <variant>

class DetailsModeSettings;
class IconsModeSettings;

/**
 * @short Helper class for accessing similar properties of IconsModeSettings
 *        and DetailsModeSettings.
 */
class ViewModeSettings
{
public:
    explicit ViewModeSettings(DolphinView::Mode mode);
    explicit ViewModeSettings(ViewSettingsTab::Mode mode);
    explicit ViewModeSettings(KStandardItemListView::ItemLayout itemLayout);

    void setIconSize(int iconSize);
    int iconSize() const;

    void setPreviewSize(int previewSize);
    int previewSize() const;

    void setUseSystemFont(bool useSystemFont);
    bool useSystemFont() const;

    void setViewFont(const QFont &font);
    QFont viewFont() const;

    void useDefaults(bool useDefaults);

    void readConfig();
    void save();

private:
    explicit ViewModeSettings();

    std::variant<IconsModeSettings *, DetailsModeSettings *> m_viewModeSettingsVariant;
};

#endif
