/**
******************************************************************************
*
* @file       distanceMeasureItem.h
* @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
* @brief      A graphicsItem representing a WayPoint
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

#ifndef __DISTANCEMEASUREITEM_H__
#define __DISTANCEMEASUREITEM_H__

#include <QGraphicsItem>
#include <QPainter>
#include <QLabel>
#include <QObject>

#include "opmapwidget.h"
#include "../internals/pointlatlng.h"

namespace mapcontrol {

class DistanceMeasureItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    enum { Type = UserType + 9 };

    DistanceMeasureItem(MapGraphicItem* map, OPMapWidget* parent);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    QRectF boundingRect() const;

    int type() const;
    void RefreshPos();

    void SetCoord(internals::PointLatLng const& value);
    internals::PointLatLng Coord()const{return coord;}
    void SetAltitude(int const& value){altitude=value;}
    int Altitude()const{return altitude;}

    void SetRemoteCoord(internals::PointLatLng const& value);
    internals::PointLatLng RemoteCoord()const{return remoteCoord;}
    void SetRemoteAltitude(int const& value){remoteAltitude=value;}
    int RemoteAltitude() const {return remoteAltitude;}

private:
    MapGraphicItem* map;
    OPMapWidget* mapwidget;

    core::Point localposition;
    internals::PointLatLng coord;
    int altitude;

    core::Point remotePosition;
    internals::PointLatLng remoteCoord;
    int remoteAltitude;

public slots:

signals:

};

} // end of namespace mapcontrol

#endif // end of __DISTANCEMEASUREITEM_H__
