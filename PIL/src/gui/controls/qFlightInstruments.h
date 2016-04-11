#ifndef __QFLIGHTINSTRUMENTS_H__
#define __QFLIGHTINSTRUMENTS_H__

#include <QtCore>
#include <QtGui>
#include <QWidget>
#include <QMap>
#include <QTableWidget>

#include "base/osa/osa++.h"

namespace pi {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Attitude indicator class
///
class QADI : public QWidget
{
    Q_OBJECT

public:
    QADI(QWidget *parent = 0);
    ~QADI();

    ///
    /// \brief Set roll & pitch values (in degree)
    /// \param r - roll
    /// \param p - pitch
    ///
    void setData(double r, double p) {
        m_roll = r;
        m_pitch = p;
        if( m_roll < -180 ) m_roll = -180;
        if( m_roll > 180  ) m_roll =  180;
        if( m_pitch < -90 ) m_pitch = -90;
        if( m_pitch > 90  ) m_pitch =  90;

        emit canvasReplot();
    }

    ///
    /// \brief Set roll angle (in degree)
    /// \param val - roll
    ///
    void setRoll(double val) {
        m_roll  = val;
        if( m_roll < -180 ) m_roll = -180;
        if( m_roll > 180  ) m_roll =  180;

        emit canvasReplot();
    }

    ///
    /// \brief Set pitch value (in degree)
    /// \param val
    ///
    void setPitch(double val) {
        m_pitch = val;
        if( m_pitch < -90 ) m_pitch = -90;
        if( m_pitch > 90  ) m_pitch =  90;

        emit canvasReplot();
    }

    ///
    /// \brief Get roll angle (in degree)
    /// \return roll angle
    ///
    double getRoll() {return m_roll;}

    ///
    /// \brief Get pitch angle (in degree)
    /// \return pitch angle
    ///
    double getPitch(){return m_pitch;}


signals:
    void canvasReplot(void);

protected slots:
    void canvasReplot_slot(void);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);

protected:
    int     m_sizeMin, m_sizeMax;           ///< widget's min/max size (in pixel)
    int     m_size, m_offset;               ///< current size & offset

    double  m_roll;                         ///< roll angle (in degree)
    double  m_pitch;                        ///< pitch angle (in degree)
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Compass & altitude display class
///
class QCompass : public QWidget
{
    Q_OBJECT

public:
    QCompass(QWidget *parent = 0);
    ~QCompass();

    ///
    /// \brief Set all data (yaw, alt, height)
    ///
    /// \param y - yaw ( in degree)
    /// \param a - altitude ( in m)
    /// \param h - height from ground (in m)
    ///
    void setData(double y, double a, double h) {
        m_yaw = y;
        m_alt = a;
        m_h   = h;

        if( m_yaw < 0   ) m_yaw = 360 + m_yaw;
        if( m_yaw > 360 ) m_yaw = m_yaw - 360;

        emit canvasReplot();
    }

    ///
    /// \brief Set yaw angle (in degree)
    /// \param val - yaw angle (in degree)
    ///
    void setYaw(double val) {
        m_yaw  = val;
        if( m_yaw < 0   ) m_yaw = 360 + m_yaw;
        if( m_yaw > 360 ) m_yaw = m_yaw - 360;

        emit canvasReplot();
    }

    ///
    /// \brief Set altitude value
    /// \param val - altitude (in m)
    ///
    void setAlt(double val) {
        m_alt = val;

        emit canvasReplot();
    }

    ///
    /// \brief Set height from ground
    /// \param val - height (in m)
    ///
    void setH(double val) {
        m_h = val;

        emit canvasReplot();
    }

    ///
    /// \brief Get yaw angle
    /// \return yaw angle (in degree)
    ///
    double getYaw() {return m_yaw;}

    ///
    /// \brief Get altitude value
    /// \return altitude (in m)
    ///
    double getAlt() {return m_alt;}

    ///
    /// \brief Get height from ground
    /// \return height from ground (in m)
    ///
    double getH()   {return m_h;}

signals:
    void canvasReplot(void);

protected slots:
    void canvasReplot_slot(void);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);

protected:
    int     m_sizeMin, m_sizeMax;               ///< widget min/max size (in pixel)
    int     m_size, m_offset;                   ///< widget size and offset size

    double  m_yaw;                              ///< yaw angle (in degree)
    double  m_alt;                              ///< altitude (in m)
    double  m_h;                                ///< height from ground (in m)
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// \brief The Attitude indicator class
///
class QFlightIns : public QWidget
{
    Q_OBJECT

public:
    QFlightIns(QWidget *parent = 0);
    ~QFlightIns();

