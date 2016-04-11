#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <QtCore>
#include <QtGui>
#include <QDebug>
#include <QTableWidget>
#include <QHeaderView>

#include "qFlightInstruments.h"

namespace pi {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


QADI::QADI(QWidget *parent)
    : QWidget(parent)
{
    connect(this, SIGNAL(canvasReplot(void)), this, SLOT(canvasReplot_slot(void)));

    m_sizeMin = 200;
    m_sizeMax = 600;
    m_offset = 2;
    m_size = m_sizeMin - 2*m_offset;

    setMinimumSize(m_sizeMin, m_sizeMin);
    setMaximumSize(m_sizeMax, m_sizeMax);
    resize(m_sizeMin, m_sizeMin);

    setFocusPolicy(Qt::NoFocus);

    m_roll  = 0.0;
    m_pitch = 0.0;
}

QADI::~QADI()
{

}


void QADI::canvasReplot_slot(void)
{
    update();
}


void QADI::resizeEvent(QResizeEvent *event)
{
    m_size = qMin(width(),height()) - 2*m_offset;
}

void QADI::paintEvent(QPaintEvent *)
{
    double      roll, pitch;

    roll  = m_roll;
    pitch = m_pitch;

    QPainter painter(this);

    QBrush bgSky(QColor(48,172,220));
    QBrush bgGround(QColor(247,168,21));

    QPen   whitePen(Qt::white);
    QPen   blackPen(Qt::black);
    QPen   pitchPen(Qt::white);
    QPen   pitchZero(Qt::green);

    whitePen.setWidth(2);
    blackPen.setWidth(2);
    pitchZero.setWidth(3);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(width() / 2, height() / 2);
    painter.rotate(roll);

    // FIXME: AHRS output left-hand values
    double pitch_tem = -pitch;

    // draw background
    {
        int y_min, y_max;

        y_min = m_size/2*-40.0/45.0;
        y_max = m_size/2* 40.0/45.0;

        int y = m_size/2*pitch_tem/45.;
        if( y < y_min ) y = y_min;
        if( y > y_max ) y = y_max;

        int x = sqrt(m_size*m_size/4 - y*y);
        qreal gr = atan((double)(y)/x);
        gr = gr * 180./3.1415926;

        painter.setPen(blackPen);
        painter.setBrush(bgSky);
        painter.drawChord(-m_size/2, -m_size/2, m_size, m_size,
                          gr*16, (180-2*gr)*16);

        painter.setBrush(bgGround);
        painter.drawChord(-m_size/2, -m_size/2, m_size, m_size,
                          gr*16, -(180+2*gr)*16);
    }

    // set mask
    QRegion maskRegion(-m_size/2, -m_size/2, m_size, m_size, QRegion::Ellipse);
    painter.setClipRegion(maskRegion);


    // draw pitch lines & marker
    {
        int x, y, x1, y1;
        int textWidth;
        double p, r;
        int ll = m_size/8, l;

        int     fontSize = 8;
        QString s;

        pitchPen.setWidth(2);
        painter.setFont(QFont("", fontSize));


        // draw lines
        for(int i=-9; i<=9; i++) {
            p = i*10;

            s = QString("%1").arg(-p);

            if( i % 3 == 0 )
                l = ll;
            else
                l = ll/2;

            if( i == 0 ) {
                painter.setPen(pitchZero);
                l = l * 1.8;
            } else {
                painter.setPen(pitchPen);
            }

            y = m_size/2*p/45.0 - m_size/2*pitch_tem/45.;
            x = l;

            r = sqrt(x*x + y*y);
            if( r > m_size/2 ) continue;

            painter.drawLine(QPointF(-l, 1.0*y), QPointF(l, 1.0*y));

            textWidth = 100;

            if( i % 3 == 0 && i != 0 ) {
                painter.setPen(QPen(Qt::white));

                x1 = -x-2-textWidth;
                y1 = y - fontSize/2 - 1;
                painter.drawText(QRectF(x1, y1, textWidth, fontSize+2),
                                 Qt::AlignRight|Qt::AlignVCenter, s);
            }
        }

        // draw marker
        int     markerSize = m_size/20;
        float   fx1, fy1, fx2, fy2, fx3, fy3;

        painter.setBrush(QBrush(Qt::red));
        painter.setPen(Qt::NoPen);

        fx1 = markerSize;
        fy1 = 0;
        fx2 = fx1 + markerSize;
        fy2 = -markerSize/2;
        fx3 = fx1 + markerSize;
        fy3 = markerSize/2;

        QPointF points[3] = {
            QPointF(fx1, fy1),
            QPointF(fx2, fy2),
            QPointF(fx3, fy3)
        };
        painter.drawPolygon(points, 3);

        QPointF points2[3] = {
            QPointF(-fx1, fy1),
            QPointF(-fx2, fy2),
            QPointF(-fx3, fy3)
        };
        painter.drawPolygon(points2, 3);
    }

    // draw roll degree lines
    {
        int     nRollLines = 36;
        float   rotAng = 360.0 / nRollLines;
        int     rollLineLeng = m_size/25;
        double  fx1, fy1, fx2, fy2;
        int     fontSize = 8;
        QString s;

        blackPen.setWidth(1);
        painter.setPen(blackPen);
        painter.setFont(QFont("", fontSize));

        for(int i=0; i<nRollLines; i++) {
            if( i < nRollLines/2 )
                s = QString("%1").arg(-i*rotAng);
            else
                s = QString("%1").arg(360-i*rotAng);

            fx1 = 0;
            fy1 = -m_size/2 + m_offset;
            fx2 = 0;

            if( i % 3 == 0 ) {
                fy2 = fy1 + rollLineLeng;
                painter.drawLine(QPointF(fx1, fy1), QPointF(fx2, fy2));

                fy2 = fy1 + rollLineLeng+2;
                painter.drawText(QRectF(-50, fy2, 100, fontSize+2),
                                 Qt::AlignCenter, s);
            } else {
                fy2 = fy1 + rollLineLeng/2;
                painter.drawLine(QPointF(fx1, fy1), QPointF(fx2, fy2));
            }

            painter.rotate(rotAng);
        }
    }

    // draw roll marker
    {
        int     rollMarkerSize = m_size/25;
        double  fx1, fy1, fx2, fy2, fx3, fy3;

        painter.rotate(-roll);
        painter.setBrush(QBrush(Qt::black));

        fx1 = 0;
        fy1 = -m_size/2 + m_offset;
        fx2 = fx1 - rollMarkerSize/2;
        fy2 = fy1 + rollMarkerSize;
        fx3 = fx1 + rollMarkerSize/2;
        fy3 = fy1 + rollMarkerSize;

        QPointF points[3] = {
            QPointF(fx1, fy1),
            QPointF(fx2, fy2),
            QPointF(fx3, fy3)
        };
        painter.drawPolygon(points, 3);
    }
}

void QADI::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        m_roll -= 1.0;
        break;
    case Qt::Key_Right:
        m_roll += 1.0;
        break;
    case Qt::Key_Down:
        if(m_pitch>-90.)
            m_pitch -=1.0;
        break;
    case Qt::Key_Up:
        if(m_pitch<90.)
            m_pitch +=1.0;
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }

    update();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


