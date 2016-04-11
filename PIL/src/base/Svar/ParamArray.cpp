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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>

#include "base/debug/debug_config.h"
#include "base/utils/utils.h"

#include "ParamArray.h"


using namespace std;

namespace pi {

#define PARAM_ARRAY_ITEM_MAXLEN     4096


////////////////////////////////////////////////////////////////////////////////
/// Global variables
////////////////////////////////////////////////////////////////////////////////

static CParamArray *g_pa = NULL;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void CVariant::set(int v)
{
    if( t == VT_STRING ) delete d.sVal;

    t = VT_INT;
    d.iVal = v;
}

void CVariant::set(ri64 v)
{
    if( t == VT_STRING ) delete d.sVal;

    t = VT_INT64;
    d.liVal = v;
}

void CVariant::set(float  v)
{
    if( t == VT_STRING ) delete d.sVal;

    t = VT_FLOAT;
    d.fVal = v;
}

void CVariant::set(double v)
{
    if( t == VT_STRING ) delete d.sVal;

    t = VT_DOUBLE;
    d.dVal = v;
}

void CVariant::set(char *v)
{
    int     n;

    if( t == VT_STRING ) delete d.sVal;

    t = VT_STRING;
    n = strlen(v);
    d.sVal = new char[n+1];
    strcpy(d.sVal, v);
}

void CVariant::set(const char *v)
{
    set((char*) v);
}

void CVariant::set(void *v)
{
    if( t == VT_STRING ) delete d.sVal;

    t = VT_POINTER;
    d.pVal = v;
}

int CVariant::to_i(void)
{
    switch(t) {
    case VT_INT:
        return d.iVal;
        break;

    case VT_FLOAT:
        return (int)(d.fVal);
        break;

    case VT_DOUBLE:
        return (int)(d.dVal);
        break;

    case VT_STRING:
        return atoi(d.sVal);
        break;

    case VT_POINTER:
        return 0;
        break;
    }

    return 0;
}

ri64 CVariant::to_li(void)
{
    switch(t) {
    case VT_INT:
        return d.iVal;
        break;

    case VT_INT64:
        return d.liVal;
        break;

    case VT_FLOAT:
        return (ri64)(d.fVal);
        break;

    case VT_DOUBLE:
        return (ri64)(d.dVal);
        break;

    case VT_STRING:
        return atoi(d.sVal);
        break;

    case VT_POINTER:
        return 0;
        break;
    }

    return 0;
}

float CVariant::to_f(void)
{
    switch(t) {
    case VT_INT:
        return (float)(d.iVal);
        break;

    case VT_FLOAT:
        return d.fVal;
        break;

    case VT_DOUBLE:
        return (float)(d.dVal);
        break;

    case VT_STRING:
        return (float)(atof(d.sVal));
        break;

    case VT_POINTER:
        return 0;
        break;
    }

    return 0;
}

double CVariant::to_d(void)
{
    switch(t) {
    case VT_INT:
        return (double)(d.iVal);
        break;

    case VT_FLOAT:
        return (double)(d.fVal);
        break;

    case VT_DOUBLE:
        return d.dVal;
        break;

    case VT_STRING:
        return (double)(atof(d.sVal));
        break;

    case VT_POINTER:
        return 0;
        break;
    }

    return 0;
}

char *CVariant::to_s(char *buf)
{
    buf[0] = 0;

    switch(t) {
    case VT_INT:
        sprintf(buf, "%d", d.iVal);
        break;

    case VT_FLOAT:
        sprintf(buf, "%g", d.fVal);
        break;

    case VT_DOUBLE:
        sprintf(buf, "%g", d.dVal);
        break;

    case VT_STRING:
        return d.sVal;
        break;

    case VT_POINTER:
        // FIXME: change to ru64
        sprintf(buf, "%llx", (ru64) d.pVal);
        break;
    }

    return buf;
}

char *CVariant::to_s(void)
{
    // FIXME: use a fixed length
    if( buf == NULL ) {
        buf = new char[PARAM_ARRAY_ITEM_MAXLEN];
    }

    return to_s(buf);
}

void *CVariant::to_p(void)
{
    if( t == VT_POINTER ) {
        return d.pVal;
    } else
        return NULL;
}

int CVariant::size(void)
{
    switch(t) {
    case VT_INT:
        return sizeof(int);
        break;

    case VT_FLOAT:
        return sizeof(float);
        break;

    case VT_DOUBLE:
        return sizeof(double);
        break;

    case VT_STRING:
        return strlen(d.sVal)+1;
        break;

    case VT_POINTER:
        return sizeof(void *);
        break;
    }

    return 0;
}

int  CVariant::stream_len(void)
{
    int     l1, l2;

    l1   = sizeof(CVariantType);
    l2   = size();
    return (l1+l2);
}

/**
 *  to stream data
 *
 *  Parameters:
 *      \param[out]     len         data length
 *      \param[out]     buf         data buff
 *  Return Value:
 *      0               successfule
 */
int  CVariant::to_stream(int *len, ru8 *buf)
{
    int     i, l1, l2;

    l1   = sizeof(CVariantType);
    l2   = size();
    *len = l1 + l2;

    // copy data type field
    i = 0;
    memcpy(buf+i, &t, sizeof(int));
    i += l1;

    // copy data
    switch(t) {
    case VT_INT:
        memcpy(buf+i, &(d.iVal), sizeof(int));
        break;

    case VT_FLOAT:
        memcpy(buf+i, &(d.fVal), sizeof(float));
        break;

    case VT_DOUBLE:
        memcpy(buf+i, &(d.dVal), sizeof(double));
        break;

    case VT_STRING:
        memcpy(buf+i, d.sVal, l2);
        break;

    case VT_POINTER:
        memcpy(buf+i, &(d.pVal), l2);
        break;
    }

    return 0;
}

int  CVariant::from_stream(int len, ru8 *buf)
{
    int     i, l1, l2;

    l1   = sizeof(CVariantType);
    l2   = len - l1;

    // copy data type field
    i = 0;
    memcpy(&t, buf+i, sizeof(int));
    i += l1;

    // copy data
    switch(t) {
    case VT_INT:
        memcpy(&(d.iVal), buf+i, sizeof(int));
        break;

    case VT_FLOAT:
        memcpy(&(d.fVal), buf+i, sizeof(float));
        break;

    case VT_DOUBLE:
        memcpy(&(d.dVal), buf+i, sizeof(double));
        break;

    case VT_STRING:
        if( d.sVal != NULL ) delete d.sVal;
        d.sVal = new char[l2+1];
        memcpy(d.sVal, buf+i, l2);
        break;

    case VT_POINTER:
        memcpy(&(d.pVal), buf+i, l2);
        break;
    }

    return 0;
}

CVariant& CVariant::operator =(const CVariant &o)
{
    if( this == &o ) return *this;

    if( o.t == VT_STRING ) {
        int l;
        l = strlen(o.d.sVal) + 1;

        if( t == VT_STRING ) delete d.sVal;
        d.sVal = new char[l];

        strcpy(d.sVal, o.d.sVal);
        t = o.t;
    } else {
        t = o.t;
        d = o.d;
    }

    return *this;
}

void CVariant::_init(void)
{
    t = VT_INT;
    memset(&d, 0, sizeof(CVariantUnion));

    buf = NULL;
}

void CVariant::_release(void)
{
    if( t == VT_STRING ) {
        delete d.sVal;
        t = VT_INT;
        d.iVal = 0;
    }

    if( buf != NULL ) {
        delete buf;
        buf = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int CParamArray::load(const string &f)
{
    FILE            *fp=NULL;
    char            *buf;
    StringArray     sa;
    string          _b;

    string          _n;
    CVariant        *_v;

    // open parameter array file
    fp = fopen(f.c_str(), "rt");
    if( fp == NULL ) {
        dbg_pe("Failed to open file: %s\n", f.c_str());
        return -1;
    }

    // clear old data
    _release(1);

    // alloc string buffer for reading file
    buf = (char *) malloc(PARAM_ARRAY_ITEM_MAXLEN);

    while(!feof(fp)) {
        // read a line
        if( NULL == fgets(buf, PARAM_ARRAY_ITEM_MAXLEN, fp) )
            break;

        // remove blank & CR
        _b = trim(buf);

        if( _b.size() < 1 )
            continue;

        // skip comment
        if( _b[0] == '#' || _b[0] == ':' )
            continue;

        // FIXME: if current line have more than one of "="
        //        then it will be failed
        sa = split_text(_b, "=");

        if( sa.size() >= 2 ) {
            _n = trim(sa[0]);

            _v = new CVariant;
            _v->set(trim(sa[1]).c_str());

            vm.insert(make_pair(_n, _v));
        }
    }

    // free file & buf
    free(buf);
    fclose(fp);

    // parse items
    parse();

    return 0;
}

int CParamArray::save(const string &f)
{
    return 0;
}

int CParamArray::i(const string &n, int &v)
{
    VariantMap::iterator    it;

    it = vm.find(n);
    if( it != vm.end() ) {
        v = it->second->to_i();
        return 0;
    }

    //dbg_pe("cann't get value [%s]\n", n.c_str());
    return -1;
}

int CParamArray::i(const string &n)
{
    VariantMap::iterator    it;
    int                     v;

    it = vm.find(n);
    if( it != vm.end() ) {
        v = it->second->to_i();
        return v;
    }

    //dbg_pe("cann't get value [%s]\n", n.c_str());
    return -1;
}



int CParamArray::f(const string &n, float &v)
{
    VariantMap::iterator    it;

    it = vm.find(n);
    if( it != vm.end() ) {
        v = it->second->to_f();
        return 0;
    }

    //dbg_pe("cann't get value [%s]\n", n.c_str());
    return -1;
}

float CParamArray::f(const string &n)
{
    VariantMap::iterator    it;
    int                     v;

    it = vm.find(n);
    if( it != vm.end() ) {
        v = it->second->to_f();
        return 0;
    }

    //dbg_pe("cann't get value [%s]\n", n.c_str());
    return 0.0;
}


int CParamArray::d(const string &n, double &v)
{
    VariantMap::iterator    it;

    it = vm.find(n);
    if( it != vm.end() ) {
        v = it->second->to_d();
        return 0;
    }

    //dbg_pe("cann't get value [%s]\n", n.c_str());
    return -1;
}

double CParamArray::d(const string &n)
{
    VariantMap::iterator    it;
    int                     v;

    it = vm.find(n);
    if( it != vm.end() ) {
        v = it->second->to_d();
        return 0;
    }

    //dbg_pe("cann't get value [%s]\n", n.c_str());
    return 0.0;
}

int CParamArray::s(const string &n, string &v)
{
    VariantMap::iterator    it;

    int     l;
    string  s;

    it = vm.find(n);
    if( it != vm.end() ) {
        l = it->second->size();
        s = it->second->to_s();

        if( s[0] == '\"' && s[l-1] == '\"' )  {
            v = s.substr(1, l-2);
        } else {
            v = s;
        }

        return 0;
    }

    //dbg_pe("cann't get value [%s]\n", n.c_str());
    return -1;
}

string CParamArray::s(const string &n)
{
    VariantMap::iterator    it;

    int     l;
    string  s, s2;

    it = vm.find(n);
    if( it != vm.end() ) {
        l = it->second->size();
        s = it->second->to_s();

        if( s[0] == '\"' && s[l-1] == '\"' )  {
            s2 = s.substr(1, l-2);
        } else {
            s2 = s;
        }

        return s2;
    }

    //dbg_pe("cann't get value [%s]\n", n.c_str());
    return "";
}

int CParamArray::p(const string &n, void **v)
{
    VariantMap::iterator    it;

    it = vm.find(n);
    if( it != vm.end() ) {
        *v = it->second->to_p();
        return 0;
    }

    //dbg_pe("cann't get value [%s]\n", n.c_str());
    return -1;
}

void *CParamArray::p(const string &n)
{
    VariantMap::iterator    it;

    it = vm.find(n);
    if( it != vm.end() ) {
        return it->second->to_p();
    }

    //dbg_pe("cann't get value [%s]\n", n.c_str());
    return NULL;
}


int  CParamArray::key_exist(const string &n)
{
    VariantMap::iterator    it;

    it = vm.find(n);
    if( it != vm.end() ) {
        return 1;
    }

    return 0;
}

int  CParamArray::set_i(const string &n, int v)
{
    VariantMap::iterator    it;

    string      _n;
    CVariant    *_v;

    it = vm.find(n);
    if( it != vm.end() ) {
        it->second->set(v);
        return 0;
    }

    // insert new item
    _n = n;
    _v = new CVariant();
    _v->set(v);
    vm.insert(make_pair(_n, _v));

    return 0;
}

int  CParamArray::set_f(const string &n, float v)
{
    VariantMap::iterator    it;

    string      _n;
    CVariant    *_v;

    it = vm.find(n);
    if( it != vm.end() ) {
        it->second->set(v);
        return 0;
    }

    // insert new item
    _n = n;
    _v = new CVariant();
    _v->set(v);
    vm.insert(make_pair(_n, _v));

    return 0;
}

int  CParamArray::set_d(const string &n, double v)
{
    VariantMap::iterator    it;

    string      _n;
    CVariant    *_v;

    it = vm.find(n);
    if( it != vm.end() ) {
        it->second->set(v);
        return 0;
    }

    // insert new item
    _n = n;
    _v = new CVariant();
    _v->set(v);
    vm.insert(make_pair(_n, _v));

    return 0;
}

int  CParamArray::set_s(const string &n, string v)
{
    VariantMap::iterator    it;

    string      _n;
    CVariant    *_v;

    it = vm.find(n);
    if( it != vm.end() ) {
        it->second->set(v.c_str());
        return 0;
    }

    // insert new item
    _n = n;
    _v = new CVariant();
    _v->set(v.c_str());
    vm.insert(make_pair(_n, _v));

    return 0;
}

int  CParamArray::set_p(const string &n, void *v)
{
    VariantMap::iterator    it;

    string      _n;
    CVariant    *_v;

    it = vm.find(n);
    if( it != vm.end() ) {
        it->second->set(v);
        return 0;
    }

    // insert new item
    _n = n;
    _v = new CVariant();
    _v->set(v);
    vm.insert(make_pair(_n, _v));

    return 0;
}

int  CParamArray::setget_i(const string &n, int v)
{
    VariantMap::iterator    it;

    string      _n;
    CVariant    *_v;

    it = vm.find(n);
    if( it != vm.end() ) {
        v = it->second->to_i();
        return 0;
    }

    // insert new item
    _n = n;
    _v = new CVariant();
    _v->set(v);
    vm.insert(make_pair(_n, _v));

    return 0;
}

int  CParamArray::setget_f(const string &n, float v)
{
    VariantMap::iterator    it;

    string      _n;
    CVariant    *_v;

    it = vm.find(n);
    if( it != vm.end() ) {
        v = it->second->to_f();
        return 0;
    }

    // insert new item
    _n = n;
    _v = new CVariant();
    _v->set(v);
    vm.insert(make_pair(_n, _v));

    return 0;
}

int  CParamArray::setget_d(const string &n, double v)
{
    VariantMap::iterator    it;

    string      _n;
    CVariant    *_v;

    it = vm.find(n);
    if( it != vm.end() ) {
        v = it->second->to_d();
        return 0;
    }

    // insert new item
    _n = n;
    _v = new CVariant();
    _v->set(v);
    vm.insert(make_pair(_n, _v));

    return 0;
}

int  CParamArray::setget_s(const string &n, string &v)
{
    VariantMap::iterator    it;

    string      _n;
    CVariant    *_v;

    it = vm.find(n);
    if( it != vm.end() ) {
        v = it->second->to_s();
        return 0;
    }

    // insert new item
    _n = n;
    _v = new CVariant();
    _v->set(v.c_str());
    vm.insert(make_pair(_n, _v));

    return 0;
}

int  CParamArray::setget_p(const string &n, void **v)
{
    VariantMap::iterator    it;

    string      _n;
    CVariant    *_v;

    it = vm.find(n);
    if( it != vm.end() ) {
        *v = it->second->to_p();
        return 0;
    }

    // insert new item
    _n = n;
    _v = new CVariant();
    _v->set(*v);
    vm.insert(make_pair(_n, _v));

    return 0;
}

int CParamArray::set_args(int argc, char *argv[])
{
    int     i;
    char    *p;
    string  v;

    for(i=1; i<argc; i++) {
        if( argv[i][0] == '-' ) {
            p = argv[i]+1;
            v = argv[++i];
            set_s(p, v);
        }
    }

    return 0;
}

int  CParamArray::stream_len(void)
{
    int         n;
    int         l1, l2, l_all;

    VariantMap::iterator    it;

    // get item number
    n = vm.size();

    // stream length + item number + each item's length
    l_all = sizeof(int) + sizeof(int) + sizeof(int)*n;

    // get each item's length
    for(it=vm.begin(); it!=vm.end(); it++) {
        l1 = strlen(it->first.c_str()) + 1;
        l2 = it->second->stream_len();

        l_all += sizeof(int)+l1+l2;
    }

    return l_all;
}

/**
 *  To stream data buf
 *
 *  Parameters:
 *      \param[out]     len     data length
 *      \param[out]     buf     data stream
 *  Return Value:
 *      0               success
 */
int  CParamArray::to_stream(int *len, ru8 *buf)
{
    int         n, i;
    int         l1, l2, l_item, p;
    int         *arr_len;

    VariantMap::iterator    it;


    // get item number
    n = vm.size();

    // alloc length array
    arr_len = new int[n];

    // get each item's length
    for(i=0, it=vm.begin(); it!=vm.end(); i++, it++) {
        l1 = strlen(it->first.c_str()) + 1;
        l2 = it->second->stream_len();

        arr_len[i] = sizeof(int) + l1 + l2;
    }

    // generate stream
    p = 0;

    // stream total length
    p += sizeof(int);

    // item number
    memcpy(buf+p, &n, sizeof(int));
    p += sizeof(int);

    // each item's length
    for(i=0; i<n; i++) {
        memcpy(buf+p, &(arr_len[i]), sizeof(int));
        p += sizeof(int);
    }

    // for each item
    for(it=vm.begin(); it!=vm.end(); it++) {
        // name length
        l1 = strlen(it->first.c_str()) + 1;
        memcpy(buf+p, &l1, sizeof(int));
        p += sizeof(int);

        // name
        memcpy(buf+p, it->first.c_str(), l1);
        p += l1;

        // value
        it->second->to_stream(&l_item, buf+p);
        p += l_item;
    }

    // set total length
    memcpy(buf, &p, sizeof(int));

    // return length
    *len = p;

    delete arr_len;

    return 0;
}

int  CParamArray::from_stream(int len, ru8 *buf)
{
    int         n, i;
    int         l1, l2, p;
    char        *str_name;
    int         pi;
    int         *arr_len;
    CVariant    *v;

    // name string
    str_name = new char[PARAM_ARRAY_ITEM_MAXLEN];

    // clear old data
    clear();

    // stream position index
    p = 0;

    // stream total length
    p += sizeof(int);

    // get item number
    memcpy(&n, buf+p, sizeof(int));
    p += sizeof(int);

    // alloc length array
    arr_len = new int[n];

    // get each item's length
    for(i=0; i<n; i++) {
        memcpy(&pi, buf+p, sizeof(int));
        arr_len[i] = pi;
        p += sizeof(int);
    }

    // for each item
    for(i=0; i<n; i++) {
        // name length
        memcpy(&pi, buf+p, sizeof(int));
        l1 = pi;
        p += sizeof(int);

        // name
        memcpy(str_name, buf+p, l1);
        p += l1;

        // value
        l2 = arr_len[i] - sizeof(int) - l1;
        v = new CVariant();
        v->from_stream(l2, buf+p);
        p += l2;

        // add name/value to array
        vm.insert(make_pair(str_name, v));
    }

    // parse items
    parse();

    // free temp variables
    delete str_name;
    delete arr_len;

    return 0;
}

int CParamArray::parse(void)
{
    return 0;
}

int CParamArray::push(void)
{
    int     buf_len;
    ru8   *buf;

    // to steam
    buf_len = stream_len();
    buf = new ru8[buf_len];
    to_stream(&buf_len, buf);

    // push to stack
    sa.push_back(buf);

    return 0;
}

int CParamArray::pop(void)
{
    int   buf_len;
    ru8   *buf;

    if( sa.size() > 0 ) {
        buf = sa.back();

        memcpy(&buf_len, buf, sizeof(int));
        from_stream(buf_len, buf);
        delete buf;

        sa.pop_back();
    }

    return 0;
}

void CParamArray::print(void)
{
    int     n_l, max_l, max_l_def;
    char    str_fmt[300];

    VariantMap::iterator    it;


    // determin max name length
    max_l_def = 10;
    max_l     = max_l_def;
    for(it=vm.begin(); it!=vm.end(); it++) {
        n_l = strlen(it->first.c_str());
        if( n_l > max_l ) max_l = n_l;
    }

    if( max_l > max_l_def ) max_l += 2;

    // generate format string
    sprintf(str_fmt, "%%%ds = %%s\n", max_l+2);

    // print
    fmt::printf("--------------------");
    fmt::print_colored(fmt::GREEN, " Parameters ");
    fmt::printf("-------------------------\n");

    for(it=vm.begin(); it!=vm.end(); it++) {
        printf(str_fmt, it->first.c_str(), it->second->to_s());
    }

    fmt::printf("---------------------------------------------------------\n\n");
}

void CParamArray::clear(void)
{
    _release(1);
}

CParamArray& CParamArray::operator =(const CParamArray &o)
{
    VariantMap::iterator        it;
    VariantMap                  *vm_n;

    // check if self-assignment
    if( this == &o ) return *this;

    // clear old contents
    _release(1);

    vm_n = const_cast<VariantMap *>( &(o.vm) );

    for(it=vm_n->begin(); it!=vm_n->end(); it++) {
        vm.insert(make_pair(it->first, it->second));
    }

    // parse some field
    parse();

    return *this;
}

int CParamArray::_init(void)
{
    // clear variant map
    vm.clear();

    // clear stack array
    sa.clear();

    return 0;
}

int CParamArray::_release(int s)
{
    int     i;

    VariantMap::iterator    it;
    CVariant                *v;

    for(it=vm.begin(); it!=vm.end(); it++) {
        v = it->second;
        delete v;

        it->second = NULL;
    }

    vm.clear();

    // clear stack objects
    if( s == 1 ) {
        for(i=0; i<sa.size(); i++) delete sa[i];
        sa.clear();
    }

    return 0;
}



CParamArray* pa_create()
{
    // delete old obj
    if( g_pa != NULL ) {
        delete g_pa;
    }

    // create new obj
    g_pa = new CParamArray;

    return g_pa;
}

void pa_free(void)
{
    if( g_pa != NULL ) {
        delete g_pa;
    }

    g_pa = NULL;
}

CParamArray* pa_get(void)
{
    if( g_pa == NULL ) {
        g_pa = new CParamArray;
    }

    return g_pa;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int CArgs::_init(void)
{
    na.clear();

    return 0;
}

int CArgs::_release(void)
{
    na.clear();

    return 0;
}

int CArgs::set_args(int argc, char *argv[])
{
    int     i;

    na.clear();
    
    for(i=0; i<argc; i++) {
        na.push_back(argv[i]);
    }

    return 0;
}

int CArgs::save(string fname)
{
    string      fn;
    FILE        *fp;
    int         i;
    tm          *now;
    time_t      t;
    char        str_time[200];


    fn = fname + "_args.txt";
    fp = fopen(fn.c_str(), "a+");
    if( fp == NULL ) {
        dbg_pe("Failed to open output file: %s\n", fn.c_str());
        return -1;
    }

    // get current time
    time(&t);
    now = localtime(&t);
    strftime(str_time, 200, "%Y-%m-%d %H:%M:%S", now);

    fprintf(fp, "--------------- %s ---------------\n", str_time);

    for(i=0; i<na.size(); i++)
        fprintf(fp, "%s ", na[i].c_str());

    fprintf(fp, "\n\n");

    fclose(fp);

    return 0;
}


} // end of namespace pi
