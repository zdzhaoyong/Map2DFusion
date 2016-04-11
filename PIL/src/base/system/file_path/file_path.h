#ifndef FILE_PATH_H
#define FILE_PATH_H

#include <string>
#include <vector>

#include "base/utils/utils_str.h"

namespace pi {


////////////////////////////////////////////////////////////////////////////////
/// file & path functions
////////////////////////////////////////////////////////////////////////////////
int path_exist(const char *p);
int path_mkdir(const char *p);

int path_delfile(const std::string &p);

int path_lsdir(const std::string &dir_name, StringArray &dl);
int path_isdir(const std::string &p);
int path_isfile(const std::string &p);

// split path & file name

///
/// \brief path_split - split path & file name
///             if path is empty then return "" in the first item
///
/// \param fname - input full path/file name
///
/// \return two items string array, [0] is path, [1] is filename
///
///
StringArray path_split(const std::string &fname);

///
/// \brief path_splitext - split path/file base name and ext name
///
/// \param fname    - input path/file name
///
/// \return two items string array, [0] path/file base name, [1] ext name (include '.')
///
/// example:
///     input pathname: /home/aa/test.txt
///     output [0]: /home/aa/test
///            [1]: .txt
///
///
StringArray path_splitext(const std::string &fname);

///
/// \brief path_extname - get file ext name
///
/// \param fname - input file name
///
/// \return file ext name
///
StringArray path_name_ext(const std::string &fname);

std::string path_extname(std::string &fname);


std::string path_join(const std::string &p1, const std::string &p2);
std::string path_join(const std::string &p1, const std::string &p2, const std::string &p3);
std::string path_join(const StringArray &p);

}


#endif // FILE_PATH_H