QCompass::QCompass(QWidget *parent)
    : QWidget(parent)
{
    connect(this, SIGNAL(canvasReplot(void)), this, SLOT(canvasReplot_slot(void)));

    m_sizeMin = 200;
    m_sizeMax = 600;
    m_offset = 2;
    m_size = m_sizeMin - 2*m_offset;

    setMinimumSize(m_sizeMin, m_sizeMin);
    setMaximumSize(m_sizeMax, m_sizeMax);
    resize(m_sizeMin, m_sizeMin);

    setFocusPolicy(Qt::NoFocus);

    m_yaw  = 0.0;
    m_alt  = 0.0;
    m_h    = 0.0;
}

QCompass::~QCompass()
{

}


void QCompass::canvasReplot_slot(void)
{
    update();
}

void QCompass::resizeEvent(QResizeEvent *event)
{
    m_size = qMin(width(),height()) - 2*m_offset;
}

void QCompass::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QBrush bgGround(QColor(48,172,220));

    QPen   whitePen(Qt::white);
    QPen   blackPen(Qt::black);
    QPen   redPen(Qt::red);
    QPen   bluePen(Qt::blue);
    QPen   greenPen(Qt::green);

    whitePen.setWidth(1);
    blackPen.setWidth(2);
    redPen.setWidth(2);
    bluePen.setWidth(2);
    greenPen.setWidth(2);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(width() / 2, height() / 2);


    // draw background
    {
        painter.setPen(blackPen);
        painter.setBrush(bgGround);

        painter.drawEllipse(-m_size/2, -m_size/2, m_size, m_size);
    }


    // draw yaw lines
    {
        int     nyawLines = 36;
        float   rotAng = 360.0 / nyawLines;
        int     yawLineLeng = m_size/25;
        double  fx1, fy1, fx2, fy2;
        int     fontSize = 8;
        QString s;

        blackPen.setWidth(1);
        painter.setPen(blackPen);

        for(int i=0; i<nyawLines; i++) {

            if( i == 0 ) {
                s = "N";
                painter.setPen(bluePen);

                painter.setFont(QFont("", fontSize*1.3));
            } else if ( i == 9 ) {
                s = "E";
                painter.setPen(blackPen);

                painter.setFont(QFont("", fontSize*1.3));
            } else if ( i == 18 ) {
                s = "S";
                painter.setPen(redPen);

                painter.setFont(QFont("", fontSize*1.3));
            } else if ( i == 27 ) {
                s = "W";
                painter.setPen(blackPen);

                painter.setFont(QFont("", fontSize*1.3));
            } else {
                s = QString("%1").arg(i*rotAng);
                painter.setPen(blackPen);

                painter.setFont(QFont("", fontSize));
            }

            fx1 = 0;
            fy1 = -m_size/2 + m_offset;
            fx2 = 0;

            if( i % 3 == 0 ) {
                fy2 = fy1 + yawLineLeng;
                painter.drawLine(QPointF(fx1, fy1), QPointF(fx2, fy2));

                fy2 = fy1 + yawLineLeng+4;
                painter.drawText(QRectF(-50, fy2, 100, fontSize+2),
                                 Qt::AlignCenter, s);
            } else {
                fy2 = fy1 + yawLineLeng/2;
                painter.drawLine(QPointF(fx1, fy1), QPointF(fx2, fy2));
            }

            painter.rotate(rotAng);
        }
    }

    // draw S/N arrow
    {
        int     arrowWidth = m_size/5;
        double  fx1, fy1, fx2, fy2, fx3, fy3;

        fx1 = 0;
        fy1 = -m_size/2 + m_offset + m_size/25 + 15;
        fx2 = -arrowWidth/2;
        fy2 = 0;
        fx3 = arrowWidth/2;
        fy3 = 0;

        painter.setPen(Qt::NoPen);

        painter.setBrush(QBrush(Qt::blue));
        QPointF pointsN[3] = {
            QPointF(fx1, fy1),
            QPointF(fx2, fy2),
            QPointF(fx3, fy3)
        };
        painter.drawPolygon(pointsN, 3);


        fx1 = 0;
        fy1 = m_size/2 - m_offset - m_size/25 - 15;
        fx2 = -arrowWidth/2;
        fy2 = 0;
        fx3 = arrowWidth/2;
        fy3 = 0;

        painter.setBrush(QBrush(Qt::red));
        QPointF pointsS[3] = {
            QPointF(fx1, fy1),
            QPointF(fx2, fy2),
            QPointF(fx3, fy3)
        };
        painter.drawPolygon(pointsS, 3);
    }


    // draw yaw marker
    {
        int     yawMarkerSize = m_size/12;
        double  fx1, fy1, fx2, fy2, fx3, fy3;

        painter.rotate(m_yaw);
        painter.setBrush(QBrush(QColor(0xFF, 0x00, 0x00, 0xE0)));

        fx1 = 0;
        fy1 = -m_size/2 + m_offset;
        fx2 = fx1 - yawMarkerSize/2;
        fy2 = fy1 + yawMarkerSize;
        fx3 = fx1 + yawMarkerSize/2;
        fy3 = fy1 + yawMarkerSize;

        QPointF points[3] = {
            QPointF(fx1, fy1),
            QPointF(fx2, fy2),
            QPointF(fx3, fy3)
        };
        painter.drawPolygon(points, 3);

        painter.rotate(-m_yaw);
    }

    // draw altitude
    {
        int     altFontSize = 13;
        int     fx, fy, w, h;
        QString s;
        char    buf[200];

        w  = 130;
        h  = 2*(altFontSize + 8);
        fx = -w/2;
        fy = -h/2;

        blackPen.setWidth(2);
        painter.setPen(blackPen);
        painter.setBrush(QBrush(Qt::white));
        painter.setFont(QFont("", altFontSize));

        painter.drawRoundedRect(fx, fy, w, h, 6, 6);

        painter.setPen(bluePen);
        sprintf(buf, "ALT: %6.1f m", m_alt);
        s = buf;
        painter.drawText(QRectF(fx, fy+2, w, h/2), Qt::AlignCenter, s);

        sprintf(buf, "H: %6.1f m", m_h);
        s = buf;
        painter.drawText(QRectF(fx, fy+h/2, w, h/2), Qt::AlignCenter, s);
    }
}

