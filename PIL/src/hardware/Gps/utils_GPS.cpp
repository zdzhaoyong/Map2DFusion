#include <stdio.h>
#include <math.h>
#include <time.h>

#include <sys/time.h>

#include "utils_GPS.h"


namespace pi {


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define EARTH_RADIUS        6378137.0               ///< Earth radius (unit: m)
#define SQR(x) ((x)*(x))

#define DEG2RAD              0.017453292519943      ///< degree to radian
#define RAD2DEG             57.295779513082323      ///< radian to degree


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

double calc_earth_dis(double long_1, double lat_1, double long_2, double lat_2)
{
    double          radLat1, radLat2;
    double          a, b, s;

    radLat1 = lat_1*M_PI/180.0;
    radLat2 = lat_2*M_PI/180.0;

    a = radLat1 - radLat2;
    b = (long_1 - long_2)*M_PI/180.0;

    s = 2*asin( sqrt(SQR(sin(a/2)) + cos(radLat1)*cos(radLat2)*SQR(b/2)) );
    s = s*EARTH_RADIUS;
    //s = round(s*10000)/10000.0;

    return s;
}


double calc_longitude_unit(double lat)
{
    double      a, f, e_2, l, l2;
    double      phi, phi_rad;
    int         i;

    a = EARTH_RADIUS;
    f = 1.0/298.257222101;
    e_2 = 2*f - f*f;

    phi_rad = lat;

    l  = M_PI/180.0 * a * cos(phi_rad) / sqrt(1 - e_2*sin(phi_rad)*sin(phi_rad));
    l2 = a*cos(phi_rad) * M_PI/180.0;

    return l;

}

int calc_earth_offset(double lng1, double lat1, double lng2, double lat2,
                      double &dx, double &dy)
{
    double      lng_unit;
    double      a;

    a = EARTH_RADIUS;

    lng_unit = calc_longitude_unit(lat1);
    dx = (lng2 - lng1) * lng_unit;
    dy = a*(lat2 - lat1)*M_PI/180.0;

    return 0;
}

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
                      int method)
{
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

} // end of namespace pi
