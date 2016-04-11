/**
******************************************************************************
*
* @file       CircularFlightItem.cpp
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

#include "circularFlightItem.h"

namespace mapcontrol {



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define EARTH_RADIUS        6378137.0               ///< Earth radius (unit: m)
#define SQR(x) ((x)*(x))

#define DEG2RAD              0.017453292519943      ///< degree to radian
#define RAD2DEG             57.295779513082323      ///< radian to degree


/// \ref http://en.wikipedia.org/wiki/Latitude
///
int calcLngLatFromDistance(double lng1, double lat1,
                           double dx, double dy,
                           double &lng2, double &lat2)
{

    double      lng_unit, lat_unit, phi_rad;
    double      a = EARTH_RADIUS;
    double      f = 1.0/298.257223563;
    double      e_2 = 2*f - f*f;

    phi_rad = lat1 * DEG2RAD;

    // longitude & latitude unit
    {
        double n0 = DEG2RAD * a * cos(phi_rad) / sqrt(1 - e_2*SQR(sin(phi_rad)));
        lng_unit = n0;


        double t0 = DEG2RAD * a * (1-e_2) / pow(1-e_2*SQR(sin(phi_rad)), 1.5);
        lat_unit = t0;
    }

    // calculate lng2, lat2
    lng2=dx/lng_unit+lng1;
    lat2=dy/lat_unit+lat1;

    return 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CircularFlightItem::CircularFlightItem(MapGraphicItem* map, OPMapWidget* parent) :
    map(map), mapwidget(parent),
    altitude(0), circleRadius(5), localRadius(5)
{
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

    coord = mapwidget->CurrentPosition();

    localPosition = map->FromLatLngToLocal(coord);
    this->setPos(localPosition.X(), localPosition.Y());

    localRadius = meterToLocal(circleRadius);

    this->setZValue(4);

    //        this->setFlag(QGraphicsItem::ItemIsMovable,true);
    //        this->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
    //        this->setFlag(QGraphicsItem::ItemIsSelectable,true);
}

void CircularFlightItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // draw circle
    QPen p = painter->pen();
    p.setColor(circleColor);
    p.setWidth(2);
    painter->setPen(p);

    painter->drawEllipse(QPoint(0, 0), localRadius, localRadius);

    // draw center marker
    p.setWidth(1);
    painter->setPen(p);
    painter->drawLine(-3,  0,  3,  0);
    painter->drawLine( 0, -3,  0,  3);
}

QRectF CircularFlightItem::boundingRect() const
{
    return QRectF(-localRadius, -localRadius, localRadius, localRadius);
}

int CircularFlightItem::type() const
{
    return Type;
}

void CircularFlightItem::RefreshPos()
{
    prepareGeometryChange();

    localPosition = map->FromLatLngToLocal(coord);
    this->setPos(localPosition.X(),localPosition.Y());

    localRadius = meterToLocal(circleRadius);
}

void CircularFlightItem::setRadius(const double r)
{
    circleRadius = r;
    localRadius = meterToLocal(circleRadius);
}


int CircularFlightItem::meterToLocal(const double d)
{
    internals::PointLatLng p2;
    double  lat2, lng2;

    calcLngLatFromDistance(coord.Lng(), coord.Lat(),
                           d, 0,
                           lng2, lat2);
    p2.SetLat(lat2);
    p2.SetLng(lng2);

    core::Point lp1, lp2;

    lp1 = map->FromLatLngToLocal(coord);
    lp2 = map->FromLatLngToLocal(p2);

    int r = abs(lp2.X() - lp1.X());
    if( r <= 0 ) r = 1;

    return r;
}

}
