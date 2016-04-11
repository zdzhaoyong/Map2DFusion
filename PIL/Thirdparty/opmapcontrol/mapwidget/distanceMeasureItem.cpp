/**
******************************************************************************
*
* @file       distanceMeasureItem.cpp
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

#include "distanceMeasureItem.h"

namespace mapcontrol {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief calcLngLatDistance - given two point, and calculate thier offset (dx, dy)
///
/// \param lng1     - longitude of point 1
/// \param lat1     - latitude of point 1
/// \param lng2     - longitude of point 2
/// \param lat2     - latitude of point 2
/// \param dx       - distance in x-axis
/// \param dy       - distance in y-axis
/// \param method   - calculation method (0, 1)
///
/// \return
///     0           - success
///
/// \ref http://en.wikipedia.org/wiki/Latitude
///
int calcLngLatDistance(double lng1, double lat1,
                      double lng2, double lat2,
                      double &dx, double &dy,
                      int method=0)
{
    #define EARTH_RADIUS        6378137.0               ///< Earth radius (unit: m)
    #define SQR(x) ((x)*(x))

    #define DEG2RAD              0.017453292519943      ///< degree to radian
    #define RAD2DEG             57.295779513082323      ///< radian to degree

     double      lng_unit, lat_unit, phi_rad;
     double      a = EARTH_RADIUS;
     double      f = 1.0/298.257223563;
     double      e_2 = 2*f - f*f;

     phi_rad = (lat1+lat2)/2.0 * DEG2RAD;

     // longitude & latitude unit
     {
         double n0 = DEG2RAD * a * cos(phi_rad) / sqrt(1 - e_2*SQR(sin(phi_rad)));
         double n1 = DEG2RAD * a * cos(phi_rad);

         if( 1 == method ) lng_unit = n1;
         else              lng_unit = n0;


         double t0 = DEG2RAD * a * (1-e_2) / pow(1-e_2*SQR(sin(phi_rad)), 1.5);
         double t1 = DEG2RAD * a;
         if( 1 == method ) lat_unit = t1;
         else              lat_unit = t0;
     }

     // calculate dx, dy
     dx = (lng2 - lng1) * lng_unit;
     dy = (lat2 - lat1) * lat_unit;

     return 0;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define MARKER_SIZE     7
#define FONT_SIZE       20

DistanceMeasureItem::DistanceMeasureItem(MapGraphicItem* map, OPMapWidget* parent) :
    map(map), mapwidget(parent),
    altitude(0), remoteAltitude(0)
{
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    this->setZValue(4);

    coord = internals::PointLatLng(mapwidget->CurrentPosition().Lat(), mapwidget->CurrentPosition().Lng());
    localposition = map->FromLatLngToLocal(coord);
    this->setPos(localposition.X(), localposition.Y());

    remoteCoord = internals::PointLatLng(mapwidget->CurrentPosition().Lat(), mapwidget->CurrentPosition().Lng());
    remotePosition = map->FromLatLngToLocal(remoteCoord);

    //this->setFlag(QGraphicsItem::ItemIsMovable,true);
    //this->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
    //this->setFlag(QGraphicsItem::ItemIsSelectable,true);
}

void DistanceMeasureItem::SetCoord(internals::PointLatLng const& value)
{
    coord = value;
    localposition = map->FromLatLngToLocal(coord);
    this->setPos(localposition.X(), localposition.Y());

    update();
}

void DistanceMeasureItem::SetRemoteCoord(internals::PointLatLng const& value)
{
    remoteCoord = value;
    remotePosition = map->FromLatLngToLocal(remoteCoord);

    update();
}


void DistanceMeasureItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // calcualte distance
    remotePosition = map->FromLatLngToLocal(remoteCoord);
    double dx = remotePosition.X() - localposition.X();
    double dy = remotePosition.Y() - localposition.Y();

    // draw connection line
    painter->setPen(QColor(220, 220, 220));
    painter->drawLine(QPointF(0, 0), QPointF(dx, dy));

    // draw origin point marker
    painter->setPen(Qt::yellow);
    painter->drawLine(QPointF(-MARKER_SIZE, 0), QPointF(MARKER_SIZE, 0));
    painter->drawLine(QPointF(0, -MARKER_SIZE), QPointF(0, MARKER_SIZE));

    // draw current marker
    painter->setPen(Qt::green);
    painter->drawLine(QPointF(dx-MARKER_SIZE, dy), QPointF(dx+MARKER_SIZE, dy));
    painter->drawLine(QPointF(dx, dy-MARKER_SIZE), QPointF(dx, dy+MARKER_SIZE));

    // draw distances
    double x, y;
    char buf[256];

    calcLngLatDistance(coord.Lng(), coord.Lat(), remoteCoord.Lng(), remoteCoord.Lat(),
                       x, y);
    sprintf(buf, "%9.2f, %9.2f (%10.2f)", x, y, sqrt(x*x+y*y));

    painter->setPen(Qt::white);
    QFont font(painter->font());
    font.setPixelSize(FONT_SIZE);
    painter->setFont(font);

    painter->drawText(QPointF(dx+MARKER_SIZE+2, dy+MARKER_SIZE+2), buf);
}

QRectF DistanceMeasureItem::boundingRect() const
{
    double minX, minY, maxX, maxY;
    minX = -MARKER_SIZE; maxX = MARKER_SIZE;
    minY = -MARKER_SIZE; maxY = MARKER_SIZE;

    double dx = remotePosition.X() - localposition.X();
    double dy = remotePosition.Y() - localposition.Y();

    if( dx < minX ) minX = dx; if( dx > maxX ) maxX = dx;
    if( dy < minY ) minY = dy; if( dy > maxY ) maxY = dy;

    double w = maxX - minX;
    double h = maxY - minY;
    return QRectF(minX, minY, w, h);
}

int DistanceMeasureItem::type()const
{
    return Type;
}

void DistanceMeasureItem::RefreshPos()
{
    prepareGeometryChange();

    localposition = map->FromLatLngToLocal(coord);
    this->setPos(localposition.X(), localposition.Y());

    remotePosition = map->FromLatLngToLocal(remoteCoord);
}

}
