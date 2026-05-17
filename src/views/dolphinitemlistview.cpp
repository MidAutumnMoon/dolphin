/*
 * SPDX-FileCopyrightText: 2011 Peter Penz <peter.penz19@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "dolphinitemlistview.h"

#include "dolphin_contentdisplaysettings.h"
#include "dolphin_detailsmodesettings.h"
#include "dolphin_generalsettings.h"

#include "dolphinfileitemlistwidget.h"
#include "settings/viewmodes/viewmodesettings.h"
#include "zoomlevelinfo.h"

#include <KIO/PreviewJob>
#include <QtMath>

DolphinItemListView::DolphinItemListView(QGraphicsWidget *parent)
    : KFileItemListView(parent)
    , m_zoomLevel(0)
{
    DolphinItemListView::updateFont();
    updateGridSize();
}

DolphinItemListView::~DolphinItemListView()
{
    writeSettings();
}

void DolphinItemListView::setZoomLevel(int level)
{
    if (level < ZoomLevelInfo::minimumLevel()) {
        level = ZoomLevelInfo::minimumLevel();
    } else if (level > ZoomLevelInfo::maximumLevel()) {
        level = ZoomLevelInfo::maximumLevel();
    }

    if (level == m_zoomLevel) {
        return;
    }

    m_zoomLevel = level;

    const bool useGlobalViewProps = GeneralSettings::globalViewProps();
    ViewModeSettings settings(itemLayout());

    if (previewsShown()) {
        m_previewSize = ZoomLevelInfo::iconSizeForZoomLevel(level);
        // Only update the icon size settings if we're using global view props
        // to prevent inconsistent state on zoom level changes
        if (useGlobalViewProps) {
            settings.setPreviewSize(m_previewSize);
        }
    } else {
        // Same as above
        m_iconSize = ZoomLevelInfo::iconSizeForZoomLevel(level);
        if (useGlobalViewProps) {
            settings.setIconSize(m_iconSize);
        }
    }

    updateGridSize();
}

int DolphinItemListView::zoomLevel() const
{
    return m_zoomLevel;
}

void DolphinItemListView::setEnabledSelectionToggles(DolphinItemListView::SelectionTogglesEnabled selectionTogglesEnabled)
{
    m_selectionTogglesEnabled = selectionTogglesEnabled;
    switch (m_selectionTogglesEnabled) {
    case True: {
        setEnabledSelectionToggles(true);
        return;
    }
    case False: {
        setEnabledSelectionToggles(false);
        return;
    }
    case FollowSetting: {
        setEnabledSelectionToggles(GeneralSettings::showSelectionToggle());
        return;
    }
    }
}

void DolphinItemListView::readSettings()
{
    // We load the settings for all view modes now because we don't load them when the view mode changes.
    DetailsModeSettings::self()->load();

    ContentDisplaySettings::self()->load();

    beginTransaction();

    setEnabledSelectionToggles(m_selectionTogglesEnabled);
    setHighlightEntireRow(itemLayoutHighlightEntireRow(itemLayout()));
    setSupportsItemExpanding(itemLayoutSupportsItemExpanding(itemLayout()));

    updateFont();
    updateGridSize();

    const KConfigGroup globalConfig(KSharedConfig::openConfig(), QStringLiteral("PreviewSettings"));
    setEnabledPlugins(globalConfig.readEntry("Plugins", KIO::PreviewJob::defaultPlugins()));
    endTransaction();
}

void DolphinItemListView::writeSettings()
{
    DetailsModeSettings::self()->save();
}

KItemListWidgetCreatorBase *DolphinItemListView::defaultWidgetCreator() const
{
    return new KItemListWidgetCreator<DolphinFileItemListWidget>();
}

bool DolphinItemListView::itemLayoutHighlightEntireRow(ItemLayout layout) const
{
    return layout == DetailsLayout && DetailsModeSettings::highlightEntireRow();
}

bool DolphinItemListView::itemLayoutSupportsItemExpanding(ItemLayout layout) const
{
    return layout == DetailsLayout && DetailsModeSettings::expandableFolders();
}

void DolphinItemListView::onItemLayoutChanged(ItemLayout current, ItemLayout previous)
{
    setHeaderVisible(current == DetailsLayout);

    updateFont();
    updateGridSize();

    KFileItemListView::onItemLayoutChanged(current, previous);
}

void DolphinItemListView::onPreviewsShownChanged(bool shown)
{
    Q_UNUSED(shown)
    updateGridSize();
}

void DolphinItemListView::onVisibleRolesChanged(const QList<QByteArray> &current, const QList<QByteArray> &previous)
{
    KFileItemListView::onVisibleRolesChanged(current, previous);
    updateGridSize();
}

void DolphinItemListView::updateFont()
{
    const ViewModeSettings settings(itemLayout());
    KItemListStyleOption option = styleOption();

    if (settings.useSystemFont()) {
        KItemListView::updateFont();
        option.font = font();
    } else {
        option.font = settings.viewFont();
    }

    option.fontMetrics = QFontMetrics(option.font);
    setStyleOption(option);
}

void DolphinItemListView::updateGridSize()
{
    const ViewModeSettings settings(itemLayout());
    const bool useGlobalViewProps = GeneralSettings::globalViewProps();

    // Calculate the size of the icon
    // Only use zoom stored in settings if we're using global view props
    const int iconSize = useGlobalViewProps ? (previewsShown() ? settings.previewSize() : settings.iconSize()) : (previewsShown() ? m_previewSize : m_iconSize);
    m_zoomLevel = ZoomLevelInfo::zoomLevelForIconSize(QSize(iconSize, iconSize));
    KItemListStyleOption option = styleOption();

    const int padding = 2;
    int horizontalMargin = 0;
    int verticalMargin = 0;

    // Calculate the item-width and item-height
    int itemWidth;
    int itemHeight;
    int maxTextLines = 0;
    int maxTextWidth = 0;

    switch (itemLayout()) {
    case KFileItemListView::DetailsLayout: {
        itemWidth = -1;
        itemHeight = padding * 2 + qMax(iconSize, option.fontMetrics.lineSpacing());
        break;
    }
    default:
        itemWidth = -1;
        itemHeight = -1;
        Q_ASSERT(false);
        break;
    }

    // Apply the calculated values
    option.padding = padding;
    option.horizontalMargin = horizontalMargin;
    option.verticalMargin = verticalMargin;
    option.iconSize = iconSize;
    option.maxTextLines = maxTextLines;
    option.maxTextWidth = maxTextWidth;
    beginTransaction();
    setStyleOption(option);
    setItemSize(QSizeF(itemWidth, itemHeight));
    endTransaction();
}

#include "moc_dolphinitemlistview.cpp"
