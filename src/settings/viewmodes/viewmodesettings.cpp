/*
 * SPDX-FileCopyrightText: 2011 Peter Penz <peter.penz19@gmail.com>
 * SPDX-FileCopyrightText: 2021 Felix Ernst <felixernst@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "viewmodesettings.h"

#include "dolphin_detailsmodesettings.h"

#include "dolphin_generalsettings.h"

ViewModeSettings::ViewModeSettings()
{
    auto removeEntries = [](KConfigGroup &group) {
        group.deleteEntry("FontFamily");
        group.deleteEntry("FontWeight");
        group.deleteEntry("ItalicFont");
    };

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    // Migrate old config entries
    if (GeneralSettings::version() < 202) {
        for (const QString &groupName : {QStringLiteral("CompactMode"), QStringLiteral("DetailsMode")}) {
            KConfigGroup group = config->group(groupName);
            const QString family = group.readEntry("FontFamily", QString{});
            if (family.isEmpty()) {
                removeEntries(group);
                continue;
            }

            QFont font;
            font.setFamily(family);
            const int weight = group.readEntry<int>("FontWeight", QFont::Normal);
            font.setWeight(static_cast<QFont::Weight>(weight));
            font.setItalic(group.readEntry("ItalicFont", false));
            removeEntries(group);

            // Write the new config entry
            group.writeEntry("ViewFont", font);
        }

        config->sync();
    }
}

ViewModeSettings::ViewModeSettings(DolphinView::Mode mode)
    : ViewModeSettings()
{
    Q_UNUSED(mode)
    m_viewModeSettingsVariant = DetailsModeSettings::self();
}

ViewModeSettings::ViewModeSettings(ViewSettingsTab::Mode mode)
    : ViewModeSettings()
{
    Q_UNUSED(mode)
    m_viewModeSettingsVariant = DetailsModeSettings::self();
}

ViewModeSettings::ViewModeSettings(KStandardItemListView::ItemLayout itemLayout)
    : ViewModeSettings()
{
    Q_UNUSED(itemLayout)
    m_viewModeSettingsVariant = DetailsModeSettings::self();
}

void ViewModeSettings::setIconSize(int iconSize)
{
    m_viewModeSettingsVariant->setIconSize(iconSize);
}

int ViewModeSettings::iconSize() const
{
    return m_viewModeSettingsVariant->iconSize();
}

void ViewModeSettings::setPreviewSize(int previewSize)
{
    m_viewModeSettingsVariant->setPreviewSize(previewSize);
}

int ViewModeSettings::previewSize() const
{
    return m_viewModeSettingsVariant->previewSize();
}

void ViewModeSettings::setUseSystemFont(bool useSystemFont)
{
    m_viewModeSettingsVariant->setUseSystemFont(useSystemFont);
}

bool ViewModeSettings::useSystemFont() const
{
    return m_viewModeSettingsVariant->useSystemFont();
}

void ViewModeSettings::setViewFont(const QFont &font)
{
    m_viewModeSettingsVariant->setViewFont(font);
}

QFont ViewModeSettings::viewFont() const
{
    return m_viewModeSettingsVariant->viewFont();
}

void ViewModeSettings::useDefaults(bool useDefaults)
{
    m_viewModeSettingsVariant->useDefaults(useDefaults);
}

void ViewModeSettings::readConfig()
{
    m_viewModeSettingsVariant->load();
}

void ViewModeSettings::save()
{
    m_viewModeSettingsVariant->save();
}
