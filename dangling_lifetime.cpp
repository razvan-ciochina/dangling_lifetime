#include <cstring>
#include <algorithm>
#include <iterator>
#include <iostream>
using namespace std;

class DumbString {
    char *_internal_str;
public:
    DumbString(const char *const in) {
        size_t len = strlen(in) + 1;
        _internal_str = new char[len];
        std::for_each(in, in + len,
          [it = _internal_str](const char &c) mutable {
              *it++ = c;
          } );

        std::cout << "DumbString ctor\n";
    }

    ~DumbString() {
        std::cout << "DumbString dtor\n";
        delete [] _internal_str;
    }

    char &operator[](const int &idx) const {
        return _internal_str[idx];
    }

    friend iostream &operator<<(iostream &os, const DumbString &str) {
        os << str._internal_str;
        return os;
    }
};

//DumbString&& foo(DumbString&& in) {
const DumbString& foo(DumbString&& in) {
    std::cout << "Inside foo\n"; // variable in contains the temporary whose lifetime lasts until the temporary in main at line 41 disappears
    // return std::move(in);
    return in;
}

int main() {
    // DumbString&& ret = foo(DumbString("Ana are mere\n")); // the rvalue ref doesn't extend the lifetime of the returned object which expired => dangling ref
    const DumbString& ret = foo(DumbString("Ana are mere\n")); // the const ref doesn't extend the lifetime of the returned object which expired => dangling ref
    std::cout << "after foo()\n";
    return ret[0];
}
