/**
******************************************************************************
*
* @file       waypointitem.cpp
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

#include <QGraphicsSceneMouseEvent>

#include "waypointitem.h"

namespace mapcontrol {


int calc_dxdy_fromLatLng(double lat1, double lng1,
                         double lat2, double lng2,
                         double &dx, double &dy)
{
    #define EARTH_RADIUS        6378137.0               ///< Earth radius (unit: m)
    #define SQR(x)              ((x)*(x))

    #define DEG2RAD             0.017453292519943      ///< degree to radian
    #define RAD2DEG             57.295779513082323      ///< radian to degree


     double      lng_unit, lat_unit, phi_rad;
     double      a = EARTH_RADIUS;
     double      f = 1.0/298.257223563;
     double      e_2 = 2*f - f*f;

     phi_rad = (lat1+lat2)/2.0 * DEG2RAD;
     lng_unit = DEG2RAD * a * cos(phi_rad) / sqrt(1 - e_2*SQR(sin(phi_rad)));
     lat_unit = DEG2RAD * a * (1-e_2) / pow(1-e_2*SQR(sin(phi_rad)), 1.5);

     // calculate dx, dy
     dx = (lng2 - lng1) * lng_unit;
     dy = (lat2 - lat1) * lat_unit;

     return 0;
}


WayPointItem::WayPointItem(const internals::PointLatLng &coord,
                           double const& altitude,
                           MapGraphicItem *map) :
    map(map),
    autoreachedEnabled(true),
    text(NULL),
    textBG(NULL),
    numberI(NULL),
    numberIBG(NULL),
    coord(coord),
    reached(false),
    description(""),
    shownumber(true),
    showHide(true),
    moveable(true),
    isDragging(false),
    altitude(altitude),             // sets a 10m default just in case
    homeAlt(400),
    height(50),
    heading(0),
    wpType("Waypoint"),
    number(0),
    m_mouseDown(false)
{
    picture.load(QString::fromUtf8(":/markers/images/marker.png"));
    number=WayPointItem::snumber;
    ++WayPointItem::snumber;

    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);

    // transf.translate(picture.width()/2,picture.height());
    // this->setTransform(transf);

    SetShowNumber(shownumber);
    RefreshToolTip();
    RefreshPos();
}

WayPointItem::WayPointItem(const internals::PointLatLng &coord,
                           double const& altitude,
                           const QString &description, MapGraphicItem *map) :
    map(map),
    autoreachedEnabled(true),
    text(NULL),
    textBG(NULL),
    numberI(NULL),
    numberIBG(NULL),
    coord(coord),
    reached(false),
    description(description),
    shownumber(true),
    showHide(true),
    moveable(true),
    isDragging(false),
    altitude(altitude), // sets a 10m default just in case
    homeAlt(400),
    height(50),
    heading(0),
    wpType("Waypoint"),
    number(0)
{
    picture.load(QString::fromUtf8(":/markers/images/marker.png"));
    number=WayPointItem::snumber;
    ++WayPointItem::snumber;

    this->setFlag(QGraphicsItem::ItemIsMovable,true);
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
    this->setFlag(QGraphicsItem::ItemIsSelectable,true);

    //transf.translate(picture.width()/2,picture.height());
    // this->setTransform(transf);

    SetShowNumber(shownumber);

    RefreshToolTip();
    RefreshPos();
}

WayPointItem::~WayPointItem()
{
    --WayPointItem::snumber;

    if( numberI != NULL ) {
        delete numberI;
        numberI = NULL;
    }

    if( numberIBG != NULL ) {
        delete numberIBG;
        numberIBG = NULL;
    }
}


QRectF WayPointItem::boundingRect() const
{
    return QRectF(-picture.width()/2, -picture.height(),
                  picture.width(), picture.height());
}

void WayPointItem::paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if( showHide == false ) return;

    painter->drawPixmap(-picture.width()/2,-picture.height(),picture);
    if(this->isSelected()) {
        painter->drawRect(QRectF(-picture.width()/2,-picture.height(),
                                 picture.width()-1,picture.height()-1));
    }
}

void WayPointItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if( event->button() == Qt::LeftButton && moveable ) {
        if (m_mouseDown) {
            //Mouse is already pressed
            return;
        }
        m_mouseDown = true;

        Q_ASSERT(text == NULL);
        Q_ASSERT(textBG == NULL);

        text=new QGraphicsSimpleTextItem(this);
        textBG=new QGraphicsRectItem(this);

        textBG->setBrush(QColor(255, 255, 255, 128));
        textBG->setOpacity(0.5);

        text->setPen(QPen(Qt::red));
        text->setPos(10,-picture.height());
        textBG->setPos(10,-picture.height());
        text->setZValue(3);
        RefreshToolTip();
        isDragging=true;
    }

    QGraphicsItem::mousePressEvent(event);
}

void WayPointItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if( event->button()==Qt::LeftButton && moveable ) {
        m_mouseDown = false;
        delete text;
        text = NULL;
        delete textBG;
        textBG = NULL;
        coord=map->FromLocalToLatLng(this->pos().x(),this->pos().y());
        QString coord_str = " " + QString::number(coord.Lat(), 'f', 6)
                + "   " + QString::number(coord.Lng(), 'f', 6);
        // qDebug() << "WP MOVE:" << coord_str << __FILE__ << __LINE__;
        isDragging=false;
        RefreshToolTip();

        emit WPValuesChanged(this);
    }

    QGraphicsItem::mouseReleaseEvent(event);
}

void WayPointItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if( isDragging && moveable ) {
        coord = map->FromLocalToLatLng(this->pos().x(),this->pos().y());

        QString coord_str = " " + QString::number(coord.Lat(), 'f', 6)
                + "   " + QString::number(coord.Lng(), 'f', 6);

        WayPointItem *wpP = getPreviousWaypoint();
        if( wpP != NULL ) {
            internals::PointLatLng coordP = wpP->Coord();
            double dx, dy, dis;

            calc_dxdy_fromLatLng(coordP.Lat(), coordP.Lng(), coord.Lat(), coord.Lng(),
                                 dx, dy);
            dis = sqrt(dx*dx + dy*dy);

            coord_str =  coord_str + " (dx=" + QString::number(dx, 'f', 2) +
                                     ", dy=" + QString::number(dy, 'f', 2) +
                                     ", dis=" + QString::number(dis, 'f', 2) +
                                     ")";
        }

        text->setText(coord_str);
        textBG->setRect(text->boundingRect());

        emit WPValuesChanged(this);
    }

    QGraphicsItem::mouseMoveEvent(event);
}

void WayPointItem::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event )
{
    if(event->button()==Qt::LeftButton) {
        emit WPEdit(number, this);
    }

    QGraphicsItem::mouseDoubleClickEvent(event);
}

void WayPointItem::SetAltitude(const double &value)
{
    altitude=value;
    RefreshToolTip();

    emit WPValuesChanged(this);
    this->update();
}

void WayPointItem::SetHeight(double const& value)
{
    height = value;
    altitude = homeAlt + height;
    RefreshToolTip();

    emit WPValuesChanged(this);
    this->update();
}

void WayPointItem::SetHomeAlt(double const& value)
{
    homeAlt = value;
    altitude = homeAlt + height;
    RefreshToolTip();

    emit WPValuesChanged(this);
    this->update();
}


void WayPointItem::SetHeading(const float &value)
{
    heading=value;
    RefreshToolTip();

    emit WPValuesChanged(this);
    this->update();
}

void WayPointItem::SetCoord(const internals::PointLatLng &value)
{
    coord=value;
    emit WPValuesChanged(this);
    RefreshPos();
    RefreshToolTip();
    this->update();
}

void WayPointItem::SetDescription(const QString &value)
{
    description=value;
    RefreshToolTip();
    emit WPValuesChanged(this);
    this->update();
}

void WayPointItem::SetNumber(const int &value)
{
    emit WPNumberChanged(number,value,this);
    number=value;
    RefreshToolTip();
    numberI->setText(QString::number(number));
    numberIBG->setRect(numberI->boundingRect().adjusted(-2,0,1,0));
    this->update();
}

void WayPointItem::SetReached(const bool &value)
{
    if (autoreachedEnabled) {
        reached=value;
        emit WPValuesChanged(this);
        if(value)
            picture.load(QString::fromUtf8(":/markers/images/bigMarkerGreen.png"));
        else
            picture.load(QString::fromUtf8(":/markers/images/marker.png"));
        this->update();
    }
}


WayPointItem* WayPointItem::getPreviousWaypoint(void)
{
    foreach(QGraphicsItem* i, map->childItems()) {
        WayPointItem *w = qgraphicsitem_cast<WayPointItem*>(i);
        if( w ) {
            if( w->Number() == number-1 ) return w;
        }
    }

    return NULL;
}


void WayPointItem::SetShowNumber(const bool &value)
{
    //        shownumber=value;
    //        if((numberI==0) && value)
    //        {
    //            numberI=new QGraphicsSimpleTextItem(this);
    //            numberIBG=new QGraphicsRectItem(this);
    //            numberIBG->setBrush(Qt::white);
    //            numberIBG->setOpacity(0.5);
    //            numberI->setZValue(3);
    //            numberI->setPen(QPen(Qt::blue));
    //            numberI->setPos(0,-13-picture.height());
    //            numberIBG->setPos(0,-13-picture.height());
    //            numberI->setText(QString::number(number));
    //            numberIBG->setRect(numberI->boundingRect().adjusted(-2,0,1,0));
    //        }
    //        else if (!value && numberI)
    //        {
    //            delete numberI;
    //            delete numberIBG;
    //        }
    //        this->update();

    shownumber=value;
    if((numberI==NULL) && value) {
        Q_ASSERT(numberI == NULL);
        Q_ASSERT(numberIBG == NULL);
        numberI=new QGraphicsSimpleTextItem(this);
        numberIBG=new QGraphicsRectItem(this);
        numberIBG->setBrush(Qt::black);
        numberIBG->setOpacity(0.5);
        numberI->setZValue(3);
        numberI->setBrush(Qt::white);
        numberI->setPos(18,-picture.height()/2-2);
        numberIBG->setPos(18,-picture.height()/2-2);
        numberI->setText(QString::number(number));
        numberIBG->setRect(numberI->boundingRect().adjusted(-2,0,1,0));
    } else if (!value && numberI) {
        delete numberI;
        numberI = NULL;

        delete numberIBG;
        numberIBG = NULL;
    }

    this->update();
}


void WayPointItem::SetShowHide(bool const& value)
{
    showHide = value;
    SetShowNumber(value);
}

void WayPointItem::SetMoveable(bool const& value)
{
    moveable = value;

    if( moveable == false ) {
        this->setFlag(QGraphicsItem::ItemIsMovable, false);
        this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    } else {
        this->setFlag(QGraphicsItem::ItemIsMovable, true);
        this->setFlag(QGraphicsItem::ItemIgnoresTransformations, false);
    }
}



void WayPointItem::WPDeleted(const int &onumber)
{
    if(number>onumber) --number;
    numberI->setText(QString::number(number));
    numberIBG->setRect(numberI->boundingRect().adjusted(-2,0,1,0));
    RefreshToolTip();
    this->update();
}

void WayPointItem::WPInserted(const int &onumber, WayPointItem *waypoint)
{
    if( waypoint!=this ) {
        if(onumber<=number) ++number;
        numberI->setText(QString::number(number));
        RefreshToolTip();
        this->update();
    }
}

void WayPointItem::WPRenumbered(const int &oldnumber,
                                const int &newnumber,
                                WayPointItem *waypoint)
{
    if (waypoint!=this) {
        if(((oldnumber>number) && (newnumber<=number))) {
            ++number;
            numberI->setText(QString::number(number));
            numberIBG->setRect(numberI->boundingRect().adjusted(-2,0,1,0));
            RefreshToolTip();
        } else if (((oldnumber<number) && (newnumber>number))) {
            --number;
            numberI->setText(QString::number(number));
            numberIBG->setRect(numberI->boundingRect().adjusted(-2,0,1,0));
            RefreshToolTip();
        } else if (newnumber==number) {
            ++number;
            numberI->setText(QString::number(number));
            RefreshToolTip();
        }

        this->update();
    }
}

int WayPointItem::type() const
{
    // Enable the use of qgraphicsitem_cast with this item.
    return Type;
}


void WayPointItem::RefreshPos()
{
    core::Point point=map->FromLatLngToLocal(coord);
    this->setPos(point.X(),point.Y());
}

void WayPointItem::RefreshToolTip()
{
    QString coord_str = QString::number(coord.Lat(), 'f', 6)
            + "   " + QString::number(coord.Lng(), 'f', 6);

    setToolTip(QString("WayPoint Number: %1\nDescription: %2\nCoordinate: %4\n"
                       "Altitude: %5 m (MSL)\nHeading: %6 deg").arg(
                   QString::number(WayPointItem::number)).arg(description).arg(coord_str).arg(QString::number(altitude)).arg(QString::number(heading)));
}

int WayPointItem::snumber=0;

} // end of namespace mapcontrol
