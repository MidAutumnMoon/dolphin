/*
 * SPDX-FileCopyrightText: 2011 Peter Penz <peter.penz19@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef DOLPHINFILEITEMLISTWIDGET_H
#define DOLPHINFILEITEMLISTWIDGET_H

#include "dolphin_export.h"
#include "kitemviews/kfileitemlistwidget.h"
class DOLPHIN_EXPORT DolphinFileItemListWidget : public KFileItemListWidget
{
    Q_OBJECT

public:
    DolphinFileItemListWidget(KItemListWidgetInformant *informant, QGraphicsItem *parent);
    ~DolphinFileItemListWidget() override;

protected:
    void refreshCache() override;

};

#endif
