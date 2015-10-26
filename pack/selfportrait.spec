Name:		SelfPortrait
Version:	0.10
Release:	1%{?dist}
Summary:	A reflection library for C++11

Group:		System Environment/Libraries
License:	MIT
URL:		http://github.com/maxdebayser/SelfPortrait
Source0:	SelfPortrait.tar.bz2
BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:	clang

%define lua_version           %(lua -v 2>&1 | grep -Po '\\d\\.\\d')


%description
This library aims to provide a reflection API for C++, similar to Java's.
SelfPortrait's most important feature is that it doesn't impose any
dependency upon the introspected code. It uses features of C++11 to
provide the following features that significantly improve it's usability:

- You can use classes and call methods without having to #include them
- You can call methods without limitation on the number of arguments
- All C++ call mechanism are supported: by value, by pointer, by
  reference, const reference and rvalue reference.
- You can use temporaries as arguments
- Dynamic proxies are supported

The meta-data for the introspectable code is registered with a set of macros
that can be compiled separately. Included is a parser, based on clang, that
generates these meta-data automatically for your. This means that you can
take an existing pre-compiled library, parser the header files and generate
the meta-data to use in your application. For those who have integrated other
languages such as lua, python or Java with C++ with binding generators such
as SWIG, SelfPortrait will look like a binding from C++ to C++ itself.


%prep
%setup -q -n SelfPortrait


%build
mkdir build
cd build
cmake ../ -DCMAKE_INSTALL_PREFIX=/usr -DSELFPORTRAIT_BUILD_PARSER=on
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
cd build
make install DESTDIR=$RPM_BUILD_ROOT

%check
cd build/src/unit_test
./unit_test


%clean
rm -rf $RPM_BUILD_ROOT


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%defattr(-,root,root,-)
%doc LICENSE README.md
%{_bindir}/selfportraitc
%{_libdir}/*.so
%{_libdir}/lua/%{lua_version}/*.so
%{_includedir}/SelfPortrait/*.h

%changelog
* Mon Nov 03 2014 Maximilien de Bayser <maxdebayser@gmail.com> 0.2
- Install the lua module in a location that is on the "luarocks path"
* Thu Oct 08 2014 Maximilien de Bayser <maxdebayser@gmail.com> 0.1
- Package first created for g++-4.9, built without parser