void QCompass::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        m_yaw -= 1.0;
        break;
    case Qt::Key_Right:
        m_yaw += 1.0;
        break;
    case Qt::Key_Down:
        m_alt -= 1.0;
        break;
    case Qt::Key_Up:
        m_alt += 1.0;
        break;
    case Qt::Key_W:
        m_h += 1.0;
        break;
    case Qt::Key_S:
        m_h -= 1.0;
        break;

    default:
        QWidget::keyPressEvent(event);
        break;
    }

    update();
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


QFlightIns::QFlightIns(QWidget *parent)
    : QWidget(parent)
{
    connect(this, SIGNAL(canvasReplot(void)), this, SLOT(canvasReplot_slot(void)));

    m_sizeMin = 240;
    m_sizeMax = 600;
    m_offset = 6;
    m_size = m_sizeMin - 6*m_offset;

    setMinimumSize(m_sizeMin, m_sizeMin*1.3);
    setMaximumSize(m_sizeMax, m_sizeMax*1.3);
    resize(m_sizeMin, m_sizeMin*1.3);

    setFocusPolicy(Qt::NoFocus);

    m_roll  = 0.0;
    m_pitch = 0.0;
    m_yaw   = 0.0;
    m_h     = 0.0;
    m_alt   = 0.0;
}