    ///
    /// \brief Set all data
    /// \param r - roll
    /// \param p - pitch
    /// \param y - yaw
    /// \param alt - altitude
    /// \param h   - height
    ///
    void setData(double r, double p, double y, double alt, double h) {
        m_roll  = r;
        m_pitch = p;
        m_yaw   = y;
        m_alt   = alt;
        m_h     = h;

        if( m_roll < -180 ) m_roll = -180;
        if( m_roll > 180  ) m_roll =  180;
        if( m_pitch < -90 ) m_pitch = -90;
        if( m_pitch > 90  ) m_pitch =  90;
        if( m_yaw < 0   ) m_yaw = 360 + m_yaw;
        if( m_yaw > 360 ) m_yaw = m_yaw - 360;


        emit canvasReplot();
    }

    ///
    /// \brief Set roll angle (in degree)
    /// \param val - roll
    ///
    void setRoll(double val) {
        m_roll  = val;
        if( m_roll < -180 ) m_roll = -180;
        if( m_roll > 180  ) m_roll =  180;

        emit canvasReplot();
    }

    ///
    /// \brief Set pitch value (in degree)
    /// \param val
    ///
    void setPitch(double val) {
        m_pitch = val;
        if( m_pitch < -90 ) m_pitch = -90;
        if( m_pitch > 90  ) m_pitch =  90;

        emit canvasReplot();
    }

    ///
    /// \brief Set yaw value (in degree)
    /// \param val
    ///
    void setYaw(double val) {
        m_yaw = val;
        if( m_yaw < 0   ) m_yaw = 360 + m_yaw;
        if( m_yaw > 360 ) m_yaw = m_yaw - 360;

        emit canvasReplot();
    }

    ///
    /// \brief Set altitude value (in meter)
    /// \param val
    ///
    void setAlt(double val) {
        m_alt = val;

        emit canvasReplot();
    }

    ///
    /// \brief Set height value (in meter)
    /// \param val
    ///
    void setH(double val) {
        m_h = val;

        emit canvasReplot();
    }

    ///
    /// \brief Get roll angle (in degree)
    /// \return roll angle
    ///
    double getRoll() {return m_roll;}

    ///
    /// \brief Get pitch angle (in degree)
    /// \return pitch angle
    ///
    double getPitch(){return m_pitch;}

    ///
    /// \brief Get yaw angle (in degree)
    /// \return yaw angle
    ///
    double getYaw(){ return m_yaw; }

    ///
    /// \brief Get altitudea (in meter)
    /// \return altitude
    ///
    double getAlt(){ return m_alt; }

    ///
    /// \brief Get height (in meter)
    /// \return height
    ///
    double getHeight(){ return m_h; }



signals:
    void canvasReplot(void);

protected slots:
    void canvasReplot_slot(void);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *event);

protected:
    int     m_sizeMin, m_sizeMax;           ///< widget's min/max size (in pixel)
    int     m_size, m_offset;               ///< current size & offset

    double  m_roll;                         ///< roll angle (in degree)
    double  m_pitch;                        ///< pitch angle (in degree)
    double  m_yaw;                          ///< yaw angle (in degree)
    double  m_alt;                          ///< altitude (in meter)
    double  m_h;                            ///< height (in meter)
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef QMap<QString, QString> ListMap;
typedef std::map<std::string, std::string> String2StringMap_;

///
/// \brief The List view class, it will display key-value pair in lines
///
class QKeyValueListView : public QTableWidget
{
public:
    Q_OBJECT

public:
    QKeyValueListView(QWidget *parent = 0);
    virtual ~QKeyValueListView();

    ///
    /// \brief Set list data
    /// \param d - list data
    ///
    void setData(ListMap &d) {
        m_data = d;
        emit listUpdate();
    }

    void setData(String2StringMap_ &ssm);

    ///
    /// \brief Get list data
    /// \param d - list data obj
    ///
    ListMap& getData(void) {
        return m_data;
    }

    void beginSetData(void) {
        m_mutex->lock();
    }

    void endSetData(void) {
        m_mutex->unlock();
    }

    ///
    /// \brief Reloat data to table widget
    ///
    void listReload(void) {
        emit listUpdate();
    }

signals:
    void listUpdate(void);

protected slots:
    void listUpdate_slot(void);

protected:
    ListMap         m_data;
    RMutex          *m_mutex;
};

} // end of namespace pi

#endif // end of __QFLIGHTINSTRUMENTS_H__
