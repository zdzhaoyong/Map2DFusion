#ifndef __UTILS_GPS_H__
#define __UTILS_GPS_H__


namespace pi {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

double calc_earth_dis(double long_1, double lat_1, double long_2, double lat_2);
double calc_longitude_unit(double lat);
int calc_earth_offset(double long1, double lat1, double long2, double lat2,
                      double &dx, double &dy);


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
                       int method=0);



/// \ref http://en.wikipedia.org/wiki/Latitude
///
int calcLngLatFromDistance(double lng1, double lat1,
                           double dx, double dy,
                           double &lng2, double &lat2);

} // end of namespace pi

#endif // end of __UTILS_GPS_H__