QFlightIns::~QFlightIns()
{

}


void QFlightIns::canvasReplot_slot(void)
{
    update();
}


void QFlightIns::resizeEvent(QResizeEvent *event)
{
    m_size = qMin(width(), height()) - 6*m_offset;
}

void QFlightIns::paintEvent(QPaintEvent *)
{
    double      roll, pitch;

    roll  = m_roll;
    pitch = m_pitch;

    QPainter painter(this);

    QBrush bgSky(QColor(48,172,220));
    QBrush bgGround(QColor(247,168,21));

    QPen   whitePen(Qt::white);
    QPen   blackPen(Qt::black);
    QPen   pitchPen(Qt::white);
    QPen   pitchZero(Qt::green);
    QPen   redPen(Qt::red);
    QPen   bluePen(Qt::blue);

    whitePen.setWidth(2);
    blackPen.setWidth(2);
    pitchZero.setWidth(3);

    redPen.setWidth(2);
    bluePen.setWidth(2);


    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(width() / 2, width() / 2 + 3*m_offset);
    painter.rotate(roll);

    // FIXME: AHRS output left-hand values
    double pitch_tem = -pitch;

    // draw background
    {
        int y_min, y_max;

        y_min = m_size/2*-40.0/45.0;
        y_max = m_size/2* 40.0/45.0;

        int y = m_size/2*pitch_tem/45.;
        if( y < y_min ) y = y_min;
        if( y > y_max ) y = y_max;

        int x = sqrt(m_size*m_size/4 - y*y);
        qreal gr = atan((double)(y)/x);
        gr = gr * 180./3.1415926;

        painter.setPen(blackPen);
        painter.setBrush(bgSky);
        painter.drawChord(-m_size/2, -m_size/2, m_size, m_size,
                          gr*16, (180-2*gr)*16);

        painter.setBrush(bgGround);
        painter.drawChord(-m_size/2, -m_size/2, m_size, m_size,
                          gr*16, -(180+2*gr)*16);
    }

    // set mask
    QRegion maskRegion(-m_size/2, -m_size/2, m_size, m_size, QRegion::Ellipse);
    painter.setClipRegion(maskRegion);


    // draw pitch lines & marker
    {
        int x, y, x1, y1;
        int textWidth;
        double p, r;
        int ll = m_size/8, l;

        int fontSize = 8;
        QString s;

        pitchPen.setWidth(2);
        painter.setFont(QFont("", fontSize));


        // draw lines
        for(int i=-9; i<=9; i++) {
            p = i*10;

            s = QString("%1").arg(-p);

            if( i % 3 == 0 )
                l = ll;
            else
                l = ll/2;

            if( i == 0 ) {
                painter.setPen(pitchZero);
                l = l * 1.8;
            } else {
                painter.setPen(pitchPen);
            }

            y = m_size/2*p/45.0 - m_size/2*pitch_tem/45.;
            x = l;

            r = sqrt(x*x + y*y);
            if( r > m_size/2 ) continue;

            painter.drawLine(QPointF(-l, 1.0*y), QPointF(l, 1.0*y));

            textWidth = 100;

            if( i % 3 == 0 && i != 0 ) {
                painter.setPen(QPen(Qt::white));

                x1 = -x-2-textWidth;
                y1 = y - fontSize/2 - 1;
                painter.drawText(QRectF(x1, y1, textWidth, fontSize+2),
                                 Qt::AlignRight|Qt::AlignVCenter, s);
            }
        }

        // draw marker
        int     markerSize = m_size/20;
        float   fx1, fy1, fx2, fy2, fx3, fy3;

        painter.setBrush(QBrush(Qt::red));
        painter.setPen(Qt::NoPen);

        fx1 = markerSize;
        fy1 = 0;
        fx2 = fx1 + markerSize;
        fy2 = -markerSize/2;
        fx3 = fx1 + markerSize;
        fy3 = markerSize/2;

        QPointF points[3] = {
            QPointF(fx1, fy1),
            QPointF(fx2, fy2),
            QPointF(fx3, fy3)
        };
        painter.drawPolygon(points, 3);

        QPointF points2[3] = {
            QPointF(-fx1, fy1),
            QPointF(-fx2, fy2),
            QPointF(-fx3, fy3)
        };
        painter.drawPolygon(points2, 3);
    }

    // draw roll degree lines
    {
        int     nRollLines = 36;
        float   rotAng = 360.0 / nRollLines;
        int     rollLineLeng = m_size/25;
        double  fx1, fy1, fx2, fy2;
        int     fontSize = 8;
        QString s;

        blackPen.setWidth(1);
        painter.setPen(blackPen);
        painter.setFont(QFont("", fontSize));

        //rollLineLeng = m_offset;

        for(int i=0; i<nRollLines; i++) {
            if( i < nRollLines/2 )
                s = QString("%1").arg(-i*rotAng);
            else
                s = QString("%1").arg(360-i*rotAng);

            fx1 = 0;
            fy1 = -m_size/2;// + m_offset;
            fx2 = 0;

            if( i % 3 == 0 ) {
                fy2 = fy1 + rollLineLeng;
                painter.drawLine(QPointF(fx1, fy1), QPointF(fx2, fy2));

                fy2 = fy1 + rollLineLeng+2;
                painter.drawText(QRectF(-50, fy2, 100, fontSize+2),
                                 Qt::AlignCenter, s);
            } else {
                fy2 = fy1 + rollLineLeng/2;
                painter.drawLine(QPointF(fx1, fy1), QPointF(fx2, fy2));
            }

            painter.rotate(rotAng);
        }
    }

    // draw roll marker
    {
        int     rollMarkerSize = m_size/25;
        double  fx1, fy1, fx2, fy2, fx3, fy3;

        painter.rotate(-roll);
        painter.setBrush(QBrush(Qt::black));

        fx1 = 0;
        fy1 = -m_size/2;
        fx2 = fx1 - rollMarkerSize/2;
        fy2 = fy1 + rollMarkerSize;
        fx3 = fx1 + rollMarkerSize/2;
        fy3 = fy1 + rollMarkerSize;

        QPointF points[3] = {
            QPointF(fx1, fy1),
            QPointF(fx2, fy2),
            QPointF(fx3, fy3)
        };
        painter.drawPolygon(points, 3);
    }

    // clear clip region
    painter.setClipRegion(QRegion(), Qt::NoClip);

    // draw yaw lines
    if( 1 ) {
        int     nyawLines = 36;
        float   rotAng = 360.0 / nyawLines;
        int     yawLineLeng = m_size/25;
        double  fx1, fy1, fx2, fy2;
        int     fontSize = 8;
        QString s;

        blackPen.setWidth(1);
        painter.setPen(blackPen);

        for(int i=0; i<nyawLines; i++) {

            if( i == 0 ) {
                s = "N";
                painter.setPen(bluePen);

                painter.setFont(QFont("", fontSize*1.3));
            } else if ( i == 9 ) {
                s = "E";
                painter.setPen(blackPen);

                painter.setFont(QFont("", fontSize*1.3));
            } else if ( i == 18 ) {
                s = "S";
                painter.setPen(redPen);

                painter.setFont(QFont("", fontSize*1.3));
            } else if ( i == 27 ) {
                s = "W";
                painter.setPen(blackPen);

                painter.setFont(QFont("", fontSize*1.3));
            } else {
                s = "";
                //s = QString("%1").arg(i*rotAng);
                painter.setPen(blackPen);

                //painter.setFont(QFont("", fontSize));
            }

            fx1 = 0;
            fy1 = -m_size/2;
            fx2 = 0;

            if( i % 3 == 0 ) {
                //fy2 = fy1 - yawLineLeng;
                //painter.drawLine(QPointF(fx1, fy1), QPointF(fx2, fy2));

                fy2 = fy1 - fontSize*2;
                painter.drawText(QRectF(-50, fy2, 100, fontSize+4),
                                 Qt::AlignCenter, s);
            } else {
                //fy2 = fy1 - yawLineLeng/2;
                //painter.drawLine(QPointF(fx1, fy1), QPointF(fx2, fy2));
            }

            painter.rotate(rotAng);
        }
    }

    // draw S/N arrow
    if( 0 ) {
        int     arrowWidth = m_size/5;
        double  fx1, fy1, fx2, fy2, fx3, fy3;

        fx1 = 0;
        fy1 = -m_size/2 + m_offset + m_size/25 + 15;
        fx2 = -arrowWidth/2;
        fy2 = 0;
        fx3 = arrowWidth/2;
        fy3 = 0;

        painter.setPen(Qt::NoPen);

        painter.setBrush(QBrush(Qt::blue));
        QPointF pointsN[3] = {
            QPointF(fx1, fy1),
            QPointF(fx2, fy2),
            QPointF(fx3, fy3)
        };
        painter.drawPolygon(pointsN, 3);


        fx1 = 0;
        fy1 = m_size/2 - m_offset - m_size/25 - 15;
        fx2 = -arrowWidth/2;
        fy2 = 0;
        fx3 = arrowWidth/2;
        fy3 = 0;

        painter.setBrush(QBrush(Qt::red));
        QPointF pointsS[3] = {
            QPointF(fx1, fy1),
            QPointF(fx2, fy2),
            QPointF(fx3, fy3)
        };
        painter.drawPolygon(pointsS, 3);
    }


    // draw yaw marker
    {
        int     yawMarkerSize = m_size/12;
        double  fx1, fy1, fx2, fy2, fx3, fy3;

        painter.rotate(m_yaw);
        painter.setBrush(QBrush(QColor(0xFF, 0x00, 0x00, 0xE0)));

        fx1 = 0;
        fy1 = -m_size/2 - yawMarkerSize;
        fx2 = fx1 - yawMarkerSize/2;
        fy2 = fy1 + yawMarkerSize;
        fx3 = fx1 + yawMarkerSize/2;
        fy3 = fy1 + yawMarkerSize;

        QPointF points[3] = {
            QPointF(fx1, fy1),
            QPointF(fx2, fy2),
            QPointF(fx3, fy3)
        };
        painter.drawPolygon(points, 3);

        painter.rotate(-m_yaw);
    }

    // draw altitude
    {
        int     altFontSize = 13;
        int     fx, fy, w, h;
        QString s;
        char    buf[200];

        w  = m_size;
        h  = 2*(altFontSize + 8);
        fx = -w/2;
        fy = m_size/2 + h/2;

        blackPen.setWidth(2);
        painter.setPen(blackPen);
        painter.setBrush(QBrush(Qt::white));
        painter.setFont(QFont("", altFontSize));

        painter.drawRoundedRect(fx, fy, w, h, 6, 6);

        painter.setPen(bluePen);
        sprintf(buf, "ALT: %9.1f m", m_alt);
        s = buf;
        painter.drawText(QRectF(fx, fy+2, w, h/2), Qt::AlignCenter, s);

        sprintf(buf, "H: %9.1f m", m_h);
        s = buf;
        painter.drawText(QRectF(fx, fy+h/2, w, h/2), Qt::AlignCenter, s);
    }
}

