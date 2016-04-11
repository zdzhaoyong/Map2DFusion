
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <iomanip>

#include "utils_str.h"


using namespace std;

namespace pi {


////////////////////////////////////////////////////////////////////////////////
/// string functions
////////////////////////////////////////////////////////////////////////////////

// split given string by delims
StringArray split_text_cpp(const string &intext, const string &delims)
{
    StringArray         r;
    string::size_type   begIdx, endIdx;
    string              s;

    begIdx = intext.find_first_not_of(delims);

    while(begIdx != string::npos) {
        // search end of the word
        endIdx = intext.find_first_of(delims, begIdx);
        if( endIdx == string::npos )
            endIdx = intext.length();

        // get the sub string
        s = intext.substr(begIdx, endIdx-begIdx);
        r.push_back(s);

        // find next begin position
        begIdx = intext.find_first_not_of(delims, endIdx);
    }

    return r;
}

inline int char_is_delims(char c, char d)
{
    if( c == d )
        return 1;
    else
        return 0;
}

inline int char_is_newline(char c)
{
    if( c == 0x0d || c == 0x0a )
        return 1;
    else
        return 0;
}

StringArray split_text(const string &intext, const string &delims)
{
    StringArray         r;

    int                 st;
    int                 n, nd, i, j, k, dd, ns;
    char                *buf;
    const char          *pb, *pd;


    n = intext.size();
    nd = delims.size();

    pb = intext.c_str();
    pd = delims.c_str();

    buf = new char[n+10];

    st = 0;
    i = 0;
    k = 0;
    ns = 0;
    buf[0] = 0;

    while( i<n ) {
        for(dd = 0, j=0; j<nd; j++) dd += char_is_delims(pb[i], pd[j]);

        if( dd > 0 ) {
            buf[k] = 0;
            r.push_back(buf);

            k = 0;
            st = 1;
        } else {
            buf[k++] = pb[i];
            st = 0;
        }

        i++;
    }

    // process last character
    if( st == 0 ) {
        buf[k] = 0;
        r.push_back(buf);
    } else {
        buf[0] = 0;
        r.push_back(buf);
    }

    delete [] buf;

    return r;
}

///
/// \brief split_line
/// \param intext - input string
/// \return
///
/// FIXME: blank line is not supported!
///
StringArray split_line(const std::string &intext)
{
    StringArray         r;

    int                 st;
    int                 n, i, k, dd;
    char                *buf;
    const char          *pb;

    n = intext.size();
    pb = intext.c_str();

    buf = new char[n+10];

    st = 0;
    i = 0;
    k = 0;
    buf[0] = 0;

    while( i<n ) {
        dd = char_is_newline(pb[i]);

        if( dd > 0 ) {
            if( k > 0 ) {
                buf[k] = 0;
                r.push_back(buf);

                k = 0;
                st = 1;
            }
        } else {
            buf[k++] = pb[i];
            st = 0;
        }

        i++;
    }

    // process last character
    if( st == 0 ) {
        buf[k] = 0;
        r.push_back(buf);
    }

    delete [] buf;

    return r;
}

// string trim functions
string ltrim(const string &s)
{
    string              delims = " \t\n\r",
                        r;
    string::size_type   i;

    i = s.find_first_not_of(delims);
    if( i == string::npos )
        r = "";
    else
        r = s.substr(i, s.size() - i);

    return r;
}


string rtrim(const string &s)
{
    string              delims = " \t\n\r",
                        r;
    string::size_type   i;

    i = s.find_last_not_of(delims);
    if( i == string::npos )
        r = "";
    else
        r = s.substr(0, i+1);

    return r;
}


string trim(const string &s)
{
    string              delims = " \t\n\r",
                        r;
    string::size_type   i, j;

    i = s.find_first_not_of(delims);
    j = s.find_last_not_of(delims);

    if( i == string::npos ) {
        r = "";
        return r;
    }

    if( j == string::npos ) {
        r = "";
        return r;
    }

    r = s.substr(i, j-i+1);
    return r;
}

string str_tolower(string &s)
{
    for(int i=0; i < s.size(); i++) {
        s[i] = tolower(s[i]);
    }

    return s;
}

string str_toupper(string &s)
{
    for(int i=0; i < s.size(); i++) {
        s[i] = toupper(s[i]);
    }

    return s;
}



int str_to_int(const string &s)
{
    return atoi(s.c_str());
}

float str_to_float(const string &s)
{
    return atof(s.c_str());
}

double str_to_double(const string &s)
{
    return atof(s.c_str());
}

std::string itos(const int &i)
{
    ostringstream ost;
    ost<<i;
    return ost.str();
}

std::string ftos(const float &i, int precision)
{
    ostringstream ost;
    ost<<setiosflags(ios::fixed)<<setprecision(precision)<<i;
    return ost.str();
}

std::string dtos(const double &i, int precision)
{
    ostringstream ost;
    ost<<setiosflags(ios::fixed)<<setprecision(precision)<<i;
    return ost.str();
}

} // end of namespace pi
