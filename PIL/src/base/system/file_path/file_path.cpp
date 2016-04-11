
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <sys/stat.h>

#include <algorithm>

#include "base/debug/debug_config.h"
#include "file_path.h"

using namespace std;

namespace pi {


int path_exist(const char *p)
{
    struct stat     st;
    int             ret;

    ret = stat(p, &st);
    return ret==0;
}

int path_mkdir(const char *p)
{
    char            cmd[2048];
    int             ret;

    // FIXME: only support UNIX system
    //ret = mknod(p, S_IFDIR | 0775, 0);
    sprintf(cmd, "mkdir -p '%s'", p);

    ret = system(cmd);
    if( ret != 0 ) ret = -1;

    return ret;
}

int path_delfile(const std::string &p)
{
    char            cmd[2048];
    int             ret;

    // FIXME: only support UNIX system
    sprintf(cmd, "rm -rf '%s'", p.c_str());

    ret = system(cmd);
    if( ret != 0 ) ret = -1;

    return ret;
}


int path_lsdir(const string &dir_name, StringArray &dl)
{
    DIR             *dir;
    struct dirent   *dp;

    // open directory
    dir = opendir(dir_name.c_str());
    if( dir == NULL ) {
        dbg_pe("Failed to open dir: %s\n", dir_name.c_str());
        return -1;
    }

    // get each items
    dl.clear();
    for(dp=readdir(dir); dp!=NULL; dp=readdir(dir)) {
        // skip .
        if( strlen(dp->d_name) == 1 && dp->d_name[0] == '.' )
            continue;

        // skip ..
        if( strlen(dp->d_name) == 2 && dp->d_name[0] == '.' && dp->d_name[1] == '.' )
            continue;

        // add to list
        dl.push_back(dp->d_name);
    }

    closedir(dir);

    // sort all file name
    std::sort(dl.begin(), dl.end());
}

int path_isdir(const std::string &p)
{
    struct stat     st;
    int             ret;

    ret = stat(p.c_str(), &st);
    if( ret == -1 ) {
        dbg_pe("Failed at stat! (%s)", p.c_str());
        return 0;
    }

    if ( (st.st_mode & S_IFMT) == S_IFDIR )
        return 1;
    else
        return 0;
}

int path_isfile(const std::string &p)
{
    struct stat     st;
    int             ret;

    ret = stat(p.c_str(), &st);
    if( ret == -1 ) {
        dbg_pe("Failed at stat! (%s)", p.c_str());
        return 0;
    }

    if ( (st.st_mode & S_IFMT) == S_IFREG )
        return 1;
    else
        return 0;
}




StringArray path_split(const string &fname)
{
    size_t      found = -1;
    StringArray r;

    r.clear();

    /* find / or \ */
    found = fname.find_last_of("/\\");

    if( found == string::npos ) {
        r.push_back("");
        r.push_back(fname);
        return r;
    }

    // folder
    r.push_back(fname.substr(0, found));
    // file
    r.push_back(fname.substr(found+1));

    return r;
}

StringArray path_splitext(const string &fname)
{
    size_t      found;
    StringArray r, ff;

    r.clear();

    // get folder & file name
    ff = path_split(fname);

    // find .
    found = ff[1].find_last_of(".");
    if( found == string::npos ) {
        r.push_back(path_join(ff[0],ff[1]));
        r.push_back("");
        return r;
    }

    // filename
    r.push_back(path_join(ff[0],ff[1].substr(0, found)));
    // extname
    r.push_back(ff[1].substr(found));

    return r;
}

std::string path_extname(std::string &fname)
{
    size_t      found;
    string      ext_name = "";


    // find .
    found = fname.find_last_of(".");
    if( found != string::npos ) {
        ext_name = fname.substr(found);
    }

    return ext_name;
}

StringArray path_name_ext(const std::string &fname)
{
    StringArray path_file=path_split(fname);
    StringArray name_ext=path_splitext(path_file[1]);
    path_file[1]=name_ext[0];
    path_file.push_back(name_ext[1]);
    return path_file;
}

std::string path_join(const std::string &p1, const std::string &p2)
{
    string      p;
    int         l;

    p = p1;

    l = p.size();
    if( p[l-1] == '/' || p[l-1] == '\\' )
        p = p.substr(0, l-1);

    p = p + "/" + p2;
    return p;
}

std::string path_join(const std::string &p1, const std::string &p2, const std::string &p3)
{
    string      p;

    p = path_join(p1, p2);
    return path_join(p, p3);
}


std::string path_join(const StringArray &p)
{
    int     i, l;
    string  p_all;

    p_all = "";
    for(i=0; i<p.size(); i++) {
        l = p_all.size();
        if( l>0 ) {
            if( p_all[l-1] == '/' || p_all[l-1] == '\\' )
                p_all = p_all.substr(0, l-1);
        }

        p_all = p_all + "/" + p[i];
    }

    return p_all;
}

}//end of namespace
