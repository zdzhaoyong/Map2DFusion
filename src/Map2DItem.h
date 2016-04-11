/******************************************************************************

  This file is part of Map2DFusion.

  Copyright 2016 (c)  Yong Zhao <zd5945@126.com> http://www.zhaoyong.adv-ci.com

  ----------------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/
#ifndef MAP2DITEM_H
#define MAP2DITEM_H
#include <opmapcontrol/opmapcontrol.h>
#include <base/types/types.h>

namespace mapcontrol{

class Map2DElement
{
public:
    Map2DElement(){}
    Map2DElement(const QPixmap& img,const internals::PointLatLng& _lt,const internals::PointLatLng& _rb)
        :image(img),lt(_lt),rb(_rb)
    {
    }

    QPixmap image;
    internals::PointLatLng lt,rb;
};

/**
 * @brief The Map2DItem class does not contains any mutex and all thing should be done in GUI thread.
 */
class Map2DItem: public QObject, public QGraphicsItem
{
//    Q_OBJECT
//    Q_INTERFACES(QGraphicsItem)
public:
    enum { Type = UserType + 25 };
    Map2DItem(MapGraphicItem* _map, OPMapWidget* parent);
    virtual ~Map2DItem();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    virtual QRectF boundingRect() const;

    bool update(const QPixmap& img,const pi::Point3d& _lt,const pi::Point3d& _rb);
    void insertGPSPoint(const internals::PointLatLng& pos){gpsPoints.push_back(pos);}

    QRectF rectGPS;
    std::map<internals::PointLatLng,Map2DElement> elements;
    std::vector<internals::PointLatLng>           gpsPoints;

    MapGraphicItem* map;
    OPMapWidget* mapwidget;
};

}

#endif // MAP2DITEM_H
