/*
 * SPDX-FileCopyrightText: 2011 Peter Penz <peter.penz19@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "dolphinfileitemlistwidget.h"
#include "../kitemviews/private/kitemviewsutils.h"

#include "dolphindebug.h"

#include <KIconLoader>

DolphinFileItemListWidget::DolphinFileItemListWidget(KItemListWidgetInformant *informant, QGraphicsItem *parent)
    : KFileItemListWidget(informant, parent)
{
}

DolphinFileItemListWidget::~DolphinFileItemListWidget() = default;

void DolphinFileItemListWidget::refreshCache()
{
    QColor color;
    const QHash<QByteArray, QVariant> values = data();
    QHash<Qt::Corner, QString> overlays;

    if (values.contains("iconOverlays")) {
        const auto corners = {Qt::Corner::BottomRightCorner, Qt::Corner::TopLeftCorner, Qt::Corner::TopRightCorner};
        const auto iconOverlays = values.value("iconOverlays").toStringList();
        auto overlaysIt = iconOverlays.constBegin();
        for (const auto &corner : corners) {
            if (overlaysIt == iconOverlays.constEnd()) {
                break;
            }
            overlays.insert(corner, *overlaysIt);
            overlaysIt = ++overlaysIt;
        }
    }

    setOverlays(overlays);
    setTextColor(color);
    setProperty(QByteArrayLiteral("iconSize"), styleOption().iconSize);
}

#include "moc_dolphinfileitemlistwidget.cpp"