void QFlightIns::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        m_roll -= 1.0;
        break;
    case Qt::Key_Right:
        m_roll += 1.0;
        break;
    case Qt::Key_Down:
        if(m_pitch>-90.)
            m_pitch -=1.0;
        break;
    case Qt::Key_Up:
        if(m_pitch<90.)
            m_pitch +=1.0;
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }

    update();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


QKeyValueListView::QKeyValueListView(QWidget *parent) : QTableWidget(parent)
{
    connect(this, SIGNAL(listUpdate(void)), this, SLOT(listUpdate_slot(void)));

    m_mutex = new RMutex();

    // set row & column numbers
    setRowCount(0);
    setColumnCount(2);

    // set no headers
    verticalHeader()->hide();
    horizontalHeader()->hide();

    // set last section is stretch-able
    QHeaderView *HorzHdr = horizontalHeader();
    HorzHdr->setStretchLastSection(true);
    HorzHdr->resizeSection(0, 80);     // set first column width

    // disable table edit & focus
    setEditTriggers(QTableWidget::NoEditTriggers);
    setFocusPolicy(Qt::NoFocus);
}

QKeyValueListView::~QKeyValueListView()
{
    delete m_mutex;
}

void QKeyValueListView::setData(String2StringMap_ &ssm)
{
    m_data.clear();

    for(String2StringMap_::iterator it=ssm.begin(); it!=ssm.end(); it++) {
        m_data.insert(QString::fromStdString(it->first), QString::fromStdString(it->second));
    }
}

