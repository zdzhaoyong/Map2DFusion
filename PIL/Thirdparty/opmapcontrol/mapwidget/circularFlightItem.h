/**
******************************************************************************
*
* @file       circularFlightItem.h
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

#ifndef __CIRCULARFLIGHTITEM_H__
#define __CIRCULARFLIGHTITEM_H__

#include <QGraphicsItem>
#include <QPainter>
#include <QLabel>
#include <QObject>

#include "opmapwidget.h"
#include "../internals/pointlatlng.h"

namespace mapcontrol {

class CircularFlightItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    enum { Type = UserType + 11 };

    CircularFlightItem(MapGraphicItem* map, OPMapWidget* parent);

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    QRectF boundingRect() const;

    void RefreshPos();

    int type() const;


    void SetCoord(internals::PointLatLng const& value) { coord=value; }
    internals::PointLatLng Coord() const { return coord; }
    void SetAltitude(int const& value){ altitude=value; }
    int Altitude() const { return altitude; }
    void setRadius(const double r);
    double Radius() const { return circleRadius; }

    void setColor(const QColor &cl) { circleColor = cl; }

    int meterToLocal(const double d);

protected:
    MapGraphicItem              *map;
    OPMapWidget                 *mapwidget;


    internals::PointLatLng      coord;                      ///< lat, lon
    int                         altitude;                   ///< altitude (m)
    double                      circleRadius;               ///< circle radius (m)

    core::Point                 localPosition;              ///< local postion (in pixel)
    int                         localRadius;                ///< local radius (in pixel)

    QColor                      circleColor;


public slots:

signals:

};

} // end of namespace mapcontrol

#endif // end of __CIRCULARFLIGHTITEM_H__
