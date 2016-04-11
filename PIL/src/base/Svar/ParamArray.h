/******************************************************************************

  Robot Toolkit ++ (RTK++)

  Copyright (c) 2007-2013 Shuhui Bu <bushuhui@nwpu.edu.cn>
  http://www.adv-ci.com

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

#ifndef __RTK_PARAMARRAY_H__
#define __RTK_PARAMARRAY_H__

#include <string>
#include <vector>
#include <map>

#include "base/types/types.h"

namespace pi {

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum CVariantType
{
    VT_INT,                 // integer value
    VT_INT64,               // 64-bit integer
    VT_FLOAT,               // float value
    VT_DOUBLE,              // double value
    VT_STRING,              // string value
    VT_POINTER,             // pointer value
    VT_BIN,                 // binary value
};

union CVariantUnion
{
    int     iVal;           // integer value
    ri64    liVal;          // 64-bit integer
    float   fVal;           // float value
    double  dVal;           // double value
    char    *sVal;          // string value
    void    *pVal;          // pointer value
    void    *bVal;          // binary value
};

class CVariant
{
public:
    // set value
    void set(int        v);
    void set(ri64       v);
    void set(float      v);
    void set(double     v);
    void set(char       *v);
    void set(const char *v);
    void set(void       *v);

    // get/convert type
    int     to_i(void);
    ri64    to_li(void);
    float   to_f(void);
    double  to_d(void);
    char    *to_s(char *buf);
    char    *to_s(void);
    void    *to_p(void);

    // get length
    int     size(void);

    // to/from stream data
    int  stream_len(void);
    int  to_stream(int *len, ru8 *buf);
    int  from_stream(int len, ru8 *buf);

    // assignment operator
    CVariant& operator =(const CVariant &o);

public:
    CVariant()          { _init(); }
    virtual ~CVariant() { _release(); }

private:
    void _init(void);
    void _release(void);

protected:
    CVariantType    t;
    CVariantUnion   d;

    char            *buf;
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef std::map<std::string, CVariant*>    VariantMap;

class CParamArray
{
public:
    CParamArray() { _init(); }
    virtual ~CParamArray() { _release(1); }

    // get value (if exist)
    int i(const std::string &n, int &v);
    int f(const std::string &n, float &v);
    int d(const std::string &n, double &v);
    int s(const std::string &n, std::string &v);
    int p(const std::string &n, void **v);

    // get value
    int         i(const std::string &n);
    float       f(const std::string &n);
    double      d(const std::string &n);
    std::string s(const std::string &n);
    void*       p(const std::string &n);

    // get key exist
    int  key_exist(const std::string &n);

    // set value
    int  set_i(const std::string &n, int v);
    int  set_f(const std::string &n, float v);
    int  set_d(const std::string &n, double v);
    int  set_s(const std::string &n, std::string v);
    int  set_p(const std::string &n, void *v);

    // if not exist, set value
    // if exist, get value
    int  setget_i(const std::string &n, int v);
    int  setget_f(const std::string &n, float v);
    int  setget_d(const std::string &n, double v);
    int  setget_s(const std::string &n, std::string &v);
    int  setget_p(const std::string &n, void **v);

    // parse arguments
    int  set_args(int argc, char *argv[]);

    // load / save
    virtual int load(const std::string &f);
    virtual int save(const std::string &f);

    // to/from stream data
    int stream_len(void);
    int to_stream(int *len, ru8 *buf);
    virtual int from_stream(int len, ru8 *buf);

    // parse item
    virtual int parse(void);

    // push/pop settings
    int push(void);
    int pop(void);

    // print parameters
    void print(void);

    // clear all items
    void clear(void);

    // assignment operator
    CParamArray& operator =(const CParamArray &o);

protected:
    int _init(void);
    int _release(int s);

protected:
    VariantMap                  vm;     // variant map
    std::vector<ru8*>           sa;     // stack array
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CParamArray* pa_create(void);
void         pa_free(void);
CParamArray* pa_get(void);



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CArgs
{
public:
    CArgs() { _init(); }
    virtual ~CArgs() { _release(); }

    // set arguments
    int set_args(int argc, char *argv[]);

    // save arguments to file
    int save(std::string fname);

protected:
    std::vector<std::string>    na;     // argument array

    int _init(void);
    int _release(void);
};


} // end of namespace pi

#endif // end of __RTK_PARAMARRAY_H__

