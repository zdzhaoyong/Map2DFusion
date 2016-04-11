
#ifndef __UTILS_STR_H__
#define __UTILS_STR_H__

#include <string>
#include <vector>
#include <ostream>
#include <sstream>


namespace pi {


////////////////////////////////////////////////////////////////////////////////
/// string array type
////////////////////////////////////////////////////////////////////////////////
typedef std::vector<std::string> StringArray;


////////////////////////////////////////////////////////////////////////////////
/// string functions
////////////////////////////////////////////////////////////////////////////////

///
/// \brief split_text
///     split given string by delims
///
/// \param intext - input text
/// \param delims - delims
/// \return string array
///
StringArray split_text(const std::string &intext, const std::string &delims);

///
/// \brief split_line
///     split given string by line
///
/// \param intext - input text
/// \return string array
///
/// FIXME: blank line is not supported!
///
StringArray split_line(const std::string &intext);

// string trim functions
std::string ltrim(const std::string &s);
std::string rtrim(const std::string &s);
std::string trim(const std::string &s);

// string lower & upper
std::string str_tolower(std::string &s);
std::string str_toupper(std::string &s);


int     str_to_int(const std::string &s);
float   str_to_float(const std::string &s);
double  str_to_double(const std::string &s);

std::string itos(const int &i);
std::string ftos(const float &f, int precision=6);
std::string dtos(const double &d, int precision=6);

template <typename T>
std::string to_str(const T& i)
{
    std::ostringstream ost;

    ost<<i;
    return ost.str();
}

} // end of namespace pi

#endif // end of __UTILS_STR_H__
