
/// \brief cppformat usage demo
///
/// \see http://cppformat.readthedocs.org/en/latest/
/// \see http://cppformat.github.io/
/// \see https://github.com/cppformat/cppformat
///

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <iostream>

#include <base/system/format/format.h>
#include <base/system/format/posix.h>

using namespace std;
using namespace fmt;


///
/// \brief The Date class
///
class Date {
public:
    Date(int year, int month, int day) : year_(year), month_(month), day_(day) {}

    friend std::ostream &operator<<(std::ostream &os, const Date &d) {
        return os << d.year_ << '-' << d.month_ << '-' << d.day_;
    }

private:
    int year_, month_, day_;
};


// Prints formatted error message.
void report_error(const char *format, fmt::ArgList args) {
    fmt::print("Error: ");
    fmt::print(format, args);
}

FMT_VARIADIC(void, report_error, const char *)


///
/// \brief main function
/// \param argc
/// \param argv
/// \return
///
int main(int argc, char *argv[])
{
    ///////////////////////////////////////////////////////////
    /// Formatting functions
    ///////////////////////////////////////////////////////////

    // std::string format(StringRef format_str, const ArgList & args)
    cout << format("Hello world! {}", 10) << endl;

    // void print(StringRef format_str, const ArgList & args)
    cout << format("float value: {0:12f}, int value: {1:05d}", 1.234, 4562) << endl;

    // void print(std::FILE * f, StringRef format_str, const ArgList & args)
    print("Elapsed time: {0:.2f} seconds\n", 1.23);

    // void print(std::FILE * f, StringRef format_str, const ArgList & args)
    print(stdout, "Don't {}!\n", "panic");

    // void print(std::ostream & os, StringRef format_str, const ArgList & args)
    print(cerr, "Don't {} again!\n", "panic");

    ///////////////////////////////////////////////////////////
    /// Printf formatting functions
    ///////////////////////////////////////////////////////////

    // int printf(StringRef format, const ArgList & args)
    fmt::printf("Elapsed time: %.2f seconds\n", 1.23);

    // int fprintf(std::FILE * f, StringRef format, const ArgList & args)
    fmt::fprintf(stderr, "Don't %s! (fprintf)\n", "panic");

    // std::string sprintf(StringRef format, const ArgList & args)
    cout << fmt::sprintf("The answer is %d\n", 42) << endl;

    ///////////////////////////////////////////////////////////
    /// MemoryWriter API
    ///////////////////////////////////////////////////////////

    fmt::MemoryWriter w;
    w << format("Hello a = ");
    w << 42;                                // replaces itoa(42, buffer, 10)
    w << format(", b=") << fmt::hex(42);    // replaces itoa(42, buffer, 16)
    cout << w.c_str() << endl;


    ///////////////////////////////////////////////////////////
    /// cooperate with iostream
    ///////////////////////////////////////////////////////////
    std::string s = fmt::format("The date is {}\n", Date(2012, 12, 9));
    cout << s;

    ///////////////////////////////////////////////////////////
    /// user defined functions
    ///////////////////////////////////////////////////////////
    report_error("file not found: {}\n", "/usr/include/math.h");

    return 0;
}
