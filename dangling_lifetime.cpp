#include <cstring>
#include <string>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <array>

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

template <size_t V>
class convert_base{
public:
    template<typename T, size_t N>
    constexpr static void convert_one(std::array<T, N> &in_ref, const T (&v)[N])
   {
       in_ref[V-1] = v[V-1];
       convert_base<V-1>::template convert_one<T, N>(in_ref, v);
   }
};

template <>
class convert_base<0> {
public:
    template<class T, size_t N>
    constexpr static void convert_one(std::array<T, N> &in_ref, const T (&v)[N]) {
        in_ref[0] = v[0];
    }
};

template <typename T, size_t N>
constexpr const std::array<T, N> convert_array_to_stdarray(const T (&carray)[N]) {
    std::array<T, N> new_array{0};
    convert_base<N>::convert_one(new_array, carray);
    return new_array;
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

        // set rhs internal to predefined string to symbolize
        // the xvalue state
        constexpr std::array unknown_str = convert_array_to_stdarray("unknown but valid");
        rhs._internal_str = new char[unknown_str.size()];
        rhs._len = unknown_str.size(); 
        memcpy(rhs._internal_str, unknown_str.data(), rhs._len);
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
        
        // set rhs internal to predefined string to symbolize
        // the xvalue state
        constexpr std::array unknown_str = convert_array_to_stdarray("unknown but valid");
        rhs._internal_str = new char[unknown_str.size()];
        rhs._len = unknown_str.size(); 
        memcpy(rhs._internal_str, unknown_str.data(), rhs._len);

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