void QKeyValueListView::listUpdate_slot(void)
{
    int                 i, n;
    ListMap::iterator   it;

    QColor              clCL1, clCL2;
    QColor              clB1, clB2;

    int                 fontSize = 8;
    int                 rowHeight = 20;

    clCL1 = QColor(0x00, 0x00, 0xFF);
    clCL2 = QColor(0x00, 0x00, 0x00);
    clB1  = QColor(0xFF, 0xFF, 0xFF);
    clB2  = QColor(0xE0, 0xE0, 0xE0);

    m_mutex->lock();

    n = m_data.size();
    setRowCount(n);
    setColumnCount(2);

    for(i=0, it=m_data.begin(); it!=m_data.end(); i++, it++) {
        // set name cell
        if( this->item(i, 0) != NULL ) {
            this->item(i, 0)->setText(it.key());
        } else {
            QTableWidgetItem* item = new QTableWidgetItem();
            item->setText(it.key());

            item->setTextColor(clCL1);
            if( i % 2 == 0 ) item->setBackgroundColor(clB1);
            else             item->setBackgroundColor(clB2);

            item->setFont(QFont("", fontSize));

            this->setItem(i, 0, item);
        }

        // set value cell
        if( this->item(i, 1) != NULL ) {
            this->item(i, 1)->setText(it.value());
        } else {
            QTableWidgetItem* item = new QTableWidgetItem();
            item->setText(it.value());

            item->setTextColor(clCL2);
            if( i % 2 == 0 ) item->setBackgroundColor(clB1);
            else             item->setBackgroundColor(clB2);

            item->setFont(QFont("", fontSize));

            this->setItem(i, 1, item);
        }

        setRowHeight(i, rowHeight);
    }

    m_mutex->unlock();
}

} // end of namespace pi

