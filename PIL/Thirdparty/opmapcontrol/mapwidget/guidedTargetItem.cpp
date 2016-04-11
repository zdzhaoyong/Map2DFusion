/**
******************************************************************************
*
* @file       guidedTargetItem.cpp
* @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
* @brief      A graphicsItem representing a trail point
* @see        The GNU Public License (GPL) Version 3
* @defgroup   OPMapWidget
* @{
*
*****************************************************************************/

/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "guidedTargetItem.h"

namespace mapcontrol {

GuidedTargetItem::GuidedTargetItem(MapGraphicItem* map, OPMapWidget* parent) :
    map(map), mapwidget(parent),
    altitude(0)
{
    pic.load(QString::fromUtf8(":/markers/images/guidedTarget.png"));
    pic = pic.scaled(30, 30, Qt::IgnoreAspectRatio);

    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

    coord = mapwidget->CurrentPosition();

    localposition = map->FromLatLngToLocal(coord);
    this->setPos(localposition.X(), localposition.Y());

    this->setZValue(5);

    //        this->setFlag(QGraphicsItem::ItemIsMovable,true);
    //        this->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
    //        this->setFlag(QGraphicsItem::ItemIsSelectable,true);
}

void GuidedTargetItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->drawPixmap(-pic.width()/2,-pic.height()/2, pic);
}

QRectF GuidedTargetItem::boundingRect() const
{
    return QRectF(-pic.width()/2, -pic.height()/2, pic.width(), pic.height());
}


int GuidedTargetItem::type() const
{
    return Type;
}

void GuidedTargetItem::RefreshPos()
{
    prepareGeometryChange();
    localposition=map->FromLatLngToLocal(coord);
    this->setPos(localposition.X(),localposition.Y());
}

}
