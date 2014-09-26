SelfPortrait: A reflection library for C++11
============================================

This library aims to provide a reflection API for C++, similar to Java's.
SelfPortrait's most important feature is that it doesn't impose any
dependency upon the introspected code. It uses features of C++11 to provide
the following features that significantly improve it's usability:
- You can use classes and call methods without having to #include them
- You can call methods without limitation on the number of arguments
- All C++ call mechanism are supported: by value, by pointer, by
reference, const reference and rvalue reference.
- You can use temporaries as arguments
- Dynamic proxies are supported
The meta-data for the introspectable code is registered with a set of macros
that can be compiled separately. Included is a parser, based on clang, that
generates these meta-data automatically for your. This means that you can take
an existing pre-compiled library, parser the header files and generate the
meta-data to use in your application. 
For those who have integrated other languages such as lua, python or Java with
C++ with binding generators such as SWIG, SelfPortrait will look like a binding
from C++ to C++ itself.
