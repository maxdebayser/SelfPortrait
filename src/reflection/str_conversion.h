/******************************************************************************
* Copyright (C) 2012-2014 Maximilien de Bayser
*
* SelfPortrait API - http://github.com/maxdebayser/SelfPortrait
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/
#ifndef STRING_CONVERSION_H
#define STRING_CONVERSION_H

#include <sstream>
#include <cctype>
#include <vector>
#include <type_traits>
#include <stdexcept>
//#include <iostream>

namespace {

namespace string_conversion_impl {

    struct stream {
        static std::istream& in();
        static std::ostream& out();
    };

    template<class T, class = decltype(stream::out() << std::declval<T>() )>
    std::true_type  supports_ostream(const T&);
    std::false_type supports_ostream(...);

    template<class T> using printable = decltype(supports_ostream(std::declval<T>()));

    template <typename T>
    struct lvalue_ref_priv {
        typedef T& type;
    };

    template <typename T>
    struct lvalue_ref_priv<const T&> {
        typedef T& type;
    };

    template<typename T>
    typename lvalue_ref_priv<T>::type lvalue_ref();

    template<class T, class = decltype(stream::in() >> lvalue_ref<T>() )>
    std::true_type  supports_istream(const T&);
    std::false_type supports_istream(...);

    template<class T> using parseable = decltype(supports_istream(std::declval<T>()));
	
	enum class ConversionTo {
		ConvertsToString,
		Printable,
		None
	};
	
	enum class ConversionFrom {
		StringConvertsTo,
		Parseable,
		None
	};
	
	template<class T, enum ConversionTo>
	struct ouput_helper {
		static ::std::string print(const T& t, bool * success) {
            if (success != nullptr) *success = false;
			return "";
		}
	};

	template<class T>
	struct ouput_helper<T, ConversionTo::ConvertsToString> {
		static ::std::string print(const T& t, bool * success) {
			if (success != nullptr) *success = true;
            std::string b = t;
			return t;
		}
	};
	
	template<class T>
	struct ouput_helper<T, ConversionTo::Printable> {
		static ::std::string print(const T& t, bool * success) {
			::std::ostringstream ss;
			ss << t;
            if (success != nullptr) *success = true;
			return ss.str();
		}
	};
			
	template<class T, enum ConversionFrom>
	struct input_helper {
		// T may not have a default constructor
		static T parse(const ::std::string&, bool * success) {
			if (success) *success = false;
			return T();
		}
	};
	
	template<class T>
	struct input_helper<T, ConversionFrom::StringConvertsTo> {
		static T parse(const ::std::string& str, bool * success) {
			if (success) *success = true;
			return T(str);
		}
	};

	template<class T>
	struct input_helper<T, ConversionFrom::Parseable> {
		static T parse(const ::std::string& str, bool * success) {
			T t;
			::std::istringstream ss(str);
			ss >> t;
			if (success) *success = !ss.fail();
			return t;
		}
	};
	
	template<class T>
	constexpr ConversionTo conversionToType() {
		return ::std::is_convertible<T, ::std::string>::value 
					? ConversionTo::ConvertsToString 
					: printable<T>::value
						?
						ConversionTo::Printable
						: ConversionTo::None;
	}
	
	template<class T>
	constexpr ConversionFrom conversionFromType() {
		return ::std::is_convertible< ::std::string, T>::value 
					? ConversionFrom::StringConvertsTo
					: parseable<T>::value
						?
						ConversionFrom::Parseable
						: ConversionFrom::None;
	}
}

}

namespace strconv {

template<class T>
constexpr bool convertible_to_string()
{	
	return string_conversion_impl::conversionToType<T>() != string_conversion_impl::ConversionTo::None;
}

template<class T>
constexpr bool convertible_from_string()
{
	return string_conversion_impl::conversionFromType<T>() != string_conversion_impl::ConversionFrom::None;
}


template<class T>
::std::string toString(const T& t, bool* success = nullptr) {
	return ::std::move(string_conversion_impl::ouput_helper<T, string_conversion_impl::conversionToType<T>()>::print(t, success));
}


template<class T>
T fromString(const ::std::string& str, bool* success = nullptr) {
	return string_conversion_impl::input_helper<T, string_conversion_impl::conversionFromType<T>()>::parse(str, success);
}


template<class... T>
::std::string fmt_str(const char* fmt, const T&... t)
{// TODO testar esse negocio
	::std::vector< ::std::string> args{ toString(t)... };
	::std::ostringstream ss;
	for (; *fmt != '\0'; ++fmt) {
		if (*fmt != '%') {
			ss << *fmt;
		} else {				
			++fmt;
			if (*fmt == '%') {
				ss << '%';
			} else {
			
				if (!::std::isdigit(*fmt)) {
					throw ::std::runtime_error("format string error: expected a digit ou a '%' after a '%'");
				}			
				int i = 0;				
				while(::std::isdigit(*fmt)) {
					i *= 10;
					i += (*fmt - '0');
					++fmt;
				}
				--fmt; // we read one to much
				if (i == 0) {
					throw ::std::runtime_error("string formating error: 0 is not a valid index");	
				}
				if (i > args.size()) {
					throw ::std::runtime_error(::std::string("string formating error: no argument provided for %") + toString(i));	
				}
				ss << args[i-1];
			}
		}
	}
	return ss.str();
}

}

#endif /* STRING_CONVERSION_H */
