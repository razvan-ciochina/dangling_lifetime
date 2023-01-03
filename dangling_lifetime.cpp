#include <cstring>
#include <string>
#include <algorithm>
#include <iterator>
#include <iostream>

#ifdef BACKTRACES
// for backtrace
#include <backward.hpp>
using namespace backward;
// end
#endif

using namespace std;

static void print_backtrace() {
#ifdef BACKTRACES
    StackTrace st;
    st.load_here(32);
    Printer p;
    p.reverse = false;
    p.print(st);
#endif
}

class DumbString {
    char *_internal_str;
    size_t _len;
public:
    DumbString(const char *const in) {
        _len = strlen(in) + 1;
        _internal_str = new char[_len];
        std::for_each(in, in + _len,
          [it = _internal_str](const char &c) mutable {
              *it++ = c;
          } );

        std::cout << "DumbString ctor\n";
    }

    DumbString(DumbString const &rhs) 
        : _internal_str(new char[rhs._len]),
          _len(rhs._len)
    {
        std::cout << "Dumbstring cpy ctor\n";
        memcpy(_internal_str, rhs._internal_str, _len);
    }

    DumbString(DumbString &&rhs) {
        std::cout << "DumbString mv ctor\n";
        _internal_str = rhs._internal_str;
        _len = rhs._len;
        rhs._internal_str = new char[18];
        memcpy(rhs._internal_str, "unknown but valid", 18);
        _len = 18;
    }

    DumbString& operator=(DumbString const &rhs) {
        std::cout << "DumbString cpy asgn\n";
        _internal_str = new char[rhs._len];
        _len = rhs._len;
        memcpy(_internal_str, rhs._internal_str, _len);
        return *this;
    }

    DumbString& operator=(DumbString &&rhs) {
        std::cout << "DumbString mv asgn\n";
        _internal_str = rhs._internal_str;
        _len = rhs._len;
        rhs._internal_str = new char[18];
        rhs._len = 18;
        memcpy(rhs._internal_str, "unknown but valid", 18);
        return *this;
    }

    ~DumbString() {
        std::cout << "DumbString dtor: "<< _internal_str << '\n';
        delete [] _internal_str;
        _len = 0;
        print_backtrace();
    }

    friend ostream &operator<<(ostream &os, const DumbString &str);
};

ostream &operator<<(ostream &os, const DumbString &str)
{
    if (str._internal_str) {
        os << str._internal_str;
    }
    return os;
}


void f(int i, const DumbString & s) {
    try {
        std::cout << s << '\n';
    } catch (std::logic_error &err) {
        std::cout << err.what() << '\n';
    }
}

//DumbString&& foo(DumbString&& in) {
const DumbString& foo(DumbString&& in) {
    std::cout << "Inside foo\n"; // variable in contains the temporary whose lifetime lasts until the temporary in main at line 41 disappears
    std::cout << "foo(): " << in << '\n';
//    return std::move(in);
    return in;
}

int main() {
    // DumbString&& ret = foo(DumbString("Ana are mere")); // the rvalue ref doesn't extend the lifetime of the returned object which expired => dangling ref
    const DumbString &ret = foo(DumbString("Ana are mere")); // the const ref doesn't extend the lifetime of the returned object which expired => dangling ref
    std::cout << "after foo()\n";
    std::cout << "main(): " << ret << '\n';
    std::cout << "The end\n";
    return 0;
}
