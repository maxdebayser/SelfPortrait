#ifndef STRING_CONVERSION_H
#define STRING_CONVERSION_H

#include <sstream>
#include <cctype>
#include <vector>
#include <type_traits>

namespace {

namespace string_conversion_impl {
	typedef char no;
	typedef char yes[2];

	struct any_t {
		template<typename T> any_t( T const& );
	};


	struct stream {
		stream(const std::ostream&);
	};

	no operator<<( std::ostream const&, any_t const& );

	//no operator<<( stream const&, any_t const& );


	no operator>>( std::istream &, const any_t & );

	yes& test_ostream( std::ostream& );
	no test_ostream( no );
	
	yes& test_istream( std::istream& );
	no test_istream( no );

	template<typename T>
	struct printable {
		static std::ostream &s;
		static T const &t;
		// Ambiguous conversions have ruined it all
		//static bool const value = sizeof( test_ostream(s << t) ) == sizeof( yes );
		static bool const value = ::std::is_arithmetic<T>::value || ::std::is_same<T, const char*>::value || ::std::is_same<T, ::std::string>::value;
	};
		
	template<typename T>
	struct parseable {
		static std::istream &s;
		static T &t;
		static bool const value = sizeof( test_istream(s >> t) ) == sizeof( yes );
	};
	
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
