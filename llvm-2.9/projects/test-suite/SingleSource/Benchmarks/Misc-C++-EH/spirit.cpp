#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <typeinfo>
#include <memory>
#include <algorithm>
#include <limits>
#include <string.h>
namespace boost { namespace spirit
{
    struct nil_t {};
}}
namespace boost {
namespace mpl {
namespace aux {
template< typename F > struct template_arity;
}
}
}
namespace boost { namespace mpl {
template< bool C_ > struct bool_;
typedef bool_<true> true_;
typedef bool_<false> false_;
}}
namespace boost { namespace mpl {
template< bool C_ > struct bool_
{
    static const bool value = C_;
    typedef bool_ type;
    typedef bool value_type;
    operator bool() const { return this->value; }
};
template< bool C_ >
bool const bool_<C_>::value;
}}
namespace boost {
template< typename T > struct is_integral : mpl::bool_< false > { };
template<> struct is_integral< unsigned char > : mpl::bool_< true > { }; template<> struct is_integral< unsigned char const > : mpl::bool_< true > { }; template<> struct is_integral< unsigned char volatile > : mpl::bool_< true > { }; template<> struct is_integral< unsigned char const volatile > : mpl::bool_< true > { };
template<> struct is_integral< unsigned short > : mpl::bool_< true > { }; template<> struct is_integral< unsigned short const > : mpl::bool_< true > { }; template<> struct is_integral< unsigned short volatile > : mpl::bool_< true > { }; template<> struct is_integral< unsigned short const volatile > : mpl::bool_< true > { };
template<> struct is_integral< unsigned int > : mpl::bool_< true > { }; template<> struct is_integral< unsigned int const > : mpl::bool_< true > { }; template<> struct is_integral< unsigned int volatile > : mpl::bool_< true > { }; template<> struct is_integral< unsigned int const volatile > : mpl::bool_< true > { };
template<> struct is_integral< unsigned long > : mpl::bool_< true > { }; template<> struct is_integral< unsigned long const > : mpl::bool_< true > { }; template<> struct is_integral< unsigned long volatile > : mpl::bool_< true > { }; template<> struct is_integral< unsigned long const volatile > : mpl::bool_< true > { };
template<> struct is_integral< signed char > : mpl::bool_< true > { }; template<> struct is_integral< signed char const > : mpl::bool_< true > { }; template<> struct is_integral< signed char volatile > : mpl::bool_< true > { }; template<> struct is_integral< signed char const volatile > : mpl::bool_< true > { };
template<> struct is_integral< signed short > : mpl::bool_< true > { }; template<> struct is_integral< signed short const > : mpl::bool_< true > { }; template<> struct is_integral< signed short volatile > : mpl::bool_< true > { }; template<> struct is_integral< signed short const volatile > : mpl::bool_< true > { };
template<> struct is_integral< signed int > : mpl::bool_< true > { }; template<> struct is_integral< signed int const > : mpl::bool_< true > { }; template<> struct is_integral< signed int volatile > : mpl::bool_< true > { }; template<> struct is_integral< signed int const volatile > : mpl::bool_< true > { };
template<> struct is_integral< signed long > : mpl::bool_< true > { }; template<> struct is_integral< signed long const > : mpl::bool_< true > { }; template<> struct is_integral< signed long volatile > : mpl::bool_< true > { }; template<> struct is_integral< signed long const volatile > : mpl::bool_< true > { };
template<> struct is_integral< bool > : mpl::bool_< true > { }; template<> struct is_integral< bool const > : mpl::bool_< true > { }; template<> struct is_integral< bool volatile > : mpl::bool_< true > { }; template<> struct is_integral< bool const volatile > : mpl::bool_< true > { };
template<> struct is_integral< char > : mpl::bool_< true > { }; template<> struct is_integral< char const > : mpl::bool_< true > { }; template<> struct is_integral< char volatile > : mpl::bool_< true > { }; template<> struct is_integral< char const volatile > : mpl::bool_< true > { };
template<> struct is_integral< wchar_t > : mpl::bool_< true > { }; template<> struct is_integral< wchar_t const > : mpl::bool_< true > { }; template<> struct is_integral< wchar_t volatile > : mpl::bool_< true > { }; template<> struct is_integral< wchar_t const volatile > : mpl::bool_< true > { };
template<> struct is_integral< unsigned long long > : mpl::bool_< true > { }; template<> struct is_integral< unsigned long long const > : mpl::bool_< true > { }; template<> struct is_integral< unsigned long long volatile > : mpl::bool_< true > { }; template<> struct is_integral< unsigned long long const volatile > : mpl::bool_< true > { };
template<> struct is_integral< long long > : mpl::bool_< true > { }; template<> struct is_integral< long long const > : mpl::bool_< true > { }; template<> struct is_integral< long long volatile > : mpl::bool_< true > { }; template<> struct is_integral< long long const volatile > : mpl::bool_< true > { };
}
namespace boost {
template< typename T > struct is_float : mpl::bool_< false > { };
template<> struct is_float< float > : mpl::bool_< true > { }; template<> struct is_float< float const > : mpl::bool_< true > { }; template<> struct is_float< float volatile > : mpl::bool_< true > { }; template<> struct is_float< float const volatile > : mpl::bool_< true > { };
template<> struct is_float< double > : mpl::bool_< true > { }; template<> struct is_float< double const > : mpl::bool_< true > { }; template<> struct is_float< double volatile > : mpl::bool_< true > { }; template<> struct is_float< double const volatile > : mpl::bool_< true > { };
template<> struct is_float< long double > : mpl::bool_< true > { }; template<> struct is_float< long double const > : mpl::bool_< true > { }; template<> struct is_float< long double volatile > : mpl::bool_< true > { }; template<> struct is_float< long double const volatile > : mpl::bool_< true > { };
}
namespace boost {
namespace type_traits {
template <bool b1, bool b2, bool b3 = false, bool b4 = false, bool b5 = false, bool b6 = false, bool b7 = false>
struct ice_or;
template <bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>
struct ice_or
{
    static const bool value = true;
};
template <>
struct ice_or<false, false, false, false, false, false, false>
{
    static const bool value = false;
};
}
}
namespace boost {
namespace detail {
template< typename T >
struct is_arithmetic_impl
{
    static const bool value = (::boost::type_traits::ice_or< ::boost::is_integral<T>::value, ::boost::is_float<T>::value >::value);
};
}
template< typename T > struct is_arithmetic : mpl::bool_< ::boost::detail::is_arithmetic_impl<T>::value > { };
}
namespace boost {
template< typename T > struct is_void : mpl::bool_< false > { };
template<> struct is_void< void > : mpl::bool_< true > { };
template<> struct is_void< void const > : mpl::bool_< true > { };
template<> struct is_void< void volatile > : mpl::bool_< true > { };
template<> struct is_void< void const volatile > : mpl::bool_< true > { };
}
namespace boost {
namespace detail {
template <typename T>
struct is_fundamental_impl
    : ::boost::type_traits::ice_or<
          ::boost::is_arithmetic<T>::value
        , ::boost::is_void<T>::value
        >
{
};
}
template< typename T > struct is_fundamental : mpl::bool_< ::boost::detail::is_fundamental_impl<T>::value > { };
}
namespace boost {
template< typename T > struct is_array : mpl::bool_< false > { };
template< typename T, std::size_t N > struct is_array< T[N] > : mpl::bool_< true > { };
template< typename T, std::size_t N > struct is_array< T const[N] > : mpl::bool_< true > { };
template< typename T, std::size_t N > struct is_array< T volatile[N] > : mpl::bool_< true > { };
template< typename T, std::size_t N > struct is_array< T const volatile[N] > : mpl::bool_< true > { };
}
namespace boost {
template< typename T > struct is_reference : mpl::bool_< false > { };
template< typename T > struct is_reference< T& > : mpl::bool_< true > { };
}
namespace boost {
namespace detail {
template <typename T>
struct add_reference_impl
{
    typedef T& type;
};
template< typename T > struct add_reference_impl<T&> { typedef T& type; };
template<> struct add_reference_impl<void> { typedef void type; };
template<> struct add_reference_impl<void const> { typedef void const type; };
template<> struct add_reference_impl<void volatile> { typedef void volatile type; };
template<> struct add_reference_impl<void const volatile> { typedef void const volatile type; };
}
template< typename T > struct add_reference { typedef typename detail::add_reference_impl<T>::type type; };
}
namespace boost {
namespace type_traits {
typedef char yes_type;
struct no_type
{
   char padding[8];
};
}
}
namespace boost {
namespace type_traits {
template <bool b1, bool b2, bool b3 = true, bool b4 = true, bool b5 = true, bool b6 = true, bool b7 = true>
struct ice_and;
template <bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>
struct ice_and
{
    static const bool value = false;
};
template <>
struct ice_and<true, true, true, true, true, true, true>
{
    static const bool value = true;
};
}
}
namespace boost {
namespace type_traits {
template <bool b>
struct ice_not
{
    static const bool value = true;
};
template <>
struct ice_not<true>
{
    static const bool value = false;
};
}
}
namespace boost {
namespace type_traits {
template <int b1, int b2>
struct ice_eq
{
    static const bool value = (b1 == b2);
};
template <int b1, int b2>
struct ice_ne
{
    static const bool value = (b1 != b2);
};
template <int b1, int b2> bool const ice_eq<b1,b2>::value;
template <int b1, int b2> bool const ice_ne<b1,b2>::value;
}
}
namespace boost {
namespace detail {
struct any_conversion
{
    template <typename T> any_conversion(const volatile T&);
    template <typename T> any_conversion(T&);
};
template <typename T> struct checker
{
    static boost::type_traits::no_type _m_check(any_conversion ...);
    static boost::type_traits::yes_type _m_check(T, int);
};
template <typename From, typename To>
struct is_convertible_basic_impl
{
    static From _m_from;
    static bool const value = sizeof( detail::checker<To>::_m_check(_m_from, 0) )
        == sizeof(::boost::type_traits::yes_type);
};
template <typename From, typename To>
struct is_convertible_impl
{
    typedef typename add_reference<From>::type ref_type;
    static const bool value = (::boost::type_traits::ice_and< ::boost::detail::is_convertible_basic_impl<ref_type,To>::value, ::boost::type_traits::ice_not< ::boost::is_array<To>::value >::value >::value);
};
    template<> struct is_convertible_impl< void,void > { static const bool value = (true); }; template<> struct is_convertible_impl< void,void const > { static const bool value = (true); }; template<> struct is_convertible_impl< void,void volatile > { static const bool value = (true); }; template<> struct is_convertible_impl< void,void const volatile > { static const bool value = (true); }; template<> struct is_convertible_impl< void const,void > { static const bool value = (true); }; template<> struct is_convertible_impl< void const,void const > { static const bool value = (true); }; template<> struct is_convertible_impl< void const,void volatile > { static const bool value = (true); }; template<> struct is_convertible_impl< void const,void const volatile > { static const bool value = (true); }; template<> struct is_convertible_impl< void volatile,void > { static const bool value = (true); }; template<> struct is_convertible_impl< void volatile,void const > { static const bool value = (true); }; template<> struct is_convertible_impl< void volatile,void volatile > { static const bool value = (true); }; template<> struct is_convertible_impl< void volatile,void const volatile > { static const bool value = (true); }; template<> struct is_convertible_impl< void const volatile,void > { static const bool value = (true); }; template<> struct is_convertible_impl< void const volatile,void const > { static const bool value = (true); }; template<> struct is_convertible_impl< void const volatile,void volatile > { static const bool value = (true); }; template<> struct is_convertible_impl< void const volatile,void const volatile > { static const bool value = (true); };
template< typename To > struct is_convertible_impl< void,To > { static const bool value = (false); };
template< typename From > struct is_convertible_impl< From,void > { static const bool value = (false); };
template< typename To > struct is_convertible_impl< void const,To > { static const bool value = (false); };
template< typename To > struct is_convertible_impl< void volatile,To > { static const bool value = (false); };
template< typename To > struct is_convertible_impl< void const volatile,To > { static const bool value = (false); };
template< typename From > struct is_convertible_impl< From,void const > { static const bool value = (false); };
template< typename From > struct is_convertible_impl< From,void volatile > { static const bool value = (false); };
template< typename From > struct is_convertible_impl< From,void const volatile > { static const bool value = (false); };
}
template< typename From, typename To > struct is_convertible : mpl::bool_< (::boost::detail::is_convertible_impl<From,To>::value) > { };
template<> struct is_convertible< float,char > : mpl::bool_< true > { }; template<> struct is_convertible< float,signed char > : mpl::bool_< true > { }; template<> struct is_convertible< float,unsigned char > : mpl::bool_< true > { }; template<> struct is_convertible< float,signed short > : mpl::bool_< true > { }; template<> struct is_convertible< float,unsigned short > : mpl::bool_< true > { }; template<> struct is_convertible< float,signed int > : mpl::bool_< true > { }; template<> struct is_convertible< float,unsigned int > : mpl::bool_< true > { }; template<> struct is_convertible< float,signed long > : mpl::bool_< true > { }; template<> struct is_convertible< float,unsigned long > : mpl::bool_< true > { }; template<> struct is_convertible< float,signed long long > : mpl::bool_< true > { }; template<> struct is_convertible< float,unsigned long long > : mpl::bool_< true > { }; template<> struct is_convertible< float const,char > : mpl::bool_< true > { }; template<> struct is_convertible< float const,signed char > : mpl::bool_< true > { }; template<> struct is_convertible< float const,unsigned char > : mpl::bool_< true > { }; template<> struct is_convertible< float const,signed short > : mpl::bool_< true > { }; template<> struct is_convertible< float const,unsigned short > : mpl::bool_< true > { }; template<> struct is_convertible< float const,signed int > : mpl::bool_< true > { }; template<> struct is_convertible< float const,unsigned int > : mpl::bool_< true > { }; template<> struct is_convertible< float const,signed long > : mpl::bool_< true > { }; template<> struct is_convertible< float const,unsigned long > : mpl::bool_< true > { }; template<> struct is_convertible< float const,signed long long > : mpl::bool_< true > { }; template<> struct is_convertible< float const,unsigned long long > : mpl::bool_< true > { }; template<> struct is_convertible< float volatile,char > : mpl::bool_< true > { }; template<> struct is_convertible< float volatile,signed char > : mpl::bool_< true > { }; template<> struct is_convertible< float volatile,unsigned char > : mpl::bool_< true > { }; template<> struct is_convertible< float volatile,signed short > : mpl::bool_< true > { }; template<> struct is_convertible< float volatile,unsigned short > : mpl::bool_< true > { }; template<> struct is_convertible< float volatile,signed int > : mpl::bool_< true > { }; template<> struct is_convertible< float volatile,unsigned int > : mpl::bool_< true > { }; template<> struct is_convertible< float volatile,signed long > : mpl::bool_< true > { }; template<> struct is_convertible< float volatile,unsigned long > : mpl::bool_< true > { }; template<> struct is_convertible< float volatile,signed long long > : mpl::bool_< true > { }; template<> struct is_convertible< float volatile,unsigned long long > : mpl::bool_< true > { }; template<> struct is_convertible< float const volatile,char > : mpl::bool_< true > { }; template<> struct is_convertible< float const volatile,signed char > : mpl::bool_< true > { }; template<> struct is_convertible< float const volatile,unsigned char > : mpl::bool_< true > { }; template<> struct is_convertible< float const volatile,signed short > : mpl::bool_< true > { }; template<> struct is_convertible< float const volatile,unsigned short > : mpl::bool_< true > { }; template<> struct is_convertible< float const volatile,signed int > : mpl::bool_< true > { }; template<> struct is_convertible< float const volatile,unsigned int > : mpl::bool_< true > { }; template<> struct is_convertible< float const volatile,signed long > : mpl::bool_< true > { }; template<> struct is_convertible< float const volatile,unsigned long > : mpl::bool_< true > { }; template<> struct is_convertible< float const volatile,signed long long > : mpl::bool_< true > { }; template<> struct is_convertible< float const volatile,unsigned long long > : mpl::bool_< true > { };
template<> struct is_convertible< double,char > : mpl::bool_< true > { }; template<> struct is_convertible< double,signed char > : mpl::bool_< true > { }; template<> struct is_convertible< double,unsigned char > : mpl::bool_< true > { }; template<> struct is_convertible< double,signed short > : mpl::bool_< true > { }; template<> struct is_convertible< double,unsigned short > : mpl::bool_< true > { }; template<> struct is_convertible< double,signed int > : mpl::bool_< true > { }; template<> struct is_convertible< double,unsigned int > : mpl::bool_< true > { }; template<> struct is_convertible< double,signed long > : mpl::bool_< true > { }; template<> struct is_convertible< double,unsigned long > : mpl::bool_< true > { }; template<> struct is_convertible< double,signed long long > : mpl::bool_< true > { }; template<> struct is_convertible< double,unsigned long long > : mpl::bool_< true > { }; template<> struct is_convertible< double const,char > : mpl::bool_< true > { }; template<> struct is_convertible< double const,signed char > : mpl::bool_< true > { }; template<> struct is_convertible< double const,unsigned char > : mpl::bool_< true > { }; template<> struct is_convertible< double const,signed short > : mpl::bool_< true > { }; template<> struct is_convertible< double const,unsigned short > : mpl::bool_< true > { }; template<> struct is_convertible< double const,signed int > : mpl::bool_< true > { }; template<> struct is_convertible< double const,unsigned int > : mpl::bool_< true > { }; template<> struct is_convertible< double const,signed long > : mpl::bool_< true > { }; template<> struct is_convertible< double const,unsigned long > : mpl::bool_< true > { }; template<> struct is_convertible< double const,signed long long > : mpl::bool_< true > { }; template<> struct is_convertible< double const,unsigned long long > : mpl::bool_< true > { }; template<> struct is_convertible< double volatile,char > : mpl::bool_< true > { }; template<> struct is_convertible< double volatile,signed char > : mpl::bool_< true > { }; template<> struct is_convertible< double volatile,unsigned char > : mpl::bool_< true > { }; template<> struct is_convertible< double volatile,signed short > : mpl::bool_< true > { }; template<> struct is_convertible< double volatile,unsigned short > : mpl::bool_< true > { }; template<> struct is_convertible< double volatile,signed int > : mpl::bool_< true > { }; template<> struct is_convertible< double volatile,unsigned int > : mpl::bool_< true > { }; template<> struct is_convertible< double volatile,signed long > : mpl::bool_< true > { }; template<> struct is_convertible< double volatile,unsigned long > : mpl::bool_< true > { }; template<> struct is_convertible< double volatile,signed long long > : mpl::bool_< true > { }; template<> struct is_convertible< double volatile,unsigned long long > : mpl::bool_< true > { }; template<> struct is_convertible< double const volatile,char > : mpl::bool_< true > { }; template<> struct is_convertible< double const volatile,signed char > : mpl::bool_< true > { }; template<> struct is_convertible< double const volatile,unsigned char > : mpl::bool_< true > { }; template<> struct is_convertible< double const volatile,signed short > : mpl::bool_< true > { }; template<> struct is_convertible< double const volatile,unsigned short > : mpl::bool_< true > { }; template<> struct is_convertible< double const volatile,signed int > : mpl::bool_< true > { }; template<> struct is_convertible< double const volatile,unsigned int > : mpl::bool_< true > { }; template<> struct is_convertible< double const volatile,signed long > : mpl::bool_< true > { }; template<> struct is_convertible< double const volatile,unsigned long > : mpl::bool_< true > { }; template<> struct is_convertible< double const volatile,signed long long > : mpl::bool_< true > { }; template<> struct is_convertible< double const volatile,unsigned long long > : mpl::bool_< true > { };
template<> struct is_convertible< long double,char > : mpl::bool_< true > { }; template<> struct is_convertible< long double,signed char > : mpl::bool_< true > { }; template<> struct is_convertible< long double,unsigned char > : mpl::bool_< true > { }; template<> struct is_convertible< long double,signed short > : mpl::bool_< true > { }; template<> struct is_convertible< long double,unsigned short > : mpl::bool_< true > { }; template<> struct is_convertible< long double,signed int > : mpl::bool_< true > { }; template<> struct is_convertible< long double,unsigned int > : mpl::bool_< true > { }; template<> struct is_convertible< long double,signed long > : mpl::bool_< true > { }; template<> struct is_convertible< long double,unsigned long > : mpl::bool_< true > { }; template<> struct is_convertible< long double,signed long long > : mpl::bool_< true > { }; template<> struct is_convertible< long double,unsigned long long > : mpl::bool_< true > { }; template<> struct is_convertible< long double const,char > : mpl::bool_< true > { }; template<> struct is_convertible< long double const,signed char > : mpl::bool_< true > { }; template<> struct is_convertible< long double const,unsigned char > : mpl::bool_< true > { }; template<> struct is_convertible< long double const,signed short > : mpl::bool_< true > { }; template<> struct is_convertible< long double const,unsigned short > : mpl::bool_< true > { }; template<> struct is_convertible< long double const,signed int > : mpl::bool_< true > { }; template<> struct is_convertible< long double const,unsigned int > : mpl::bool_< true > { }; template<> struct is_convertible< long double const,signed long > : mpl::bool_< true > { }; template<> struct is_convertible< long double const,unsigned long > : mpl::bool_< true > { }; template<> struct is_convertible< long double const,signed long long > : mpl::bool_< true > { }; template<> struct is_convertible< long double const,unsigned long long > : mpl::bool_< true > { }; template<> struct is_convertible< long double volatile,char > : mpl::bool_< true > { }; template<> struct is_convertible< long double volatile,signed char > : mpl::bool_< true > { }; template<> struct is_convertible< long double volatile,unsigned char > : mpl::bool_< true > { }; template<> struct is_convertible< long double volatile,signed short > : mpl::bool_< true > { }; template<> struct is_convertible< long double volatile,unsigned short > : mpl::bool_< true > { }; template<> struct is_convertible< long double volatile,signed int > : mpl::bool_< true > { }; template<> struct is_convertible< long double volatile,unsigned int > : mpl::bool_< true > { }; template<> struct is_convertible< long double volatile,signed long > : mpl::bool_< true > { }; template<> struct is_convertible< long double volatile,unsigned long > : mpl::bool_< true > { }; template<> struct is_convertible< long double volatile,signed long long > : mpl::bool_< true > { }; template<> struct is_convertible< long double volatile,unsigned long long > : mpl::bool_< true > { }; template<> struct is_convertible< long double const volatile,char > : mpl::bool_< true > { }; template<> struct is_convertible< long double const volatile,signed char > : mpl::bool_< true > { }; template<> struct is_convertible< long double const volatile,unsigned char > : mpl::bool_< true > { }; template<> struct is_convertible< long double const volatile,signed short > : mpl::bool_< true > { }; template<> struct is_convertible< long double const volatile,unsigned short > : mpl::bool_< true > { }; template<> struct is_convertible< long double const volatile,signed int > : mpl::bool_< true > { }; template<> struct is_convertible< long double const volatile,unsigned int > : mpl::bool_< true > { }; template<> struct is_convertible< long double const volatile,signed long > : mpl::bool_< true > { }; template<> struct is_convertible< long double const volatile,unsigned long > : mpl::bool_< true > { }; template<> struct is_convertible< long double const volatile,signed long long > : mpl::bool_< true > { }; template<> struct is_convertible< long double const volatile,unsigned long long > : mpl::bool_< true > { };
}
namespace boost {
namespace type_traits {
struct false_result
{
    template <typename T> struct result_
    {
        static const bool value = false;
    };
};
}}
namespace boost {
namespace type_traits {
template <class R>
struct is_function_ptr_helper
{
    static const bool value = false;
};
template <class R>
struct is_function_ptr_helper<R (*)()> { static const bool value = true; };
template <class R,class T0>
struct is_function_ptr_helper<R (*)(T0)> { static const bool value = true; };
template <class R,class T0,class T1>
struct is_function_ptr_helper<R (*)(T0,T1)> { static const bool value = true; };
template <class R,class T0,class T1,class T2>
struct is_function_ptr_helper<R (*)(T0,T1,T2)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20,class T21>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20,class T21,class T22>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20,class T21,class T22,class T23>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23)> { static const bool value = true; };
template <class R,class T0,class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19,class T20,class T21,class T22,class T23,class T24>
struct is_function_ptr_helper<R (*)(T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13,T14,T15,T16,T17,T18,T19,T20,T21,T22,T23,T24)> { static const bool value = true; };
}
}
namespace boost {
namespace detail {
template<bool is_ref = true>
struct is_function_chooser
    : ::boost::type_traits::false_result
{
};
template <>
struct is_function_chooser<false>
{
    template< typename T > struct result_
        : ::boost::type_traits::is_function_ptr_helper<T*>
    {
    };
};
template <typename T>
struct is_function_impl
    : is_function_chooser< ::boost::is_reference<T>::value >
        ::template result_<T>
{
};
}
template< typename T > struct is_function : mpl::bool_< ::boost::detail::is_function_impl<T>::value > { };
}
namespace boost {
namespace detail {
template <typename T> struct cv_traits_imp {};
template <typename T>
struct cv_traits_imp<T*>
{
    static const bool is_const = false;
    static const bool is_volatile = false;
    typedef T unqualified_type;
};
template <typename T>
struct cv_traits_imp<const T*>
{
    static const bool is_const = true;
    static const bool is_volatile = false;
    typedef T unqualified_type;
};
template <typename T>
struct cv_traits_imp<volatile T*>
{
    static const bool is_const = false;
    static const bool is_volatile = true;
    typedef T unqualified_type;
};
template <typename T>
struct cv_traits_imp<const volatile T*>
{
    static const bool is_const = true;
    static const bool is_volatile = true;
    typedef T unqualified_type;
};
}
}
namespace boost {
template< typename T > struct remove_cv { typedef typename detail::cv_traits_imp<T*>::unqualified_type type; };
template< typename T > struct remove_cv<T&> { typedef T& type; };
template< typename T, std::size_t N > struct remove_cv<T const[N]> { typedef T type[N]; };
template< typename T, std::size_t N > struct remove_cv<T volatile[N]> { typedef T type[N]; };
template< typename T, std::size_t N > struct remove_cv<T const volatile[N]> { typedef T type[N]; };
}
namespace boost {
namespace detail {
template <typename T> struct is_union_impl
{
   static const bool value = false;
};
}
template< typename T > struct is_union : mpl::bool_< ::boost::detail::is_union_impl<T>::value > { };
}
namespace boost {
namespace detail {
template <class U> ::boost::type_traits::yes_type is_class_tester(void(U::*)(void));
template <class U> ::boost::type_traits::no_type is_class_tester(...);
template <typename T>
struct is_class_impl
{
    static const bool value = (::boost::type_traits::ice_and< sizeof(is_class_tester<T>(0)) == sizeof(::boost::type_traits::yes_type), ::boost::type_traits::ice_not< ::boost::is_union<T>::value >::value >::value);
};
}
template< typename T > struct is_class : mpl::bool_< ::boost::detail::is_class_impl<T>::value > { };
}
namespace boost {
namespace detail {
template <typename T>
struct is_class_or_union
{
   static const bool value = (::boost::type_traits::ice_or< ::boost::is_class<T>::value , ::boost::is_union<T>::value >::value);
};
struct int_convertible
{
    int_convertible(int);
};
template <bool is_typename_arithmetic_or_reference = true>
struct is_enum_helper
{
    template <typename T> struct type
    {
        static const bool value = false;
    };
};
template <>
struct is_enum_helper<false>
{
    template <typename T> struct type
        : ::boost::is_convertible<T,::boost::detail::int_convertible>
    {
    };
};
template <typename T> struct is_enum_impl
{
   typedef ::boost::add_reference<T> ar_t;
   typedef typename ar_t::type r_type;
   static const bool selector = (::boost::type_traits::ice_or< ::boost::is_arithmetic<T>::value , ::boost::is_reference<T>::value , ::boost::is_function<T>::value , is_class_or_union<T>::value >::value);
    typedef ::boost::detail::is_enum_helper<selector> se_t;
    typedef typename se_t::template type<r_type> helper;
    static const bool value = helper::value;
};
template<> struct is_enum_impl< void > { static const bool value = (false); };
template<> struct is_enum_impl< void const > { static const bool value = (false); };
template<> struct is_enum_impl< void volatile > { static const bool value = (false); };
template<> struct is_enum_impl< void const volatile > { static const bool value = (false); };
}
template< typename T > struct is_enum : mpl::bool_< ::boost::detail::is_enum_impl<T>::value > { };
}
namespace boost {
namespace type_traits {
template <typename T>
struct is_mem_fun_pointer_impl
{
    static const bool value = false;
};
template <class R, class T >
struct is_mem_fun_pointer_impl<R (T::*)() > { static const bool value = true; };
template <class R, class T >
struct is_mem_fun_pointer_impl<R (T::*)() const > { static const bool value = true; };
template <class R, class T >
struct is_mem_fun_pointer_impl<R (T::*)() volatile > { static const bool value = true; };
template <class R, class T >
struct is_mem_fun_pointer_impl<R (T::*)() const volatile > { static const bool value = true; };
template <class R, class T , class T0>
struct is_mem_fun_pointer_impl<R (T::*)( T0) > { static const bool value = true; };
template <class R, class T , class T0>
struct is_mem_fun_pointer_impl<R (T::*)( T0) const > { static const bool value = true; };
template <class R, class T , class T0>
struct is_mem_fun_pointer_impl<R (T::*)( T0) volatile > { static const bool value = true; };
template <class R, class T , class T0>
struct is_mem_fun_pointer_impl<R (T::*)( T0) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1) > { static const bool value = true; };
template <class R, class T , class T0 , class T1>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23) const volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23 , class T24>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23 , T24) > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23 , class T24>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23 , T24) const > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23 , class T24>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23 , T24) volatile > { static const bool value = true; };
template <class R, class T , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23 , class T24>
struct is_mem_fun_pointer_impl<R (T::*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23 , T24) const volatile > { static const bool value = true; };
}
}
namespace boost {
template< typename T > struct is_member_function_pointer : mpl::bool_< ::boost::type_traits::is_mem_fun_pointer_impl<T>::value > { };
}
namespace boost {
template< typename T > struct is_member_pointer : mpl::bool_< ::boost::is_member_function_pointer<T>::value > { };
template< typename T, typename U > struct is_member_pointer< U T::* > : mpl::bool_< true > { };
}
namespace boost {
namespace detail {
template< typename T > struct is_pointer_helper
{
    static const bool value = false;
};
template< typename T > struct is_pointer_helper<T*> { static const bool value = true; };
template< typename T > struct is_pointer_helper<T* const> { static const bool value = true; };
template< typename T > struct is_pointer_helper<T* volatile> { static const bool value = true; };
template< typename T > struct is_pointer_helper<T* const volatile> { static const bool value = true; };
template< typename T >
struct is_pointer_impl
{
    static const bool value = (::boost::type_traits::ice_and< ::boost::detail::is_pointer_helper<T>::value , ::boost::type_traits::ice_not< ::boost::is_member_pointer<T>::value >::value >::value);
};
}
template< typename T > struct is_pointer : mpl::bool_< ::boost::detail::is_pointer_impl<T>::value > { };
}
namespace boost{
namespace detail{
template <typename T, bool small_>
struct ct_imp2
{
   typedef const T& param_type;
};
template <typename T>
struct ct_imp2<T, true>
{
   typedef const T param_type;
};
template <typename T, bool isp, bool b1>
struct ct_imp
{
   typedef const T& param_type;
};
template <typename T, bool isp>
struct ct_imp<T, isp, true>
{
   typedef typename ct_imp2<T, sizeof(T) <= sizeof(void*)>::param_type param_type;
};
template <typename T, bool b1>
struct ct_imp<T, true, b1>
{
   typedef T const param_type;
};
}
template <typename T>
struct call_traits
{
public:
   typedef T value_type;
   typedef T& reference;
   typedef const T& const_reference;
   typedef typename detail::ct_imp<
      T,
      ::boost::is_pointer<T>::value,
      ::boost::is_arithmetic<T>::value
   >::param_type param_type;
};
template <typename T>
struct call_traits<T&>
{
   typedef T& value_type;
   typedef T& reference;
   typedef const T& const_reference;
   typedef T& param_type;
};
template <typename T, std::size_t N>
struct call_traits<T [N]>
{
private:
   typedef T array_type[N];
public:
   typedef const T* value_type;
   typedef array_type& reference;
   typedef const array_type& const_reference;
   typedef const T* const param_type;
};
template <typename T, std::size_t N>
struct call_traits<const T [N]>
{
private:
   typedef const T array_type[N];
public:
   typedef const T* value_type;
   typedef array_type& reference;
   typedef const array_type& const_reference;
   typedef const T* const param_type;
};
}
#include <assert.h>
namespace boost {
  template <class T>
  struct type {};
}
namespace boost { namespace mpl {
template< std::size_t N > struct size_t;
}}
namespace boost { namespace mpl {
template< std::size_t N >
struct size_t
{
    static const std::size_t value = N;
    typedef size_t type;
    typedef std::size_t value_type;
    typedef mpl::size_t< static_cast<std::size_t>((value + 1)) > next;
    typedef mpl::size_t< static_cast<std::size_t>((value - 1)) > prior;
    operator std::size_t() const { return static_cast<std::size_t>(this->value); }
};
template< std::size_t N >
std::size_t const mpl::size_t< N >::value;
}}
namespace boost {
template <typename T> struct alignment_of;
namespace detail {
template <typename T>
struct alignment_of_hack
{
    char c;
    T t;
    alignment_of_hack();
};
template <unsigned A, unsigned S>
struct alignment_logic
{
    static const std::size_t value = A < S ? A : S;
};
template< typename T >
struct alignment_of_impl
{
    static const std::size_t value = (::boost::detail::alignment_logic< sizeof(detail::alignment_of_hack<T>) - sizeof(T), sizeof(T) >::value);
};
}
template< typename T > struct alignment_of : mpl::size_t< ::boost::detail::alignment_of_impl<T>::value > { };
template <typename T>
struct alignment_of<T&>
    : alignment_of<T*>
{
};
template<> struct alignment_of<void> : mpl::size_t< 0 > { };
template<> struct alignment_of<void const> : mpl::size_t< 0 > { };
template<> struct alignment_of<void volatile> : mpl::size_t< 0 > { };
template<> struct alignment_of<void const volatile> : mpl::size_t< 0 > { };
}
namespace boost {
namespace mpl {
struct void_;
}
}
namespace boost {
namespace mpl {
struct void_ { typedef void_ type; };
template< typename T >
struct is_void_
    : false_
{
};
template<>
struct is_void_<void_>
    : true_
{
};
}
}
namespace boost {
namespace mpl {
template<
      typename T
    , typename Tag
    , typename Arity
    >
struct lambda;
}
}
namespace boost { namespace mpl {
template< int N > struct int_;
}}
namespace boost {
namespace mpl {
template< int N > struct arg;
}
}
namespace boost {
namespace mpl {
namespace algo_ {
template<
      bool C
    , typename T1
    , typename T2
    >
struct if_c
{
    typedef T1 type;
};
template<
      typename T1
    , typename T2
    >
struct if_c<false,T1,T2>
{
    typedef T2 type;
};
template<
      typename C = void_
    , typename T1 = void_
    , typename T2 = void_
    >
struct if_
{
 private:
    typedef if_c<
          static_cast<bool>(C::value)
        , T1
        , T2
        > almost_type_;
 public:
    typedef typename almost_type_::type type;
   
};
} using namespace algo_;
namespace algo_ { template<> struct if_< void_,void_,void_ > { template< typename T1,typename T2,typename T3 , typename T4 =void_ ,typename T5 =void_ > struct apply : if_< T1,T2,T3 > { }; }; } using namespace algo_; template<> struct lambda< algo_:: if_< void_,void_,void_ > , void_ , int_<-1> > { typedef algo_:: if_< void_,void_,void_ > type; }; namespace aux { template< typename T1,typename T2,typename T3 > struct template_arity< algo_:: if_< T1,T2,T3 > > { static const int value = 3; }; template<> struct template_arity< algo_:: if_< void_,void_,void_ > > { static const int value = -1; }; }
template <class T1, class T2, class T3, class T4> struct bind3;
template <template <class T1, class T2, class T3> class F, class tag> struct quote3;
namespace aux
{
  template <
      typename T
    , typename U1,typename U2,typename U3,typename U4,typename U5
  > struct resolve_bind_arg;
  template<
        typename T
      , typename Arg
      >
  struct replace_unnamed_arg;
}
template<
      typename T1, typename T2, typename T3
    >
struct bind3<quote3<if_, void_>, T1, T2, T3>
{
    template<
          typename U1 = void_, typename U2 = void_, typename U3 = void_
        , typename U4 = void_, typename U5 = void_
        >
    struct apply
    {
     private:
        typedef quote3<if_, void_> a0;
        typedef mpl::arg< 1> n1;
        typedef aux::replace_unnamed_arg< T1,n1 > r1;
        typedef typename r1::type a1;
        typedef typename r1::next_arg n2;
        typedef typename aux::resolve_bind_arg< a1,U1,U2,U3,U4,U5 >::type t1;
        typedef aux::replace_unnamed_arg< T2,n2 > r2;
        typedef typename r2::type a2;
        typedef typename r2::next_arg n3;
        typedef typename aux::resolve_bind_arg< a2,U1,U2,U3,U4,U5 > f2;
        typedef aux::replace_unnamed_arg< T3,n3 > r3;
        typedef typename r3::type a3;
        typedef typename r3::next_arg n4;
        typedef typename aux::resolve_bind_arg< a3,U1,U2,U3,U4,U5 > f3;
        typedef typename if_<t1,f2,f3>::type f_;
     public:
        typedef typename f_::type type;
    };
};
}
}
namespace boost {
namespace detail {
template <typename T>
struct is_scalar_impl
{
   static const bool value = (::boost::type_traits::ice_or< ::boost::is_arithmetic<T>::value, ::boost::is_enum<T>::value, ::boost::is_pointer<T>::value, ::boost::is_member_pointer<T>::value >::value);
};
template <> struct is_scalar_impl<void>{ static const bool value = false; };
template <> struct is_scalar_impl<void const>{ static const bool value = false; };
template <> struct is_scalar_impl<void volatile>{ static const bool value = false; };
template <> struct is_scalar_impl<void const volatile>{ static const bool value = false; };
}
template< typename T > struct is_scalar : mpl::bool_< ::boost::detail::is_scalar_impl<T>::value > { };
}
namespace boost {
template< typename T > struct is_POD;
namespace detail {
template <typename T> struct is_pod_impl
{
    static const bool value = (::boost::type_traits::ice_or< ::boost::is_scalar<T>::value, ::boost::is_void<T>::value, false >::value);
};
template <typename T, std::size_t sz>
struct is_pod_impl<T[sz]>
    : is_pod_impl<T>
{
};
template<> struct is_pod_impl< void > { static const bool value = (true); };
template<> struct is_pod_impl< void const > { static const bool value = (true); };
template<> struct is_pod_impl< void volatile > { static const bool value = (true); };
template<> struct is_pod_impl< void const volatile > { static const bool value = (true); };
}
template< typename T > struct is_POD : mpl::bool_< ::boost::detail::is_pod_impl<T>::value > { };
template< typename T > struct is_pod : mpl::bool_< ::boost::detail::is_pod_impl<T>::value > { };
}
namespace boost{
template <bool x> struct STATIC_ASSERTION_FAILURE;
template <> struct STATIC_ASSERTION_FAILURE<true> { enum { value = 1 }; };
template<int x> struct static_assert_test{};
}
namespace boost {
namespace detail {
class alignment_dummy;
typedef void (*function_ptr)();
typedef int (alignment_dummy::*member_ptr);
typedef int (alignment_dummy::*member_function_ptr)();
template <bool found, std::size_t target, class TestType>
struct lower_alignment_helper
{
    typedef char type;
    enum { value = true };
};
template <std::size_t target, class TestType>
struct lower_alignment_helper<false,target,TestType>
{
    enum { value = (alignment_of<TestType>::value == target) };
    typedef typename mpl::if_c<value, TestType, char>::type type;
};
template <typename T>
struct has_one_T
{
  T data;
};
template <std::size_t target>
union lower_alignment
{
    enum { found0 = false };
    typename lower_alignment_helper< found0,target,char >::type t0; enum { found1 = lower_alignment_helper<found0,target,char >::value }; typename lower_alignment_helper< found1,target,short >::type t1; enum { found2 = lower_alignment_helper<found1,target,short >::value }; typename lower_alignment_helper< found2,target,int >::type t2; enum { found3 = lower_alignment_helper<found2,target,int >::value }; typename lower_alignment_helper< found3,target,long >::type t3; enum { found4 = lower_alignment_helper<found3,target,long >::value }; typename lower_alignment_helper< found4,target,float >::type t4; enum { found5 = lower_alignment_helper<found4,target,float >::value }; typename lower_alignment_helper< found5,target,double >::type t5; enum { found6 = lower_alignment_helper<found5,target,double >::value }; typename lower_alignment_helper< found6,target,long double >::type t6; enum { found7 = lower_alignment_helper<found6,target,long double >::value }; typename lower_alignment_helper< found7,target,void* >::type t7; enum { found8 = lower_alignment_helper<found7,target,void* >::value }; typename lower_alignment_helper< found8,target,function_ptr >::type t8; enum { found9 = lower_alignment_helper<found8,target,function_ptr >::value }; typename lower_alignment_helper< found9,target,member_ptr >::type t9; enum { found10 = lower_alignment_helper<found9,target,member_ptr >::value }; typename lower_alignment_helper< found10,target,member_function_ptr >::type t10; enum { found11 = lower_alignment_helper<found10,target,member_function_ptr >::value }; typename lower_alignment_helper< found11,target,boost::detail::has_one_T<char> >::type t11; enum { found12 = lower_alignment_helper<found11,target,boost::detail::has_one_T<char> >::value }; typename lower_alignment_helper< found12,target,boost::detail::has_one_T<short> >::type t12; enum { found13 = lower_alignment_helper<found12,target,boost::detail::has_one_T<short> >::value }; typename lower_alignment_helper< found13,target,boost::detail::has_one_T<int> >::type t13; enum { found14 = lower_alignment_helper<found13,target,boost::detail::has_one_T<int> >::value }; typename lower_alignment_helper< found14,target,boost::detail::has_one_T<long> >::type t14; enum { found15 = lower_alignment_helper<found14,target,boost::detail::has_one_T<long> >::value }; typename lower_alignment_helper< found15,target,boost::detail::has_one_T<float> >::type t15; enum { found16 = lower_alignment_helper<found15,target,boost::detail::has_one_T<float> >::value }; typename lower_alignment_helper< found16,target,boost::detail::has_one_T<double> >::type t16; enum { found17 = lower_alignment_helper<found16,target,boost::detail::has_one_T<double> >::value }; typename lower_alignment_helper< found17,target,boost::detail::has_one_T<long double> >::type t17; enum { found18 = lower_alignment_helper<found17,target,boost::detail::has_one_T<long double> >::value }; typename lower_alignment_helper< found18,target,boost::detail::has_one_T<void*> >::type t18; enum { found19 = lower_alignment_helper<found18,target,boost::detail::has_one_T<void*> >::value }; typename lower_alignment_helper< found19,target,boost::detail::has_one_T<function_ptr> >::type t19; enum { found20 = lower_alignment_helper<found19,target,boost::detail::has_one_T<function_ptr> >::value }; typename lower_alignment_helper< found20,target,boost::detail::has_one_T<member_ptr> >::type t20; enum { found21 = lower_alignment_helper<found20,target,boost::detail::has_one_T<member_ptr> >::value }; typename lower_alignment_helper< found21,target,boost::detail::has_one_T<member_function_ptr> >::type t21; enum { found22 = lower_alignment_helper<found21,target,boost::detail::has_one_T<member_function_ptr> >::value };
};
union max_align
{
    char t0; short t1; int t2; long t3; float t4; double t5; long double t6; void* t7; function_ptr t8; member_ptr t9; member_function_ptr t10; boost::detail::has_one_T<char> t11; boost::detail::has_one_T<short> t12; boost::detail::has_one_T<int> t13; boost::detail::has_one_T<long> t14; boost::detail::has_one_T<float> t15; boost::detail::has_one_T<double> t16; boost::detail::has_one_T<long double> t17; boost::detail::has_one_T<void*> t18; boost::detail::has_one_T<function_ptr> t19; boost::detail::has_one_T<member_ptr> t20; boost::detail::has_one_T<member_function_ptr> t21;
};
template<int TAlign, int Align>
struct is_aligned
{
    static const bool value = (TAlign >= Align) & (TAlign % Align == 0);
};
}
template<std::size_t Align>
struct is_pod< ::boost::detail::lower_alignment<Align> >
{
        static const std::size_t value = true;
};
template <int Align>
class type_with_alignment
{
    typedef detail::lower_alignment<Align> t1;
    typedef typename mpl::if_c<
          ::boost::detail::is_aligned< ::boost::alignment_of<t1>::value,Align >::value
        , t1
        , detail::max_align
        >::type align_t;
    static const std::size_t found = alignment_of<align_t>::value;
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( found >= Align ) >)> boost_static_assert_typedef_197;
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( found % Align == 0 ) >)> boost_static_assert_typedef_198;
 public:
    typedef align_t type;
};
namespace align {
struct __attribute__((__aligned__(2))) a2 {};
struct __attribute__((__aligned__(4))) a4 {};
struct __attribute__((__aligned__(8))) a8 {};
struct __attribute__((__aligned__(16))) a16 {};
struct __attribute__((__aligned__(32))) a32 {};
}
template<> class type_with_alignment<1> { public: typedef char type; };
template<> class type_with_alignment<2> { public: typedef align::a2 type; };
template<> class type_with_alignment<4> { public: typedef align::a4 type; };
template<> class type_with_alignment<8> { public: typedef align::a8 type; };
template<> class type_with_alignment<16> { public: typedef align::a16 type; };
template<> class type_with_alignment<32> { public: typedef align::a32 type; };
namespace detail {
template<> struct is_pod_impl< ::boost::align::a2 > { static const bool value = (true); };
template<> struct is_pod_impl< ::boost::align::a4 > { static const bool value = (true); };
template<> struct is_pod_impl< ::boost::align::a8 > { static const bool value = (true); };
template<> struct is_pod_impl< ::boost::align::a16 > { static const bool value = (true); };
template<> struct is_pod_impl< ::boost::align::a32 > { static const bool value = (true); };
}
}
namespace boost {
template< typename T > struct remove_reference { typedef T type; };
template< typename T > struct remove_reference<T&> { typedef T type; };
}
namespace boost { namespace mpl { namespace aux {
template< typename T >
struct nested_type_wknd
    : T::type
{
};
}}}
namespace boost {
namespace mpl {
namespace aux {
template< long C_ >
struct not_impl
    : bool_<!C_>
{
};
}
template<
      typename T = void_
    >
struct not_
    : aux::not_impl<
          ::boost::mpl::aux::nested_type_wknd<T>::value
        >
{
   
};
template<> struct not_< void_ > { template< typename T1 , typename T2 =void_ ,typename T3 =void_ ,typename T4 =void_ ,typename T5 =void_ > struct apply : not_< T1 > { }; }; template<> struct lambda< not_< void_ > , void_ , int_<-1> > { typedef not_< void_ > type; }; namespace aux { template< typename T1 > struct template_arity< not_< T1 > > { static const int value = 1; }; template<> struct template_arity< not_< void_ > > { static const int value = -1; }; }
}
}
namespace boost {
template< typename T > struct is_volatile : mpl::bool_< ::boost::detail::cv_traits_imp<T*>::is_volatile > { };
template< typename T > struct is_volatile< T& > : mpl::bool_< false > { };
}
namespace boost {
namespace detail {
template <typename T>
struct has_trivial_copy_impl
{
   static const bool value = (::boost::type_traits::ice_and< ::boost::type_traits::ice_or< ::boost::is_pod<T>::value, false >::value, ::boost::type_traits::ice_not< ::boost::is_volatile<T>::value >::value >::value);
};
}
template< typename T > struct has_trivial_copy : mpl::bool_< ::boost::detail::has_trivial_copy_impl<T>::value > { };
}
namespace boost {
template< typename T > struct has_nothrow_copy : mpl::bool_< ::boost::has_trivial_copy<T>::value > { };
}
namespace boost {
namespace detail {
template <typename RefT>
class reference_content
{
private:
    RefT content_;
public:
    ~reference_content()
    {
    }
    reference_content(RefT r)
        : content_( r )
    {
    }
    reference_content(const reference_content& operand)
        : content_( operand.content_ )
    {
    }
private:
    reference_content& operator=(const reference_content&);
public:
    RefT get() const
    {
        return content_;
    }
};
template <typename T = mpl::void_> struct make_reference_content;
template <typename T>
struct make_reference_content
{
    typedef T type;
};
template <typename T>
struct make_reference_content< T& >
{
    typedef reference_content<T&> type;
};
template <>
struct make_reference_content< mpl::void_ >
{
    template <typename T>
    struct apply
        : make_reference_content<T>
    {
    };
    typedef mpl::void_ type;
};
}
template <typename T>
struct has_nothrow_copy<
      ::boost::detail::reference_content< T& >
    >
    : mpl::true_
{
};
}
namespace boost {
namespace detail {
struct none_helper{};
typedef int none_helper::*none_t ;
}
}
#include <functional>
namespace boost {
template<class OptionalPointee>
inline
bool equal_pointees ( OptionalPointee const& x, OptionalPointee const& y )
{
  return (!x) != (!y) ? false : ( !x ? true : (*x) == (*y) ) ;
}
template<class OptionalPointee>
struct equal_pointees_t : std::binary_function<OptionalPointee,OptionalPointee,bool>
{
  bool operator() ( OptionalPointee const& x, OptionalPointee const& y ) const
    { return equal_pointees(x,y) ; }
} ;
template<class OptionalPointee>
inline
bool less_pointees ( OptionalPointee const& x, OptionalPointee const& y )
{
  return !y ? false : ( !x ? true : (*x) < (*y) ) ;
}
template<class OptionalPointee>
struct less_pointees_t : std::binary_function<OptionalPointee,OptionalPointee,bool>
{
  bool operator() ( OptionalPointee const& x, OptionalPointee const& y ) const
    { return less_pointees(x,y) ; }
} ;
}
namespace boost {
class InPlaceFactoryBase ;
class TypedInPlaceFactoryBase ;
namespace optional_detail {
template <class T>
class aligned_storage
{
    union dummy_u
    {
        char data[ sizeof(T) ];
        typename type_with_alignment<
          ::boost::alignment_of<T>::value >::type aligner_;
    } dummy_ ;
  public:
    void const* address() const { return &dummy_.data[0]; }
    void * address() { return &dummy_.data[0]; }
} ;
template<class T>
struct types_when_isnt_ref
{
  typedef T const& reference_const_type ;
  typedef T & reference_type ;
  typedef T const* pointer_const_type ;
  typedef T * pointer_type ;
  typedef T const& argument_type ;
} ;
template<class T>
struct types_when_is_ref
{
  typedef typename remove_reference<T>::type raw_type ;
  typedef raw_type& reference_const_type ;
  typedef raw_type& reference_type ;
  typedef raw_type* pointer_const_type ;
  typedef raw_type* pointer_type ;
  typedef raw_type& argument_type ;
} ;
struct optional_tag {} ;
template<class T>
class optional_base : public optional_tag
{
  private :
    typedef typename detail::make_reference_content<T>::type internal_type ;
    typedef aligned_storage<internal_type> storage_type ;
    typedef types_when_isnt_ref<T> types_when_not_ref ;
    typedef types_when_is_ref<T> types_when_ref ;
    typedef optional_base<T> this_type ;
  protected :
    typedef T value_type ;
    typedef mpl::true_ is_reference_tag ;
    typedef mpl::false_ is_not_reference_tag ;
    typedef typename is_reference<T>::type is_reference_predicate ;
    typedef typename mpl::if_<is_reference_predicate,types_when_ref,types_when_not_ref>::type types ;
    typedef bool (this_type::*unspecified_bool_type)() const;
    typedef typename types::reference_type reference_type ;
    typedef typename types::reference_const_type reference_const_type ;
    typedef typename types::pointer_type pointer_type ;
    typedef typename types::pointer_const_type pointer_const_type ;
    typedef typename types::argument_type argument_type ;
    optional_base()
      :
      m_initialized(false) {}
    optional_base ( detail::none_t const& )
      :
      m_initialized(false) {}
    optional_base ( argument_type val )
      :
      m_initialized(false)
    {
      construct(val);
    }
    optional_base ( optional_base const& rhs )
      :
      m_initialized(false)
    {
      if ( rhs.is_initialized() )
        construct(rhs.get_impl());
    }
    template<class Expr>
    explicit optional_base ( Expr const& expr, Expr const* tag )
      :
      m_initialized(false)
    {
      construct(expr,tag);
    }
    ~optional_base() { destroy() ; }
    void assign ( optional_base const& rhs )
      {
        destroy();
        if ( rhs.is_initialized() )
          construct(rhs.get_impl());
      }
    void assign ( argument_type val )
      {
        destroy();
        construct(val);
      }
    void assign ( detail::none_t const& ) { destroy(); }
    template<class Expr>
    void assign_expr ( Expr const& expr, Expr const* tag )
      {
        destroy();
        construct(expr,tag);
      }
  public :
    void reset() { destroy(); }
    void reset ( argument_type val ) { assign(val); }
    pointer_const_type get_ptr() const { return m_initialized ? get_ptr_impl() : 0 ; }
    pointer_type get_ptr() { return m_initialized ? get_ptr_impl() : 0 ; }
    bool is_initialized() const { return m_initialized ; }
  protected :
    void construct ( argument_type val )
     {
       new (m_storage.address()) internal_type(val) ;
       m_initialized = true ;
     }
    template<class Expr>
    void construct ( Expr const& factory, InPlaceFactoryBase const* )
     {
       typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ::boost::mpl::not_<is_reference_predicate>::value ) >)> boost_static_assert_typedef_268 ;
       factory.template apply<value_type>(m_storage.address()) ;
       m_initialized = true ;
     }
    template<class Expr>
    void construct ( Expr const& factory, TypedInPlaceFactoryBase const* )
     {
       typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ::boost::mpl::not_<is_reference_predicate>::value ) >)> boost_static_assert_typedef_277 ;
       factory.apply(m_storage.address()) ;
       m_initialized = true ;
     }
    template<class Expr>
    void construct ( Expr const& expr, void const* )
     {
       new (m_storage.address()) internal_type(expr) ;
       m_initialized = true ;
     }
    void destroy()
      {
        if ( m_initialized )
          destroy_impl(is_reference_predicate()) ;
      }
    unspecified_bool_type safe_bool() const { return m_initialized ? &this_type::is_initialized : 0 ; }
    reference_const_type get_impl() const { return dereference(get_object(), is_reference_predicate() ) ; }
    reference_type get_impl() { return dereference(get_object(), is_reference_predicate() ) ; }
    pointer_const_type get_ptr_impl() const { return cast_ptr(get_object(), is_reference_predicate() ) ; }
    pointer_type get_ptr_impl() { return cast_ptr(get_object(), is_reference_predicate() ) ; }
  private :
    internal_type const* get_object() const { return static_cast<internal_type const*>(m_storage.address()); }
    internal_type * get_object() { return static_cast<internal_type *> (m_storage.address()); }
    reference_const_type dereference( internal_type const* p, is_not_reference_tag ) const { return *p ; }
    reference_type dereference( internal_type* p, is_not_reference_tag ) { return *p ; }
    reference_const_type dereference( internal_type const* p, is_reference_tag ) const { return p->get() ; }
    reference_type dereference( internal_type* p, is_reference_tag ) { return p->get() ; }
    void destroy_impl ( is_not_reference_tag ) { get_impl().~T() ; m_initialized = false ; }
    void destroy_impl ( is_reference_tag ) { m_initialized = false ; }
    pointer_const_type cast_ptr( internal_type const* p, is_not_reference_tag ) const { return p ; }
    pointer_type cast_ptr( internal_type * p, is_not_reference_tag ) { return p ; }
    bool m_initialized ;
    storage_type m_storage ;
} ;
}
template<class T>
class optional : public optional_detail::optional_base<T>
{
    typedef optional_detail::optional_base<T> base ;
    typedef typename base::unspecified_bool_type unspecified_bool_type ;
  public :
    typedef optional<T> this_type ;
    typedef typename base::value_type value_type ;
    typedef typename base::reference_type reference_type ;
    typedef typename base::reference_const_type reference_const_type ;
    typedef typename base::pointer_type pointer_type ;
    typedef typename base::pointer_const_type pointer_const_type ;
    typedef typename base::argument_type argument_type ;
    optional() : base() {}
    optional( detail::none_t const& none_ ) : base(none_) {}
    optional ( argument_type val ) : base(val) {}
    template<class U>
    explicit optional ( optional<U> const& rhs )
      :
      base()
    {
      if ( rhs.is_initialized() )
        this->construct(rhs.get());
    }
    template<class Expr>
    explicit optional ( Expr const& expr ) : base(expr,&expr) {}
    optional ( optional const& rhs ) : base(rhs) {}
    ~optional() {}
    template<class Expr>
    optional& operator= ( Expr expr )
      {
        this->assign_expr(expr,&expr);
        return *this ;
      }
    template<class U>
    optional& operator= ( optional<U> const& rhs )
      {
        this->destroy();
        if ( rhs.is_initialized() )
        {
          this->assign(rhs.get());
        }
        return *this ;
      }
    optional& operator= ( optional const& rhs )
      {
        this->assign( rhs ) ;
        return *this ;
      }
    optional& operator= ( argument_type val )
      {
        this->assign( val ) ;
        return *this ;
      }
    optional& operator= ( detail::none_t const& none_ )
      {
        this->assign( none_ ) ;
        return *this ;
      }
    reference_const_type get() const { ((void)0) ; return this->get_impl(); }
    reference_type get() { ((void)0) ; return this->get_impl(); }
    pointer_const_type operator->() const { ((void)0) ; return this->get_ptr_impl() ; }
    pointer_type operator->() { ((void)0) ; return this->get_ptr_impl() ; }
    reference_const_type operator *() const { return this->get() ; }
    reference_type operator *() { return this->get() ; }
    operator unspecified_bool_type() const { return this->safe_bool() ; }
       bool operator!() const { return !this->is_initialized() ; }
} ;
template<class T>
inline
typename optional<T>::reference_const_type
get ( optional<T> const& opt )
{
  return opt.get() ;
}
template<class T>
inline
typename optional<T>::reference_type
get ( optional<T>& opt )
{
  return opt.get() ;
}
template<class T>
inline
typename optional<T>::pointer_const_type
get ( optional<T> const* opt )
{
  return opt->get_ptr() ;
}
template<class T>
inline
typename optional<T>::pointer_type
get ( optional<T>* opt )
{
  return opt->get_ptr() ;
}
template<class T>
inline
typename optional<T>::pointer_const_type
get_pointer ( optional<T> const& opt )
{
  return opt.get_ptr() ;
}
template<class T>
inline
typename optional<T>::pointer_type
get_pointer ( optional<T>& opt )
{
  return opt.get_ptr() ;
}
template<class T>
inline
bool operator == ( optional<T> const& x, optional<T> const& y )
{ return equal_pointees(x,y); }
template<class T>
inline
bool operator < ( optional<T> const& x, optional<T> const& y )
{ return less_pointees(x,y); }
template<class T>
inline
bool operator != ( optional<T> const& x, optional<T> const& y )
{ return !( x == y ) ; }
template<class T>
inline
bool operator > ( optional<T> const& x, optional<T> const& y )
{ return y < x ; }
template<class T>
inline
bool operator <= ( optional<T> const& x, optional<T> const& y )
{ return !( y < x ) ; }
template<class T>
inline
bool operator >= ( optional<T> const& x, optional<T> const& y )
{ return !( x < y ) ; }
template<class T>
inline
bool operator == ( optional<T> const& x, detail::none_t const& )
{ return equal_pointees(x, optional<T>() ); }
template<class T>
inline
bool operator < ( optional<T> const& x, detail::none_t const& )
{ return less_pointees(x,optional<T>() ); }
template<class T>
inline
bool operator != ( optional<T> const& x, detail::none_t const& y )
{ return !( x == y ) ; }
template<class T>
inline
bool operator > ( optional<T> const& x, detail::none_t const& y )
{ return y < x ; }
template<class T>
inline
bool operator <= ( optional<T> const& x, detail::none_t const& y )
{ return !( y < x ) ; }
template<class T>
inline
bool operator >= ( optional<T> const& x, detail::none_t const& y )
{ return !( x < y ) ; }
template<class T>
inline
bool operator == ( detail::none_t const& x, optional<T> const& y )
{ return equal_pointees(optional<T>() ,y); }
template<class T>
inline
bool operator < ( detail::none_t const& x, optional<T> const& y )
{ return less_pointees(optional<T>() ,y); }
template<class T>
inline
bool operator != ( detail::none_t const& x, optional<T> const& y )
{ return !( x == y ) ; }
template<class T>
inline
bool operator > ( detail::none_t const& x, optional<T> const& y )
{ return y < x ; }
template<class T>
inline
bool operator <= ( detail::none_t const& x, optional<T> const& y )
{ return !( y < x ) ; }
template<class T>
inline
bool operator >= ( detail::none_t const& x, optional<T> const& y )
{ return !( x < y ) ; }
namespace optional_detail {
template<class T>
inline
void optional_swap ( optional<T>& x, optional<T>& y )
{
  if ( !x && !!y )
  {
    x.reset(*y);
    y.reset();
  }
  else if ( !!x && !y )
  {
    y.reset(*x);
    x.reset();
  }
  else if ( !!x && !!y )
  {
    using std::swap ;
    swap(*x,*y);
  }
}
}
template<class T> inline void swap ( optional<T>& x, optional<T>& y )
{
  optional_detail::optional_swap(x,y);
}
}
namespace boost
{
template<class E> void throw_exception(E const & e)
{
    throw e;
}
}
namespace boost { namespace spirit
{
    namespace impl
    {
        template <typename T>
        struct no_base {};
        template <typename T>
        struct safe_bool_impl
        {
            typedef T* TP;
            TP stub;
            typedef TP safe_bool_impl::*type;
        };
    }
    template <typename DerivedT, typename BaseT = impl::no_base<DerivedT> >
    struct safe_bool : BaseT
    {
    private:
        typedef impl::safe_bool_impl<DerivedT> impl_t;
        typedef typename impl_t::type bool_type;
    public:
        operator bool_type() const
        {
            return static_cast<const DerivedT*>(this)->operator_bool() ?
                &impl_t::stub : 0;
        }
        operator bool_type()
        {
            return static_cast<DerivedT*>(this)->operator_bool() ?
                &impl_t::stub : 0;
        }
    };
}}
namespace boost { namespace spirit { namespace impl
{
    template <typename T>
    struct match_attr_traits
    {
        typedef typename
            boost::optional<T>::reference_const_type
        const_reference;
        static void
        convert(boost::optional<T>& dest, const_reference src)
        { dest.reset(src); }
        static void
        convert(boost::optional<T>& dest, ... )
        { dest.reset(); }
        template <typename OtherMatchT>
        static void
        copy(boost::optional<T>& dest, OtherMatchT const& src)
        {
            if (src.has_valid_attribute())
                convert(dest, src.value());
        }
        template <typename OtherMatchT>
        static void
        assign(boost::optional<T>& dest, OtherMatchT const& src)
        {
            if (src.has_valid_attribute())
                convert(dest, src.value());
            else
                dest.reset();
        }
        template <typename ValueT>
        static void
        set_value(boost::optional<T>& dest, ValueT const& val, mpl::false_)
        {
            dest.reset(val);
        }
        template <typename ValueT>
        static void
        set_value(boost::optional<T>& dest, ValueT const& val, mpl::true_)
        {
            dest.get() = val;
        }
    };
}}}
namespace boost {
template< typename T > struct add_const { typedef T const type; };
template< typename T > struct add_const<T&> { typedef T& type; };
}
namespace boost { namespace spirit
{
    template <typename T = nil_t>
    class match : public safe_bool<match<T> >
    {
    public:
        typedef typename boost::optional<T> optional_type;
        typedef typename optional_type::argument_type ctor_param_t;
        typedef typename optional_type::reference_const_type return_t;
        typedef T attr_t;
                                match();
        explicit match(std::size_t length);
                                match(std::size_t length, ctor_param_t val);
        bool operator!() const;
        std::ptrdiff_t length() const;
        bool has_valid_attribute() const;
        return_t value() const;
        void swap(match& other);
        template <typename T2>
        match(match<T2> const& other)
        : len(other.length()), val()
        {
            impl::match_attr_traits<T>::copy(val, other);
        }
        template <typename T2>
        match&
        operator=(match<T2> const& other)
        {
            impl::match_attr_traits<T>::assign(val, other);
            len = other.length();
            return *this;
        }
        template <typename MatchT>
        void
        concat(MatchT const& other)
        {
            ;
            len += other.length();
        }
        template <typename ValueT>
        void
        value(ValueT const& val_)
        {
            impl::match_attr_traits<T>::set_value(val, val_, is_reference<T>());
        }
        bool operator_bool() const
        {
            return len >= 0;
        }
    private:
        std::ptrdiff_t len;
        optional_type val;
    };
    template <>
    class match<nil_t> : public safe_bool<match<nil_t> >
    {
    public:
        typedef nil_t attr_t;
        typedef nil_t return_t;
                                match();
        explicit match(std::size_t length);
                                match(std::size_t length, nil_t);
        bool operator!() const;
        bool has_valid_attribute() const;
        std::ptrdiff_t length() const;
        nil_t value() const;
        void value(nil_t);
        void swap(match& other);
        template <typename T>
        match(match<T> const& other)
        : len(other.length()) {}
        template <typename T>
        match<>&
        operator=(match<T> const& other)
        {
            len = other.length();
            return *this;
        }
        template <typename T>
        void
        concat(match<T> const& other)
        {
            ;
            len += other.length();
        }
        bool operator_bool() const
        {
            return len >= 0;
        }
    private:
        std::ptrdiff_t len;
    };
}}
namespace boost { namespace spirit
{
    template <typename T>
    inline match<T>::match()
    : len(-1), val() {}
    template <typename T>
    inline match<T>::match(std::size_t length)
    : len(length), val() {}
    template <typename T>
    inline match<T>::match(std::size_t length, ctor_param_t val_)
    : len(length), val(val_) {}
    template <typename T>
    inline bool
    match<T>::operator!() const
    {
        return len < 0;
    }
    template <typename T>
    inline std::ptrdiff_t
    match<T>::length() const
    {
        return len;
    }
    template <typename T>
    inline bool
    match<T>::has_valid_attribute() const
    {
        return val.is_initialized();
    }
    template <typename T>
    inline typename match<T>::return_t
    match<T>::value() const
    {
        ;
        return *val;
    }
    template <typename T>
    inline void
    match<T>::swap(match& other)
    {
        std::swap(len, other.len);
        std::swap(val, other.val);
    }
    inline match<nil_t>::match()
    : len(-1) {}
    inline match<nil_t>::match(std::size_t length)
    : len(length) {}
    inline match<nil_t>::match(std::size_t length, nil_t)
    : len(length) {}
    inline bool
    match<nil_t>::operator!() const
    {
        return len < 0;
    }
    inline bool
    match<nil_t>::has_valid_attribute() const
    {
        return false;
    }
    inline std::ptrdiff_t
    match<nil_t>::length() const
    {
        return len;
    }
    inline nil_t
    match<nil_t>::value() const
    {
        return nil_t();
    }
    inline void
    match<nil_t>::value(nil_t) {}
    inline void
    match<nil_t>::swap(match& other)
    {
        std::swap(len, other.len);
    }
}}
#include <iterator>
namespace boost { namespace spirit {
    class parser_id
    {
    public:
                    parser_id() : p(0) {}
        explicit parser_id(void const* prule) : p(prule) {}
                    parser_id(std::size_t l_) : l(l_) {}
        bool operator==(parser_id const& x) const { return p == x.p; }
        bool operator!=(parser_id const& x) const { return !(*this == x); }
        bool operator<(parser_id const& x) const { return p < x.p; }
        std::size_t to_long() const { return l; }
    private:
        union
        {
            void const* p;
            std::size_t l;
        };
    };
    struct parser_tag_base {};
    struct parser_address_tag : parser_tag_base
    {
        parser_id id() const
        { return parser_id(reinterpret_cast<std::size_t>(this)); }
    };
    template <int N>
    struct parser_tag : parser_tag_base
    {
        static parser_id id()
        { return parser_id(std::size_t(N)); }
    };
    class dynamic_parser_tag : public parser_tag_base
    {
    public:
        dynamic_parser_tag()
        : tag(std::size_t(0)) {}
        parser_id
        id() const
        {
            return
                tag.to_long()
                ? tag
                : parser_id(reinterpret_cast<std::size_t>(this));
        }
        void set_id(parser_id id) { tag = id; }
    private:
        parser_id tag;
    };
}}
namespace boost { namespace detail {
template <class Iterator>
struct iterator_traits
    : std::iterator_traits<Iterator>
{};
using std::distance;
}}
namespace boost { namespace spirit
{
    struct iteration_policy
    {
        template <typename ScannerT>
        void
        advance(ScannerT const& scan) const
        {
            ++scan.first;
        }
        template <typename ScannerT>
        bool at_end(ScannerT const& scan) const
        {
            return scan.first == scan.last;
        }
        template <typename T>
        T filter(T ch) const
        {
            return ch;
        }
        template <typename ScannerT>
        typename ScannerT::ref_t
        get(ScannerT const& scan) const
        {
            return *scan.first;
        }
    };
    struct match_policy
    {
        template <typename T>
        struct result { typedef match<T> type; };
        const match<nil_t>
        no_match() const
        {
            return match<nil_t>();
        }
        const match<nil_t>
        empty_match() const
        {
            return match<nil_t>(0, nil_t());
        }
        template <typename AttrT, typename IteratorT>
        match<AttrT>
        create_match(
            std::size_t length,
            AttrT const& val,
            IteratorT const& ,
            IteratorT const& ) const
        {
            return match<AttrT>(length, val);
        }
        template <typename MatchT, typename IteratorT>
        void group_match(
            MatchT& ,
            parser_id const& ,
            IteratorT const& ,
            IteratorT const& ) const {}
        template <typename Match1T, typename Match2T>
        void concat_match(Match1T& l, Match2T const& r) const
        {
            l.concat(r);
        }
    };
    template <typename MatchPolicyT, typename T>
    struct match_result
    {
        typedef typename MatchPolicyT::template result<T>::type type;
    };
    template <typename AttrT>
    struct attributed_action_policy
    {
        template <typename ActorT, typename IteratorT>
        static void
        call(
            ActorT const& actor,
            AttrT& val,
            IteratorT const&,
            IteratorT const&)
        {
            actor(val);
        }
    };
    template <>
    struct attributed_action_policy<nil_t>
    {
        template <typename ActorT, typename IteratorT>
        static void
        call(
            ActorT const& actor,
            nil_t,
            IteratorT const& first,
            IteratorT const& last)
        {
            actor(first, last);
        }
    };
    struct action_policy
    {
        template <typename ActorT, typename AttrT, typename IteratorT>
        void
        do_action(
            ActorT const& actor,
            AttrT& val,
            IteratorT const& first,
            IteratorT const& last) const
        {
            attributed_action_policy<AttrT>::call(actor, val, first, last);
        }
    };
    template <
        typename IterationPolicyT = iteration_policy,
        typename MatchPolicyT = match_policy,
        typename ActionPolicyT = action_policy>
    struct scanner_policies :
        public IterationPolicyT,
        public MatchPolicyT,
        public ActionPolicyT
    {
        typedef IterationPolicyT iteration_policy_t;
        typedef MatchPolicyT match_policy_t;
        typedef ActionPolicyT action_policy_t;
        scanner_policies(
            IterationPolicyT const& i_policy = IterationPolicyT(),
            MatchPolicyT const& m_policy = MatchPolicyT(),
            ActionPolicyT const& a_policy = ActionPolicyT())
        : IterationPolicyT(i_policy)
        , MatchPolicyT(m_policy)
        , ActionPolicyT(a_policy) {}
        template <typename ScannerPoliciesT>
        scanner_policies(ScannerPoliciesT const& policies)
        : IterationPolicyT(policies)
        , MatchPolicyT(policies)
        , ActionPolicyT(policies) {}
    };
    struct scanner_base {};
    template <
        typename IteratorT = char const*,
        typename PoliciesT = scanner_policies<> >
    class scanner : public PoliciesT, public scanner_base
    {
    public:
        typedef IteratorT iterator_t;
        typedef PoliciesT policies_t;
        typedef typename boost::detail::
            iterator_traits<IteratorT>::value_type value_t;
        typedef typename boost::detail::
            iterator_traits<IteratorT>::reference ref_t;
        typedef typename boost::
            call_traits<IteratorT>::param_type iter_param_t;
        scanner(
            IteratorT& first_,
            iter_param_t last_,
            PoliciesT const& policies = PoliciesT())
        : PoliciesT(policies), first(first_), last(last_)
        {
            at_end();
        }
        scanner(scanner const& other)
        : PoliciesT(other), first(other.first), last(other.last) {}
        scanner(scanner const& other, IteratorT& first_)
        : PoliciesT(other), first(first_), last(other.last) {}
        bool
        at_end() const
        {
            typedef typename PoliciesT::iteration_policy_t iteration_policy_t;
            return iteration_policy_t::at_end(*this);
        }
        value_t
        operator*() const
        {
            typedef typename PoliciesT::iteration_policy_t iteration_policy_t;
            return iteration_policy_t::filter(iteration_policy_t::get(*this));
        }
        scanner const&
        operator++() const
        {
            typedef typename PoliciesT::iteration_policy_t iteration_policy_t;
            iteration_policy_t::advance(*this);
            return *this;
        }
        template <typename PoliciesT2>
        struct rebind_policies
        {
            typedef scanner<IteratorT, PoliciesT2> type;
        };
        template <typename PoliciesT2>
        scanner<IteratorT, PoliciesT2>
        change_policies(PoliciesT2 const& policies) const
        {
            return scanner<IteratorT, PoliciesT2>(first, last, policies);
        }
        template <typename IteratorT2>
        struct rebind_iterator
        {
            typedef scanner<IteratorT2, PoliciesT> type;
        };
        template <typename IteratorT2>
        scanner<IteratorT2, PoliciesT>
        change_iterator(IteratorT2 const& first_, IteratorT2 const &last_) const
        {
            return scanner<IteratorT2, PoliciesT>(first_, last_, *this);
        }
        IteratorT& first;
        IteratorT const last;
    private:
        scanner&
        operator=(scanner const& other);
    };
    template <typename ScannerT, typename PoliciesT>
    struct rebind_scanner_policies
    {
        typedef typename ScannerT::template
            rebind_policies<PoliciesT>::type type;
    };
    template <typename ScannerT, typename IteratorT>
    struct rebind_scanner_iterator
    {
        typedef typename ScannerT::template
            rebind_iterator<IteratorT>::type type;
    };
}}
namespace boost { namespace spirit
{
    template <typename ParserT, typename ActionT>
    class action;
    struct plain_parser_category {};
    struct binary_parser_category : plain_parser_category {};
    struct unary_parser_category : plain_parser_category {};
    struct action_parser_category : unary_parser_category {};
    template <typename ParserT, typename ScannerT>
    struct parser_result
    {
        typedef typename ParserT::template result<ScannerT>::type type;
    };
    template <typename DerivedT>
    struct parser
    {
        typedef DerivedT embed_t;
        typedef DerivedT derived_t;
        typedef plain_parser_category parser_category_t;
        template <typename ScannerT>
        struct result
        {
            typedef typename match_result<ScannerT, nil_t>::type type;
        };
        DerivedT& derived()
        {
            return *static_cast<DerivedT*>(this);
        }
        DerivedT const& derived() const
        {
            return *static_cast<DerivedT const*>(this);
        }
        template <typename ActionT>
        action<DerivedT, ActionT>
        operator[](ActionT const& actor) const
        {
            return action<DerivedT, ActionT>(derived(), actor);
        }
    };
    template <typename IteratorT = char const*>
    struct parse_info
    {
        IteratorT stop;
        bool hit;
        bool full;
        std::size_t length;
        parse_info(
            IteratorT const& stop_ = IteratorT(),
            bool hit_ = false,
            bool full_ = false,
            std::size_t length_ = 0)
        : stop(stop_)
        , hit(hit_)
        , full(full_)
        , length(length_) {}
        template <typename ParseInfoT>
        parse_info(ParseInfoT const& pi)
        : stop(pi.stop)
        , hit(pi.hit)
        , full(pi.full)
        , length(pi.length) {}
    };
    template <typename IteratorT, typename DerivedT>
    parse_info<IteratorT>
    parse(
        IteratorT const& first,
        IteratorT const& last,
        parser<DerivedT> const& p);
    template <typename CharT, typename DerivedT>
    parse_info<CharT const*>
    parse(
        CharT const* str,
        parser<DerivedT> const& p);
}}
namespace boost { namespace spirit
{
    template <typename IteratorT, typename DerivedT>
    inline parse_info<IteratorT>
    parse(
        IteratorT const& first_
      , IteratorT const& last
      , parser<DerivedT> const& p)
    {
        IteratorT first = first_;
        scanner<IteratorT, scanner_policies<> > scan(first, last);
        match<nil_t> hit = p.derived().parse(scan);
        return parse_info<IteratorT>(
            first, hit, hit && (first == last), hit.length());
    }
    template <typename CharT, typename DerivedT>
    inline parse_info<CharT const*>
    parse(CharT const* str, parser<DerivedT> const& p)
    {
        CharT const* last = str;
        while (*last)
            last++;
        return parse(str, last, p);
    }
}}
namespace boost {
template <typename T> T*
addressof(T& v)
{
  return reinterpret_cast<T*>(
       &const_cast<char&>(reinterpret_cast<const volatile char &>(v)));
}
}
namespace boost
{
template<class T> class reference_wrapper
{
public:
    typedef T type;
    explicit reference_wrapper(T& t): t_(boost::addressof(t)) {}
    operator T& () const { return *t_; }
    T& get() const { return *t_; }
    T* get_pointer() const { return t_; }
private:
    T* t_;
};
template<class T> inline reference_wrapper<T> const ref(T & t)
{
    return reference_wrapper<T>(t);
}
template<class T> inline reference_wrapper<T const> const cref(T const & t)
{
    return reference_wrapper<T const>(t);
}
template<typename T>
class is_reference_wrapper
    : public mpl::false_
{
};
template<typename T>
class is_reference_wrapper<reference_wrapper<T> >
    : public mpl::true_
{
};
template<typename T>
class unwrap_reference
{
 public:
    typedef T type;
};
template<typename T>
class unwrap_reference<reference_wrapper<T> >
{
 public:
    typedef T type;
};
}
namespace boost { namespace spirit {
    template <typename DrivedT> struct char_parser;
    namespace impl
    {
        template <typename IteratorT>
        inline IteratorT
        get_last(IteratorT first)
        {
            while (*first)
                first++;
            return first;
        }
        template<
            typename RT,
            typename IteratorT,
            typename ScannerT>
        inline RT
        string_parser_parse(
            IteratorT str_first,
            IteratorT str_last,
            ScannerT& scan)
        {
            typedef typename ScannerT::iterator_t iterator_t;
            iterator_t saved = scan.first;
            std::size_t slen = str_last - str_first;
            while (str_first != str_last)
            {
                if (scan.at_end() || (*str_first != *scan))
                    return scan.no_match();
                ++str_first;
                ++scan;
            }
            return scan.create_match(slen, nil_t(), saved, scan.first);
        }
        template <typename CharT>
        inline typename
        std::char_traits<CharT>::int_type
        to_int_type(CharT c)
        {
            return std
                ::char_traits<CharT>::to_int_type(c);
        }
        template <typename CharT>
        inline CharT
        to_char_type(typename
            std::char_traits<CharT>::int_type c)
        {
            return std
                ::char_traits<CharT>::to_char_type(c);
        }
        inline bool
        isalnum_(char c)
        {
            using namespace std;
            return isalnum(to_int_type(c));
        }
        inline bool
        isalpha_(char c)
        {
            using namespace std;
            return isalpha(to_int_type(c));
        }
        inline bool
        iscntrl_(char c)
        {
            using namespace std;
            return iscntrl(to_int_type(c));
        }
        inline bool
        isdigit_(char c)
        {
            using namespace std;
            return isdigit(to_int_type(c));
        }
        inline bool
        isgraph_(char c)
        {
            using namespace std;
            return isgraph(to_int_type(c));
        }
        inline bool
        islower_(char c)
        {
            using namespace std;
            return islower(to_int_type(c));
        }
        inline bool
        isprint_(char c)
        {
            using namespace std;
            return isprint(to_int_type(c));
        }
        inline bool
        ispunct_(char c)
        {
            using namespace std;
            return ispunct(to_int_type(c));
        }
        inline bool
        isspace_(char c)
        {
            using namespace std;
            return isspace(to_int_type(c));
        }
        inline bool
        isupper_(char c)
        {
            using namespace std;
            return isupper(to_int_type(c)); }
        inline bool
        isxdigit_(char c)
        {
            using namespace std;
            return isxdigit(to_int_type(c));
        }
        inline bool
        isblank_(char c)
        {
            return (c == ' ' || c == '\t');
        }
        inline char
        tolower_(char c)
        {
            using namespace std;
            return to_char_type<char>(tolower(to_int_type(c)));
        }
        inline char
        toupper_(char c)
        {
            using namespace std;
            return to_char_type<char>(toupper(to_int_type(c)));
        }
}}}
namespace boost { namespace spirit {
    template <typename BaseT>
    struct no_skipper_iteration_policy;
    template <typename BaseT = iteration_policy>
    struct skipper_iteration_policy : public BaseT
    {
        typedef BaseT base_t;
        skipper_iteration_policy()
        : BaseT() {}
        template <typename PolicyT>
        skipper_iteration_policy(PolicyT const& other)
        : BaseT(other) {}
        template <typename ScannerT>
        void
        advance(ScannerT const& scan) const
        {
            BaseT::advance(scan);
            scan.skip(scan);
        }
        template <typename ScannerT>
        bool
        at_end(ScannerT const& scan) const
        {
            scan.skip(scan);
            return BaseT::at_end(scan);
        }
        template <typename ScannerT>
        void
        skip(ScannerT const& scan) const
        {
            while (!BaseT::at_end(scan) && impl::isspace_(BaseT::get(scan)))
                BaseT::advance(scan);
        }
    };
    namespace impl
    {
        template <typename ST, typename ScannerT, typename BaseT>
        void
        skipper_skip(
            ST const& s,
            ScannerT const& scan,
            skipper_iteration_policy<BaseT> const&);
        template <typename ST, typename ScannerT, typename BaseT>
        void
        skipper_skip(
            ST const& s,
            ScannerT const& scan,
            no_skipper_iteration_policy<BaseT> const&);
        template <typename ST, typename ScannerT>
        void
        skipper_skip(
            ST const& s,
            ScannerT const& scan,
            iteration_policy const&);
    }
    template <typename ParserT, typename BaseT = iteration_policy>
    class skip_parser_iteration_policy : public skipper_iteration_policy<BaseT>
    {
    public:
        typedef skipper_iteration_policy<BaseT> base_t;
        skip_parser_iteration_policy(
            ParserT const& skip_parser,
            base_t const& base = base_t())
        : base_t(base), subject(skip_parser) {}
        template <typename PolicyT>
        skip_parser_iteration_policy(PolicyT const& other)
        : base_t(other), subject(other.skipper()) {}
        template <typename ScannerT>
        void
        skip(ScannerT const& scan) const
        {
            impl::skipper_skip(subject, scan, scan);
        }
        ParserT const&
        skipper() const
        {
            return subject;
        }
    private:
        ParserT const& subject;
    };
    template <typename IteratorT, typename ParserT, typename SkipT>
    parse_info<IteratorT>
    parse(
        IteratorT const& first,
        IteratorT const& last,
        parser<ParserT> const& p,
        parser<SkipT> const& skip);
    template <typename CharT, typename ParserT, typename SkipT>
    parse_info<CharT const*>
    parse(
        CharT const* str,
        parser<ParserT> const& p,
        parser<SkipT> const& skip);
    typedef skipper_iteration_policy<> iter_policy_t;
    typedef scanner_policies<iter_policy_t> scanner_policies_t;
    typedef scanner<char const*, scanner_policies_t> phrase_scanner_t;
    typedef scanner<wchar_t const*, scanner_policies_t> wide_phrase_scanner_t;
}}
namespace boost { namespace spirit {
    struct space_parser;
    template <typename BaseT>
    struct no_skipper_iteration_policy;
    namespace impl
    {
        template <typename ST, typename ScannerT, typename BaseT>
        inline void
        skipper_skip(
            ST const& s,
            ScannerT const& scan,
            skipper_iteration_policy<BaseT> const&)
        {
            typedef scanner_policies<
                no_skipper_iteration_policy<
                    typename ScannerT::iteration_policy_t>,
                typename ScannerT::match_policy_t,
                typename ScannerT::action_policy_t
            > policies_t;
            scanner<typename ScannerT::iterator_t, policies_t>
                scan2(scan.first, scan.last, policies_t(scan));
            typedef typename ScannerT::iterator_t iterator_t;
            for (;;)
            {
                iterator_t save = scan.first;
                if (!s.parse(scan2))
                {
                    scan.first = save;
                    break;
                }
            }
        }
        template <typename ST, typename ScannerT, typename BaseT>
        inline void
        skipper_skip(
            ST const& s,
            ScannerT const& scan,
            no_skipper_iteration_policy<BaseT> const&)
        {
            for (;;)
            {
                typedef typename ScannerT::iterator_t iterator_t;
                iterator_t save = scan.first;
                if (!s.parse(scan))
                {
                    scan.first = save;
                    break;
                }
            }
        }
        template <typename ST, typename ScannerT>
        inline void
        skipper_skip(
            ST const& s,
            ScannerT const& scan,
            iteration_policy const&)
        {
            for (;;)
            {
                typedef typename ScannerT::iterator_t iterator_t;
                iterator_t save = scan.first;
                if (!s.parse(scan))
                {
                    scan.first = save;
                    break;
                }
            }
        }
        template <typename SkipT>
        struct phrase_parser
        {
            template <typename IteratorT, typename ParserT>
            static parse_info<IteratorT>
            parse(
                IteratorT const& first_,
                IteratorT const& last,
                ParserT const& p,
                SkipT const& skip)
            {
                typedef skip_parser_iteration_policy<SkipT> iter_policy_t;
                typedef scanner_policies<iter_policy_t> scanner_policies_t;
                typedef scanner<IteratorT, scanner_policies_t> scanner_t;
                iter_policy_t iter_policy(skip);
                scanner_policies_t policies(iter_policy);
                IteratorT first = first_;
                scanner_t scan(first, last, policies);
                match<nil_t> hit = p.parse(scan);
                scan.skip(scan);
                return parse_info<IteratorT>(
                    first, hit, hit && (first == last),
                    hit.length());
            }
        };
        template <>
        struct phrase_parser<space_parser>
        {
            template <typename IteratorT, typename ParserT>
            static parse_info<IteratorT>
            parse(
                IteratorT const& first_,
                IteratorT const& last,
                ParserT const& p,
                space_parser const&)
            {
                typedef skipper_iteration_policy<> iter_policy_t;
                typedef scanner_policies<iter_policy_t> scanner_policies_t;
                typedef scanner<IteratorT, scanner_policies_t> scanner_t;
                IteratorT first = first_;
                scanner_t scan(first, last);
                match<nil_t> hit = p.parse(scan);
                scan.skip(scan);
                return parse_info<IteratorT>(
                    first, hit, hit && (first == last),
                    hit.length());
            }
        };
    }
    template <typename IteratorT, typename ParserT, typename SkipT>
    inline parse_info<IteratorT>
    parse(
        IteratorT const& first,
        IteratorT const& last,
        parser<ParserT> const& p,
        parser<SkipT> const& skip)
    {
        return impl::phrase_parser<SkipT>::
            parse(first, last, p.derived(), skip.derived());
    }
    template <typename CharT, typename ParserT, typename SkipT>
    inline parse_info<CharT const*>
    parse(
        CharT const* str,
        parser<ParserT> const& p,
        parser<SkipT> const& skip)
    {
        CharT const* last = str;
        while (*last)
            last++;
        return parse(str, last, p, skip);
    }
}}
namespace boost { namespace spirit {
    template <typename BaseT>
    struct no_skipper_iteration_policy;
    template <typename BaseT>
    struct inhibit_case_iteration_policy;
    template <typename A, typename B>
    struct alternative;
    template <typename A, typename B>
    struct longest_alternative;
    template <typename A, typename B>
    struct shortest_alternative;
    namespace impl
    {
        template <typename RT, typename ST, typename ScannerT, typename BaseT>
        inline RT
        contiguous_parser_parse(
            ST const& s,
            ScannerT const& scan,
            skipper_iteration_policy<BaseT> const&)
        {
            typedef scanner_policies<
                no_skipper_iteration_policy<
                    typename ScannerT::iteration_policy_t>,
                typename ScannerT::match_policy_t,
                typename ScannerT::action_policy_t
            > policies_t;
            scan.skip(scan);
            RT hit = s.parse(scan.change_policies(policies_t(scan)));
            return hit;
        }
        template <typename RT, typename ST, typename ScannerT, typename BaseT>
        inline RT
        contiguous_parser_parse(
            ST const& s,
            ScannerT const& scan,
            no_skipper_iteration_policy<BaseT> const&)
        {
            return s.parse(scan);
        }
        template <typename RT, typename ST, typename ScannerT>
        inline RT
        contiguous_parser_parse(
            ST const& s,
            ScannerT const& scan,
            iteration_policy const&)
        {
            return s.parse(scan);
        }
        template <
            typename RT,
            typename ParserT,
            typename ScannerT,
            typename BaseT>
        inline RT
        implicit_lexeme_parse(
            ParserT const& p,
            ScannerT const& scan,
            skipper_iteration_policy<BaseT> const&)
        {
            typedef scanner_policies<
                no_skipper_iteration_policy<
                    typename ScannerT::iteration_policy_t>,
                typename ScannerT::match_policy_t,
                typename ScannerT::action_policy_t
            > policies_t;
            scan.skip(scan);
            RT hit = p.parse_main(scan.change_policies(policies_t(scan)));
            return hit;
        }
        template <
            typename RT,
            typename ParserT,
            typename ScannerT,
            typename BaseT>
        inline RT
        implicit_lexeme_parse(
            ParserT const& p,
            ScannerT const& scan,
            no_skipper_iteration_policy<BaseT> const&)
        {
            return p.parse_main(scan);
        }
        template <typename RT, typename ParserT, typename ScannerT>
        inline RT
        implicit_lexeme_parse(
            ParserT const& p,
            ScannerT const& scan,
            iteration_policy const&)
        {
            return p.parse_main(scan);
        }
        template <typename RT, typename ST, typename ScannerT>
        inline RT
        inhibit_case_parser_parse(
            ST const& s,
            ScannerT const& scan,
            iteration_policy const&)
        {
            typedef scanner_policies<
                inhibit_case_iteration_policy<
                    typename ScannerT::iteration_policy_t>,
                typename ScannerT::match_policy_t,
                typename ScannerT::action_policy_t
            > policies_t;
            return s.parse(scan.change_policies(policies_t(scan)));
        }
        template <typename RT, typename ST, typename ScannerT, typename BaseT>
        inline RT
        inhibit_case_parser_parse(
            ST const& s,
            ScannerT const& scan,
            inhibit_case_iteration_policy<BaseT> const&)
        {
            return s.parse(scan);
        }
        template <typename T>
        struct to_longest_alternative
        {
            typedef T result_t;
            static result_t const&
            convert(T const& a)
            { return a; }
        };
        template <typename A, typename B>
        struct to_longest_alternative<alternative<A, B> >
        {
            typedef typename to_longest_alternative<A>::result_t a_t;
            typedef typename to_longest_alternative<B>::result_t b_t;
            typedef longest_alternative<a_t, b_t> result_t;
            static result_t
            convert(alternative<A, B> const& alt)
            {
                return result_t(
                    to_longest_alternative<A>::convert(alt.left()),
                    to_longest_alternative<B>::convert(alt.right()));
            }
        };
        template <typename T>
        struct to_shortest_alternative
        {
            typedef T result_t;
            static result_t const&
            convert(T const& a)
            { return a; }
        };
        template <typename A, typename B>
        struct to_shortest_alternative<alternative<A, B> >
        {
            typedef typename to_shortest_alternative<A>::result_t a_t;
            typedef typename to_shortest_alternative<B>::result_t b_t;
            typedef shortest_alternative<a_t, b_t> result_t;
            static result_t
            convert(alternative<A, B> const& alt)
            {
                return result_t(
                    to_shortest_alternative<A>::convert(alt.left()),
                    to_shortest_alternative<B>::convert(alt.right()));
            }
        };
    }
}}
namespace boost { namespace spirit {
    template <typename DerivedT>
    struct char_parser : public parser<DerivedT>
    {
        typedef DerivedT self_t;
        template <typename ScannerT>
        struct result
        {
            typedef typename match_result<
                ScannerT,
                typename ScannerT::value_t
            >::type type;
        };
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            typedef typename ScannerT::value_t value_t;
            typedef typename ScannerT::iterator_t iterator_t;
            if (!scan.at_end())
            {
                value_t ch = *scan;
                if (this->derived().test(ch))
                {
                    iterator_t save(scan.first);
                    ++scan;
                    return scan.create_match(1, ch, save, scan.first);
                }
            }
            return scan.no_match();
        }
    };
    template <typename PositiveT>
    struct negated_char_parser
    : public char_parser<negated_char_parser<PositiveT> >
    {
        typedef negated_char_parser<PositiveT> self_t;
        typedef PositiveT positive_t;
        negated_char_parser(positive_t const& p)
        : positive(p.derived()) {}
        template <typename T>
        bool test(T ch) const
        {
            return !positive.test(ch);
        }
        positive_t const positive;
    };
    template <typename ParserT>
    inline negated_char_parser<ParserT>
    operator~(char_parser<ParserT> const& p)
    {
        return negated_char_parser<ParserT>(p.derived());
    }
    template <typename ParserT>
    inline ParserT
    operator~(negated_char_parser<ParserT> const& n)
    {
        return n.positive;
    }
    template <typename CharT = char>
    struct chlit : public char_parser<chlit<CharT> >
    {
        chlit(CharT ch_)
        : ch(ch_) {}
        template <typename T>
        bool test(T ch_) const
        {
            return ch_ == ch;
        }
        CharT ch;
    };
    template <typename CharT>
    inline chlit<CharT>
    ch_p(CharT ch)
    {
        return chlit<CharT>(ch);
    }
    template <typename CharT = char>
    struct range : public char_parser<range<CharT> >
    {
        range(CharT first_, CharT last_)
        : first(first_), last(last_)
        {
            ;
        }
        template <typename T>
        bool test(T ch) const
        {
            return !(CharT(ch) < first) && !(last < CharT(ch));
        }
        CharT first;
        CharT last;
    };
    template <typename CharT>
    inline range<CharT>
    range_p(CharT first, CharT last)
    {
        return range<CharT>(first, last);
    }
    template <typename IteratorT = char const*>
    class chseq : public parser<chseq<IteratorT> >
    {
    public:
        typedef chseq<IteratorT> self_t;
        chseq(IteratorT first_, IteratorT last_)
        : first(first_), last(last_) {}
        chseq(IteratorT first_)
        : first(first_), last(impl::get_last(first_)) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename boost::unwrap_reference<IteratorT>::type striter_t;
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            return impl::string_parser_parse<result_t>(
                striter_t(first),
                striter_t(last),
                scan);
        }
    private:
        IteratorT first;
        IteratorT last;
    };
    template <typename CharT>
    inline chseq<CharT const*>
    chseq_p(CharT const* str)
    {
        return chseq<CharT const*>(str);
    }
    template <typename IteratorT>
    inline chseq<IteratorT>
    chseq_p(IteratorT first, IteratorT last)
    {
        return chseq<IteratorT>(first, last);
    }
    template <typename IteratorT = char const*>
    class strlit : public parser<strlit<IteratorT> >
    {
    public:
        typedef strlit<IteratorT> self_t;
        strlit(IteratorT first, IteratorT last)
        : seq(first, last) {}
        strlit(IteratorT first)
        : seq(first) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            return impl::contiguous_parser_parse<result_t>
                (seq, scan, scan);
        }
    private:
        chseq<IteratorT> seq;
    };
    template <typename CharT>
    inline strlit<CharT const*>
    str_p(CharT const* str)
    {
        return strlit<CharT const*>(str);
    }
    template <typename IteratorT>
    inline strlit<IteratorT>
    str_p(IteratorT first, IteratorT last)
    {
        return strlit<IteratorT>(first, last);
    }
    struct nothing_parser : public parser<nothing_parser>
    {
        typedef nothing_parser self_t;
        nothing_parser() {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            return scan.no_match();
        }
    };
    nothing_parser const nothing_p = nothing_parser();
    struct anychar_parser : public char_parser<anychar_parser>
    {
        typedef anychar_parser self_t;
        anychar_parser() {}
        template <typename CharT>
        bool test(CharT) const
        {
            return true;
        }
    };
    anychar_parser const anychar_p = anychar_parser();
    inline nothing_parser
    operator~(anychar_parser)
    {
        return nothing_p;
    }
    struct alnum_parser : public char_parser<alnum_parser>
    {
        typedef alnum_parser self_t;
        alnum_parser() {}
        template <typename CharT>
        bool test(CharT ch) const
        {
            return impl::isalnum_(ch);
        }
    };
    alnum_parser const alnum_p = alnum_parser();
    struct alpha_parser : public char_parser<alpha_parser>
    {
        typedef alpha_parser self_t;
        alpha_parser() {}
        template <typename CharT>
        bool test(CharT ch) const
        {
            return impl::isalpha_(ch);
        }
    };
    alpha_parser const alpha_p = alpha_parser();
    struct cntrl_parser : public char_parser<cntrl_parser>
    {
        typedef cntrl_parser self_t;
        cntrl_parser() {}
        template <typename CharT>
        bool test(CharT ch) const
        {
            return impl::iscntrl_(ch);
        }
    };
    cntrl_parser const cntrl_p = cntrl_parser();
    struct digit_parser : public char_parser<digit_parser>
    {
        typedef digit_parser self_t;
        digit_parser() {}
        template <typename CharT>
        bool test(CharT ch) const
        {
            return impl::isdigit_(ch);
        }
    };
    digit_parser const digit_p = digit_parser();
    struct graph_parser : public char_parser<graph_parser>
    {
        typedef graph_parser self_t;
        graph_parser() {}
        template <typename CharT>
        bool test(CharT ch) const
        {
            return impl::isgraph_(ch);
        }
    };
    graph_parser const graph_p = graph_parser();
    struct lower_parser : public char_parser<lower_parser>
    {
        typedef lower_parser self_t;
        lower_parser() {}
        template <typename CharT>
        bool test(CharT ch) const
        {
            return impl::islower_(ch);
        }
    };
    lower_parser const lower_p = lower_parser();
    struct print_parser : public char_parser<print_parser>
    {
        typedef print_parser self_t;
        print_parser() {}
        template <typename CharT>
        bool test(CharT ch) const
        {
            return impl::isprint_(ch);
        }
    };
    print_parser const print_p = print_parser();
    struct punct_parser : public char_parser<punct_parser>
    {
        typedef punct_parser self_t;
        punct_parser() {}
        template <typename CharT>
        bool test(CharT ch) const
        {
            return impl::ispunct_(ch);
        }
    };
    punct_parser const punct_p = punct_parser();
    struct blank_parser : public char_parser<blank_parser>
    {
        typedef blank_parser self_t;
        blank_parser() {}
        template <typename CharT>
        bool test(CharT ch) const
        {
            return impl::isblank_(ch);
        }
    };
    blank_parser const blank_p = blank_parser();
    struct space_parser : public char_parser<space_parser>
    {
        typedef space_parser self_t;
        space_parser() {}
        template <typename CharT>
        bool test(CharT ch) const
        {
            return impl::isspace_(ch);
        }
    };
    space_parser const space_p = space_parser();
    struct upper_parser : public char_parser<upper_parser>
    {
        typedef upper_parser self_t;
        upper_parser() {}
        template <typename CharT>
        bool test(CharT ch) const
        {
            return impl::isupper_(ch);
        }
    };
    upper_parser const upper_p = upper_parser();
    struct xdigit_parser : public char_parser<xdigit_parser>
    {
        typedef xdigit_parser self_t;
        xdigit_parser() {}
        template <typename CharT>
        bool test(CharT ch) const
        {
            return impl::isxdigit_(ch);
        }
    };
    xdigit_parser const xdigit_p = xdigit_parser();
    struct eol_parser : public parser<eol_parser>
    {
        typedef eol_parser self_t;
        eol_parser() {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typename ScannerT::iterator_t save = scan.first;
            std::size_t len = 0;
            if (!scan.at_end() && *scan == '\r')
            {
                ++scan;
                ++len;
            }
            if (!scan.at_end() && *scan == '\n')
            {
                ++scan;
                ++len;
            }
            if (len)
                return scan.create_match(len, nil_t(), save, scan.first);
            return scan.no_match();
        }
    };
    eol_parser const eol_p = eol_parser();
    struct end_parser : public parser<end_parser>
    {
        typedef end_parser self_t;
        end_parser() {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            if (scan.at_end())
                return scan.empty_match();
            return scan.no_match();
        }
    };
    end_parser const end_p = end_parser();
    inline strlit<char const*> const
    pizza_p(char const* your_favorite_pizza)
    {
        return your_favorite_pizza;
    }
}}
namespace boost {
template< typename T > struct is_const : mpl::bool_< ::boost::detail::cv_traits_imp<T*>::is_const > { };
template< typename T > struct is_const< T& > : mpl::bool_< false > { };
}
namespace boost {
namespace detail {
template <typename T>
struct has_trivial_assign_impl
{
   static const bool value = (::boost::type_traits::ice_and< ::boost::type_traits::ice_or< ::boost::is_pod<T>::value, false >::value, ::boost::type_traits::ice_not< ::boost::is_const<T>::value >::value, ::boost::type_traits::ice_not< ::boost::is_volatile<T>::value >::value >::value);
};
}
template< typename T > struct has_trivial_assign : mpl::bool_< ::boost::detail::has_trivial_assign_impl<T>::value > { };
}
namespace boost {
namespace detail {
template <typename T>
struct has_trivial_ctor_impl
{
   static const bool value = (::boost::type_traits::ice_or< ::boost::is_pod<T>::value, false >::value);
};
}
template< typename T > struct has_trivial_constructor : mpl::bool_< ::boost::detail::has_trivial_ctor_impl<T>::value > { };
}
namespace boost {
namespace detail {
template <typename T>
struct has_trivial_dtor_impl
{
   static const bool value = (::boost::type_traits::ice_or< ::boost::is_pod<T>::value, false >::value);
};
}
template< typename T > struct has_trivial_destructor : mpl::bool_< ::boost::detail::has_trivial_dtor_impl<T>::value > { };
}
namespace boost {
template< typename T > struct has_nothrow_constructor : mpl::bool_< ::boost::has_trivial_constructor<T>::value > { };
}
namespace boost {
template< typename T > struct has_nothrow_assign : mpl::bool_< ::boost::has_trivial_assign<T>::value > { };
}
namespace boost {
template< typename T, typename U > struct is_same : mpl::bool_< false > { };
template< typename T > struct is_same< T,T > : mpl::bool_< true > { };
}
namespace boost {
namespace detail {
template <typename B, typename D>
struct bd_helper
{
    template <typename T>
    static type_traits::yes_type check(D const volatile *, T);
    static type_traits::no_type check(B const volatile *, int);
};
template<typename B, typename D>
struct is_base_and_derived_impl2
{
    struct Host
    {
        operator B const volatile *() const;
        operator D const volatile *();
    };
    static const bool value = sizeof(bd_helper<B,D>::check(Host(), 0)) == sizeof(type_traits::yes_type);
};
template <typename B, typename D>
struct is_base_and_derived_impl3
{
    static const bool value = false;
};
template <bool ic1, bool ic2, bool iss>
struct is_base_and_derived_select
{
   template <class T, class U>
   struct rebind
   {
      typedef is_base_and_derived_impl3<T,U> type;
   };
};
template <>
struct is_base_and_derived_select<true,true,false>
{
   template <class T, class U>
   struct rebind
   {
      typedef is_base_and_derived_impl2<T,U> type;
   };
};
template <typename B, typename D>
struct is_base_and_derived_impl
{
    typedef typename remove_cv<B>::type ncvB;
    typedef typename remove_cv<D>::type ncvD;
    typedef is_base_and_derived_select<
       ::boost::is_class<B>::value,
       ::boost::is_class<D>::value,
       ::boost::is_same<B,D>::value> selector;
    typedef typename selector::template rebind<ncvB,ncvD> binder;
    typedef typename binder::type bound_type;
    static const bool value = bound_type::value;
};
}
template< typename Base, typename Derived > struct is_base_and_derived : mpl::bool_< (::boost::detail::is_base_and_derived_impl<Base,Derived>::value) > { };
template< typename Base, typename Derived > struct is_base_and_derived< Base&,Derived > : mpl::bool_< false > { };
template< typename Base, typename Derived > struct is_base_and_derived< Base,Derived& > : mpl::bool_< false > { };
template< typename Base, typename Derived > struct is_base_and_derived< Base&,Derived& > : mpl::bool_< false > { };
}
namespace boost {
namespace detail {
template <typename T>
struct is_compound_impl
{
   static const bool value = (::boost::type_traits::ice_not< ::boost::is_fundamental<T>::value >::value);
};
}
template< typename T > struct is_compound : mpl::bool_< ::boost::detail::is_compound_impl<T>::value > { };
}
namespace boost {
namespace detail {
template <typename T>
struct empty_helper_t1 : public T
{
    empty_helper_t1();
    int i[256];
};
struct empty_helper_t2 { int i[256]; };
template <typename T, bool is_a_class = false>
struct empty_helper
{
    static const bool value = false;
};
template <typename T>
struct empty_helper<T, true>
{
    static const bool value = (sizeof(empty_helper_t1<T>) == sizeof(empty_helper_t2));
};
template <typename T>
struct is_empty_impl
{
    typedef typename remove_cv<T>::type cvt;
    static const bool value = ( ::boost::type_traits::ice_or< ::boost::detail::empty_helper<cvt,::boost::is_class<T>::value>::value , false >::value );
};
template<> struct is_empty_impl< void > { static const bool value = (false); };
template<> struct is_empty_impl< void const > { static const bool value = (false); };
template<> struct is_empty_impl< void volatile > { static const bool value = (false); };
template<> struct is_empty_impl< void const volatile > { static const bool value = (false); };
}
template< typename T > struct is_empty : mpl::bool_< ::boost::detail::is_empty_impl<T>::value > { };
}
namespace boost {
namespace detail {
template <typename T>
struct is_object_impl
{
   static const bool value = (::boost::type_traits::ice_and< ::boost::type_traits::ice_not< ::boost::is_reference<T>::value>::value, ::boost::type_traits::ice_not< ::boost::is_void<T>::value>::value, ::boost::type_traits::ice_not< ::boost::is_function<T>::value>::value >::value);
};
}
template< typename T > struct is_object : mpl::bool_< ::boost::detail::is_object_impl<T>::value > { };
}
namespace boost {
namespace detail {
template <typename T>
struct is_stateless_impl
{
  static const bool value = (::boost::type_traits::ice_and< ::boost::has_trivial_constructor<T>::value, ::boost::has_trivial_copy<T>::value, ::boost::has_trivial_destructor<T>::value, ::boost::is_class<T>::value, ::boost::is_empty<T>::value >::value);
};
}
template< typename T > struct is_stateless : mpl::bool_< ::boost::detail::is_stateless_impl<T>::value > { };
}
namespace boost
{
template <class T1, class T2>
class compressed_pair;
namespace details
{
   template <class T1, class T2, bool IsSame, bool FirstEmpty, bool SecondEmpty>
   struct compressed_pair_switch;
   template <class T1, class T2>
   struct compressed_pair_switch<T1, T2, false, false, false>
      {static const int value = 0;};
   template <class T1, class T2>
   struct compressed_pair_switch<T1, T2, false, true, true>
      {static const int value = 3;};
   template <class T1, class T2>
   struct compressed_pair_switch<T1, T2, false, true, false>
      {static const int value = 1;};
   template <class T1, class T2>
   struct compressed_pair_switch<T1, T2, false, false, true>
      {static const int value = 2;};
   template <class T1, class T2>
   struct compressed_pair_switch<T1, T2, true, true, true>
      {static const int value = 4;};
   template <class T1, class T2>
   struct compressed_pair_switch<T1, T2, true, false, false>
      {static const int value = 5;};
   template <class T1, class T2, int Version> class compressed_pair_imp;
   using std::swap;
   template <typename T>
   inline void cp_swap(T& t1, T& t2)
   {
      swap(t1, t2);
   }
   template <class T1, class T2>
   class compressed_pair_imp<T1, T2, 0>
   {
   public:
      typedef T1 first_type;
      typedef T2 second_type;
      typedef typename call_traits<first_type>::param_type first_param_type;
      typedef typename call_traits<second_type>::param_type second_param_type;
      typedef typename call_traits<first_type>::reference first_reference;
      typedef typename call_traits<second_type>::reference second_reference;
      typedef typename call_traits<first_type>::const_reference first_const_reference;
      typedef typename call_traits<second_type>::const_reference second_const_reference;
      compressed_pair_imp() {}
      compressed_pair_imp(first_param_type x, second_param_type y)
         : first_(x), second_(y) {}
      compressed_pair_imp(first_param_type x)
         : first_(x) {}
      compressed_pair_imp(second_param_type y)
         : second_(y) {}
      first_reference first() {return first_;}
      first_const_reference first() const {return first_;}
      second_reference second() {return second_;}
      second_const_reference second() const {return second_;}
      void swap(::boost::compressed_pair<T1, T2>& y)
      {
         cp_swap(first_, y.first());
         cp_swap(second_, y.second());
      }
   private:
      first_type first_;
      second_type second_;
   };
   template <class T1, class T2>
   class compressed_pair_imp<T1, T2, 1>
      : private T1
   {
   public:
      typedef T1 first_type;
      typedef T2 second_type;
      typedef typename call_traits<first_type>::param_type first_param_type;
      typedef typename call_traits<second_type>::param_type second_param_type;
      typedef typename call_traits<first_type>::reference first_reference;
      typedef typename call_traits<second_type>::reference second_reference;
      typedef typename call_traits<first_type>::const_reference first_const_reference;
      typedef typename call_traits<second_type>::const_reference second_const_reference;
      compressed_pair_imp() {}
      compressed_pair_imp(first_param_type x, second_param_type y)
         : first_type(x), second_(y) {}
      compressed_pair_imp(first_param_type x)
         : first_type(x) {}
      compressed_pair_imp(second_param_type y)
         : second_(y) {}
      first_reference first() {return *this;}
      first_const_reference first() const {return *this;}
      second_reference second() {return second_;}
      second_const_reference second() const {return second_;}
      void swap(::boost::compressed_pair<T1,T2>& y)
      {
         cp_swap(second_, y.second());
      }
   private:
      second_type second_;
   };
   template <class T1, class T2>
   class compressed_pair_imp<T1, T2, 2>
      : private T2
   {
   public:
      typedef T1 first_type;
      typedef T2 second_type;
      typedef typename call_traits<first_type>::param_type first_param_type;
      typedef typename call_traits<second_type>::param_type second_param_type;
      typedef typename call_traits<first_type>::reference first_reference;
      typedef typename call_traits<second_type>::reference second_reference;
      typedef typename call_traits<first_type>::const_reference first_const_reference;
      typedef typename call_traits<second_type>::const_reference second_const_reference;
      compressed_pair_imp() {}
      compressed_pair_imp(first_param_type x, second_param_type y)
         : second_type(y), first_(x) {}
      compressed_pair_imp(first_param_type x)
         : first_(x) {}
      compressed_pair_imp(second_param_type y)
         : second_type(y) {}
      first_reference first() {return first_;}
      first_const_reference first() const {return first_;}
      second_reference second() {return *this;}
      second_const_reference second() const {return *this;}
      void swap(::boost::compressed_pair<T1,T2>& y)
      {
         cp_swap(first_, y.first());
      }
   private:
      first_type first_;
   };
   template <class T1, class T2>
   class compressed_pair_imp<T1, T2, 3>
      : private T1,
        private T2
   {
   public:
      typedef T1 first_type;
      typedef T2 second_type;
      typedef typename call_traits<first_type>::param_type first_param_type;
      typedef typename call_traits<second_type>::param_type second_param_type;
      typedef typename call_traits<first_type>::reference first_reference;
      typedef typename call_traits<second_type>::reference second_reference;
      typedef typename call_traits<first_type>::const_reference first_const_reference;
      typedef typename call_traits<second_type>::const_reference second_const_reference;
      compressed_pair_imp() {}
      compressed_pair_imp(first_param_type x, second_param_type y)
         : first_type(x), second_type(y) {}
      compressed_pair_imp(first_param_type x)
         : first_type(x) {}
      compressed_pair_imp(second_param_type y)
         : second_type(y) {}
      first_reference first() {return *this;}
      first_const_reference first() const {return *this;}
      second_reference second() {return *this;}
      second_const_reference second() const {return *this;}
      void swap(::boost::compressed_pair<T1,T2>&) {}
   };
   template <class T1, class T2>
   class compressed_pair_imp<T1, T2, 4>
      : private T1
   {
   public:
      typedef T1 first_type;
      typedef T2 second_type;
      typedef typename call_traits<first_type>::param_type first_param_type;
      typedef typename call_traits<second_type>::param_type second_param_type;
      typedef typename call_traits<first_type>::reference first_reference;
      typedef typename call_traits<second_type>::reference second_reference;
      typedef typename call_traits<first_type>::const_reference first_const_reference;
      typedef typename call_traits<second_type>::const_reference second_const_reference;
      compressed_pair_imp() {}
      compressed_pair_imp(first_param_type x, second_param_type)
         : first_type(x) {}
      compressed_pair_imp(first_param_type x)
         : first_type(x) {}
      first_reference first() {return *this;}
      first_const_reference first() const {return *this;}
      second_reference second() {return *this;}
      second_const_reference second() const {return *this;}
      void swap(::boost::compressed_pair<T1,T2>&) {}
   private:
   };
   template <class T1, class T2>
   class compressed_pair_imp<T1, T2, 5>
   {
   public:
      typedef T1 first_type;
      typedef T2 second_type;
      typedef typename call_traits<first_type>::param_type first_param_type;
      typedef typename call_traits<second_type>::param_type second_param_type;
      typedef typename call_traits<first_type>::reference first_reference;
      typedef typename call_traits<second_type>::reference second_reference;
      typedef typename call_traits<first_type>::const_reference first_const_reference;
      typedef typename call_traits<second_type>::const_reference second_const_reference;
      compressed_pair_imp() {}
      compressed_pair_imp(first_param_type x, second_param_type y)
         : first_(x), second_(y) {}
      compressed_pair_imp(first_param_type x)
         : first_(x), second_(x) {}
      first_reference first() {return first_;}
      first_const_reference first() const {return first_;}
      second_reference second() {return second_;}
      second_const_reference second() const {return second_;}
      void swap(::boost::compressed_pair<T1, T2>& y)
      {
         cp_swap(first_, y.first());
         cp_swap(second_, y.second());
      }
   private:
      first_type first_;
      second_type second_;
   };
}
template <class T1, class T2>
class compressed_pair
   : private ::boost::details::compressed_pair_imp<T1, T2,
             ::boost::details::compressed_pair_switch<
                    T1,
                    T2,
                    ::boost::is_same<typename remove_cv<T1>::type, typename remove_cv<T2>::type>::value,
                    ::boost::is_empty<T1>::value,
                    ::boost::is_empty<T2>::value>::value>
{
private:
   typedef details::compressed_pair_imp<T1, T2,
             ::boost::details::compressed_pair_switch<
                    T1,
                    T2,
                    ::boost::is_same<typename remove_cv<T1>::type, typename remove_cv<T2>::type>::value,
                    ::boost::is_empty<T1>::value,
                    ::boost::is_empty<T2>::value>::value> base;
public:
   typedef T1 first_type;
   typedef T2 second_type;
   typedef typename call_traits<first_type>::param_type first_param_type;
   typedef typename call_traits<second_type>::param_type second_param_type;
   typedef typename call_traits<first_type>::reference first_reference;
   typedef typename call_traits<second_type>::reference second_reference;
   typedef typename call_traits<first_type>::const_reference first_const_reference;
   typedef typename call_traits<second_type>::const_reference second_const_reference;
            compressed_pair() : base() {}
            compressed_pair(first_param_type x, second_param_type y) : base(x, y) {}
   explicit compressed_pair(first_param_type x) : base(x) {}
   explicit compressed_pair(second_param_type y) : base(y) {}
   first_reference first() {return base::first();}
   first_const_reference first() const {return base::first();}
   second_reference second() {return base::second();}
   second_const_reference second() const {return base::second();}
   void swap(compressed_pair& y) { base::swap(y); }
};
template <class T>
class compressed_pair<T, T>
   : private details::compressed_pair_imp<T, T,
             ::boost::details::compressed_pair_switch<
                    T,
                    T,
                    ::boost::is_same<typename remove_cv<T>::type, typename remove_cv<T>::type>::value,
                    ::boost::is_empty<T>::value,
                    ::boost::is_empty<T>::value>::value>
{
private:
   typedef details::compressed_pair_imp<T, T,
             ::boost::details::compressed_pair_switch<
                    T,
                    T,
                    ::boost::is_same<typename remove_cv<T>::type, typename remove_cv<T>::type>::value,
                    ::boost::is_empty<T>::value,
                    ::boost::is_empty<T>::value>::value> base;
public:
   typedef T first_type;
   typedef T second_type;
   typedef typename call_traits<first_type>::param_type first_param_type;
   typedef typename call_traits<second_type>::param_type second_param_type;
   typedef typename call_traits<first_type>::reference first_reference;
   typedef typename call_traits<second_type>::reference second_reference;
   typedef typename call_traits<first_type>::const_reference first_const_reference;
   typedef typename call_traits<second_type>::const_reference second_const_reference;
            compressed_pair() : base() {}
            compressed_pair(first_param_type x, second_param_type y) : base(x, y) {}
   explicit
      compressed_pair(first_param_type x) : base(x) {}
   first_reference first() {return base::first();}
   first_const_reference first() const {return base::first();}
   second_reference second() {return base::second();}
   second_const_reference second() const {return base::second();}
   void swap(::boost::compressed_pair<T,T>& y) { base::swap(y); }
};
template <class T1, class T2>
inline
void
swap(compressed_pair<T1, T2>& x, compressed_pair<T1, T2>& y)
{
   x.swap(y);
}
}
namespace boost { namespace spirit {
    template <typename S, typename BaseT>
    class unary : public BaseT
    {
    public:
        typedef BaseT base_t;
        typedef typename boost::call_traits<S>::param_type param_t;
        typedef typename boost::call_traits<S>::const_reference return_t;
        typedef S subject_t;
        typedef typename S::embed_t subject_embed_t;
        unary(param_t subj_)
        : base_t(), subj(subj_) {}
        unary(BaseT const& base, param_t subj_)
        : base_t(base), subj(subj_) {}
        return_t
        subject() const
        { return subj; }
    private:
        subject_embed_t subj;
    };
    template <typename A, typename B, typename BaseT>
    class binary : public BaseT
    {
    public:
        typedef BaseT base_t;
        typedef typename boost::call_traits<A>::param_type left_param_t;
        typedef typename boost::call_traits<A>::const_reference left_return_t;
        typedef typename boost::call_traits<B>::param_type right_param_t;
        typedef typename boost::call_traits<B>::const_reference right_return_t;
        typedef A left_t;
        typedef typename A::embed_t left_embed_t;
        typedef B right_t;
        typedef typename B::embed_t right_embed_t;
        binary(left_param_t a, right_param_t b)
        : base_t(), subj(a, b) {}
        left_return_t
        left() const
        { return subj.first(); }
        right_return_t
        right() const
        { return subj.second(); }
    private:
        boost::compressed_pair<left_embed_t, right_embed_t> subj;
    };
}}
namespace boost { namespace spirit {
    template <typename BaseT>
    struct no_skipper_iteration_policy : public BaseT
    {
        typedef BaseT base_t;
        no_skipper_iteration_policy()
        : BaseT() {}
        template <typename PolicyT>
        no_skipper_iteration_policy(PolicyT const& other)
        : BaseT(other) {}
        template <typename ScannerT>
        void
        skip(ScannerT const& ) const {}
    };
    struct lexeme_parser_gen;
    template <typename ParserT>
    struct contiguous
    : public unary<ParserT, parser<contiguous<ParserT> > >
    {
        typedef contiguous<ParserT> self_t;
        typedef unary_parser_category parser_category_t;
        typedef lexeme_parser_gen parser_generator_t;
        typedef unary<ParserT, parser<self_t> > base_t;
        template <typename ScannerT>
        struct result
        {
            typedef typename parser_result<ParserT, ScannerT>::type type;
        };
        contiguous(ParserT const& p)
        : base_t(p) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            return impl::contiguous_parser_parse<result_t>
                (this->subject(), scan, scan);
        }
    };
    struct lexeme_parser_gen
    {
        template <typename ParserT>
        struct result {
            typedef contiguous<ParserT> type;
        };
        template <typename ParserT>
        static contiguous<ParserT>
        generate(parser<ParserT> const& subject)
        {
            return contiguous<ParserT>(subject.derived());
        }
        template <typename ParserT>
        contiguous<ParserT>
        operator[](parser<ParserT> const& subject) const
        {
            return contiguous<ParserT>(subject.derived());
        }
    };
    const lexeme_parser_gen lexeme_d = lexeme_parser_gen();
    template <typename ScannerT>
    struct lexeme_scanner
    {
        typedef scanner_policies<
            no_skipper_iteration_policy<
                typename ScannerT::iteration_policy_t>,
            typename ScannerT::match_policy_t,
            typename ScannerT::action_policy_t
        > policies_t;
        typedef typename
            rebind_scanner_policies<ScannerT, policies_t>::type type;
    };
    template <typename BaseT>
    struct inhibit_case_iteration_policy : public BaseT
    {
        typedef BaseT base_t;
        inhibit_case_iteration_policy()
        : BaseT() {}
        template <typename PolicyT>
        inhibit_case_iteration_policy(PolicyT const& other)
        : BaseT(other) {}
        template <typename CharT>
        CharT filter(CharT ch) const
        { return impl::tolower_(ch); }
    };
    struct inhibit_case_parser_gen;
    template <typename ParserT>
    struct inhibit_case
    : public unary<ParserT, parser<inhibit_case<ParserT> > >
    {
        typedef inhibit_case<ParserT> self_t;
        typedef unary_parser_category parser_category_t;
        typedef inhibit_case_parser_gen parser_generator_t;
        typedef unary<ParserT, parser<self_t> > base_t;
        template <typename ScannerT>
        struct result
        {
            typedef typename parser_result<ParserT, ScannerT>::type type;
        };
        inhibit_case(ParserT const& p)
        : base_t(p) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            return impl::inhibit_case_parser_parse<result_t>
                (this->subject(), scan, scan);
        }
    };
    template <int N>
    struct inhibit_case_parser_gen_base
    {
        static inhibit_case<strlit<char const*> >
        generate(char const* str)
        { return inhibit_case<strlit<char const*> >(str); }
        static inhibit_case<strlit<wchar_t const*> >
        generate(wchar_t const* str)
        { return inhibit_case<strlit<wchar_t const*> >(str); }
        static inhibit_case<chlit<char> >
        generate(char ch)
        { return inhibit_case<chlit<char> >(ch); }
        static inhibit_case<chlit<wchar_t> >
        generate(wchar_t ch)
        { return inhibit_case<chlit<wchar_t> >(ch); }
        template <typename ParserT>
        static inhibit_case<ParserT>
        generate(parser<ParserT> const& subject)
        { return inhibit_case<ParserT>(subject.derived()); }
        inhibit_case<strlit<char const*> >
        operator[](char const* str) const
        { return inhibit_case<strlit<char const*> >(str); }
        inhibit_case<strlit<wchar_t const*> >
        operator[](wchar_t const* str) const
        { return inhibit_case<strlit<wchar_t const*> >(str); }
        inhibit_case<chlit<char> >
        operator[](char ch) const
        { return inhibit_case<chlit<char> >(ch); }
        inhibit_case<chlit<wchar_t> >
        operator[](wchar_t ch) const
        { return inhibit_case<chlit<wchar_t> >(ch); }
        template <typename ParserT>
        inhibit_case<ParserT>
        operator[](parser<ParserT> const& subject) const
        { return inhibit_case<ParserT>(subject.derived()); }
    };
    struct inhibit_case_parser_gen : public inhibit_case_parser_gen_base<0>
    {
        inhibit_case_parser_gen() {}
    };
    const inhibit_case_parser_gen nocase_d = inhibit_case_parser_gen();
    const inhibit_case_parser_gen as_lower_d = inhibit_case_parser_gen();
    template <typename ScannerT>
    struct as_lower_scanner
    {
        typedef scanner_policies<
            inhibit_case_iteration_policy<
                typename ScannerT::iteration_policy_t>,
            typename ScannerT::match_policy_t,
            typename ScannerT::action_policy_t
        > policies_t;
        typedef typename
            rebind_scanner_policies<ScannerT, policies_t>::type type;
    };
    struct longest_parser_gen;
    template <typename A, typename B>
    struct longest_alternative
    : public binary<A, B, parser<longest_alternative<A, B> > >
    {
        typedef longest_alternative<A, B> self_t;
        typedef binary_parser_category parser_category_t;
        typedef longest_parser_gen parser_generator_t;
        typedef binary<A, B, parser<self_t> > base_t;
        longest_alternative(A const& a, B const& b)
        : base_t(a, b) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            typename ScannerT::iterator_t save = scan.first;
            result_t l = this->left().parse(scan);
            std::swap(scan.first, save);
            result_t r = this->right().parse(scan);
            if (l || r)
            {
                if (l.length() > r.length())
                {
                    scan.first = save;
                    return l;
                }
                return r;
            }
            return scan.no_match();
        }
    };
    struct longest_parser_gen
    {
        template <typename A, typename B>
        struct result {
            typedef typename
                impl::to_longest_alternative<alternative<A, B> >::result_t
            type;
        };
        template <typename A, typename B>
        static typename
        impl::to_longest_alternative<alternative<A, B> >::result_t
        generate(alternative<A, B> const& alt)
        {
            return impl::to_longest_alternative<alternative<A, B> >::
                convert(alt);
        }
        template <typename A, typename B>
        typename impl::to_longest_alternative<alternative<A, B> >::result_t
        operator[](alternative<A, B> const& alt) const
        {
            return impl::to_longest_alternative<alternative<A, B> >::
                convert(alt);
        }
    };
    const longest_parser_gen longest_d = longest_parser_gen();
    struct shortest_parser_gen;
    template <typename A, typename B>
    struct shortest_alternative
    : public binary<A, B, parser<shortest_alternative<A, B> > >
    {
        typedef shortest_alternative<A, B> self_t;
        typedef binary_parser_category parser_category_t;
        typedef shortest_parser_gen parser_generator_t;
        typedef binary<A, B, parser<self_t> > base_t;
        shortest_alternative(A const& a, B const& b)
        : base_t(a, b) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            typename ScannerT::iterator_t save = scan.first;
            result_t l = this->left().parse(scan);
            std::swap(scan.first, save);
            result_t r = this->right().parse(scan);
            if (l || r)
            {
                if (l.length() < r.length())
                {
                    scan.first = save;
                    return l;
                }
                return r;
            }
            return scan.no_match();
        }
    };
    struct shortest_parser_gen
    {
        template <typename A, typename B>
        struct result {
            typedef typename
                impl::to_shortest_alternative<alternative<A, B> >::result_t
            type;
        };
        template <typename A, typename B>
        static typename
        impl::to_shortest_alternative<alternative<A, B> >::result_t
        generate(alternative<A, B> const& alt)
        {
            return impl::to_shortest_alternative<alternative<A, B> >::
                convert(alt);
        }
        template <typename A, typename B>
        typename impl::to_shortest_alternative<alternative<A, B> >::result_t
        operator[](alternative<A, B> const& alt) const
        {
            return impl::to_shortest_alternative<alternative<A, B> >::
                convert(alt);
        }
    };
    const shortest_parser_gen shortest_d = shortest_parser_gen();
    template <typename BoundsT>
    struct min_bounded_gen;
    template <typename ParserT, typename BoundsT>
    struct min_bounded
    : public unary<ParserT, parser<min_bounded<ParserT, BoundsT> > >
    {
        typedef min_bounded<ParserT, BoundsT> self_t;
        typedef unary_parser_category parser_category_t;
        typedef min_bounded_gen<BoundsT> parser_generator_t;
        typedef unary<ParserT, parser<self_t> > base_t;
        template <typename ScannerT>
        struct result
        {
            typedef typename parser_result<ParserT, ScannerT>::type type;
        };
        min_bounded(ParserT const& p, BoundsT const& min__)
        : base_t(p)
        , min_(min__) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            result_t hit = this->subject().parse(scan);
            if (hit.value() < min_)
                return scan.no_match();
            return hit;
        }
        BoundsT min_;
    };
    template <typename BoundsT>
    struct min_bounded_gen
    {
        min_bounded_gen(BoundsT const& min__)
        : min_(min__) {}
        template <typename DerivedT>
        min_bounded<DerivedT, BoundsT>
        operator[](parser<DerivedT> const& p) const
        { return min_bounded<DerivedT, BoundsT>(p.derived(), min_); }
        BoundsT min_;
    };
    template <typename BoundsT>
    inline min_bounded_gen<BoundsT>
    min_limit_d(BoundsT const& min_)
    { return min_bounded_gen<BoundsT>(min_); }
    template <typename BoundsT>
    struct max_bounded_gen;
    template <typename ParserT, typename BoundsT>
    struct max_bounded
    : public unary<ParserT, parser<max_bounded<ParserT, BoundsT> > >
    {
        typedef max_bounded<ParserT, BoundsT> self_t;
        typedef unary_parser_category parser_category_t;
        typedef max_bounded_gen<BoundsT> parser_generator_t;
        typedef unary<ParserT, parser<self_t> > base_t;
        template <typename ScannerT>
        struct result
        {
            typedef typename parser_result<ParserT, ScannerT>::type type;
        };
        max_bounded(ParserT const& p, BoundsT const& max__)
        : base_t(p)
        , max_(max__) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            result_t hit = this->subject().parse(scan);
            if (hit.value() > max_)
                return scan.no_match();
            return hit;
        }
        BoundsT max_;
    };
    template <typename BoundsT>
    struct max_bounded_gen
    {
        max_bounded_gen(BoundsT const& max__)
        : max_(max__) {}
        template <typename DerivedT>
        max_bounded<DerivedT, BoundsT>
        operator[](parser<DerivedT> const& p) const
        { return max_bounded<DerivedT, BoundsT>(p.derived(), max_); }
        BoundsT max_;
    };
    template <typename BoundsT>
    inline max_bounded_gen<BoundsT>
    max_limit_d(BoundsT const& max_)
    { return max_bounded_gen<BoundsT>(max_); }
    template <typename BoundsT>
    struct bounded_gen;
    template <typename ParserT, typename BoundsT>
    struct bounded
    : public unary<ParserT, parser<bounded<ParserT, BoundsT> > >
    {
        typedef bounded<ParserT, BoundsT> self_t;
        typedef unary_parser_category parser_category_t;
        typedef bounded_gen<BoundsT> parser_generator_t;
        typedef unary<ParserT, parser<self_t> > base_t;
        template <typename ScannerT>
        struct result
        {
            typedef typename parser_result<ParserT, ScannerT>::type type;
        };
        bounded(ParserT const& p, BoundsT const& min__, BoundsT const& max__)
        : base_t(p)
        , min_(min__)
        , max_(max__) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            result_t hit = this->subject().parse(scan);
            if (hit.value() < min_ || hit.value() > max_)
                return scan.no_match();
            return hit;
        }
        BoundsT min_, max_;
    };
    template <typename BoundsT>
    struct bounded_gen
    {
        bounded_gen(BoundsT const& min__, BoundsT const& max__)
        : min_(min__)
        , max_(max__) {}
        template <typename DerivedT>
        bounded<DerivedT, BoundsT>
        operator[](parser<DerivedT> const& p) const
        { return bounded<DerivedT, BoundsT>(p.derived(), min_, max_); }
        BoundsT min_, max_;
    };
    template <typename BoundsT>
    inline bounded_gen<BoundsT>
    limit_d(BoundsT const& min_, BoundsT const& max_)
    { return bounded_gen<BoundsT>(min_, max_); }
}}
#include <cmath>
namespace boost { namespace spirit {
    struct sign_parser;
    namespace impl
    {
        template <typename ScannerT>
        bool
        extract_sign(ScannerT const& scan, std::size_t& count)
        {
            count = 0;
            bool neg = *scan == '-';
            if (neg || (*scan == '+'))
            {
                ++scan;
                ++count;
                return neg;
            }
            return false;
        }
        template<const int Radix>
        struct radix_traits;
        template<>
        struct radix_traits<2>
        {
            template<typename CharT>
            static bool is_valid(CharT ch)
            {
                return ('0' == ch || '1' == ch);
            }
            template<typename CharT>
            static int digit(CharT ch)
            {
                return ch - '0';
            }
        };
        template<>
        struct radix_traits<8>
        {
            template<typename CharT>
            static bool is_valid(CharT ch)
            {
                return ('0' <= ch && ch <= '7');
            }
            template<typename CharT>
            static int digit(CharT ch)
            {
                return ch - '0';
            }
        };
        template<>
        struct radix_traits<10>
        {
            template<typename CharT>
            static bool is_valid(CharT ch)
            {
                return impl::isdigit_(ch);
            }
            template<typename CharT>
            static int digit(CharT ch)
            {
                return ch - '0';
            }
        };
        template<>
        struct radix_traits<16>
        {
            template<typename CharT>
            static bool is_valid(CharT ch)
            {
                return impl::isxdigit_(ch);
            }
            template<typename CharT>
            static int digit(CharT ch)
            {
                if (impl::isdigit_(ch))
                    return ch - '0';
                return impl::tolower_(ch) - 'a' + 10;
            }
        };
        template <int Radix>
        struct positive_accumulate
        {
            template <typename T>
            static bool check(T const& n, T const& prev)
            {
                return n < prev;
            }
            template <typename T, typename CharT>
            static void add(T& n, CharT ch)
            {
                n += radix_traits<Radix>::digit(ch);
            }
        };
        template <int Radix>
        struct negative_accumulate
        {
            template <typename T>
            static bool check(T const& n, T const& prev)
            {
                return n > prev;
            }
            template <typename T, typename CharT>
            static void add(T& n, CharT ch)
            {
                n -= radix_traits<Radix>::digit(ch);
            }
        };
        template <int Radix, typename Accumulate>
        struct extract_int_base
        {
            template <typename ScannerT, typename T>
            static bool
            f(ScannerT& scan, T& n)
            {
                T prev = n;
                n *= Radix;
                if (Accumulate::check(n, prev))
                    return false;
                prev = n;
                Accumulate::add(n, *scan);
                if (Accumulate::check(n, prev))
                    return false;
                return true;
            }
        };
        template <bool Bounded>
        struct extract_int_
        {
            template <
                int Radix,
                unsigned MinDigits,
                int MaxDigits,
                typename Accumulate
            >
            struct apply
            {
                typedef extract_int_base<Radix, Accumulate> base;
                typedef radix_traits<Radix> check;
                template <typename ScannerT, typename T>
                static bool
                f(ScannerT& scan, T& n, std::size_t& count)
                {
                    std::size_t i = 0;
                    for (; (i < MaxDigits) && !scan.at_end()
                        && check::is_valid(*scan);
                        ++i, ++scan, ++count)
                    {
                        if (!base::f(scan, n))
                            return false;
                    }
                    return i >= MinDigits;
                }
            };
        };
        template <>
        struct extract_int_<false>
        {
            template <
                int Radix,
                unsigned MinDigits,
                int MaxDigits,
                typename Accumulate
            >
            struct apply
            {
                typedef extract_int_base<Radix, Accumulate> base;
                typedef radix_traits<Radix> check;
                template <typename ScannerT, typename T>
                static bool
                f(ScannerT& scan, T& n, std::size_t& count)
                {
                    std::size_t i = 0;
                    for (; !scan.at_end() && check::is_valid(*scan);
                        ++i, ++scan, ++count)
                    {
                        if (!base::f(scan, n))
                            return false;
                    }
                    return i >= MinDigits;
                }
            };
        };
        template <
            int Radix, unsigned MinDigits, int MaxDigits,
            typename Accumulate = positive_accumulate<Radix>
        >
        struct extract_int
        {
            template <typename ScannerT, typename T>
            static bool
            f(ScannerT& scan, T& n, std::size_t& count)
            {
                return extract_int_<(MaxDigits >= 0)>::template
                    apply<Radix, MinDigits, MaxDigits, Accumulate>::
                        f(scan, n, count);
            }
        };
        template <
            typename T = unsigned,
            int Radix = 10,
            unsigned MinDigits = 1,
            int MaxDigits = -1
        >
        struct uint_parser_impl
            : parser<uint_parser_impl<T, Radix, MinDigits, MaxDigits> >
        {
            typedef uint_parser_impl<T, Radix, MinDigits, MaxDigits> self_t;
            template <typename ScannerT>
            struct result
            {
                typedef typename match_result<ScannerT, T>::type type;
            };
            template <typename ScannerT>
            typename parser_result<self_t, ScannerT>::type
            parse(ScannerT const& scan) const
            {
                if (!scan.at_end())
                {
                    T n = 0;
                    std::size_t count = 0;
                    typename ScannerT::iterator_t save = scan.first;
                    if (extract_int<Radix, MinDigits, MaxDigits>::
                        f(scan, n, count))
                    {
                        return scan.create_match(count, n, save, scan.first);
                    }
                }
                return scan.no_match();
            }
        };
        template <
            typename T = unsigned,
            int Radix = 10,
            unsigned MinDigits = 1,
            int MaxDigits = -1
        >
        struct int_parser_impl
            : parser<int_parser_impl<T, Radix, MinDigits, MaxDigits> >
        {
            typedef int_parser_impl<T, Radix, MinDigits, MaxDigits> self_t;
            template <typename ScannerT>
            struct result
            {
                typedef typename match_result<ScannerT, T>::type type;
            };
            template <typename ScannerT>
            typename parser_result<self_t, ScannerT>::type
            parse(ScannerT const& scan) const
            {
                typedef extract_int<Radix, MinDigits, MaxDigits,
                    negative_accumulate<Radix> > extract_int_neg_t;
                typedef extract_int<Radix, MinDigits, MaxDigits>
                    extract_int_pos_t;
                if (!scan.at_end())
                {
                    T n = 0;
                    std::size_t count = 0;
                    typename ScannerT::iterator_t save = scan.first;
                    bool hit = impl::extract_sign(scan, count);
                    if (hit)
                        hit = extract_int_neg_t::f(scan, n, count);
                    else
                        hit = extract_int_pos_t::f(scan, n, count);
                    if (hit)
                        return scan.create_match(count, n, save, scan.first);
                    else
                        scan.first = save;
                }
                return scan.no_match();
            }
        };
        template <typename RT, typename T, typename RealPoliciesT>
        struct real_parser_impl
        {
            typedef real_parser_impl<RT, T, RealPoliciesT> self_t;
            template <typename ScannerT>
            RT parse_main(ScannerT const& scan) const
            {
                if (scan.at_end())
                    return scan.no_match();
                typename ScannerT::iterator_t save = scan.first;
                typedef typename parser_result<sign_parser, ScannerT>::type
                    sign_match_t;
                typedef typename parser_result<chlit<>, ScannerT>::type
                    exp_match_t;
                sign_match_t sign_match = RealPoliciesT::parse_sign(scan);
                std::size_t count = sign_match ? sign_match.length() : 0;
                bool neg = sign_match.has_valid_attribute() ?
                                    sign_match.value() : false;
                RT n_match = RealPoliciesT::parse_n(scan);
                T n = n_match.has_valid_attribute() ?
                                    n_match.value() : T(0);
                bool got_a_number = n_match;
                exp_match_t e_hit;
                if (!got_a_number && !RealPoliciesT::allow_leading_dot)
                     return scan.no_match();
                else
                    count += n_match.length();
                if (neg)
                    n = -n;
                if (RealPoliciesT::parse_dot(scan))
                {
                    if (RT hit = RealPoliciesT::parse_frac_n(scan))
                    {
                        hit.value(hit.value()
                            * std::
                                pow(T(10), T(-hit.length())));
                        if (neg)
                            n -= hit.value();
                        else
                            n += hit.value();
                        count += hit.length() + 1;
                    }
                    else if (!got_a_number ||
                        !RealPoliciesT::allow_trailing_dot)
                        return scan.no_match();
                    e_hit = RealPoliciesT::parse_exp(scan);
                }
                else
                {
                    if (!got_a_number)
                        return scan.no_match();
                    e_hit = RealPoliciesT::parse_exp(scan);
                    if (RealPoliciesT::expect_dot && !e_hit)
                        return scan.no_match();
                }
                if (e_hit)
                {
                    if (RT e_n_hit = RealPoliciesT::parse_exp_n(scan))
                    {
                        n *= std::
                            pow(T(10), T(e_n_hit.value()));
                        count += e_n_hit.length() + e_hit.length();
                    }
                    else
                    {
                        return scan.no_match();
                    }
                }
                return scan.create_match(count, n, save, scan.first);
            }
            template <typename ScannerT>
            static RT parse(ScannerT const& scan)
            {
                static self_t this_;
                return impl::implicit_lexeme_parse<RT>(this_, scan, scan);
            }
        };
    }
}}
namespace boost { namespace spirit
{
    template <
        typename T = unsigned,
        int Radix = 10,
        unsigned MinDigits = 1,
        int MaxDigits = -1
    >
    struct uint_parser : parser<uint_parser<T, Radix, MinDigits, MaxDigits> >
    {
        typedef uint_parser<T, Radix, MinDigits, MaxDigits> self_t;
        template <typename ScannerT>
        struct result
        {
            typedef typename match_result<ScannerT, T>::type type;
        };
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef impl::uint_parser_impl<T, Radix, MinDigits, MaxDigits> impl_t;
            typedef typename parser_result<impl_t, ScannerT>::type result_t;
            return impl::contiguous_parser_parse<result_t>(impl_t(), scan, scan);
        }
    };
    template <
        typename T = unsigned,
        int Radix = 10,
        unsigned MinDigits = 1,
        int MaxDigits = -1
    >
    struct int_parser : parser<int_parser<T, Radix, MinDigits, MaxDigits> >
    {
        typedef int_parser<T, Radix, MinDigits, MaxDigits> self_t;
        template <typename ScannerT>
        struct result
        {
            typedef typename match_result<ScannerT, T>::type type;
        };
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef impl::int_parser_impl<T, Radix, MinDigits, MaxDigits> impl_t;
            typedef typename parser_result<impl_t, ScannerT>::type result_t;
            return impl::contiguous_parser_parse<result_t>(impl_t(), scan, scan);
        }
    };
    int_parser<int> const
        int_p = int_parser<int>();
    uint_parser<unsigned> const
        uint_p = uint_parser<unsigned>();
    uint_parser<unsigned, 2> const
        bin_p = uint_parser<unsigned, 2>();
    uint_parser<unsigned, 8> const
        oct_p = uint_parser<unsigned, 8>();
    uint_parser<unsigned, 16> const
        hex_p = uint_parser<unsigned, 16>();
    namespace impl
    {
        template <typename ScannerT>
        bool extract_sign(ScannerT const& scan, std::size_t& count);
    }
    struct sign_parser : public parser<sign_parser>
    {
        typedef sign_parser self_t;
        template <typename ScannerT>
        struct result
        {
            typedef typename match_result<ScannerT, bool>::type type;
        };
        sign_parser() {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            if (!scan.at_end())
            {
                std::size_t length;
                typename ScannerT::iterator_t save(scan.first);
                bool neg = impl::extract_sign(scan, length);
                if (length)
                    return scan.create_match(1, neg, save, scan.first);
            }
            return scan.no_match();
        }
    };
    sign_parser const sign_p = sign_parser();
    template <typename T>
    struct ureal_parser_policies
    {
        static const bool allow_leading_dot = true;
        static const bool allow_trailing_dot = true;
        static const bool expect_dot = false;
        typedef uint_parser<T, 10, 1, -1> uint_parser_t;
        typedef int_parser<T, 10, 1, -1> int_parser_t;
        template <typename ScannerT>
        static typename match_result<ScannerT, nil_t>::type
        parse_sign(ScannerT& scan)
        {
            return scan.no_match();
        }
        template <typename ScannerT>
        static typename parser_result<uint_parser_t, ScannerT>::type
        parse_n(ScannerT& scan)
        {
            return uint_parser_t().parse(scan);
        }
        template <typename ScannerT>
        static typename parser_result<chlit<>, ScannerT>::type
        parse_dot(ScannerT& scan)
        {
            return ch_p('.').parse(scan);
        }
        template <typename ScannerT>
        static typename parser_result<uint_parser_t, ScannerT>::type
        parse_frac_n(ScannerT& scan)
        {
            return uint_parser_t().parse(scan);
        }
        template <typename ScannerT>
        static typename parser_result<chlit<>, ScannerT>::type
        parse_exp(ScannerT& scan)
        {
            return as_lower_d['e'].parse(scan);
        }
        template <typename ScannerT>
        static typename parser_result<int_parser_t, ScannerT>::type
        parse_exp_n(ScannerT& scan)
        {
            return int_parser_t().parse(scan);
        }
    };
    template <typename T>
    struct real_parser_policies : public ureal_parser_policies<T>
    {
        template <typename ScannerT>
        static typename parser_result<sign_parser, ScannerT>::type
        parse_sign(ScannerT& scan)
        {
            return sign_p.parse(scan);
        }
    };
    template <
        typename T = double,
        typename RealPoliciesT = ureal_parser_policies<T>
    >
    struct real_parser
    : public parser<real_parser<T, RealPoliciesT> >
    {
        typedef real_parser<T, RealPoliciesT> self_t;
        template <typename ScannerT>
        struct result
        {
            typedef typename match_result<ScannerT, T>::type type;
        };
        real_parser() {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            return impl::real_parser_impl<result_t, T, RealPoliciesT>::parse(scan);
        }
    };
    real_parser<double, ureal_parser_policies<double> > const
        ureal_p = real_parser<double, ureal_parser_policies<double> >();
    real_parser<double, real_parser_policies<double> > const
        real_p = real_parser<double, real_parser_policies<double> >();
    template <typename T>
    struct strict_ureal_parser_policies : public ureal_parser_policies<T>
    {
        static const bool expect_dot = true;
    };
    template <typename T>
    struct strict_real_parser_policies : public real_parser_policies<T>
    {
        static const bool expect_dot = true;
    };
    real_parser<double, strict_ureal_parser_policies<double> > const
        strict_ureal_p
            = real_parser<double, strict_ureal_parser_policies<double> >();
    real_parser<double, strict_real_parser_policies<double> > const
        strict_real_p
            = real_parser<double, strict_real_parser_policies<double> >();
}}
namespace boost
{
    namespace spirit
    {
    struct default_parser_context_base
    {
        template <typename DerivedT>
        struct aux {};
    };
    struct parser_context_base {};
    struct nil_t;
    template<typename ContextT> struct parser_context_linker;
    template<typename AttrT = nil_t>
    struct parser_context : parser_context_base
    {
        typedef AttrT attr_t;
        typedef default_parser_context_base base_t;
        typedef parser_context_linker<parser_context<AttrT> > context_linker_t;
        template <typename ParserT>
        parser_context(ParserT const&) {}
        template <typename ParserT, typename ScannerT>
        void
        pre_parse(ParserT const&, ScannerT const&) {}
        template <typename ResultT, typename ParserT, typename ScannerT>
        ResultT&
        post_parse(ResultT& hit, ParserT const&, ScannerT const&)
        { return hit; }
    };
    template <typename ContextT, typename DerivedT>
    struct context_aux : public ContextT::base_t::template aux<DerivedT> {};
    template<typename ScannerT>
    struct parser_scanner_linker : public ScannerT
    {
        parser_scanner_linker(ScannerT const scan_) : ScannerT(scan_) {}
    };
    template<typename ContextT>
    struct parser_context_linker : public ContextT
    {
        template <typename ParserT>
        parser_context_linker(ParserT const& p)
        : ContextT(p) {}
        template <typename ParserT, typename ScannerT>
        void pre_parse(ParserT const& p, ScannerT const& scan)
        { ContextT::pre_parse(p, scan); }
        template <typename ResultT, typename ParserT, typename ScannerT>
        ResultT&
        post_parse(ResultT& hit, ParserT const& p, ScannerT const& scan)
        { return ContextT::post_parse(hit, p, scan); }
    };
    }
}
namespace boost { namespace spirit {
    template <typename FirstT, typename RestT>
    struct subrule_list;
    template <int ID, typename DefT, typename ContextT>
    struct subrule_parser;
    namespace impl {
        template <int N, typename ListT>
        struct get_subrule
        {
            typedef typename get_subrule<N, typename ListT::rest_t>::type type;
        };
        template <int ID, typename DefT, typename ContextT, typename RestT>
        struct get_subrule<
            ID,
            subrule_list<
                subrule_parser<ID, DefT, ContextT>,
                RestT> >
        {
            typedef DefT type;
        };
        template <int ID>
        struct get_subrule<ID, nil_t>
        {
            typedef nil_t type;
        };
        template <typename T1, typename T2>
        struct get_result_t {
            typedef typename mpl::if_<
                boost::is_same<T1, nil_t>, T2, T1
            >::type type;
        };
        template <int ID, typename ScannerT, typename ContextResultT>
        struct get_subrule_result
        {
            typedef typename
                impl::get_subrule<ID, typename ScannerT::list_t>::type
            parser_t;
            typedef typename parser_result<parser_t, ScannerT>::type
            def_result_t;
            typedef typename match_result<ScannerT, ContextResultT>::type
            context_result_t;
            typedef typename get_result_t<context_result_t, def_result_t>::type
            type;
        };
        template <typename DefT, typename ScannerT, typename ContextResultT>
        struct get_subrule_parser_result
        {
            typedef typename parser_result<DefT, ScannerT>::type
            def_result_t;
            typedef typename match_result<ScannerT, ContextResultT>::type
            context_result_t;
            typedef typename get_result_t<context_result_t, def_result_t>::type
            type;
        };
        template <typename SubruleT, int ID>
        struct same_subrule_id
        {
            static const bool value = (SubruleT::id == ID);
        };
        template <typename RT, typename ScannerT, int ID>
        struct parse_subrule
        {
            template <typename ListT>
            static void
            do_parse(RT& r, ScannerT const& scan, ListT const& list, mpl::true_)
            {
                r = list.first.rhs.parse(scan);
            }
            template <typename ListT>
            static void
            do_parse(RT& r, ScannerT const& scan, ListT const& list, mpl::false_)
            {
                typedef typename ListT::rest_t::first_t subrule_t;
                mpl::bool_<same_subrule_id<subrule_t, ID>::value> same_id;
                do_parse(r, scan, list.rest, same_id);
            }
            static void
            do_(RT& r, ScannerT const& scan)
            {
                typedef typename ScannerT::list_t::first_t subrule_t;
                mpl::bool_<same_subrule_id<subrule_t, ID>::value> same_id;
                do_parse(r, scan, scan.list, same_id);
            }
        };
}}}
namespace boost { namespace spirit {
    template <typename ScannerT, typename ListT>
    struct subrules_scanner : public ScannerT
    {
        typedef ScannerT scanner_t;
        typedef ListT list_t;
        typedef subrules_scanner<ScannerT, ListT> self_t;
        subrules_scanner(ScannerT const& scan, ListT const& list_)
        : ScannerT(scan), list(list_) {}
        template <typename PoliciesT>
        struct rebind_policies
        {
            typedef typename rebind_scanner_policies<ScannerT, PoliciesT>::type
                rebind_scanner;
            typedef subrules_scanner<rebind_scanner, ListT> type;
        };
        template <typename PoliciesT>
        subrules_scanner<
            typename rebind_scanner_policies<ScannerT, PoliciesT>::type,
            ListT>
        change_policies(PoliciesT const& policies) const
        {
            typedef subrules_scanner<
                typename
                    rebind_scanner_policies<ScannerT, PoliciesT>::type,
                ListT>
            subrules_scanner_t;
            return subrules_scanner_t(
                    ScannerT::change_policies(policies),
                    list);
        }
        template <typename IteratorT>
        struct rebind_iterator
        {
            typedef typename rebind_scanner_iterator<ScannerT, IteratorT>::type
                rebind_scanner;
            typedef subrules_scanner<rebind_scanner, ListT> type;
        };
        template <typename IteratorT>
        subrules_scanner<
            typename rebind_scanner_iterator<ScannerT, IteratorT>::type,
            ListT>
        change_iterator(IteratorT const& first, IteratorT const &last) const
        {
            typedef subrules_scanner<
                typename
                    rebind_scanner_iterator<ScannerT, IteratorT>::type,
                ListT>
            subrules_scanner_t;
            return subrules_scanner_t(
                    ScannerT::change_iterator(first, last),
                    list);
        }
        ListT const& list;
    };
    template <typename ScannerT, typename ListT>
    struct subrules_scanner_finder
    {
          typedef subrules_scanner<ScannerT, ListT> type;
    };
    template <typename ScannerT, typename ListT>
    struct subrules_scanner_finder<subrules_scanner<ScannerT, ListT>, ListT>
    {
          typedef subrules_scanner<ScannerT, ListT> type;
    };
    template <typename FirstT, typename RestT>
    struct subrule_list : public parser<subrule_list<FirstT, RestT> >
    {
        typedef subrule_list<FirstT, RestT> self_t;
        typedef FirstT first_t;
        typedef RestT rest_t;
        subrule_list(FirstT const& first_, RestT const& rest_)
        : first(first_), rest(rest_) {}
        template <typename ScannerT>
        struct result
        {
            typedef typename parser_result<FirstT, ScannerT>::type type;
        };
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename subrules_scanner_finder<ScannerT, self_t>::type
            subrules_scanner_t;
            subrules_scanner_t g_arg(scan, *this);
            return first.start.parse(g_arg);
        }
        template <int ID, typename DefT, typename ContextT>
        subrule_list<
            FirstT,
            subrule_list<
                subrule_parser<ID, DefT, ContextT>,
                RestT> >
        operator,(subrule_parser<ID, DefT, ContextT> const& rhs)
        {
            return subrule_list<
                FirstT,
                subrule_list<
                    subrule_parser<ID, DefT, ContextT>,
                    RestT> >(
                        first,
                        subrule_list<
                            subrule_parser<ID, DefT, ContextT>,
                            RestT>(rhs, rest));
        }
        FirstT first;
        RestT rest;
    };
    template <int ID, typename ContextT = parser_context<> >
    struct subrule;
    template <int ID, typename DefT, typename ContextT>
    struct subrule_parser
    : public parser<subrule_parser<ID, DefT, ContextT> >
    {
        typedef subrule_parser<ID, DefT, ContextT> self_t;
        typedef subrule<ID, ContextT> subrule_t;
        typedef DefT def_t;
        static const int id = ID;
        template <typename ScannerT>
        struct result
        {
            typedef typename
                impl::get_subrule_parser_result<
                    DefT, ScannerT, typename subrule_t::attr_t>::type type;
        };
        subrule_parser(subrule_t const& start_, DefT const& rhs_)
        : rhs(rhs_), start(start_) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef subrule_list<self_t, nil_t> list_t;
            typedef subrules_scanner<ScannerT, list_t> scanner_t;
            list_t list(*this, nil_t());
            scanner_t g_arg(scan, list);
            return start.parse(g_arg);
        }
        template <int ID2, typename DefT2, typename ContextT2>
        inline subrule_list<
            self_t,
            subrule_list<
                subrule_parser<ID2, DefT2, ContextT2>,
                nil_t> >
        operator,(subrule_parser<ID2, DefT2, ContextT2> const& rhs) const
        {
            return subrule_list<
                self_t,
                subrule_list<
                    subrule_parser<ID2, DefT2, ContextT2>,
                    nil_t> >(
                        *this,
                        subrule_list<
                            subrule_parser<ID2, DefT2, ContextT2>, nil_t>(
                                rhs, nil_t()));
        }
        typename DefT::embed_t rhs;
        subrule_t const& start;
    };
    template <int ID, typename ContextT>
    struct subrule
        : public parser<subrule<ID, ContextT> >
        , public ContextT::base_t
        , public context_aux<ContextT, subrule<ID, ContextT> >
    {
        typedef subrule<ID, ContextT> self_t;
        typedef subrule<ID, ContextT> const& embed_t;
        typedef typename ContextT::context_linker_t context_t;
        typedef typename context_t::attr_t attr_t;
        static const int id = ID;
        template <typename ScannerT>
        struct result
        {
            typedef typename
                impl::get_subrule_result<ID, ScannerT, attr_t>::type type;
        };
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse_main(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            result_t result;
            impl::parse_subrule<result_t, ScannerT, ID>::
                do_(result, scan);
            return result;
        }
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            typedef parser_scanner_linker<ScannerT> scanner_t;
            scanner_t scan_wrap(scan); context_t context_wrap(*this); context_wrap.pre_parse(*this, scan_wrap); result_t hit = parse_main(scan); return context_wrap.post_parse(hit, *this, scan_wrap);;
        }
        template <typename DefT>
        subrule_parser<ID, DefT, ContextT>
        operator=(parser<DefT> const& rhs) const
        {
            return subrule_parser<ID, DefT, ContextT>(*this, rhs.derived());
        }
    private:
        subrule& operator=(subrule const&);
        template <int ID2, typename ContextT2>
        subrule& operator=(subrule<ID2, ContextT2> const&);
    };
}}
typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( 1 > 0 ) >)> boost_static_assert_typedef_30;
#include <assert.h>
namespace boost
{
template<class T> inline void checked_delete(T * x)
{
    typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
    (void) sizeof(type_must_be_complete);
    delete x;
}
template<class T> inline void checked_array_delete(T * x)
{
    typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
    (void) sizeof(type_must_be_complete);
    delete [] x;
}
template<class T> struct checked_deleter
{
    typedef void result_type;
    typedef T * argument_type;
    void operator()(T * x) const
    {
        boost::checked_delete(x);
    }
};
template<class T> struct checked_array_deleter
{
    typedef void result_type;
    typedef T * argument_type;
    void operator()(T * x) const
    {
        boost::checked_array_delete(x);
    }
};
}
namespace boost
{
template<class T> class scoped_ptr
{
private:
    T * ptr;
    scoped_ptr(scoped_ptr const &);
    scoped_ptr & operator=(scoped_ptr const &);
    typedef scoped_ptr<T> this_type;
public:
    typedef T element_type;
    explicit scoped_ptr(T * p = 0): ptr(p)
    {
    }
    explicit scoped_ptr(std::auto_ptr<T> p): ptr(p.release())
    {
    }
    ~scoped_ptr()
    {
        boost::checked_delete(ptr);
    }
    void reset(T * p = 0)
    {
        ((void)0);
        this_type(p).swap(*this);
    }
    T & operator*() const
    {
        ((void)0);
        return *ptr;
    }
    T * operator->() const
    {
        ((void)0);
        return ptr;
    }
    T * get() const
    {
        return ptr;
    }
    typedef T * this_type::*unspecified_bool_type;
    operator unspecified_bool_type() const
    {
        return ptr == 0? 0: &this_type::ptr;
    }
    bool operator! () const
    {
        return ptr == 0;
    }
    void swap(scoped_ptr & b)
    {
        T * tmp = b.ptr;
        b.ptr = ptr;
        ptr = tmp;
    }
};
template<class T> inline void swap(scoped_ptr<T> & a, scoped_ptr<T> & b)
{
    a.swap(b);
}
template<class T> inline T * get_pointer(scoped_ptr<T> const & p)
{
    return p.get();
}
}
namespace boost { namespace spirit {
    namespace impl
    {
        template <typename BaseT, typename DefaultT
            , typename T0, typename T1, typename T2>
        struct get_param
        {
            typedef typename mpl::if_<
                is_base_and_derived<BaseT, T0>
              , T0
              , typename mpl::if_<
                    is_base_and_derived<BaseT, T1>
                  , T1
                  , typename mpl::if_<
                        is_base_and_derived<BaseT, T2>
                      , T2
                      , DefaultT
                    >::type
                >::type
            >::type type;
        };
        template <typename T0, typename T1, typename T2>
        struct get_context
        {
            typedef typename get_param<
                parser_context_base, parser_context<>, T0, T1, T2>::type
            type;
        };
        template <typename T0, typename T1, typename T2>
        struct get_tag
        {
            typedef typename get_param<
                parser_tag_base, parser_address_tag, T0, T1, T2>::type
            type;
        };
        template <typename T0, typename T1, typename T2>
        struct get_scanner
        {
            typedef typename get_param<
                scanner_base, scanner<>, T0, T1, T2>::type
            type;
        };
        template <
            typename DerivedT
          , typename EmbedT
          , typename T0 = nil_t
          , typename T1 = nil_t
          , typename T2 = nil_t
        >
        class rule_base;
        class rule_base_access
        {
            template <
                typename DerivedT
              , typename EmbedT
              , typename T0
              , typename T1
              , typename T2
            >
           friend class rule_base;
            template <typename RuleT>
            static typename RuleT::abstract_parser_t*
            get(RuleT const& r)
            {
                return r.get();
            }
        };
        template <
            typename DerivedT
          , typename EmbedT
          , typename T0
          , typename T1
          , typename T2
        >
        class rule_base
            : public parser<DerivedT>
            , public impl::get_context<T0, T1, T2>::type::base_t
            , public context_aux<
                typename impl::get_context<T0, T1, T2>::type, DerivedT>
            , public impl::get_tag<T0, T1, T2>::type
        {
        public:
            typedef typename impl::get_scanner<T0, T1, T2>::type scanner_t;
            typedef typename impl::get_context<T0, T1, T2>::type context_t;
            typedef typename impl::get_tag<T0, T1, T2>::type tag_t;
            typedef EmbedT embed_t;
            typedef typename context_t::context_linker_t linked_context_t;
            typedef typename linked_context_t::attr_t attr_t;
            template <typename ScannerT>
            struct result
            {
                typedef typename match_result<ScannerT, attr_t>::type type;
            };
            template <typename ScannerT>
            typename parser_result<DerivedT, ScannerT>::type
            parse(ScannerT const& scan) const
            {
                typedef parser_scanner_linker<ScannerT> linked_scanner_t;
                typedef typename parser_result<DerivedT, ScannerT>::type result_t;
                linked_scanner_t scan_wrap(scan); linked_context_t context_wrap(*this); context_wrap.pre_parse(*this, scan_wrap); result_t hit = parse_main(scan); return context_wrap.post_parse(hit, *this, scan_wrap);;
            }
            template <typename ScannerT>
            typename parser_result<DerivedT, ScannerT>::type
            parse_main(ScannerT const& scan) const
            {
                typename parser_result<DerivedT, ScannerT>::type hit;
                DerivedT const * derivedThis =
                    static_cast<DerivedT const *>(this);
                if (rule_base_access::get(*derivedThis))
                {
                    typename ScannerT::iterator_t s(scan.first);
                    hit = rule_base_access::get(*derivedThis)
                            ->do_parse_virtual(scan);
                    scan.group_match(hit, this->id(), s, scan.first);
                }
                else
                {
                    hit = scan.no_match();
                }
                return hit;
            }
        };
        template <typename ScannerT, typename AttrT>
        struct abstract_parser
        {
            abstract_parser() {}
            virtual ~abstract_parser() {}
            virtual typename match_result<ScannerT, AttrT>::type
            do_parse_virtual(ScannerT const& scan) const = 0;
            virtual abstract_parser*
            clone() const = 0;
        };
        template <typename ParserT, typename ScannerT, typename AttrT>
        struct concrete_parser : abstract_parser<ScannerT, AttrT>
        {
            concrete_parser(ParserT const& p) : p(p) {}
            virtual ~concrete_parser() {}
            virtual typename match_result<ScannerT, AttrT>::type
            do_parse_virtual(ScannerT const& scan) const
            {
                return p.parse(scan);
            }
            virtual abstract_parser<ScannerT, AttrT>*
            clone() const
            {
                return new concrete_parser(p);
            }
            typename ParserT::embed_t p;
        };
    }
}}
namespace boost { namespace spirit {
    template <
        typename T0 = nil_t
      , typename T1 = nil_t
      , typename T2 = nil_t
    >
    class rule
        : public impl::rule_base<
            rule<T0, T1, T2>
          , rule<T0, T1, T2> const&
          , T0, T1, T2>
    {
    public:
        typedef rule<T0, T1, T2> self_t;
        typedef impl::rule_base<
            self_t
          , self_t const&
          , T0, T1, T2>
        base_t;
        typedef typename base_t::scanner_t scanner_t;
        typedef typename base_t::attr_t attr_t;
        typedef impl::abstract_parser<scanner_t, attr_t> abstract_parser_t;
        rule() : ptr() {}
        ~rule() {}
        rule(rule const& r)
        : ptr(new impl::concrete_parser<rule, scanner_t, attr_t>(r)) {}
        template <typename ParserT>
        rule(ParserT const& p)
        : ptr(new impl::concrete_parser<ParserT, scanner_t, attr_t>(p)) {}
        template <typename ParserT>
        rule& operator=(ParserT const& p)
        {
            ptr.reset(new impl::concrete_parser<ParserT, scanner_t, attr_t>(p));
            return *this;
        }
        rule& operator=(rule const& r)
        {
            ptr.reset(new impl::concrete_parser<rule, scanner_t, attr_t>(r));
            return *this;
        }
        rule<T0, T1, T2>
        copy() const
        {
            return rule<T0, T1, T2>(ptr.get() ? ptr->clone() : 0);
        }
    private:
        friend class impl::rule_base_access;
        abstract_parser_t*
        get() const
        {
            return ptr.get();
        }
        rule(abstract_parser_t const* ptr)
        : ptr(ptr) {}
        scoped_ptr<abstract_parser_t> ptr;
    };
}}
#include <assert.h>
#include <pthread.h>
namespace boost
{
namespace detail
{
class lightweight_mutex
{
private:
    pthread_mutex_t m_;
    lightweight_mutex(lightweight_mutex const &);
    lightweight_mutex & operator=(lightweight_mutex const &);
public:
    lightweight_mutex()
    {
        pthread_mutex_init(&m_, 0);
    }
    ~lightweight_mutex()
    {
        pthread_mutex_destroy(&m_);
    }
    class scoped_lock;
    friend class scoped_lock;
    class scoped_lock
    {
    private:
        pthread_mutex_t & m_;
        scoped_lock(scoped_lock const &);
        scoped_lock & operator=(scoped_lock const &);
    public:
        scoped_lock(lightweight_mutex & m): m_(m.m_)
        {
            pthread_mutex_lock(&m_);
        }
        ~scoped_lock()
        {
            pthread_mutex_unlock(&m_);
        }
    };
};
}
}
namespace boost
{
class bad_weak_ptr: public std::exception
{
public:
    virtual char const * what() const throw()
    {
        return "boost::bad_weak_ptr";
    }
};
namespace detail
{
class sp_counted_base
{
private:
    typedef detail::lightweight_mutex mutex_type;
public:
    sp_counted_base(): use_count_(1), weak_count_(1)
    {
    }
    virtual ~sp_counted_base()
    {
    }
    virtual void dispose() = 0;
    virtual void destruct()
    {
        delete this;
    }
    virtual void * get_deleter(std::type_info const & ti) = 0;
    void add_ref_copy()
    {
        mutex_type::scoped_lock lock(mtx_);
        ++use_count_;
    }
    void add_ref_lock()
    {
        mutex_type::scoped_lock lock(mtx_);
        if(use_count_ == 0) boost::throw_exception(boost::bad_weak_ptr());
        ++use_count_;
    }
    void release()
    {
        {
            mutex_type::scoped_lock lock(mtx_);
            long new_use_count = --use_count_;
            if(new_use_count != 0) return;
        }
        dispose();
        weak_release();
    }
    void weak_add_ref()
    {
        mutex_type::scoped_lock lock(mtx_);
        ++weak_count_;
    }
    void weak_release()
    {
        long new_weak_count;
        {
            mutex_type::scoped_lock lock(mtx_);
            new_weak_count = --weak_count_;
        }
        if(new_weak_count == 0)
        {
            destruct();
        }
    }
    long use_count() const
    {
        mutex_type::scoped_lock lock(mtx_);
        return use_count_;
    }
private:
    sp_counted_base(sp_counted_base const &);
    sp_counted_base & operator= (sp_counted_base const &);
    long use_count_;
    long weak_count_;
    mutable mutex_type mtx_;
};
template<class P, class D> class sp_counted_base_impl: public sp_counted_base
{
private:
    P ptr;
    D del;
    sp_counted_base_impl(sp_counted_base_impl const &);
    sp_counted_base_impl & operator= (sp_counted_base_impl const &);
    typedef sp_counted_base_impl<P, D> this_type;
public:
    sp_counted_base_impl(P p, D d): ptr(p), del(d)
    {
    }
    virtual void dispose()
    {
        del(ptr);
    }
    virtual void * get_deleter(std::type_info const & ti)
    {
        return ti == typeid(D)? &del: 0;
    }
};
class weak_count;
class shared_count
{
private:
    sp_counted_base * pi_;
    friend class weak_count;
public:
    shared_count(): pi_(0)
    {
    }
    template<class P, class D> shared_count(P p, D d): pi_(0)
    {
        try
        {
            pi_ = new sp_counted_base_impl<P, D>(p, d);
        }
        catch(...)
        {
            d(p);
            throw;
        }
    }
    template<class Y>
    explicit shared_count(std::auto_ptr<Y> & r): pi_(new sp_counted_base_impl< Y *, checked_deleter<Y> >(r.get(), checked_deleter<Y>()))
    {
        r.release();
    }
    ~shared_count()
    {
        if(pi_ != 0) pi_->release();
    }
    shared_count(shared_count const & r): pi_(r.pi_)
    {
        if(pi_ != 0) pi_->add_ref_copy();
    }
    explicit shared_count(weak_count const & r);
    shared_count & operator= (shared_count const & r)
    {
        sp_counted_base * tmp = r.pi_;
        if(tmp != 0) tmp->add_ref_copy();
        if(pi_ != 0) pi_->release();
        pi_ = tmp;
        return *this;
    }
    void swap(shared_count & r)
    {
        sp_counted_base * tmp = r.pi_;
        r.pi_ = pi_;
        pi_ = tmp;
    }
    long use_count() const
    {
        return pi_ != 0? pi_->use_count(): 0;
    }
    bool unique() const
    {
        return use_count() == 1;
    }
    friend inline bool operator==(shared_count const & a, shared_count const & b)
    {
        return a.pi_ == b.pi_;
    }
    friend inline bool operator<(shared_count const & a, shared_count const & b)
    {
        return std::less<sp_counted_base *>()(a.pi_, b.pi_);
    }
    void * get_deleter(std::type_info const & ti) const
    {
        return pi_? pi_->get_deleter(ti): 0;
    }
};
class weak_count
{
private:
    sp_counted_base * pi_;
    friend class shared_count;
public:
    weak_count(): pi_(0)
    {
    }
    weak_count(shared_count const & r): pi_(r.pi_)
    {
        if(pi_ != 0) pi_->weak_add_ref();
    }
    weak_count(weak_count const & r): pi_(r.pi_)
    {
        if(pi_ != 0) pi_->weak_add_ref();
    }
    ~weak_count()
    {
        if(pi_ != 0) pi_->weak_release();
    }
    weak_count & operator= (shared_count const & r)
    {
        sp_counted_base * tmp = r.pi_;
        if(tmp != 0) tmp->weak_add_ref();
        if(pi_ != 0) pi_->weak_release();
        pi_ = tmp;
        return *this;
    }
    weak_count & operator= (weak_count const & r)
    {
        sp_counted_base * tmp = r.pi_;
        if(tmp != 0) tmp->weak_add_ref();
        if(pi_ != 0) pi_->weak_release();
        pi_ = tmp;
        return *this;
    }
    void swap(weak_count & r)
    {
        sp_counted_base * tmp = r.pi_;
        r.pi_ = pi_;
        pi_ = tmp;
    }
    long use_count() const
    {
        return pi_ != 0? pi_->use_count(): 0;
    }
    friend inline bool operator==(weak_count const & a, weak_count const & b)
    {
        return a.pi_ == b.pi_;
    }
    friend inline bool operator<(weak_count const & a, weak_count const & b)
    {
        return std::less<sp_counted_base *>()(a.pi_, b.pi_);
    }
};
inline shared_count::shared_count(weak_count const & r): pi_(r.pi_)
{
    if(pi_ != 0)
    {
        pi_->add_ref_lock();
    }
    else
    {
        boost::throw_exception(boost::bad_weak_ptr());
    }
}
}
}
namespace boost
{
template<class T> class weak_ptr;
template<class T> class enable_shared_from_this;
namespace detail
{
struct static_cast_tag {};
struct const_cast_tag {};
struct dynamic_cast_tag {};
struct polymorphic_cast_tag {};
template<class T> struct shared_ptr_traits
{
    typedef T & reference;
};
template<> struct shared_ptr_traits<void>
{
    typedef void reference;
};
template<> struct shared_ptr_traits<void const>
{
    typedef void reference;
};
template<> struct shared_ptr_traits<void volatile>
{
    typedef void reference;
};
template<> struct shared_ptr_traits<void const volatile>
{
    typedef void reference;
};
template<class T, class Y> void sp_enable_shared_from_this(boost::enable_shared_from_this<T> * pe, Y * px, shared_count const & pn)
{
    if(pe != 0) pe->_internal_weak_this._internal_assign(px, pn);
}
inline void sp_enable_shared_from_this(void const volatile *, void const volatile *, shared_count const &)
{
}
}
template<class T> class shared_ptr
{
private:
    typedef shared_ptr<T> this_type;
public:
    typedef T element_type;
    typedef T value_type;
    typedef T * pointer;
    typedef typename detail::shared_ptr_traits<T>::reference reference;
    shared_ptr(): px(0), pn()
    {
    }
    template<class Y>
    explicit shared_ptr(Y * p): px(p), pn(p, checked_deleter<Y>())
    {
        detail::sp_enable_shared_from_this(p, p, pn);
    }
    template<class Y, class D> shared_ptr(Y * p, D d): px(p), pn(p, d)
    {
        detail::sp_enable_shared_from_this(p, p, pn);
    }
    shared_ptr & operator=(shared_ptr const & r)
    {
        px = r.px;
        pn = r.pn;
        return *this;
    }
    template<class Y>
    explicit shared_ptr(weak_ptr<Y> const & r): pn(r.pn)
    {
        px = r.px;
    }
    template<class Y>
    shared_ptr(shared_ptr<Y> const & r): px(r.px), pn(r.pn)
    {
    }
    template<class Y>
    shared_ptr(shared_ptr<Y> const & r, detail::static_cast_tag): px(static_cast<element_type *>(r.px)), pn(r.pn)
    {
    }
    template<class Y>
    shared_ptr(shared_ptr<Y> const & r, detail::const_cast_tag): px(const_cast<element_type *>(r.px)), pn(r.pn)
    {
    }
    template<class Y>
    shared_ptr(shared_ptr<Y> const & r, detail::dynamic_cast_tag): px(dynamic_cast<element_type *>(r.px)), pn(r.pn)
    {
        if(px == 0)
        {
            pn = detail::shared_count();
        }
    }
    template<class Y>
    shared_ptr(shared_ptr<Y> const & r, detail::polymorphic_cast_tag): px(dynamic_cast<element_type *>(r.px)), pn(r.pn)
    {
        if(px == 0)
        {
            boost::throw_exception(std::bad_cast());
        }
    }
    template<class Y>
    explicit shared_ptr(std::auto_ptr<Y> & r): px(r.get()), pn()
    {
        Y * tmp = r.get();
        pn = detail::shared_count(r);
        detail::sp_enable_shared_from_this(tmp, tmp, pn);
    }
    template<class Y>
    shared_ptr & operator=(shared_ptr<Y> const & r)
    {
        px = r.px;
        pn = r.pn;
        return *this;
    }
    template<class Y>
    shared_ptr & operator=(std::auto_ptr<Y> & r)
    {
        this_type(r).swap(*this);
        return *this;
    }
    void reset()
    {
        this_type().swap(*this);
    }
    template<class Y> void reset(Y * p)
    {
        ((void)0);
        this_type(p).swap(*this);
    }
    template<class Y, class D> void reset(Y * p, D d)
    {
        this_type(p, d).swap(*this);
    }
    reference operator* () const
    {
        ((void)0);
        return *px;
    }
    T * operator-> () const
    {
        ((void)0);
        return px;
    }
    T * get() const
    {
        return px;
    }
    typedef T * this_type::*unspecified_bool_type;
    operator unspecified_bool_type() const
    {
        return px == 0? 0: &this_type::px;
    }
    bool operator! () const
    {
        return px == 0;
    }
    bool unique() const
    {
        return pn.unique();
    }
    long use_count() const
    {
        return pn.use_count();
    }
    void swap(shared_ptr<T> & other)
    {
        std::swap(px, other.px);
        pn.swap(other.pn);
    }
    template<class Y> bool _internal_less(shared_ptr<Y> const & rhs) const
    {
        return pn < rhs.pn;
    }
    void * _internal_get_deleter(std::type_info const & ti) const
    {
        return pn.get_deleter(ti);
    }
private:
    template<class Y> friend class shared_ptr;
    template<class Y> friend class weak_ptr;
    T * px;
    detail::shared_count pn;
};
template<class T, class U> inline bool operator==(shared_ptr<T> const & a, shared_ptr<U> const & b)
{
    return a.get() == b.get();
}
template<class T, class U> inline bool operator!=(shared_ptr<T> const & a, shared_ptr<U> const & b)
{
    return a.get() != b.get();
}
template<class T, class U> inline bool operator<(shared_ptr<T> const & a, shared_ptr<U> const & b)
{
    return a._internal_less(b);
}
template<class T> inline void swap(shared_ptr<T> & a, shared_ptr<T> & b)
{
    a.swap(b);
}
template<class T, class U> shared_ptr<T> static_pointer_cast(shared_ptr<U> const & r)
{
    return shared_ptr<T>(r, detail::static_cast_tag());
}
template<class T, class U> shared_ptr<T> const_pointer_cast(shared_ptr<U> const & r)
{
    return shared_ptr<T>(r, detail::const_cast_tag());
}
template<class T, class U> shared_ptr<T> dynamic_pointer_cast(shared_ptr<U> const & r)
{
    return shared_ptr<T>(r, detail::dynamic_cast_tag());
}
template<class T, class U> shared_ptr<T> shared_static_cast(shared_ptr<U> const & r)
{
    return shared_ptr<T>(r, detail::static_cast_tag());
}
template<class T, class U> shared_ptr<T> shared_dynamic_cast(shared_ptr<U> const & r)
{
    return shared_ptr<T>(r, detail::dynamic_cast_tag());
}
template<class T, class U> shared_ptr<T> shared_polymorphic_cast(shared_ptr<U> const & r)
{
    return shared_ptr<T>(r, detail::polymorphic_cast_tag());
}
template<class T, class U> shared_ptr<T> shared_polymorphic_downcast(shared_ptr<U> const & r)
{
    ((void)0);
    return shared_static_cast<T>(r);
}
template<class T> inline T * get_pointer(shared_ptr<T> const & p)
{
    return p.get();
}
template<class E, class T, class Y> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, shared_ptr<Y> const & p)
{
    os << p.get();
    return os;
}
template<class D, class T> D * get_deleter(shared_ptr<T> const & p)
{
    return static_cast<D *>(p._internal_get_deleter(typeid(D)));
}
}
namespace boost { namespace spirit {
    namespace impl {
        template <typename IdT = std::size_t>
        struct object_with_id_base_supply
        {
            typedef IdT object_id;
            typedef std::vector<object_id> id_vector;
            object_with_id_base_supply() : max_id(object_id()) {}
            object_id max_id;
            id_vector free_ids;
            object_id acquire();
            void release(object_id);
        };
        template <typename TagT, typename IdT = std::size_t>
        struct object_with_id_base
        {
            typedef TagT tag_t;
            typedef IdT object_id;
        protected:
            object_id acquire_object_id();
            void release_object_id(object_id);
        private:
            boost::shared_ptr<object_with_id_base_supply<IdT> > id_supply;
        };
        template<class TagT, typename IdT = std::size_t>
        struct object_with_id : private object_with_id_base<TagT, IdT>
        {
            typedef object_with_id<TagT, IdT> self_t;
            typedef object_with_id_base<TagT, IdT> base_t;
            typedef IdT object_id;
            object_with_id() : id(base_t::acquire_object_id()) {}
            object_with_id(self_t const &other)
                : base_t(other)
                , id(base_t::acquire_object_id())
            {}
            self_t &operator = (self_t const &other)
            {
                base_t::operator=(other);
                return *this;
            }
            ~object_with_id() { base_t::release_object_id(id); }
            object_id get_object_id() const { return id; }
        private:
            object_id const id;
        };
        template <typename IdT>
        inline IdT
        object_with_id_base_supply<IdT>::acquire()
        {
            if (free_ids.size())
            {
                object_id id = *free_ids.rbegin();
                free_ids.pop_back();
                return id;
            }
            else
            {
                if (free_ids.capacity()<=max_id)
                    free_ids.reserve(max_id*3/2+1);
                return ++max_id;
            }
        }
        template <typename IdT>
        inline void
        object_with_id_base_supply<IdT>::release(IdT id)
        {
            if (max_id == id)
                max_id--;
            else
                free_ids.push_back(id);
        }
        template <typename TagT, typename IdT>
        inline IdT
        object_with_id_base<TagT, IdT>::acquire_object_id()
        {
            {
                static boost::shared_ptr<object_with_id_base_supply<IdT> >
                    static_supply;
                if (!static_supply.get())
                    static_supply.reset(new object_with_id_base_supply<IdT>());
                id_supply = static_supply;
            }
            return id_supply->acquire();
        }
        template <typename TagT, typename IdT>
        inline void
        object_with_id_base<TagT, IdT>::release_object_id(IdT id)
        {
            id_supply->release(id);
        }
    }
}}
namespace boost
{
template<class T> class weak_ptr
{
private:
    typedef weak_ptr<T> this_type;
public:
    typedef T element_type;
    weak_ptr(): px(0), pn()
    {
    }
    template<class Y>
    weak_ptr(weak_ptr<Y> const & r): pn(r.pn)
    {
        px = r.lock().get();
    }
    template<class Y>
    weak_ptr(shared_ptr<Y> const & r): px(r.px), pn(r.pn)
    {
    }
    template<class Y>
    weak_ptr & operator=(weak_ptr<Y> const & r)
    {
        px = r.lock().get();
        pn = r.pn;
        return *this;
    }
    template<class Y>
    weak_ptr & operator=(shared_ptr<Y> const & r)
    {
        px = r.px;
        pn = r.pn;
        return *this;
    }
    shared_ptr<T> lock() const
    {
        if(expired())
        {
            return shared_ptr<element_type>();
        }
        try
        {
            return shared_ptr<element_type>(*this);
        }
        catch(bad_weak_ptr const &)
        {
            return shared_ptr<element_type>();
        }
    }
    long use_count() const
    {
        return pn.use_count();
    }
    bool expired() const
    {
        return pn.use_count() == 0;
    }
    void reset()
    {
        this_type().swap(*this);
    }
    void swap(this_type & other)
    {
        std::swap(px, other.px);
        pn.swap(other.pn);
    }
    void _internal_assign(T * px2, detail::shared_count const & pn2)
    {
        px = px2;
        pn = pn2;
    }
    template<class Y> bool _internal_less(weak_ptr<Y> const & rhs) const
    {
        return pn < rhs.pn;
    }
private:
    template<class Y> friend class weak_ptr;
    template<class Y> friend class shared_ptr;
    T * px;
    detail::weak_count pn;
};
template<class T, class U> inline bool operator<(weak_ptr<T> const & a, weak_ptr<U> const & b)
{
    return a._internal_less(b);
}
template<class T> void swap(weak_ptr<T> & a, weak_ptr<T> & b)
{
    a.swap(b);
}
template<class T> shared_ptr<T> make_shared(weak_ptr<T> const & r)
{
    return r.lock();
}
}
namespace boost { namespace spirit {
template <typename DerivedT, typename ContextT>
struct grammar;
template <typename GrammarT, typename ScannerT>
struct grammar_definition
{
    typedef typename GrammarT::template definition<ScannerT> type;
};
    namespace impl
    {
    struct grammar_tag {};
    template <typename GrammarT>
    struct grammar_helper_base
    {
        virtual int undefine(GrammarT *) = 0;
        virtual ~grammar_helper_base() {}
    };
    template <typename GrammarT>
    struct grammar_helper_list
    {
        typedef GrammarT grammar_t;
        typedef grammar_helper_base<GrammarT> helper_t;
        typedef std::vector<helper_t*> vector_t;
        grammar_helper_list() {}
        grammar_helper_list(grammar_helper_list const& x)
        {
        }
        grammar_helper_list& operator=(grammar_helper_list const& x)
        {
            return *this;
        }
        void push_back(helper_t *helper)
        { helpers.push_back(helper); }
        void pop_back()
        { helpers.pop_back(); }
        typename vector_t::size_type
        size() const
        { return helpers.size(); }
        typename vector_t::reverse_iterator
        rbegin()
        { return helpers.rbegin(); }
        typename vector_t::reverse_iterator
        rend()
        { return helpers.rend(); }
    private:
        vector_t helpers;
    };
    struct grammartract_helper_list;
    struct grammartract_helper_list
    {
        template<typename GrammarT>
        static grammar_helper_list<GrammarT>&
        do_(GrammarT const* g)
        {
            return g->helpers;
        }
    };
    template <typename GrammarT, typename DerivedT, typename ScannerT>
    struct grammar_helper : private grammar_helper_base<GrammarT>
    {
        typedef GrammarT grammar_t;
        typedef ScannerT scanner_t;
        typedef DerivedT derived_t;
        typedef typename grammar_definition<DerivedT, ScannerT>::type definition_t;
        typedef grammar_helper<grammar_t, derived_t, scanner_t> helper_t;
        typedef boost::shared_ptr<helper_t> helper_ptr_t;
        typedef boost::weak_ptr<helper_t> helper_weak_ptr_t;
        grammar_helper*
        this_() { return this; }
        grammar_helper(helper_weak_ptr_t& p)
        : definitions_cnt(0)
        , self(this_())
        { p = self; }
        definition_t&
        define(grammar_t const* target_grammar)
        {
            grammar_helper_list<GrammarT> &helpers =
                grammartract_helper_list::do_(target_grammar);
            typename grammar_t::object_id id = target_grammar->get_object_id();
            if (definitions.size()<=id)
                definitions.resize(id*3/2+1);
            if (definitions[id]!=0)
                return *definitions[id];
            std::auto_ptr<definition_t>
                result(new definition_t(target_grammar->derived()));
            helpers.push_back(this);
            ++definitions_cnt;
            definitions[id] = result.get();
            return *(result.release());
        }
        int
        undefine(grammar_t* target_grammar)
        {
            typename grammar_t::object_id id = target_grammar->get_object_id();
            if (definitions.size()<=id)
                return 0;
            delete definitions[id];
            definitions[id] = 0;
            if (--definitions_cnt==0)
                self.reset();
            return 0;
        }
    private:
        std::vector<definition_t*> definitions;
        unsigned long definitions_cnt;
        helper_ptr_t self;
    };
    template<typename DerivedT, typename ContextT, typename ScannerT>
    inline typename DerivedT::template definition<ScannerT> &
    get_definition(grammar<DerivedT, ContextT> const* self)
    {
        typedef grammar<DerivedT, ContextT> self_t;
        typedef impl::grammar_helper<self_t, DerivedT, ScannerT> helper_t;
        typedef typename helper_t::helper_weak_ptr_t ptr_t;
        static ptr_t helper;
        if (!boost::make_shared(helper).get())
            new helper_t(helper);
        return boost::make_shared(helper)->define(self);
    }
    template <int N>
    struct call_helper {
        template <typename RT, typename DefinitionT, typename ScannerT>
        static void
        do_ (RT &result, DefinitionT &def, ScannerT const &scan)
        {
            result = def.template get_start_parser<N>()->parse(scan);
        }
    };
    template <>
    struct call_helper<0> {
        template <typename RT, typename DefinitionT, typename ScannerT>
        static void
        do_ (RT &result, DefinitionT &def, ScannerT const &scan)
        {
            result = def.start().parse(scan);
        }
    };
    template<int N, typename DerivedT, typename ContextT, typename ScannerT>
    inline typename parser_result<grammar<DerivedT, ContextT>, ScannerT>::type
    grammar_parser_parse(
        grammar<DerivedT, ContextT> const* self,
        ScannerT const &scan)
    {
        typedef
            typename parser_result<grammar<DerivedT, ContextT>, ScannerT>::type
            result_t;
        typedef typename DerivedT::template definition<ScannerT> definition_t;
        result_t result;
        definition_t &def = get_definition<DerivedT, ContextT, ScannerT>(self);
        call_helper<N>::do_(result, def, scan);
        return result;
    }
    template<typename GrammarT>
    inline void
    grammar_destruct(GrammarT* self)
    {
        typedef impl::grammar_helper_base<GrammarT> helper_base_t;
        typedef grammar_helper_list<GrammarT> helper_list_t;
        typedef typename helper_list_t::vector_t::reverse_iterator iterator_t;
        helper_list_t& helpers =
            grammartract_helper_list::do_(self);
        std::for_each(helpers.rbegin(), helpers.rend(),
            std::bind2nd(std::mem_fun(&helper_base_t::undefine), self));
    }
    template <typename DerivedT, int N, typename ContextT>
    class entry_grammar
        : public parser<entry_grammar<DerivedT, N, ContextT> >
    {
    public:
        typedef entry_grammar<DerivedT, N, ContextT> self_t;
        typedef DerivedT const& embed_t;
        typedef typename ContextT::context_linker_t context_t;
        typedef typename context_t::attr_t attr_t;
        template <typename ScannerT>
        struct result
        {
            typedef typename match_result<ScannerT, attr_t>::type type;
        };
        entry_grammar(DerivedT const &p) : target_grammar(p) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse_main(ScannerT const& scan) const
        { return impl::grammar_parser_parse<N>(&target_grammar, scan); }
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            typedef parser_scanner_linker<ScannerT> scanner_t;
            scanner_t scan_wrap(scan); context_t context_wrap(*this); context_wrap.pre_parse(*this, scan_wrap); result_t hit = parse_main(scan); return context_wrap.post_parse(hit, *this, scan_wrap);
        }
    private:
        DerivedT const &target_grammar;
    };
    }
}}
namespace boost { namespace spirit {
template <typename DerivedT, typename ContextT = parser_context<> >
struct grammar
    : public parser<DerivedT>
    , public ContextT::base_t
    , public context_aux<ContextT, DerivedT>
    , public impl::object_with_id<impl::grammar_tag>
{
    typedef grammar<DerivedT, ContextT> self_t;
    typedef DerivedT const& embed_t;
    typedef typename ContextT::context_linker_t context_t;
    typedef typename context_t::attr_t attr_t;
    template <typename ScannerT>
    struct result
    {
        typedef typename match_result<ScannerT, attr_t>::type type;
    };
    grammar() {}
    ~grammar() { impl::grammar_destruct(this); }
    template <typename ScannerT>
    typename parser_result<self_t, ScannerT>::type
    parse_main(ScannerT const& scan) const
    { return impl::grammar_parser_parse<0>(this, scan); }
    template <typename ScannerT>
    typename parser_result<self_t, ScannerT>::type
    parse(ScannerT const& scan) const
    {
        typedef typename parser_result<self_t, ScannerT>::type result_t;
        typedef parser_scanner_linker<ScannerT> scanner_t;
        scanner_t scan_wrap(scan); context_t context_wrap(*this); context_wrap.pre_parse(*this, scan_wrap); result_t hit = parse_main(scan); return context_wrap.post_parse(hit, *this, scan_wrap);
    }
    template <int N>
    impl::entry_grammar<DerivedT, N, ContextT>
    use_parser() const
    { return impl::entry_grammar<DerivedT, N, ContextT>( this->derived()); }
    private: friend struct impl::grammartract_helper_list; mutable impl::grammar_helper_list<self_t> helpers;
};
}}
namespace boost { namespace spirit {
    template <typename ParserT, typename ActionT>
    class action : public unary<ParserT, parser<action<ParserT, ActionT> > >
    {
    public:
        typedef action<ParserT, ActionT> self_t;
        typedef action_parser_category parser_category_t;
        typedef unary<ParserT, parser<self_t> > base_t;
        typedef ActionT predicate_t;
        template <typename ScannerT>
        struct result
        {
            typedef typename parser_result<ParserT, ScannerT>::type type;
        };
        action(ParserT const& p, ActionT const& a)
        : base_t(p)
        , actor(a) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename ScannerT::iterator_t iterator_t;
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            scan.at_end();
            iterator_t save = scan.first;
            result_t hit = this->subject().parse(scan);
            if (hit)
            {
                typename result_t::return_t val = hit.value();
                scan.do_action(actor, val, save, scan.first);
            }
            return hit;
        }
        ActionT const& predicate() const { return actor; }
    private:
        ActionT actor;
    };
}}
namespace boost { namespace spirit {
    namespace impl
    {
        template<typename T>
        struct default_as_parser
        {
            typedef T type;
            static type const& convert(type const& p)
            {
                return p;
            }
        };
        struct char_as_parser
        {
            typedef chlit<char> type;
            static type convert(char ch)
            {
                return type(ch);
            }
        };
        struct wchar_as_parser
        {
            typedef chlit<wchar_t> type;
            static type convert(wchar_t ch)
            {
                return type(ch);
            }
        };
        struct string_as_parser
        {
            typedef strlit<char const*> type;
            static type convert(char const* str)
            {
                return type(str);
            }
        };
        struct wstring_as_parser
        {
            typedef strlit<wchar_t const*> type;
            static type convert(wchar_t const* str)
            {
                return type(str);
            }
        };
    }
    template<typename T>
    struct as_parser : impl::default_as_parser<T> {};
    template<>
    struct as_parser<char> : impl::char_as_parser {};
    template<>
    struct as_parser<wchar_t> : impl::wchar_as_parser {};
    template<>
    struct as_parser<char*> : impl::string_as_parser {};
    template<>
    struct as_parser<char const*> : impl::string_as_parser {};
    template<>
    struct as_parser<wchar_t*> : impl::wstring_as_parser {};
    template<>
    struct as_parser<wchar_t const*> : impl::wstring_as_parser {};
    template<int N>
    struct as_parser<char[N]> : impl::string_as_parser {};
    template<int N>
    struct as_parser<wchar_t[N]> : impl::wstring_as_parser {};
    template<int N>
    struct as_parser<char const[N]> : impl::string_as_parser {};
    template<int N>
    struct as_parser<wchar_t const[N]> : impl::wstring_as_parser {};
}}
namespace boost { namespace spirit {
    struct sequence_parser_gen;
    template <typename A, typename B>
    struct sequence : public binary<A, B, parser<sequence<A, B> > >
    {
        typedef sequence<A, B> self_t;
        typedef binary_parser_category parser_category_t;
        typedef sequence_parser_gen parser_generator_t;
        typedef binary<A, B, parser<self_t> > base_t;
        sequence(A const& a, B const& b)
        : base_t(a, b) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            if (result_t ma = this->left().parse(scan))
                if (result_t mb = this->right().parse(scan))
                {
                    scan.concat_match(ma, mb);
                    return ma;
                }
            return scan.no_match();
        }
    };
    struct sequence_parser_gen
    {
        template <typename A, typename B>
        struct result
        {
            typedef
                sequence<
                    typename as_parser<A>::type
                  , typename as_parser<B>::type
                >
            type;
        };
        template <typename A, typename B>
        static sequence<
            typename as_parser<A>::type
          , typename as_parser<B>::type
        >
        generate(A const& a, B const& b)
        {
            return sequence<typename as_parser<A>::type,
                typename as_parser<B>::type>
                    (as_parser<A>::convert(a), as_parser<B>::convert(b));
        }
    };
    template <typename A, typename B>
    sequence<A, B>
    operator>>(parser<A> const& a, parser<B> const& b);
    template <typename A>
    sequence<A, chlit<char> >
    operator>>(parser<A> const& a, char b);
    template <typename B>
    sequence<chlit<char>, B>
    operator>>(char a, parser<B> const& b);
    template <typename A>
    sequence<A, strlit<char const*> >
    operator>>(parser<A> const& a, char const* b);
    template <typename B>
    sequence<strlit<char const*>, B>
    operator>>(char const* a, parser<B> const& b);
    template <typename A>
    sequence<A, chlit<wchar_t> >
    operator>>(parser<A> const& a, wchar_t b);
    template <typename B>
    sequence<chlit<wchar_t>, B>
    operator>>(wchar_t a, parser<B> const& b);
    template <typename A>
    sequence<A, strlit<wchar_t const*> >
    operator>>(parser<A> const& a, wchar_t const* b);
    template <typename B>
    sequence<strlit<wchar_t const*>, B>
    operator>>(wchar_t const* a, parser<B> const& b);
}}
namespace boost { namespace spirit {
    template <typename A, typename B>
    inline sequence<A, B>
    operator>>(parser<A> const& a, parser<B> const& b)
    {
        return sequence<A, B>(a.derived(), b.derived());
    }
    template <typename A>
    inline sequence<A, chlit<char> >
    operator>>(parser<A> const& a, char b)
    {
        return sequence<A, chlit<char> >(a.derived(), b);
    }
    template <typename B>
    inline sequence<chlit<char>, B>
    operator>>(char a, parser<B> const& b)
    {
        return sequence<chlit<char>, B>(a, b.derived());
    }
    template <typename A>
    inline sequence<A, strlit<char const*> >
    operator>>(parser<A> const& a, char const* b)
    {
        return sequence<A, strlit<char const*> >(a.derived(), b);
    }
    template <typename B>
    inline sequence<strlit<char const*>, B>
    operator>>(char const* a, parser<B> const& b)
    {
        return sequence<strlit<char const*>, B>(a, b.derived());
    }
    template <typename A>
    inline sequence<A, chlit<wchar_t> >
    operator>>(parser<A> const& a, wchar_t b)
    {
        return sequence<A, chlit<wchar_t> >(a.derived(), b);
    }
    template <typename B>
    inline sequence<chlit<wchar_t>, B>
    operator>>(wchar_t a, parser<B> const& b)
    {
        return sequence<chlit<wchar_t>, B>(a, b.derived());
    }
    template <typename A>
    inline sequence<A, strlit<wchar_t const*> >
    operator>>(parser<A> const& a, wchar_t const* b)
    {
        return sequence<A, strlit<wchar_t const*> >(a.derived(), b);
    }
    template <typename B>
    inline sequence<strlit<wchar_t const*>, B>
    operator>>(wchar_t const* a, parser<B> const& b)
    {
        return sequence<strlit<wchar_t const*>, B>(a, b.derived());
    }
}}
namespace boost { namespace spirit {
    template <typename A, typename B>
    sequence<A, B>
    operator&&(parser<A> const& a, parser<B> const& b);
    template <typename A>
    sequence<A, chlit<char> >
    operator&&(parser<A> const& a, char b);
    template <typename B>
    sequence<chlit<char>, B>
    operator&&(char a, parser<B> const& b);
    template <typename A>
    sequence<A, strlit<char const*> >
    operator&&(parser<A> const& a, char const* b);
    template <typename B>
    sequence<strlit<char const*>, B>
    operator&&(char const* a, parser<B> const& b);
    template <typename A>
    sequence<A, chlit<wchar_t> >
    operator&&(parser<A> const& a, wchar_t b);
    template <typename B>
    sequence<chlit<wchar_t>, B>
    operator&&(wchar_t a, parser<B> const& b);
    template <typename A>
    sequence<A, strlit<wchar_t const*> >
    operator&&(parser<A> const& a, wchar_t const* b);
    template <typename B>
    sequence<strlit<wchar_t const*>, B>
    operator&&(wchar_t const* a, parser<B> const& b);
}}
namespace boost { namespace spirit {
    template <typename A, typename B>
    inline sequence<A, B>
    operator&&(parser<A> const& a, parser<B> const& b)
    {
        return sequence<A, B>(a.derived(), b.derived());
    }
    template <typename A>
    inline sequence<A, chlit<char> >
    operator&&(parser<A> const& a, char b)
    {
        return sequence<A, chlit<char> >(a.derived(), b);
    }
    template <typename B>
    inline sequence<chlit<char>, B>
    operator&&(char a, parser<B> const& b)
    {
        return sequence<chlit<char>, B>(a, b.derived());
    }
    template <typename A>
    inline sequence<A, strlit<char const*> >
    operator&&(parser<A> const& a, char const* b)
    {
        return sequence<A, strlit<char const*> >(a.derived(), b);
    }
    template <typename B>
    inline sequence<strlit<char const*>, B>
    operator&&(char const* a, parser<B> const& b)
    {
        return sequence<strlit<char const*>, B>(a, b.derived());
    }
    template <typename A>
    inline sequence<A, chlit<wchar_t> >
    operator&&(parser<A> const& a, wchar_t b)
    {
        return sequence<A, chlit<wchar_t> >(a.derived(), b);
    }
    template <typename B>
    inline sequence<chlit<wchar_t>, B>
    operator&&(wchar_t a, parser<B> const& b)
    {
        return sequence<chlit<wchar_t>, B>(a, b.derived());
    }
    template <typename A>
    inline sequence<A, strlit<wchar_t const*> >
    operator&&(parser<A> const& a, wchar_t const* b)
    {
        return sequence<A, strlit<wchar_t const*> >(a.derived(), b);
    }
    template <typename B>
    inline sequence<strlit<wchar_t const*>, B>
    operator&&(wchar_t const* a, parser<B> const& b)
    {
        return sequence<strlit<wchar_t const*>, B>(a, b.derived());
    }
}}
namespace boost { namespace spirit {
    struct sequential_or_parser_gen;
    template <typename A, typename B>
    struct sequential_or : public binary<A, B, parser<sequential_or<A, B> > >
    {
        typedef sequential_or<A, B> self_t;
        typedef binary_parser_category parser_category_t;
        typedef sequential_or_parser_gen parser_generator_t;
        typedef binary<A, B, parser<self_t> > base_t;
        sequential_or(A const& a, B const& b)
        : base_t(a, b) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            typedef typename ScannerT::iterator_t iterator_t;
            {
                iterator_t save = scan.first;
                if (result_t ma = this->left().parse(scan))
                {
                    save = scan.first;
                    if (result_t mb = this->right().parse(scan))
                    {
                        scan.concat_match(ma, mb);
                        return ma;
                    }
                    else
                    {
                        scan.first = save;
                        return ma;
                    }
                }
                scan.first = save;
            }
            return this->right().parse(scan);
        }
    };
    struct sequential_or_parser_gen
    {
        template <typename A, typename B>
        struct result
        {
            typedef
                sequential_or<
                    typename as_parser<A>::type
                  , typename as_parser<B>::type
                >
            type;
        };
        template <typename A, typename B>
        static sequential_or<
            typename as_parser<A>::type
          , typename as_parser<B>::type
        >
        generate(A const& a, B const& b)
        {
            return sequential_or<typename as_parser<A>::type,
                typename as_parser<B>::type>
                    (as_parser<A>::convert(a), as_parser<B>::convert(b));
        }
    };
    template <typename A, typename B>
    sequential_or<A, B>
    operator||(parser<A> const& a, parser<B> const& b);
    template <typename A>
    sequential_or<A, chlit<char> >
    operator||(parser<A> const& a, char b);
    template <typename B>
    sequential_or<chlit<char>, B>
    operator||(char a, parser<B> const& b);
    template <typename A>
    sequential_or<A, strlit<char const*> >
    operator||(parser<A> const& a, char const* b);
    template <typename B>
    sequential_or<strlit<char const*>, B>
    operator||(char const* a, parser<B> const& b);
    template <typename A>
    sequential_or<A, chlit<wchar_t> >
    operator||(parser<A> const& a, wchar_t b);
    template <typename B>
    sequential_or<chlit<wchar_t>, B>
    operator||(wchar_t a, parser<B> const& b);
    template <typename A>
    sequential_or<A, strlit<wchar_t const*> >
    operator||(parser<A> const& a, wchar_t const* b);
    template <typename B>
    sequential_or<strlit<wchar_t const*>, B>
    operator||(wchar_t const* a, parser<B> const& b);
}}
namespace boost { namespace spirit {
    template <typename A, typename B>
    inline sequential_or<A, B>
    operator||(parser<A> const& a, parser<B> const& b)
    {
        return sequential_or<A, B>(a.derived(), b.derived());
    }
    template <typename A>
    inline sequential_or<A, chlit<char> >
    operator||(parser<A> const& a, char b)
    {
        return sequential_or<A, chlit<char> >(a.derived(), b);
    }
    template <typename B>
    inline sequential_or<chlit<char>, B>
    operator||(char a, parser<B> const& b)
    {
        return sequential_or<chlit<char>, B>(a, b.derived());
    }
    template <typename A>
    inline sequential_or<A, strlit<char const*> >
    operator||(parser<A> const& a, char const* b)
    {
        return sequential_or<A, strlit<char const*> >(a.derived(), b);
    }
    template <typename B>
    inline sequential_or<strlit<char const*>, B>
    operator||(char const* a, parser<B> const& b)
    {
        return sequential_or<strlit<char const*>, B>(a, b.derived());
    }
    template <typename A>
    inline sequential_or<A, chlit<wchar_t> >
    operator||(parser<A> const& a, wchar_t b)
    {
        return sequential_or<A, chlit<wchar_t> >(a.derived(), b);
    }
    template <typename B>
    inline sequential_or<chlit<wchar_t>, B>
    operator||(wchar_t a, parser<B> const& b)
    {
        return sequential_or<chlit<wchar_t>, B>(a, b.derived());
    }
    template <typename A>
    inline sequential_or<A, strlit<wchar_t const*> >
    operator||(parser<A> const& a, wchar_t const* b)
    {
        return sequential_or<A, strlit<wchar_t const*> >(a.derived(), b);
    }
    template <typename B>
    inline sequential_or<strlit<wchar_t const*>, B>
    operator||(wchar_t const* a, parser<B> const& b)
    {
        return sequential_or<strlit<wchar_t const*>, B>(a, b.derived());
    }
}}
namespace boost { namespace spirit {
    struct alternative_parser_gen;
    template <typename A, typename B>
    struct alternative
    : public binary<A, B, parser<alternative<A, B> > >
    {
        typedef alternative<A, B> self_t;
        typedef binary_parser_category parser_category_t;
        typedef alternative_parser_gen parser_generator_t;
        typedef binary<A, B, parser<self_t> > base_t;
        alternative(A const& a, B const& b)
        : base_t(a, b) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            typedef typename ScannerT::iterator_t iterator_t;
            {
                iterator_t save = scan.first;
                if (result_t hit = this->left().parse(scan))
                    return hit;
                scan.first = save;
            }
            return this->right().parse(scan);
        }
    };
    struct alternative_parser_gen
    {
        template <typename A, typename B>
        struct result
        {
            typedef
                alternative<
                    typename as_parser<A>::type
                  , typename as_parser<B>::type
                >
            type;
        };
        template <typename A, typename B>
        static alternative<
            typename as_parser<A>::type
          , typename as_parser<B>::type
        >
        generate(A const& a, B const& b)
        {
            return alternative<typename as_parser<A>::type,
                typename as_parser<B>::type>
                    (as_parser<A>::convert(a), as_parser<B>::convert(b));
        }
    };
    template <typename A, typename B>
    alternative<A, B>
    operator|(parser<A> const& a, parser<B> const& b);
    template <typename A>
    alternative<A, chlit<char> >
    operator|(parser<A> const& a, char b);
    template <typename B>
    alternative<chlit<char>, B>
    operator|(char a, parser<B> const& b);
    template <typename A>
    alternative<A, strlit<char const*> >
    operator|(parser<A> const& a, char const* b);
    template <typename B>
    alternative<strlit<char const*>, B>
    operator|(char const* a, parser<B> const& b);
    template <typename A>
    alternative<A, chlit<wchar_t> >
    operator|(parser<A> const& a, wchar_t b);
    template <typename B>
    alternative<chlit<wchar_t>, B>
    operator|(wchar_t a, parser<B> const& b);
    template <typename A>
    alternative<A, strlit<wchar_t const*> >
    operator|(parser<A> const& a, wchar_t const* b);
    template <typename B>
    alternative<strlit<wchar_t const*>, B>
    operator|(wchar_t const* a, parser<B> const& b);
}}
namespace boost { namespace spirit {
    template <typename A, typename B>
    inline alternative<A, B>
    operator|(parser<A> const& a, parser<B> const& b)
    {
        return alternative<A, B>(a.derived(), b.derived());
    }
    template <typename A>
    inline alternative<A, chlit<char> >
    operator|(parser<A> const& a, char b)
    {
        return alternative<A, chlit<char> >(a.derived(), b);
    }
    template <typename B>
    inline alternative<chlit<char>, B>
    operator|(char a, parser<B> const& b)
    {
        return alternative<chlit<char>, B>(a, b.derived());
    }
    template <typename A>
    inline alternative<A, strlit<char const*> >
    operator|(parser<A> const& a, char const* b)
    {
        return alternative<A, strlit<char const*> >(a.derived(), b);
    }
    template <typename B>
    inline alternative<strlit<char const*>, B>
    operator|(char const* a, parser<B> const& b)
    {
        return alternative<strlit<char const*>, B>(a, b.derived());
    }
    template <typename A>
    inline alternative<A, chlit<wchar_t> >
    operator|(parser<A> const& a, wchar_t b)
    {
        return alternative<A, chlit<wchar_t> >(a.derived(), b);
    }
    template <typename B>
    inline alternative<chlit<wchar_t>, B>
    operator|(wchar_t a, parser<B> const& b)
    {
        return alternative<chlit<wchar_t>, B>(a, b.derived());
    }
    template <typename A>
    inline alternative<A, strlit<wchar_t const*> >
    operator|(parser<A> const& a, wchar_t const* b)
    {
        return alternative<A, strlit<wchar_t const*> >(a.derived(), b);
    }
    template <typename B>
    inline alternative<strlit<wchar_t const*>, B>
    operator|(wchar_t const* a, parser<B> const& b)
    {
        return alternative<strlit<wchar_t const*>, B>(a, b.derived());
    }
}}
namespace boost { namespace spirit {
    struct difference_parser_gen;
    template <typename A, typename B>
    struct difference
    : public binary<A, B, parser<difference<A, B> > >
    {
        typedef difference<A, B> self_t;
        typedef binary_parser_category parser_category_t;
        typedef difference_parser_gen parser_generator_t;
        typedef binary<A, B, parser<self_t> > base_t;
        difference(A const& a, B const& b)
        : base_t(a, b) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            typedef typename ScannerT::iterator_t iterator_t;
            iterator_t save = scan.first;
            if (result_t hl = this->left().parse(scan))
            {
                std::swap(save, scan.first);
                result_t hr = this->right().parse(scan);
                if (!hr || (hr.length() < hl.length()))
                {
                    scan.first = save;
                    return hl;
                }
            }
            return scan.no_match();
        }
    };
    struct difference_parser_gen
    {
        template <typename A, typename B>
        struct result
        {
            typedef
                difference<
                    typename as_parser<A>::type
                  , typename as_parser<B>::type
                >
            type;
        };
        template <typename A, typename B>
        static difference<
            typename as_parser<A>::type
          , typename as_parser<B>::type
        >
        generate(A const& a, B const& b)
        {
            return difference<typename as_parser<A>::type,
                typename as_parser<B>::type>
                    (as_parser<A>::convert(a), as_parser<B>::convert(b));
        }
    };
    template <typename A, typename B>
    difference<A, B>
    operator-(parser<A> const& a, parser<B> const& b);
    template <typename A>
    difference<A, chlit<char> >
    operator-(parser<A> const& a, char b);
    template <typename B>
    difference<chlit<char>, B>
    operator-(char a, parser<B> const& b);
    template <typename A>
    difference<A, strlit<char const*> >
    operator-(parser<A> const& a, char const* b);
    template <typename B>
    difference<strlit<char const*>, B>
    operator-(char const* a, parser<B> const& b);
    template <typename A>
    difference<A, chlit<wchar_t> >
    operator-(parser<A> const& a, wchar_t b);
    template <typename B>
    difference<chlit<wchar_t>, B>
    operator-(wchar_t a, parser<B> const& b);
    template <typename A>
    difference<A, strlit<wchar_t const*> >
    operator-(parser<A> const& a, wchar_t const* b);
    template <typename B>
    difference<strlit<wchar_t const*>, B>
    operator-(wchar_t const* a, parser<B> const& b);
}}
namespace boost { namespace spirit {
    template <typename A, typename B>
    inline difference<A, B>
    operator-(parser<A> const& a, parser<B> const& b)
    {
        return difference<A, B>(a.derived(), b.derived());
    }
    template <typename A>
    inline difference<A, chlit<char> >
    operator-(parser<A> const& a, char b)
    {
        return difference<A, chlit<char> >(a.derived(), b);
    }
    template <typename B>
    inline difference<chlit<char>, B>
    operator-(char a, parser<B> const& b)
    {
        return difference<chlit<char>, B>(a, b.derived());
    }
    template <typename A>
    inline difference<A, strlit<char const*> >
    operator-(parser<A> const& a, char const* b)
    {
        return difference<A, strlit<char const*> >(a.derived(), b);
    }
    template <typename B>
    inline difference<strlit<char const*>, B>
    operator-(char const* a, parser<B> const& b)
    {
        return difference<strlit<char const*>, B>(a, b.derived());
    }
    template <typename A>
    inline difference<A, chlit<wchar_t> >
    operator-(parser<A> const& a, wchar_t b)
    {
        return difference<A, chlit<wchar_t> >(a.derived(), b);
    }
    template <typename B>
    inline difference<chlit<wchar_t>, B>
    operator-(wchar_t a, parser<B> const& b)
    {
        return difference<chlit<wchar_t>, B>(a, b.derived());
    }
    template <typename A>
    inline difference<A, strlit<wchar_t const*> >
    operator-(parser<A> const& a, wchar_t const* b)
    {
        return difference<A, strlit<wchar_t const*> >(a.derived(), b);
    }
    template <typename B>
    inline difference<strlit<wchar_t const*>, B>
    operator-(wchar_t const* a, parser<B> const& b)
    {
        return difference<strlit<wchar_t const*>, B>(a, b.derived());
    }
}}
namespace boost { namespace spirit {
    struct intersection_parser_gen;
    template <typename A, typename B>
    struct intersection
    : public binary<A, B, parser<intersection<A, B> > >
    {
        typedef intersection<A, B> self_t;
        typedef binary_parser_category parser_category_t;
        typedef intersection_parser_gen parser_generator_t;
        typedef binary<A, B, parser<self_t> > base_t;
        intersection(A const& a, B const& b)
        : base_t(a, b) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            typedef typename ScannerT::iterator_t iterator_t;
            iterator_t save = scan.first;
            if (result_t hl = this->left().parse(scan))
            {
                ScannerT bscan(scan.first, scan.first);
                scan.first = save;
                result_t hr = this->right().parse(bscan);
                if (hl.length() == hr.length())
                    return hl;
            }
            return scan.no_match();
        }
    };
    struct intersection_parser_gen
    {
        template <typename A, typename B>
        struct result
        {
            typedef
                intersection<
                    typename as_parser<A>::type
                  , typename as_parser<B>::type
                >
            type;
        };
        template <typename A, typename B>
        static intersection<
            typename as_parser<A>::type
          , typename as_parser<B>::type
        >
        generate(A const& a, B const& b)
        {
            return intersection<typename as_parser<A>::type,
                typename as_parser<B>::type>
                    (as_parser<A>::convert(a), as_parser<B>::convert(b));
        }
    };
    template <typename A, typename B>
    intersection<A, B>
    operator&(parser<A> const& a, parser<B> const& b);
    template <typename A>
    intersection<A, chlit<char> >
    operator&(parser<A> const& a, char b);
    template <typename B>
    intersection<chlit<char>, B>
    operator&(char a, parser<B> const& b);
    template <typename A>
    intersection<A, strlit<char const*> >
    operator&(parser<A> const& a, char const* b);
    template <typename B>
    intersection<strlit<char const*>, B>
    operator&(char const* a, parser<B> const& b);
    template <typename A>
    intersection<A, chlit<wchar_t> >
    operator&(parser<A> const& a, wchar_t b);
    template <typename B>
    intersection<chlit<wchar_t>, B>
    operator&(wchar_t a, parser<B> const& b);
    template <typename A>
    intersection<A, strlit<wchar_t const*> >
    operator&(parser<A> const& a, wchar_t const* b);
    template <typename B>
    intersection<strlit<wchar_t const*>, B>
    operator&(wchar_t const* a, parser<B> const& b);
}}
namespace boost { namespace spirit {
    template <typename A, typename B>
    inline intersection<A, B>
    operator&(parser<A> const& a, parser<B> const& b)
    {
        return intersection<A, B>(a.derived(), b.derived());
    }
    template <typename A>
    inline intersection<A, chlit<char> >
    operator&(parser<A> const& a, char b)
    {
        return intersection<A, chlit<char> >(a.derived(), b);
    }
    template <typename B>
    inline intersection<chlit<char>, B>
    operator&(char a, parser<B> const& b)
    {
        return intersection<chlit<char>, B>(a, b.derived());
    }
    template <typename A>
    inline intersection<A, strlit<char const*> >
    operator&(parser<A> const& a, char const* b)
    {
        return intersection<A, strlit<char const*> >(a.derived(), b);
    }
    template <typename B>
    inline intersection<strlit<char const*>, B>
    operator&(char const* a, parser<B> const& b)
    {
        return intersection<strlit<char const*>, B>(a, b.derived());
    }
    template <typename A>
    inline intersection<A, chlit<wchar_t> >
    operator&(parser<A> const& a, wchar_t b)
    {
        return intersection<A, chlit<wchar_t> >(a.derived(), b);
    }
    template <typename B>
    inline intersection<chlit<wchar_t>, B>
    operator&(wchar_t a, parser<B> const& b)
    {
        return intersection<chlit<wchar_t>, B>(a, b.derived());
    }
    template <typename A>
    inline intersection<A, strlit<wchar_t const*> >
    operator&(parser<A> const& a, wchar_t const* b)
    {
        return intersection<A, strlit<wchar_t const*> >(a.derived(), b);
    }
    template <typename B>
    inline intersection<strlit<wchar_t const*>, B>
    operator&(wchar_t const* a, parser<B> const& b)
    {
        return intersection<strlit<wchar_t const*>, B>(a, b.derived());
    }
}}
namespace boost { namespace spirit {
    struct exclusive_or_parser_gen;
    template <typename A, typename B>
    struct exclusive_or
    : public binary<A, B, parser<exclusive_or<A, B> > >
    {
        typedef exclusive_or<A, B> self_t;
        typedef binary_parser_category parser_category_t;
        typedef exclusive_or_parser_gen parser_generator_t;
        typedef binary<A, B, parser<self_t> > base_t;
        exclusive_or(A const& a, B const& b)
        : base_t(a, b) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            typedef typename ScannerT::iterator_t iterator_t;
            iterator_t save = scan.first;
            result_t l = this->left().parse(scan);
            std::swap(save, scan.first);
            result_t r = this->right().parse(scan);
            if (l ? !bool(r) : bool(r))
            {
                if (l)
                    scan.first = save;
                return l ? l : r;
            }
            return scan.no_match();
        }
    };
    struct exclusive_or_parser_gen
    {
        template <typename A, typename B>
        struct result
        {
            typedef
                exclusive_or<
                    typename as_parser<A>::type
                  , typename as_parser<B>::type
                >
            type;
        };
        template <typename A, typename B>
        static exclusive_or<
            typename as_parser<A>::type
          , typename as_parser<B>::type
        >
        generate(A const& a, B const& b)
        {
            return exclusive_or<typename as_parser<A>::type,
                typename as_parser<B>::type>
                    (as_parser<A>::convert(a), as_parser<B>::convert(b));
        }
    };
    template <typename A, typename B>
    exclusive_or<A, B>
    operator^(parser<A> const& a, parser<B> const& b);
    template <typename A>
    exclusive_or<A, chlit<char> >
    operator^(parser<A> const& a, char b);
    template <typename B>
    exclusive_or<chlit<char>, B>
    operator^(char a, parser<B> const& b);
    template <typename A>
    exclusive_or<A, strlit<char const*> >
    operator^(parser<A> const& a, char const* b);
    template <typename B>
    exclusive_or<strlit<char const*>, B>
    operator^(char const* a, parser<B> const& b);
    template <typename A>
    exclusive_or<A, chlit<wchar_t> >
    operator^(parser<A> const& a, wchar_t b);
    template <typename B>
    exclusive_or<chlit<wchar_t>, B>
    operator^(wchar_t a, parser<B> const& b);
    template <typename A>
    exclusive_or<A, strlit<wchar_t const*> >
    operator^(parser<A> const& a, wchar_t const* b);
    template <typename B>
    exclusive_or<strlit<wchar_t const*>, B>
    operator^(wchar_t const* a, parser<B> const& b);
}}
namespace boost { namespace spirit {
    template <typename A, typename B>
    inline exclusive_or<A, B>
    operator^(parser<A> const& a, parser<B> const& b)
    {
        return exclusive_or<A, B>(a.derived(), b.derived());
    }
    template <typename A>
    inline exclusive_or<A, chlit<char> >
    operator^(parser<A> const& a, char b)
    {
        return exclusive_or<A, chlit<char> >(a.derived(), b);
    }
    template <typename B>
    inline exclusive_or<chlit<char>, B>
    operator^(char a, parser<B> const& b)
    {
        return exclusive_or<chlit<char>, B>(a, b.derived());
    }
    template <typename A>
    inline exclusive_or<A, strlit<char const*> >
    operator^(parser<A> const& a, char const* b)
    {
        return exclusive_or<A, strlit<char const*> >(a.derived(), b);
    }
    template <typename B>
    inline exclusive_or<strlit<char const*>, B>
    operator^(char const* a, parser<B> const& b)
    {
        return exclusive_or<strlit<char const*>, B>(a, b.derived());
    }
    template <typename A>
    inline exclusive_or<A, chlit<wchar_t> >
    operator^(parser<A> const& a, wchar_t b)
    {
        return exclusive_or<A, chlit<wchar_t> >(a.derived(), b);
    }
    template <typename B>
    inline exclusive_or<chlit<wchar_t>, B>
    operator^(wchar_t a, parser<B> const& b)
    {
        return exclusive_or<chlit<wchar_t>, B>(a, b.derived());
    }
    template <typename A>
    inline exclusive_or<A, strlit<wchar_t const*> >
    operator^(parser<A> const& a, wchar_t const* b)
    {
        return exclusive_or<A, strlit<wchar_t const*> >(a.derived(), b);
    }
    template <typename B>
    inline exclusive_or<strlit<wchar_t const*>, B>
    operator^(wchar_t const* a, parser<B> const& b)
    {
        return exclusive_or<strlit<wchar_t const*>, B>(a, b.derived());
    }
}}
namespace boost { namespace spirit {
    struct kleene_star_parser_gen;
    template <typename S>
    struct kleene_star
    : public unary<S, parser<kleene_star<S> > >
    {
        typedef kleene_star<S> self_t;
        typedef unary_parser_category parser_category_t;
        typedef kleene_star_parser_gen parser_generator_t;
        typedef unary<S, parser<self_t> > base_t;
        kleene_star(S const& a)
        : base_t(a) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            typedef typename ScannerT::iterator_t iterator_t;
            result_t hit = scan.empty_match();
            for (;;)
            {
                iterator_t save = scan.first;
                if (result_t next = this->subject().parse(scan))
                {
                    scan.concat_match(hit, next);
                }
                else
                {
                    scan.first = save;
                    return hit;
                }
            }
        }
    };
    struct kleene_star_parser_gen
    {
        template <typename S>
        struct result
        {
            typedef kleene_star<S> type;
        };
        template <typename S>
        static kleene_star<S>
        generate(parser<S> const& a)
        {
            return kleene_star<S>(a.derived());
        }
    };
    template <typename S>
    kleene_star<S>
    operator*(parser<S> const& a);
}}
namespace boost { namespace spirit {
    template <typename S>
    inline kleene_star<S>
    operator*(parser<S> const& a)
    {
        return kleene_star<S>(a.derived());
    }
}}
namespace boost { namespace spirit {
    struct positive_parser_gen;
    template <typename S>
    struct positive
    : public unary<S, parser<positive<S> > >
    {
        typedef positive<S> self_t;
        typedef unary_parser_category parser_category_t;
        typedef positive_parser_gen parser_generator_t;
        typedef unary<S, parser<self_t> > base_t;
        positive(S const& a)
        : base_t(a) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            typedef typename ScannerT::iterator_t iterator_t;
            result_t hit = this->subject().parse(scan);
            if (hit)
            {
                for (;;)
                {
                    iterator_t save = scan.first;
                    if (result_t next = this->subject().parse(scan))
                    {
                        scan.concat_match(hit, next);
                    }
                    else
                    {
                        scan.first = save;
                        break;
                    }
                }
            }
            return hit;
        }
    };
    struct positive_parser_gen
    {
        template <typename S>
        struct result
        {
            typedef positive<S> type;
        };
        template <typename S>
        static positive<S>
        generate(parser<S> const& a)
        {
            return positive<S>(a.derived());
        }
    };
    template <typename S>
    inline positive<S>
    operator+(parser<S> const& a);
}}
namespace boost { namespace spirit {
    template <typename S>
    inline positive<S>
    operator+(parser<S> const& a)
    {
        return positive<S>(a.derived());
    }
}}
namespace boost { namespace spirit {
    struct optional_parser_gen;
    template <typename S>
    struct optional
    : public unary<S, parser<optional<S> > >
    {
        typedef optional<S> self_t;
        typedef unary_parser_category parser_category_t;
        typedef optional_parser_gen parser_generator_t;
        typedef unary<S, parser<self_t> > base_t;
        optional(S const& a)
        : base_t(a) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            typedef typename ScannerT::iterator_t iterator_t;
            iterator_t save = scan.first;
            if (result_t r = this->subject().parse(scan))
            {
                return r;
            }
            else
            {
                scan.first = save;
                return scan.empty_match();
            }
        }
    };
    struct optional_parser_gen
    {
        template <typename S>
        struct result
        {
            typedef optional<S> type;
        };
        template <typename S>
        static optional<S>
        generate(parser<S> const& a)
        {
            return optional<S>(a.derived());
        }
    };
    template <typename S>
    optional<S>
    operator!(parser<S> const& a);
}}
namespace boost { namespace spirit {
    template <typename S>
    optional<S>
    operator!(parser<S> const& a)
    {
        return optional<S>(a.derived());
    }
}}
namespace boost { namespace spirit {
    template <typename A, typename B>
    sequence<A, kleene_star<sequence<B, A> > >
    operator%(parser<A> const& a, parser<B> const& b);
    template <typename A>
    sequence<A, kleene_star<sequence<chlit<char>, A> > >
    operator%(parser<A> const& a, char b);
    template <typename B>
    sequence<chlit<char>, kleene_star<sequence<B, chlit<char> > > >
    operator%(char a, parser<B> const& b);
    template <typename A>
    sequence<A, kleene_star<sequence<strlit<char const*>, A> > >
    operator%(parser<A> const& a, char const* b);
    template <typename B>
    sequence<strlit<char const*>,
        kleene_star<sequence<B, strlit<char const*> > > >
    operator%(char const* a, parser<B> const& b);
    template <typename A>
    sequence<A, kleene_star<sequence<chlit<wchar_t>, A> > >
    operator%(parser<A> const& a, wchar_t b);
    template <typename B>
    sequence<chlit<wchar_t>, kleene_star<sequence<B, chlit<wchar_t> > > >
    operator%(wchar_t a, parser<B> const& b);
    template <typename A>
    sequence<A, kleene_star<sequence<strlit<wchar_t const*>, A> > >
    operator%(parser<A> const& a, wchar_t const* b);
    template <typename B>
    sequence<strlit<wchar_t const*>,
        kleene_star<sequence<B, strlit<wchar_t const*> > > >
    operator%(wchar_t const* a, parser<B> const& b);
}}
namespace boost { namespace spirit {
    template <typename A, typename B>
    inline sequence<A, kleene_star<sequence<B, A> > >
    operator%(parser<A> const& a, parser<B> const& b)
    {
        return a.derived() >> *(b.derived() >> a.derived());
    }
    template <typename A>
    inline sequence<A, kleene_star<sequence<chlit<char>, A> > >
    operator%(parser<A> const& a, char b)
    {
        return a.derived() >> *(b >> a.derived());
    }
    template <typename B>
    inline sequence<chlit<char>, kleene_star<sequence<B, chlit<char> > > >
    operator%(char a, parser<B> const& b)
    {
        return a >> *(b.derived() >> a);
    }
    template <typename A>
    inline sequence<A, kleene_star<sequence<strlit<char const*>, A> > >
    operator%(parser<A> const& a, char const* b)
    {
        return a.derived() >> *(b >> a.derived());
    }
    template <typename B>
    inline sequence<strlit<char const*>,
        kleene_star<sequence<B, strlit<char const*> > > >
    operator%(char const* a, parser<B> const& b)
    {
        return a >> *(b.derived() >> a);
    }
    template <typename A>
    inline sequence<A, kleene_star<sequence<chlit<wchar_t>, A> > >
    operator%(parser<A> const& a, wchar_t b)
    {
        return a.derived() >> *(b >> a.derived());
    }
    template <typename B>
    inline sequence<chlit<wchar_t>, kleene_star<sequence<B, chlit<wchar_t> > > >
    operator%(wchar_t a, parser<B> const& b)
    {
        return a >> *(b.derived() >> a);
    }
    template <typename A>
    inline sequence<A, kleene_star<sequence<strlit<wchar_t const*>, A> > >
    operator%(parser<A> const& a, wchar_t const* b)
    {
        return a.derived() >> *(b >> a.derived());
    }
    template <typename B>
    inline sequence<strlit<wchar_t const*>,
        kleene_star<sequence<B, strlit<wchar_t const*> > > >
    operator%(wchar_t const* a, parser<B> const& b)
    {
        return a >> *(b.derived() >> a);
    }
}}
namespace boost { namespace spirit {
namespace impl
{
    struct parser_type_traits_base {
        static const bool is_alternative = false;
        static const bool is_sequence = false;
        static const bool is_sequential_or = false;
        static const bool is_intersection = false;
        static const bool is_difference = false;
        static const bool is_exclusive_or = false;
        static const bool is_optional = false;
        static const bool is_kleene_star = false;
        static const bool is_positive = false;
    };
    template <typename ParserT>
    struct parser_type_traits : public parser_type_traits_base {
    };
    template <typename A, typename B>
    struct parser_type_traits<alternative<A, B> >
    : public parser_type_traits_base {
        static const bool is_alternative = true;
    };
    template <typename A, typename B>
    struct parser_type_traits<sequence<A, B> >
    : public parser_type_traits_base {
        static const bool is_sequence = true;
    };
    template <typename A, typename B>
    struct parser_type_traits<sequential_or<A, B> >
    : public parser_type_traits_base {
        static const bool is_sequential_or = true;
    };
    template <typename A, typename B>
    struct parser_type_traits<intersection<A, B> >
    : public parser_type_traits_base {
        static const bool is_intersection = true;
    };
    template <typename A, typename B>
    struct parser_type_traits<difference<A, B> >
    : public parser_type_traits_base {
        static const bool is_difference = true;
    };
    template <typename A, typename B>
    struct parser_type_traits<exclusive_or<A, B> >
    : public parser_type_traits_base {
        static const bool is_exclusive_or = true;
    };
    template <typename S>
    struct parser_type_traits<optional<S> >
    : public parser_type_traits_base {
        static const bool is_optional = true;
    };
    template <typename S>
    struct parser_type_traits<kleene_star<S> >
    : public parser_type_traits_base {
        static const bool is_kleene_star = true;
    };
    template <typename S>
    struct parser_type_traits<positive<S> >
    : public parser_type_traits_base {
        static const bool is_positive = true;
    };
}
}}
namespace boost { namespace spirit {
template <typename T>
struct is_parser
{
    static const bool value = (::boost::is_base_and_derived<parser<T>, T>::value);
};
template <typename UnaryT>
struct is_unary_composite {
    static const bool value = (::boost::is_convertible< typename UnaryT::parser_category_t, unary_parser_category>::value);
};
template <typename ActionT>
struct is_action_parser {
    static const bool value = (::boost::is_convertible< typename ActionT::parser_category_t, action_parser_category>::value);
};
template <typename BinaryT>
struct is_binary_composite {
    static const bool value = (::boost::is_convertible< typename BinaryT::parser_category_t, binary_parser_category>::value);
};
template <typename CompositeT>
struct is_composite_parser {
    static const bool value = ( ::boost::spirit::is_unary_composite<CompositeT>::value || ::boost::spirit::is_binary_composite<CompositeT>::value);
};
template <typename ParserT>
struct is_alternative {
    static const bool value = ( ::boost::spirit::impl::parser_type_traits<ParserT>::is_alternative);
};
template <typename ParserT>
struct is_sequence {
    static const bool value = ( ::boost::spirit::impl::parser_type_traits<ParserT>::is_sequence);
};
template <typename ParserT>
struct is_sequential_or {
    static const bool value = ( ::boost::spirit::impl::parser_type_traits<ParserT>::is_sequential_or);
};
template <typename ParserT>
struct is_intersection {
    static const bool value = ( ::boost::spirit::impl::parser_type_traits<ParserT>::is_intersection);
};
template <typename ParserT>
struct is_difference {
    static const bool value = ( ::boost::spirit::impl::parser_type_traits<ParserT>::is_difference);
};
template <typename ParserT>
struct is_exclusive_or {
    static const bool value = ( ::boost::spirit::impl::parser_type_traits<ParserT>::is_exclusive_or);
};
template <typename ParserT>
struct is_optional {
    static const bool value = ( ::boost::spirit::impl::parser_type_traits<ParserT>::is_optional);
};
template <typename ParserT>
struct is_kleene_star {
    static const bool value = ( ::boost::spirit::impl::parser_type_traits<ParserT>::is_kleene_star);
};
template <typename ParserT>
struct is_positive {
    static const bool value = ( ::boost::spirit::impl::parser_type_traits<ParserT>::is_positive);
};
template <typename UnaryT>
struct unary_subject {
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ::boost::spirit::is_unary_composite<UnaryT>::value ) >)> boost_static_assert_typedef_183;
    typedef typename UnaryT::subject_t type;
};
template <typename UnaryT>
inline typename unary_subject<UnaryT>::type const &
get_unary_subject(UnaryT const &unary_)
{
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ::boost::spirit::is_unary_composite<UnaryT>::value ) >)> boost_static_assert_typedef_199;
    return unary_.subject();
}
template <typename BinaryT>
struct binary_left_subject {
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ::boost::spirit::is_binary_composite<BinaryT>::value ) >)> boost_static_assert_typedef_215;
    typedef typename BinaryT::left_t type;
};
template <typename BinaryT>
struct binary_right_subject {
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ::boost::spirit::is_binary_composite<BinaryT>::value ) >)> boost_static_assert_typedef_222;
    typedef typename BinaryT::right_t type;
};
template <typename BinaryT>
inline typename binary_left_subject<BinaryT>::type const &
get_binary_left_subject(BinaryT const &binary_)
{
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ::boost::spirit::is_binary_composite<BinaryT>::value ) >)> boost_static_assert_typedef_239;
    return binary_.left();
}
template <typename BinaryT>
inline typename binary_right_subject<BinaryT>::type const &
get_binary_right_subject(BinaryT const &binary_)
{
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ::boost::spirit::is_binary_composite<BinaryT>::value ) >)> boost_static_assert_typedef_247;
    return binary_.right();
}
template <typename ActionT>
struct action_subject {
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ::boost::spirit::is_action_parser<ActionT>::value ) >)> boost_static_assert_typedef_262;
    typedef typename ActionT::subject_t type;
};
template <typename ActionT>
inline typename action_subject<ActionT>::type const &
get_action_subject(ActionT const &action_)
{
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ::boost::spirit::is_action_parser<ActionT>::value ) >)> boost_static_assert_typedef_278;
    return action_.subject();
}
template <typename ActionT>
struct semantic_action {
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ::boost::spirit::is_action_parser<ActionT>::value ) >)> boost_static_assert_typedef_293;
    typedef typename ActionT::predicate_t type;
};
template <typename ActionT>
inline typename semantic_action<ActionT>::type const &
get_semantic_action(ActionT const &action_)
{
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ::boost::spirit::is_action_parser<ActionT>::value ) >)> boost_static_assert_typedef_309;
    return action_.predicate();
}
}}
namespace boost {
namespace spirit {
template<typename BaseT = action_policy>
struct no_actions_action_policy:
    public BaseT
{
    typedef BaseT base_t;
    no_actions_action_policy():
        BaseT()
    {}
    template<typename PolicyT>
    no_actions_action_policy(PolicyT const& other):
        BaseT(other)
    {}
    template<typename ActorT, typename AttrT, typename IteratorT>
    void
    do_action(
        ActorT const& actor,
        AttrT& val,
        IteratorT const& first,
        IteratorT const& last) const
    {}
};
template<typename ScannerT = scanner<> >
struct no_actions_scanner
{
    typedef scanner_policies<
        typename ScannerT::iteration_policy_t,
        typename ScannerT::match_policy_t,
        no_actions_action_policy<typename ScannerT::action_policy_t>
    > policies_t;
    typedef typename
        rebind_scanner_policies<ScannerT, policies_t>::type type;
};
struct no_actions_parser_gen;
template<typename ParserT>
struct no_actions_parser:
    public unary<ParserT, parser<no_actions_parser<ParserT> > >
{
    typedef no_actions_parser<ParserT> self_t;
    typedef unary_parser_category parser_category_t;
    typedef no_actions_parser_gen parser_generator_t;
    typedef unary<ParserT, parser<self_t> > base_t;
    template<typename ScannerT>
    struct result
    {
        typedef typename parser_result<ParserT, ScannerT>::type type;
    };
    no_actions_parser(ParserT const& p)
    : base_t(p)
    {}
    template<typename ScannerT>
    typename result<ScannerT>::type
    parse(ScannerT const& scan) const
    {
        typedef typename no_actions_scanner<ScannerT>::policies_t policies_t;
        return this->subject().parse(scan.change_policies(policies_t(scan)));
    }
};
struct no_actions_parser_gen
{
    template<typename ParserT>
    struct result
    {
        typedef no_actions_parser<ParserT> type;
    };
    template<typename ParserT>
    static no_actions_parser<ParserT>
    generate(parser<ParserT> const& subject)
    {
        return no_actions_parser<ParserT>(subject.derived());
    }
    template<typename ParserT>
    no_actions_parser<ParserT>
    operator[](parser<ParserT> const& subject) const
    {
        return no_actions_parser<ParserT>(subject.derived());
    }
};
const no_actions_parser_gen no_actions_d = no_actions_parser_gen();
}
}
namespace boost { namespace spirit {
    template <typename CondT, bool positive = true>
    struct condition_parser : parser<condition_parser<CondT, positive> >
    {
        typedef condition_parser<CondT, positive> self_t;
        condition_parser(CondT const& cond_) : cond(cond_) {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            if (positive == cond())
                return scan.empty_match();
            else
                return scan.no_match();
        }
        condition_parser<CondT, !positive>
        negate() const
        { return condition_parser<CondT, !positive>(cond); }
    private:
        CondT cond;
    };
    template <typename CondT, bool positive>
    inline condition_parser<CondT, !positive>
    operator~(condition_parser<CondT, positive> const& p)
    { return p.negate(); }
    struct empty_match_parser_gen;
    struct negated_empty_match_parser_gen;
    template <typename SubjectT>
    struct negated_empty_match_parser;
    template<typename SubjectT>
    struct empty_match_parser
    : unary<SubjectT, parser<empty_match_parser<SubjectT> > >
    {
        typedef empty_match_parser<SubjectT> self_t;
        typedef unary<SubjectT, parser<self_t> > base_t;
        typedef unary_parser_category parser_category_t;
        typedef empty_match_parser_gen parser_genererator_t;
        typedef self_t embed_t;
        explicit empty_match_parser(SubjectT const& p) : base_t(p) {}
        template <typename ScannerT>
        struct result
        { typedef typename match_result<ScannerT, nil_t>::type type; };
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typename ScannerT::iterator_t save(scan.first);
            typedef typename no_actions_scanner<ScannerT>::policies_t
                policies_t;
            bool matches = this->subject().parse(
                scan.change_policies(policies_t(scan)));
            if (matches)
            {
                scan.first = save;
                return scan.empty_match();
            }
            else
            {
                return scan.no_match();
            }
        }
        negated_empty_match_parser<SubjectT>
        negate() const
        { return negated_empty_match_parser<SubjectT>(this->subject()); }
    };
    template<typename SubjectT>
    struct negated_empty_match_parser
    : public unary<SubjectT, parser<negated_empty_match_parser<SubjectT> > >
    {
        typedef negated_empty_match_parser<SubjectT> self_t;
        typedef unary<SubjectT, parser<self_t> > base_t;
        typedef unary_parser_category parser_category_t;
        typedef negated_empty_match_parser_gen parser_genererator_t;
        explicit negated_empty_match_parser(SubjectT const& p) : base_t(p) {}
        template <typename ScannerT>
        struct result
        { typedef typename match_result<ScannerT, nil_t>::type type; };
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typename ScannerT::iterator_t save(scan.first);
            bool matches = this->subject().parse(scan);
            if (!matches)
            {
                scan.first = save;
                return scan.empty_match();
            }
            else
            {
                return scan.no_match();
            }
        }
        empty_match_parser<SubjectT>
        negate() const
        { return empty_match_parser<SubjectT>(this->subject()); }
    };
    struct empty_match_parser_gen
    {
        template <typename SubjectT>
        struct result
        { typedef empty_match_parser<SubjectT> type; };
        template <typename SubjectT>
        static empty_match_parser<SubjectT>
        generate(parser<SubjectT> const& subject)
        { return empty_match_parser<SubjectT>(subject.derived()); }
    };
    struct negated_empty_match_parser_gen
    {
        template <typename SubjectT>
        struct result
        { typedef negated_empty_match_parser<SubjectT> type; };
        template <typename SubjectT>
        static negated_empty_match_parser<SubjectT>
        generate(parser<SubjectT> const& subject)
        { return negated_empty_match_parser<SubjectT>(subject.derived()); }
    };
    template <typename SubjectT>
    inline negated_empty_match_parser<SubjectT>
    operator~(empty_match_parser<SubjectT> const& p)
    { return p.negate(); }
    template <typename SubjectT>
    inline empty_match_parser<SubjectT>
    operator~(negated_empty_match_parser<SubjectT> const& p)
    { return p.negate(); }
    namespace impl
    {
        template <typename SubjectT>
        struct epsilon_selector
        {
            typedef typename as_parser<SubjectT>::type subject_t;
            typedef typename
                mpl::if_<
                    is_parser<subject_t>
                    ,empty_match_parser<subject_t>
                    ,condition_parser<subject_t>
                >::type type;
        };
    }
    struct epsilon_parser : public parser<epsilon_parser>
    {
        typedef epsilon_parser self_t;
        epsilon_parser() {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        { return scan.empty_match(); }
        template <typename SubjectT>
        typename impl::epsilon_selector<SubjectT>::type
        operator()(SubjectT const& subject) const
        {
            typedef typename impl::epsilon_selector<SubjectT>::type result_t;
            return result_t(subject);
        }
    };
    epsilon_parser const epsilon_p = epsilon_parser();
    epsilon_parser const eps_p = epsilon_parser();
}}
#include <bitset>
namespace boost { namespace spirit { namespace utility { namespace impl {
    template <typename CharT>
    struct range {
                        range(CharT first, CharT last);
        bool is_valid() const;
        bool includes(CharT v) const;
        bool includes(range const& r) const;
        bool overlaps(range const& r) const;
        void merge(range const& r);
        CharT first;
        CharT last;
    };
    template <typename CharT>
    struct range_char_compare {
        bool operator()(range<CharT> const& x, const CharT y) const
        { return x.first < y; }
    };
    template <typename CharT>
    struct range_compare {
        bool operator()(range<CharT> const& x, range<CharT> const& y) const
        { return x.first < y.first; }
    };
    template <typename CharT>
    class range_run {
    public:
        typedef range<CharT> range_t;
        typedef std::vector<range_t> run_t;
        typedef typename run_t::iterator iterator;
        typedef typename run_t::const_iterator const_iterator;
        void swap(range_run& rr);
        bool test(CharT v) const;
        void set(range_t const& r);
        void clear(range_t const& r);
        void clear();
        const_iterator begin() const;
        const_iterator end() const;
    private:
        void merge(iterator iter, range_t const& r);
        run_t run;
    };
}}}}
namespace boost { namespace spirit {
    namespace utility { namespace impl {
        template <typename CharT>
        inline range<CharT>::range(CharT first_, CharT last_)
        : first(first_), last(last_) {}
        template <typename CharT>
        inline bool
        range<CharT>::is_valid() const
        { return first <= last; }
        template <typename CharT>
        inline bool
        range<CharT>::includes(range const& r) const
        { return (first <= r.first) && (last >= r.last); }
        template <typename CharT>
        inline bool
        range<CharT>::includes(CharT v) const
        { return (first <= v) && (last >= v); }
        template <typename CharT>
        inline bool
        range<CharT>::overlaps(range const& r) const
        {
            CharT decr_first =
                first == std::numeric_limits<CharT>::min() ? first : first-1;
            CharT incr_last =
                last == std::numeric_limits<CharT>::max() ? last : last+1;
            return (decr_first <= r.last) && (incr_last >= r.first);
        }
        template <typename CharT>
        inline void
        range<CharT>::merge(range const& r)
        {
            first = std::min(first, r.first);
            last = std::max(last, r.last);
        }
        template <typename CharT>
        inline bool
        range_run<CharT>::test(CharT v) const
        {
            if (!run.empty())
            {
                const_iterator iter =
                    std::lower_bound(
                        run.begin(), run.end(), v,
                        range_char_compare<CharT>()
                    );
                if (iter != run.end() && iter->includes(v))
                    return true;
                if (iter != run.begin())
                    return (--iter)->includes(v);
            }
            return false;
        }
        template <typename CharT>
        inline void
        range_run<CharT>::swap(range_run& rr)
        { run.swap(rr.run); }
        template <typename CharT>
        void
        range_run<CharT>::merge(iterator iter, range<CharT> const& r)
        {
            iter->merge(r);
            iterator i = iter + 1;
            while (i != run.end() && iter->overlaps(*i))
                iter->merge(*i++);
            run.erase(iter+1, i);
        }
        template <typename CharT>
        void
        range_run<CharT>::set(range<CharT> const& r)
        {
            ;
            if (!run.empty())
            {
                iterator iter =
                    std::lower_bound(
                        run.begin(), run.end(), r,
                        range_compare<CharT>()
                    );
                if (iter != run.end() && iter->includes(r) ||
                    ((iter != run.begin()) && (iter - 1)->includes(r)))
                    return;
                if (iter != run.begin() && (iter - 1)->overlaps(r))
                    merge(--iter, r);
                else if (iter != run.end() && iter->overlaps(r))
                    merge(iter, r);
                else
                    run.insert(iter, r);
            }
            else
            {
                run.push_back(r);
            }
        }
        template <typename CharT>
        void
        range_run<CharT>::clear(range<CharT> const& r)
        {
            ;
            if (!run.empty())
            {
                iterator iter =
                    std::lower_bound(
                        run.begin(), run.end(), r,
                        range_compare<CharT>()
                    );
                iterator left_iter;
                if ((iter != run.begin()) &&
                        (left_iter = (iter - 1))->includes(r.first))
                    if (left_iter->last > r.last)
                    {
                        CharT save_last = left_iter->last;
                        left_iter->last = r.first-1;
                        run.insert(iter, range<CharT>(r.last+1, save_last));
                        return;
                    }
                    else
                    {
                        left_iter->last = r.first-1;
                    }
                iterator i = iter;
                while (i != run.end() && r.includes(*i))
                    i++;
                if (i != run.end() && i->includes(r.last))
                    i->first = r.last+1;
                run.erase(iter, i);
            }
        }
        template <typename CharT>
        inline void
        range_run<CharT>::clear()
        { run.clear(); }
        template <typename CharT>
        inline typename range_run<CharT>::const_iterator
        range_run<CharT>::begin() const
        { return run.begin(); }
        template <typename CharT>
        inline typename range_run<CharT>::const_iterator
        range_run<CharT>::end() const
        { return run.end(); }
    }}
}}
namespace boost { namespace spirit {
    template <typename CharT>
    class basic_chset
    {
    public:
                            basic_chset();
                            basic_chset(basic_chset const& arg_);
        bool test(CharT v) const;
        void set(CharT from, CharT to);
        void set(CharT c);
        void clear(CharT from, CharT to);
        void clear(CharT c);
        void clear();
        void inverse();
        void swap(basic_chset& x);
        basic_chset& operator|=(basic_chset const& x);
        basic_chset& operator&=(basic_chset const& x);
        basic_chset& operator-=(basic_chset const& x);
        basic_chset& operator^=(basic_chset const& x);
        private: utility::impl::range_run<CharT> rr;
    };
    template <typename CharT>
    class basic_chset_8bit {
    public:
                            basic_chset_8bit();
                            basic_chset_8bit(basic_chset_8bit const& arg_);
        bool test(CharT v) const;
        void set(CharT from, CharT to);
        void set(CharT c);
        void clear(CharT from, CharT to);
        void clear(CharT c);
        void clear();
        void inverse();
        void swap(basic_chset_8bit& x);
        basic_chset_8bit& operator|=(basic_chset_8bit const& x);
        basic_chset_8bit& operator&=(basic_chset_8bit const& x);
        basic_chset_8bit& operator-=(basic_chset_8bit const& x);
        basic_chset_8bit& operator^=(basic_chset_8bit const& x);
        private: std::bitset<256> bset;
    };
    template <>
    class basic_chset<char>
    : public basic_chset_8bit<char> {};
    template <>
    class basic_chset<signed char>
    : public basic_chset_8bit<signed char> {};
    template <>
    class basic_chset<unsigned char>
    : public basic_chset_8bit<unsigned char> {};
}}
namespace boost { namespace spirit {
template <typename CharT>
inline basic_chset<CharT>::basic_chset() {}
template <typename CharT>
inline basic_chset<CharT>::basic_chset(basic_chset const& arg_)
: rr(arg_.rr) {}
template <typename CharT>
inline bool
basic_chset<CharT>::test(CharT v) const
{ return rr.test(v); }
template <typename CharT>
inline void
basic_chset<CharT>::set(CharT from, CharT to)
{ rr.set(utility::impl::range<CharT>(from, to)); }
template <typename CharT>
inline void
basic_chset<CharT>::set(CharT c)
{ rr.set(utility::impl::range<CharT>(c, c)); }
template <typename CharT>
inline void
basic_chset<CharT>::clear(CharT from, CharT to)
{ rr.clear(utility::impl::range<CharT>(from, to)); }
template <typename CharT>
inline void
basic_chset<CharT>::clear()
{ rr.clear(); }
template <typename CharT>
inline void
basic_chset<CharT>::inverse()
{
    basic_chset inv;
    inv.set(
        std::numeric_limits<CharT>::min(),
        std::numeric_limits<CharT>::max()
    );
    inv -= *this;
    swap(inv);
}
template <typename CharT>
inline void
basic_chset<CharT>::swap(basic_chset& x)
{ rr.swap(x.rr); }
template <typename CharT>
inline basic_chset<CharT>&
basic_chset<CharT>::operator|=(basic_chset<CharT> const& x)
{
    typedef typename utility::impl::range_run<CharT>::const_iterator const_iterator;
    for (const_iterator iter = x.rr.begin(); iter != x.rr.end(); ++iter)
        rr.set(*iter);
    return *this;
}
template <typename CharT>
inline basic_chset<CharT>&
basic_chset<CharT>::operator&=(basic_chset<CharT> const& x)
{
    basic_chset inv;
    inv.set(
        std::numeric_limits<CharT>::min(),
        std::numeric_limits<CharT>::max()
    );
    inv -= x;
    *this -= inv;
    return *this;
}
template <typename CharT>
inline basic_chset<CharT>&
basic_chset<CharT>::operator-=(basic_chset<CharT> const& x)
{
    typedef typename utility::impl::range_run<CharT>::const_iterator const_iterator;
    for (const_iterator iter = x.rr.begin(); iter != x.rr.end(); ++iter)
        rr.clear(*iter);
    return *this;
}
template <typename CharT>
inline basic_chset<CharT>&
basic_chset<CharT>::operator^=(basic_chset<CharT> const& x)
{
    basic_chset bma = x;
    bma -= *this;
    *this -= x;
    *this |= bma;
    return *this;
}
template <typename CharT>
inline basic_chset_8bit<CharT>::basic_chset_8bit() {}
template <typename CharT>
inline basic_chset_8bit<CharT>::basic_chset_8bit(basic_chset_8bit const& arg_)
: bset(arg_.bset) {}
template <typename CharT>
inline bool
basic_chset_8bit<CharT>::test(CharT v) const
{ return bset.test((unsigned char)v); }
template <typename CharT>
inline void
basic_chset_8bit<CharT>::set(CharT from, CharT to)
{
    for (int i = from; i <= to; ++i)
        bset.set((unsigned char)i);
}
template <typename CharT>
inline void
basic_chset_8bit<CharT>::set(CharT c)
{ bset.set((unsigned char)c); }
template <typename CharT>
inline void
basic_chset_8bit<CharT>::clear(CharT from, CharT to)
{
    for (int i = from; i <= to; ++i)
        bset.reset((unsigned char)i);
}
template <typename CharT>
inline void
basic_chset_8bit<CharT>::clear(CharT c)
{ bset.reset((unsigned char)c); }
template <typename CharT>
inline void
basic_chset_8bit<CharT>::clear()
{ bset.reset(); }
template <typename CharT>
inline void
basic_chset_8bit<CharT>::inverse()
{ bset.flip(); }
template <typename CharT>
inline void
basic_chset_8bit<CharT>::swap(basic_chset_8bit& x)
{ std::swap(bset, x.bset); }
template <typename CharT>
inline basic_chset_8bit<CharT>&
basic_chset_8bit<CharT>::operator|=(basic_chset_8bit const& x)
{
    bset |= x.bset;
    return *this;
}
template <typename CharT>
inline basic_chset_8bit<CharT>&
basic_chset_8bit<CharT>::operator&=(basic_chset_8bit const& x)
{
    bset &= x.bset;
    return *this;
}
template <typename CharT>
inline basic_chset_8bit<CharT>&
basic_chset_8bit<CharT>::operator-=(basic_chset_8bit const& x)
{
    bset &= ~x.bset;
    return *this;
}
template <typename CharT>
inline basic_chset_8bit<CharT>&
basic_chset_8bit<CharT>::operator^=(basic_chset_8bit const& x)
{
    bset ^= x.bset;
    return *this;
}
}}
namespace boost { namespace spirit {
namespace utility { namespace impl {
    template <typename CharT, typename CharT2>
    void construct_chset(boost::shared_ptr<basic_chset<CharT> >& ptr,
            CharT2 const* definition);
}}
template <typename CharT = char>
class chset: public char_parser<chset<CharT> > {
public:
                    chset();
                    chset(chset const& arg_);
    explicit chset(CharT arg_);
    explicit chset(anychar_parser arg_);
    explicit chset(nothing_parser arg_);
    explicit chset(chlit<CharT> const& arg_);
    explicit chset(range<CharT> const& arg_);
    explicit chset(negated_char_parser<chlit<CharT> > const& arg_);
    explicit chset(negated_char_parser<range<CharT> > const& arg_);
                    template <typename CharT2>
    explicit chset(CharT2 const* definition)
                    : ptr(new basic_chset<CharT>())
                    {
                        utility::impl::construct_chset(ptr, definition);
                    }
                    ~chset();
    chset& operator=(chset const& rhs);
    chset& operator=(CharT rhs);
    chset& operator=(anychar_parser rhs);
    chset& operator=(nothing_parser rhs);
    chset& operator=(chlit<CharT> const& rhs);
    chset& operator=(range<CharT> const& rhs);
    chset& operator=(negated_char_parser<chlit<CharT> > const& rhs);
    chset& operator=(negated_char_parser<range<CharT> > const& rhs);
    void set(range<CharT> const& arg_);
    void set(negated_char_parser<chlit<CharT> > const& arg_);
    void set(negated_char_parser<range<CharT> > const& arg_);
    void clear(range<CharT> const& arg_);
    void clear(negated_char_parser<range<CharT> > const& arg_);
    bool test(CharT ch) const;
    chset& inverse();
    void swap(chset& x);
    chset& operator|=(chset const& x);
    chset& operator&=(chset const& x);
    chset& operator-=(chset const& x);
    chset& operator^=(chset const& x);
private:
    boost::shared_ptr<basic_chset<CharT> > ptr;
};
template <typename CharT>
inline chset<CharT>
chset_p(chlit<CharT> const& arg_)
{ return chset<CharT>(arg_); }
template <typename CharT>
inline chset<CharT>
chset_p(range<CharT> const& arg_)
{ return chset<CharT>(arg_); }
template <typename CharT>
inline chset<CharT>
chset_p(negated_char_parser<chlit<CharT> > const& arg_)
{ return chset<CharT>(arg_); }
template <typename CharT>
inline chset<CharT>
chset_p(negated_char_parser<range<CharT> > const& arg_)
{ return chset<CharT>(arg_); }
inline chset<char>
chset_p(char const* init)
{ return chset<char>(init); }
inline chset<wchar_t>
chset_p(wchar_t const* init)
{ return chset<wchar_t>(init); }
inline chset<char>
chset_p(char ch)
{ return chset<char>(ch); }
inline chset<wchar_t>
chset_p(wchar_t ch)
{ return chset<wchar_t>(ch); }
inline chset<int>
chset_p(int ch)
{ return chset<int>(ch); }
inline chset<unsigned int>
chset_p(unsigned int ch)
{ return chset<unsigned int>(ch); }
inline chset<short>
chset_p(short ch)
{ return chset<short>(ch); }
inline chset<unsigned short>
chset_p(unsigned short ch)
{ return chset<unsigned short>(ch); }
inline chset<long>
chset_p(long ch)
{ return chset<long>(ch); }
inline chset<unsigned long>
chset_p(unsigned long ch)
{ return chset<unsigned long>(ch); }
inline chset<long long>
chset_p(long long ch)
{ return chset<long long>(ch); }
inline chset<unsigned long long>
chset_p(unsigned long long ch)
{ return chset<unsigned long long>(ch); }
}}
namespace boost { namespace spirit {
template <typename CharT>
inline chset<CharT>
operator|(chset<CharT> const& a, chset<CharT> const& b)
{
    return chset<CharT>(a) |= b;
}
template <typename CharT>
inline chset<CharT>
operator-(chset<CharT> const& a, chset<CharT> const& b)
{
    return chset<CharT>(a) -= b;
}
template <typename CharT>
inline chset<CharT>
operator~(chset<CharT> const& a)
{
    return chset<CharT>(a).inverse();
}
template <typename CharT>
inline chset<CharT>
operator&(chset<CharT> const& a, chset<CharT> const& b)
{
    return chset<CharT>(a) &= b;
}
template <typename CharT>
inline chset<CharT>
operator^(chset<CharT> const& a, chset<CharT> const& b)
{
    return chset<CharT>(a) ^= b;
}
template <typename CharT>
inline chset<CharT>
operator|(chset<CharT> const& a, range<CharT> const& b)
{
    chset<CharT> a_(a);
    a_.set(b);
    return a_;
}
template <typename CharT>
inline chset<CharT>
operator&(chset<CharT> const& a, range<CharT> const& b)
{
    chset<CharT> a_(a);
    if(b.first != std::numeric_limits<CharT>::min()) {
        a_.clear(range<CharT>(std::numeric_limits<CharT>::min(), b.first - 1));
    }
    if(b.last != std::numeric_limits<CharT>::max()) {
        a_.clear(range<CharT>(b.last + 1, std::numeric_limits<CharT>::max()));
    }
    return a_;
}
template <typename CharT>
inline chset<CharT>
operator-(chset<CharT> const& a, range<CharT> const& b)
{
    chset<CharT> a_(a);
    a_.clear(b);
    return a_;
}
template <typename CharT>
inline chset<CharT>
operator^(chset<CharT> const& a, range<CharT> const& b)
{
    return a ^ chset<CharT>(b);
}
template <typename CharT>
inline chset<CharT>
operator|(range<CharT> const& a, chset<CharT> const& b)
{
    chset<CharT> b_(b);
    b_.set(a);
    return b_;
}
template <typename CharT>
inline chset<CharT>
operator&(range<CharT> const& a, chset<CharT> const& b)
{
    chset<CharT> b_(b);
    if(a.first != std::numeric_limits<CharT>::min()) {
        b_.clear(range<CharT>(std::numeric_limits<CharT>::min(), a.first - 1));
    }
    if(a.last != std::numeric_limits<CharT>::max()) {
        b_.clear(range<CharT>(a.last + 1, std::numeric_limits<CharT>::max()));
    }
    return b_;
}
template <typename CharT>
inline chset<CharT>
operator-(range<CharT> const& a, chset<CharT> const& b)
{
    return chset<CharT>(a) - b;
}
template <typename CharT>
inline chset<CharT>
operator^(range<CharT> const& a, chset<CharT> const& b)
{
    return chset<CharT>(a) ^ b;
}
template <typename CharT>
inline chset<CharT>
operator|(chset<CharT> const& a, CharT b)
{
    return a | chset<CharT>(b);
}
template <typename CharT>
inline chset<CharT>
operator&(chset<CharT> const& a, CharT b)
{
    return a & chset<CharT>(b);
}
template <typename CharT>
inline chset<CharT>
operator-(chset<CharT> const& a, CharT b)
{
    return a - chset<CharT>(b);
}
template <typename CharT>
inline chset<CharT>
operator^(chset<CharT> const& a, CharT b)
{
    return a ^ chset<CharT>(b);
}
template <typename CharT>
inline chset<CharT>
operator|(CharT a, chset<CharT> const& b)
{
    return chset<CharT>(a) | b;
}
template <typename CharT>
inline chset<CharT>
operator&(CharT a, chset<CharT> const& b)
{
    return chset<CharT>(a) & b;
}
template <typename CharT>
inline chset<CharT>
operator-(CharT a, chset<CharT> const& b)
{
    return chset<CharT>(a) - b;
}
template <typename CharT>
inline chset<CharT>
operator^(CharT a, chset<CharT> const& b)
{
    return chset<CharT>(a) ^ b;
}
template <typename CharT>
inline chset<CharT>
operator|(chset<CharT> const& a, chlit<CharT> const& b)
{
    return a | chset<CharT>(b.ch);
}
template <typename CharT>
inline chset<CharT>
operator&(chset<CharT> const& a, chlit<CharT> const& b)
{
    return a & chset<CharT>(b.ch);
}
template <typename CharT>
inline chset<CharT>
operator-(chset<CharT> const& a, chlit<CharT> const& b)
{
    return a - chset<CharT>(b.ch);
}
template <typename CharT>
inline chset<CharT>
operator^(chset<CharT> const& a, chlit<CharT> const& b)
{
    return a ^ chset<CharT>(b.ch);
}
template <typename CharT>
inline chset<CharT>
operator|(chlit<CharT> const& a, chset<CharT> const& b)
{
    return chset<CharT>(a.ch) | b;
}
template <typename CharT>
inline chset<CharT>
operator&(chlit<CharT> const& a, chset<CharT> const& b)
{
    return chset<CharT>(a.ch) & b;
}
template <typename CharT>
inline chset<CharT>
operator-(chlit<CharT> const& a, chset<CharT> const& b)
{
    return chset<CharT>(a.ch) - b;
}
template <typename CharT>
inline chset<CharT>
operator^(chlit<CharT> const& a, chset<CharT> const& b)
{
    return chset<CharT>(a.ch) ^ b;
}
template <typename CharT>
inline chset<CharT>
operator|(chset<CharT> const& a, negated_char_parser<range<CharT> > const& b)
{
    return a | chset<CharT>(b);
}
template <typename CharT>
inline chset<CharT>
operator&(chset<CharT> const& a, negated_char_parser<range<CharT> > const& b)
{
    return a & chset<CharT>(b);
}
template <typename CharT>
inline chset<CharT>
operator-(chset<CharT> const& a, negated_char_parser<range<CharT> > const& b)
{
    return a - chset<CharT>(b);
}
template <typename CharT>
inline chset<CharT>
operator^(chset<CharT> const& a, negated_char_parser<range<CharT> > const& b)
{
    return a ^ chset<CharT>(b);
}
template <typename CharT>
inline chset<CharT>
operator|(negated_char_parser<range<CharT> > const& a, chset<CharT> const& b)
{
    return chset<CharT>(a) | b;
}
template <typename CharT>
inline chset<CharT>
operator&(negated_char_parser<range<CharT> > const& a, chset<CharT> const& b)
{
    return chset<CharT>(a) & b;
}
template <typename CharT>
inline chset<CharT>
operator-(negated_char_parser<range<CharT> > const& a, chset<CharT> const& b)
{
    return chset<CharT>(a) - b;
}
template <typename CharT>
inline chset<CharT>
operator^(negated_char_parser<range<CharT> > const& a, chset<CharT> const& b)
{
    return chset<CharT>(a) ^ b;
}
template <typename CharT>
inline chset<CharT>
operator|(chset<CharT> const& a, negated_char_parser<chlit<CharT> > const& b)
{
    return a | chset<CharT>(b);
}
template <typename CharT>
inline chset<CharT>
operator&(chset<CharT> const& a, negated_char_parser<chlit<CharT> > const& b)
{
    return a & chset<CharT>(b);
}
template <typename CharT>
inline chset<CharT>
operator-(chset<CharT> const& a, negated_char_parser<chlit<CharT> > const& b)
{
    return a - chset<CharT>(b);
}
template <typename CharT>
inline chset<CharT>
operator^(chset<CharT> const& a, negated_char_parser<chlit<CharT> > const& b)
{
    return a ^ chset<CharT>(b);
}
template <typename CharT>
inline chset<CharT>
operator|(negated_char_parser<chlit<CharT> > const& a, chset<CharT> const& b)
{
    return chset<CharT>(a) | b;
}
template <typename CharT>
inline chset<CharT>
operator&(negated_char_parser<chlit<CharT> > const& a, chset<CharT> const& b)
{
    return chset<CharT>(a) & b;
}
template <typename CharT>
inline chset<CharT>
operator-(negated_char_parser<chlit<CharT> > const& a, chset<CharT> const& b)
{
    return chset<CharT>(a) - b;
}
template <typename CharT>
inline chset<CharT>
operator^(negated_char_parser<chlit<CharT> > const& a, chset<CharT> const& b)
{
    return chset<CharT>(a) ^ b;
}
namespace impl {
    template <typename CharT>
    inline boost::spirit::range<CharT> const&
    full()
    {
        static boost::spirit::range<CharT> full_(
            std::numeric_limits<CharT>::min(),
            std::numeric_limits<CharT>::max());
        return full_;
    }
    template <typename CharT>
    inline boost::spirit::range<CharT> const&
    empty()
    {
        static boost::spirit::range<CharT> empty_;
        return empty_;
    }
}
template <typename CharT>
inline chset<CharT>
operator|(chset<CharT> const&, anychar_parser)
{
    return chset<CharT>(impl::full<CharT>());
}
template <typename CharT>
inline chset<CharT>
operator&(chset<CharT> const& a, anychar_parser)
{
    return a;
}
template <typename CharT>
inline chset<CharT>
operator-(chset<CharT> const&, anychar_parser)
{
    return chset<CharT>();
}
template <typename CharT>
inline chset<CharT>
operator^(chset<CharT> const& a, anychar_parser)
{
    return ~a;
}
template <typename CharT>
inline chset<CharT>
operator|(anychar_parser, chset<CharT> const& )
{
    return chset<CharT>(impl::full<CharT>());
}
template <typename CharT>
inline chset<CharT>
operator&(anychar_parser, chset<CharT> const& b)
{
    return b;
}
template <typename CharT>
inline chset<CharT>
operator-(anychar_parser, chset<CharT> const& b)
{
    return ~b;
}
template <typename CharT>
inline chset<CharT>
operator^(anychar_parser, chset<CharT> const& b)
{
    return ~b;
}
template <typename CharT>
inline chset<CharT>
operator|(chset<CharT> const& a, nothing_parser)
{
    return a;
}
template <typename CharT>
inline chset<CharT>
operator&(chset<CharT> const& , nothing_parser)
{
    return impl::empty<CharT>();
}
template <typename CharT>
inline chset<CharT>
operator-(chset<CharT> const& a, nothing_parser)
{
    return a;
}
template <typename CharT>
inline chset<CharT>
operator^(chset<CharT> const& a, nothing_parser)
{
    return a;
}
template <typename CharT>
inline chset<CharT>
operator|(nothing_parser, chset<CharT> const& b)
{
    return b;
}
template <typename CharT>
inline chset<CharT>
operator&(nothing_parser, chset<CharT> const& )
{
    return impl::empty<CharT>();
}
template <typename CharT>
inline chset<CharT>
operator-(nothing_parser, chset<CharT> const& )
{
    return impl::empty<CharT>();
}
template <typename CharT>
inline chset<CharT>
operator^(nothing_parser, chset<CharT> const& b)
{
    return b;
}
}}
namespace boost { namespace spirit {
template <typename CharT>
chset<CharT>
operator~(chset<CharT> const& a);
template <typename CharT>
chset<CharT>
operator|(chset<CharT> const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator&(chset<CharT> const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator-(chset<CharT> const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator^(chset<CharT> const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator|(chset<CharT> const& a, range<CharT> const& b);
template <typename CharT>
chset<CharT>
operator&(chset<CharT> const& a, range<CharT> const& b);
template <typename CharT>
chset<CharT>
operator-(chset<CharT> const& a, range<CharT> const& b);
template <typename CharT>
chset<CharT>
operator^(chset<CharT> const& a, range<CharT> const& b);
template <typename CharT>
chset<CharT>
operator|(range<CharT> const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator&(range<CharT> const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator-(range<CharT> const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator^(range<CharT> const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator|(chset<CharT> const& a, chlit<CharT> const& b);
template <typename CharT>
chset<CharT>
operator&(chset<CharT> const& a, chlit<CharT> const& b);
template <typename CharT>
chset<CharT>
operator-(chset<CharT> const& a, chlit<CharT> const& b);
template <typename CharT>
chset<CharT>
operator^(chset<CharT> const& a, chlit<CharT> const& b);
template <typename CharT>
chset<CharT>
operator|(chlit<CharT> const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator&(chlit<CharT> const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator-(chlit<CharT> const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator^(chlit<CharT> const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator|(chset<CharT> const& a, negated_char_parser<range<CharT> > const& b);
template <typename CharT>
chset<CharT>
operator&(chset<CharT> const& a, negated_char_parser<range<CharT> > const& b);
template <typename CharT>
chset<CharT>
operator-(chset<CharT> const& a, negated_char_parser<range<CharT> > const& b);
template <typename CharT>
chset<CharT>
operator^(chset<CharT> const& a, negated_char_parser<range<CharT> > const& b);
template <typename CharT>
chset<CharT>
operator|(negated_char_parser<range<CharT> > const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator&(negated_char_parser<range<CharT> > const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator-(negated_char_parser<range<CharT> > const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator^(negated_char_parser<range<CharT> > const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator|(chset<CharT> const& a, negated_char_parser<chlit<CharT> > const& b);
template <typename CharT>
chset<CharT>
operator&(chset<CharT> const& a, negated_char_parser<chlit<CharT> > const& b);
template <typename CharT>
chset<CharT>
operator-(chset<CharT> const& a, negated_char_parser<chlit<CharT> > const& b);
template <typename CharT>
chset<CharT>
operator^(chset<CharT> const& a, negated_char_parser<chlit<CharT> > const& b);
template <typename CharT>
chset<CharT>
operator|(negated_char_parser<chlit<CharT> > const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator&(negated_char_parser<chlit<CharT> > const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator-(negated_char_parser<chlit<CharT> > const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator^(negated_char_parser<chlit<CharT> > const& a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator|(chset<CharT> const& a, CharT b);
template <typename CharT>
chset<CharT>
operator&(chset<CharT> const& a, CharT b);
template <typename CharT>
chset<CharT>
operator-(chset<CharT> const& a, CharT b);
template <typename CharT>
chset<CharT>
operator^(chset<CharT> const& a, CharT b);
template <typename CharT>
chset<CharT>
operator|(CharT a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator&(CharT a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator-(CharT a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator^(CharT a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator|(chset<CharT> const& a, anychar_parser b);
template <typename CharT>
chset<CharT>
operator&(chset<CharT> const& a, anychar_parser b);
template <typename CharT>
chset<CharT>
operator-(chset<CharT> const& a, anychar_parser b);
template <typename CharT>
chset<CharT>
operator^(chset<CharT> const& a, anychar_parser b);
template <typename CharT>
chset<CharT>
operator|(anychar_parser a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator&(anychar_parser a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator-(anychar_parser a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator^(anychar_parser a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator|(chset<CharT> const& a, nothing_parser b);
template <typename CharT>
chset<CharT>
operator&(chset<CharT> const& a, nothing_parser b);
template <typename CharT>
chset<CharT>
operator-(chset<CharT> const& a, nothing_parser b);
template <typename CharT>
chset<CharT>
operator^(chset<CharT> const& a, nothing_parser b);
template <typename CharT>
chset<CharT>
operator|(nothing_parser a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator&(nothing_parser a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator-(nothing_parser a, chset<CharT> const& b);
template <typename CharT>
chset<CharT>
operator^(nothing_parser a, chset<CharT> const& b);
}}
namespace boost { namespace spirit {
namespace utility { namespace impl {
    template <typename CharT>
    inline void
    detach(boost::shared_ptr<basic_chset<CharT> >& ptr)
    {
        if (!ptr.unique())
            ptr = boost::shared_ptr<basic_chset<CharT> >
                (new basic_chset<CharT>(*ptr));
    }
    template <typename CharT>
    inline void
    detach_clear(boost::shared_ptr<basic_chset<CharT> >& ptr)
    {
        if (ptr.unique())
            ptr->clear();
        else
            ptr.reset(new basic_chset<CharT>());
    }
    template <typename CharT, typename CharT2>
    void construct_chset(boost::shared_ptr<basic_chset<CharT> >& ptr,
            CharT2 const* definition)
    {
        CharT2 ch = *definition++;
        while (ch)
        {
            CharT2 next = *definition++;
            if (next == '-')
            {
                next = *definition++;
                if (next == 0)
                {
                    ptr->set(ch);
                    ptr->set('-');
                    break;
                }
                ptr->set(ch, next);
            }
            else
            {
                ptr->set(ch);
            }
            ch = next;
        }
    }
}}
template <typename CharT>
inline chset<CharT>::chset()
: ptr(new basic_chset<CharT>()) {}
template <typename CharT>
inline chset<CharT>::chset(chset const& arg_)
: ptr(new basic_chset<CharT>(*arg_.ptr)) {}
template <typename CharT>
inline chset<CharT>::chset(CharT arg_)
: ptr(new basic_chset<CharT>())
{ ptr->set(arg_); }
template <typename CharT>
inline chset<CharT>::chset(anychar_parser arg_)
: ptr(new basic_chset<CharT>())
{
    ptr->set(
        std::numeric_limits<CharT>::min(),
        std::numeric_limits<CharT>::max()
    );
}
template <typename CharT>
inline chset<CharT>::chset(nothing_parser arg_)
: ptr(new basic_chset<CharT>()) {}
template <typename CharT>
inline chset<CharT>::chset(chlit<CharT> const& arg_)
: ptr(new basic_chset<CharT>())
{ ptr->set(arg_.ch); }
template <typename CharT>
inline chset<CharT>::chset(range<CharT> const& arg_)
: ptr(new basic_chset<CharT>())
{ ptr->set(arg_.first, arg_.last); }
template <typename CharT>
inline chset<CharT>::chset(negated_char_parser<chlit<CharT> > const& arg_)
: ptr(new basic_chset<CharT>())
{
    set(arg_);
}
template <typename CharT>
inline chset<CharT>::chset(negated_char_parser<range<CharT> > const& arg_)
: ptr(new basic_chset<CharT>())
{
    set(arg_);
}
template <typename CharT>
inline chset<CharT>::~chset() {}
template <typename CharT>
inline chset<CharT>&
chset<CharT>::operator=(chset const& rhs)
{
    ptr = rhs.ptr;
    return *this;
}
template <typename CharT>
inline chset<CharT>&
chset<CharT>::operator=(CharT rhs)
{
    utility::impl::detach_clear(ptr);
    ptr->set(rhs);
    return *this;
}
template <typename CharT>
inline chset<CharT>&
chset<CharT>::operator=(anychar_parser rhs)
{
    utility::impl::detach_clear(ptr);
    ptr->set(
        std::numeric_limits<CharT>::min(),
        std::numeric_limits<CharT>::max()
    );
    return *this;
}
template <typename CharT>
inline chset<CharT>&
chset<CharT>::operator=(nothing_parser rhs)
{
    utility::impl::detach_clear(ptr);
    return *this;
}
template <typename CharT>
inline chset<CharT>&
chset<CharT>::operator=(chlit<CharT> const& rhs)
{
    utility::impl::detach_clear(ptr);
    ptr->set(rhs.ch);
    return *this;
}
template <typename CharT>
inline chset<CharT>&
chset<CharT>::operator=(range<CharT> const& rhs)
{
    utility::impl::detach_clear(ptr);
    ptr->set(rhs.first, rhs.last);
    return *this;
}
template <typename CharT>
inline chset<CharT>&
chset<CharT>::operator=(negated_char_parser<chlit<CharT> > const& rhs)
{
    utility::impl::detach_clear(ptr);
    set(rhs);
    return *this;
}
template <typename CharT>
inline chset<CharT>&
chset<CharT>::operator=(negated_char_parser<range<CharT> > const& rhs)
{
    utility::impl::detach_clear(ptr);
    set(rhs);
    return *this;
}
template <typename CharT>
inline void
chset<CharT>::set(range<CharT> const& arg_)
{
    utility::impl::detach(ptr);
    ptr->set(arg_.first, arg_.last);
}
template <typename CharT>
inline void
chset<CharT>::set(negated_char_parser<chlit<CharT> > const& arg_)
{
    utility::impl::detach(ptr);
    if(arg_.positive.ch != std::numeric_limits<CharT>::min()) {
        ptr->set(std::numeric_limits<CharT>::min(), arg_.positive.ch - 1);
    }
    if(arg_.positive.ch != std::numeric_limits<CharT>::max()) {
        ptr->set(arg_.positive.ch + 1, std::numeric_limits<CharT>::max());
    }
}
template <typename CharT>
inline void
chset<CharT>::set(negated_char_parser<range<CharT> > const& arg_)
{
    utility::impl::detach(ptr);
    if(arg_.positive.first != std::numeric_limits<CharT>::min()) {
        ptr->set(std::numeric_limits<CharT>::min(), arg_.positive.first - 1);
    }
    if(arg_.positive.last != std::numeric_limits<CharT>::max()) {
        ptr->set(arg_.positive.last + 1, std::numeric_limits<CharT>::max());
    }
}
template <typename CharT>
inline void
chset<CharT>::clear(range<CharT> const& arg_)
{
    utility::impl::detach(ptr);
    ptr->clear(arg_.first, arg_.last);
}
template <typename CharT>
inline void
chset<CharT>::clear(negated_char_parser<range<CharT> > const& arg_)
{
    utility::impl::detach(ptr);
    if(arg_.positive.first != std::numeric_limits<CharT>::min()) {
        ptr->clear(std::numeric_limits<CharT>::min(), arg_.positive.first - 1);
    }
    if(arg_.positive.last != std::numeric_limits<CharT>::max()) {
        ptr->clear(arg_.positive.last + 1, std::numeric_limits<CharT>::max());
    }
}
template <typename CharT>
inline bool
chset<CharT>::test(CharT ch) const
{ return ptr->test(ch); }
template <typename CharT>
inline chset<CharT>&
chset<CharT>::inverse()
{
    utility::impl::detach(ptr);
    ptr->inverse();
    return *this;
}
template <typename CharT>
inline void
chset<CharT>::swap(chset& x)
{ ptr.swap(x.ptr); }
template <typename CharT>
inline chset<CharT>&
chset<CharT>::operator|=(chset const& x)
{
    utility::impl::detach(ptr);
    *ptr |= *x.ptr;
    return *this;
}
template <typename CharT>
inline chset<CharT>&
chset<CharT>::operator&=(chset const& x)
{
    utility::impl::detach(ptr);
    *ptr &= *x.ptr;
    return *this;
}
template <typename CharT>
inline chset<CharT>&
chset<CharT>::operator-=(chset const& x)
{
    utility::impl::detach(ptr);
    *ptr -= *x.ptr;
    return *this;
}
template <typename CharT>
inline chset<CharT>&
chset<CharT>::operator^=(chset const& x)
{
    utility::impl::detach(ptr);
    *ptr ^= *x.ptr;
    return *this;
}
}}
namespace boost { namespace spirit {
const unsigned long c_escapes = 1;
const unsigned long lex_escapes = c_escapes << 1;
namespace impl {
    template <unsigned long Flags, typename CharT>
    struct escape_char_action_parse {
        template <typename ParserT, typename ScannerT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ScannerT const& scan, ParserT const &p)
        {
            typedef CharT char_t;
            typedef typename ScannerT::iterator_t iterator_t;
            typedef typename parser_result<ParserT, ScannerT>::type result_t;
            if (scan.first != scan.last) {
                iterator_t save = scan.first;
                if (result_t hit = p.subject().parse(scan)) {
                    char_t unescaped;
                    scan.first = save;
                    if (*scan.first == '\\') {
                        ++scan.first;
                        switch (*scan.first) {
                        case 'b': unescaped = '\b'; ++scan.first; break;
                        case 't': unescaped = '\t'; ++scan.first; break;
                        case 'n': unescaped = '\n'; ++scan.first; break;
                        case 'f': unescaped = '\f'; ++scan.first; break;
                        case 'r': unescaped = '\r'; ++scan.first; break;
                        case '"': unescaped = '"'; ++scan.first; break;
                        case '\'': unescaped = '\''; ++scan.first; break;
                        case '\\': unescaped = '\\'; ++scan.first; break;
                        case 'x': case 'X':
                            {
                                char_t hex = 0;
                                char_t const lim =
                                    std::numeric_limits<char_t>::max() >> 4;
                                ++scan.first;
                                while (scan.first != scan.last)
                                {
                                    char_t c = *scan.first;
                                    if (hex > lim && impl::isxdigit_(c))
                                    {
                                        scan.first = save;
                                        return scan.no_match();
                                    }
                                    if (impl::isdigit_(c))
                                    {
                                        hex <<= 4;
                                        hex |= c - '0';
                                        ++scan.first;
                                    }
                                    else if (impl::isxdigit_(c))
                                    {
                                        hex <<= 4;
                                        c = impl::toupper_(c);
                                        hex |= c - 'A' + 0xA;
                                        ++scan.first;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                                unescaped = hex;
                            }
                            break;
                        case '0': case '1': case '2': case '3':
                        case '4': case '5': case '6': case '7':
                            {
                                char_t oct = 0;
                                char_t const lim =
                                    std::numeric_limits<char_t>::max() >> 3;
                                while (scan.first != scan.last)
                                {
                                    char_t c = *scan.first;
                                    if (oct > lim && (c >= '0' && c <= '7'))
                                    {
                                        scan.first = save;
                                        return scan.no_match();
                                    }
                                    if (c >= '0' && c <= '7')
                                    {
                                        oct <<= 3;
                                        oct |= c - '0';
                                        ++scan.first;
                                    }
                                    else
                                    {
                                        break;
                                    }
                                }
                                unescaped = oct;
                            }
                            break;
                        default:
                            if (Flags & c_escapes)
                            {
                                scan.first = save;
                                return scan.no_match();
                            }
                            else
                            {
                                unescaped = *scan.first;
                                ++scan.first;
                            }
                            break;
                        }
                    }
                    else {
                        unescaped = *scan.first;
                        ++scan.first;
                    }
                    scan.do_action(p.predicate(), unescaped, save, scan.first);
                    return hit;
                }
            }
            return scan.no_match();
        }
    };
    template <typename CharT>
    struct escape_char_parse {
        template <typename ScannerT, typename ParserT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ScannerT const &scan, ParserT const & )
        {
            typedef
                uint_parser<CharT, 8, 1,
                    std::numeric_limits<CharT>::digits / 3 + 1
                >
                oct_parser_t;
            typedef
                uint_parser<CharT, 16, 1,
                    std::numeric_limits<CharT>::digits / 4 + 1
                >
                hex_parser_t;
            typedef alternative<difference<anychar_parser, chlit<CharT> >,
                sequence<chlit<CharT>, alternative<alternative<oct_parser_t,
                sequence<inhibit_case<chlit<CharT> >, hex_parser_t > >,
                difference<difference<anychar_parser,
                inhibit_case<chlit<CharT> > >, oct_parser_t > > > >
                parser_t;
            static parser_t p =
                ( (anychar_p - CharT('\\'))
                | (CharT('\\') >>
                    ( oct_parser_t()
                     | as_lower_d[CharT('x')] >> hex_parser_t()
                     | (anychar_p - as_lower_d[CharT('x')] - oct_parser_t())
                    )
                ));
            ;
            return p.parse(scan);
        }
    };
}
}}
namespace boost { namespace spirit {
template <
    typename ParserT, typename ActionT,
    unsigned long Flags, typename CharT
>
struct escape_char_action
: public unary<ParserT,
        parser<escape_char_action<ParserT, ActionT, Flags, CharT> > >
{
    typedef escape_char_action
        <ParserT, ActionT, Flags, CharT> self_t;
    typedef action_parser_category parser_category_t;
    typedef unary<ParserT, parser<self_t> > base_t;
    template <typename ScannerT>
    struct result
    {
        typedef typename match_result<ScannerT, CharT>::type type;
    };
    escape_char_action(ParserT const& p, ActionT const& a)
    : base_t(p), actor(a) {}
    template <typename ScannerT>
    typename parser_result<self_t, ScannerT>::type
    parse(ScannerT const& scan) const
    {
        return impl::escape_char_action_parse<Flags, CharT>::
            parse(scan, *this);
    }
    ActionT const& predicate() const { return actor; }
private:
    ActionT actor;
};
template <unsigned long Flags, typename CharT>
struct escape_char_action_parser_gen;
template <unsigned long Flags, typename CharT = char>
struct escape_char_parser :
    public parser<escape_char_parser<Flags, CharT> > {
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( Flags == c_escapes || Flags == lex_escapes ) >)> boost_static_assert_typedef_118;
    typedef escape_char_parser<Flags, CharT> self_t;
    typedef
        escape_char_action_parser_gen<Flags, CharT>
        action_parser_generator_t;
    template <typename ScannerT>
    struct result {
        typedef typename match_result<ScannerT, CharT>::type type;
    };
    template <typename ActionT>
    escape_char_action<self_t, ActionT, Flags, CharT>
    operator[](ActionT const& actor) const
    {
        return escape_char_action<self_t, ActionT, Flags, CharT>(*this, actor);
    }
    template <typename ScannerT>
    typename parser_result<self_t, ScannerT>::type
    parse(ScannerT const &scan) const
    {
        return impl::escape_char_parse<CharT>::parse(scan, *this);
    }
};
template <unsigned long Flags, typename CharT>
struct escape_char_action_parser_gen {
    template <typename ParserT, typename ActionT>
    static escape_char_action<ParserT, ActionT, Flags, CharT>
    generate (ParserT const &p, ActionT const &actor)
    {
        typedef
            escape_char_action<ParserT, ActionT, Flags, CharT>
            action_parser_t;
        return action_parser_t(p, actor);
    }
};
const escape_char_parser<lex_escapes> lex_escape_ch_p =
    escape_char_parser<lex_escapes>();
const escape_char_parser<c_escapes> c_escape_ch_p =
    escape_char_parser<c_escapes>();
}}
namespace boost { namespace spirit {
    template < class FunctorT >
    struct functor_parser : public parser<functor_parser<FunctorT> >
    {
        FunctorT functor;
        functor_parser(): functor() {}
        functor_parser(FunctorT const& functor_): functor(functor_) {}
        typedef typename FunctorT::result_t functor_result_t;
        typedef functor_parser<FunctorT> self_t;
        template <typename ScannerT>
        struct result
        {
            typedef typename match_result<ScannerT, functor_result_t>::type
            type;
        };
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            typedef typename parser_result<self_t, ScannerT>::type result_t;
            typedef typename ScannerT::value_t value_t;
            typedef typename ScannerT::iterator_t iterator_t;
            iterator_t const s(scan.first);
            functor_result_t result;
            std::ptrdiff_t len = functor(scan, result);
            if (len < 0)
                return scan.no_match();
            else
                return scan.create_match(std::size_t(len), result, s, scan.first);
        }
    };
}}
namespace boost { namespace spirit {
    template <typename ParserT, typename ExactT>
    class fixed_loop
    : public unary<ParserT, parser <fixed_loop <ParserT, ExactT> > >
    {
    public:
        typedef fixed_loop<ParserT, ExactT> self_t;
        typedef unary<ParserT, parser<self_t> > base_t;
        fixed_loop (ParserT const & subject, ExactT const & exact)
        : base_t(subject), m_exact(exact) {}
        template <typename ScannerT>
        typename parser_result <self_t, ScannerT>::type
        parse (ScannerT const & scan) const
        {
            typename parser_result<self_t, ScannerT>::type hit(0);
            std::size_t n = m_exact;
            for (std::size_t i = 0; i < n; ++i)
            {
                typename parser_result<self_t, ScannerT>::type next
                    = this->subject().parse(scan);
                if (!next)
                    return scan.no_match();
                hit.concat(next);
            }
            return hit;
        }
        template <typename ScannerT>
        struct result
        {
            typedef typename match_result<ScannerT, nil_t>::type type;
        };
    private:
        ExactT m_exact;
    };
    template <typename ParserT, typename MinT, typename MaxT>
    class finite_loop
    : public unary<ParserT, parser<finite_loop<ParserT, MinT, MaxT> > >
    {
    public:
        typedef finite_loop <ParserT, MinT, MaxT> self_t;
        typedef unary<ParserT, parser<self_t> > base_t;
        finite_loop (ParserT const & subject, MinT const & min, MaxT const & max)
        : base_t(subject), m_min(min), m_max(max) {}
        template <typename ScannerT>
        typename parser_result <self_t, ScannerT>::type
        parse(ScannerT const & scan) const
        {
            ;
            typename parser_result<self_t, ScannerT>::type hit(0);
            std::size_t n1 = m_min;
            std::size_t n2 = m_max;
            for (std::size_t i = 0; i < n2; ++i)
            {
                typename ScannerT::iterator_t save = scan.first;
                typename parser_result<self_t, ScannerT>::type next
                    = this->subject().parse(scan);
                if (!next)
                {
                    if (i >= n1)
                    {
                        scan.first = save;
                        break;
                    }
                    else
                    {
                        return scan.no_match();
                    }
                }
                hit.concat(next);
            }
            return hit;
        }
        template <typename ScannerT>
        struct result
        {
            typedef typename match_result<ScannerT, nil_t>::type type;
        };
    private:
        MinT m_min;
        MaxT m_max;
    };
    struct more_t {};
    more_t const more = more_t ();
    template <typename ParserT, typename MinT>
    class infinite_loop
     : public unary<ParserT, parser<infinite_loop<ParserT, MinT> > >
    {
    public:
        typedef infinite_loop <ParserT, MinT> self_t;
        typedef unary<ParserT, parser<self_t> > base_t;
        infinite_loop (
            ParserT const& subject,
            MinT const& min,
            more_t const&
        )
        : base_t(subject), m_min(min) {}
        template <typename ScannerT>
        typename parser_result <self_t, ScannerT>::type
        parse(ScannerT const & scan) const
        {
            typename parser_result<self_t, ScannerT>::type hit(0);
            std::size_t n = m_min;
            for (std::size_t i = 0; ; ++i)
            {
                typename ScannerT::iterator_t save = scan.first;
                typename parser_result<self_t, ScannerT>::type next
                    = this->subject().parse(scan);
                if (!next)
                {
                    if (i >= n)
                    {
                        scan.first = save;
                        break;
                    }
                    else
                    {
                        return scan.no_match();
                    }
                }
                hit.concat(next);
            }
            return hit;
        }
        template <typename ScannerT>
        struct result
        {
            typedef typename match_result<ScannerT, nil_t>::type type;
        };
        private:
        MinT m_min;
    };
    template <typename ExactT>
    struct fixed_loop_gen
    {
        fixed_loop_gen (ExactT const & exact)
        : m_exact (exact) {}
        template <typename ParserT>
        fixed_loop <ParserT, ExactT>
        operator[](parser <ParserT> const & subject) const
        {
            return fixed_loop <ParserT, ExactT> (subject.derived (), m_exact);
        }
        ExactT m_exact;
    };
    namespace impl {
        template <typename ParserT, typename MinT, typename MaxT>
        struct loop_traits
        {
            typedef typename mpl::if_<
                boost::is_same<MaxT, more_t>,
                infinite_loop<ParserT, MinT>,
                finite_loop<ParserT, MinT, MaxT>
            >::type type;
        };
    }
    template <typename MinT, typename MaxT>
    struct nonfixed_loop_gen
    {
       nonfixed_loop_gen (MinT min, MaxT max)
        : m_min (min), m_max (max) {}
       template <typename ParserT>
       typename impl::loop_traits<ParserT, MinT, MaxT>::type
       operator[](parser <ParserT> const & subject) const
       {
           typedef typename impl::loop_traits<ParserT, MinT, MaxT>::type ret_t;
           return ret_t(
                subject.derived(),
                m_min,
                m_max);
       }
       MinT m_min;
       MaxT m_max;
    };
    template <typename ExactT>
    fixed_loop_gen <ExactT>
    repeat_p(ExactT const & exact)
    {
        return fixed_loop_gen <ExactT> (exact);
    }
    template <typename MinT, typename MaxT>
    nonfixed_loop_gen <MinT, MaxT>
    repeat_p(MinT const & min, MaxT const & max)
    {
        return nonfixed_loop_gen <MinT, MaxT> (min, max);
    }
}}
namespace boost { namespace spirit {
struct non_nested_refactoring { typedef non_nested_refactoring embed_t; };
struct self_nested_refactoring { typedef self_nested_refactoring embed_t; };
namespace impl {
    template <typename CategoryT>
    struct refactor_unary_nested {
        template <
            typename ParserT, typename NestedT,
            typename ScannerT, typename BinaryT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &, ScannerT const& scan, BinaryT const& binary,
            NestedT const& )
        {
            return binary.parse(scan);
        }
    };
    template <>
    struct refactor_unary_nested<unary_parser_category> {
        template <
            typename ParserT, typename ScannerT, typename BinaryT,
            typename NestedT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &, ScannerT const& scan, BinaryT const& binary,
            NestedT const& nested_d)
        {
            typedef typename BinaryT::parser_generator_t op_t;
            typedef
                typename BinaryT::left_t::parser_generator_t
                unary_t;
            return
                unary_t::generate(
                    nested_d[
                        op_t::generate(binary.left().subject(), binary.right())
                    ]
                ).parse(scan);
        }
    };
    template <typename CategoryT>
    struct refactor_unary_non_nested {
        template <typename ParserT, typename ScannerT, typename BinaryT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &, ScannerT const& scan, BinaryT const& binary)
        {
            return binary.parse(scan);
        }
    };
    template <>
    struct refactor_unary_non_nested<unary_parser_category> {
        template <typename ParserT, typename ScannerT, typename BinaryT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &, ScannerT const& scan, BinaryT const& binary)
        {
            typedef typename BinaryT::parser_generator_t op_t;
            typedef
                typename BinaryT::left_t::parser_generator_t
                unary_t;
            return unary_t::generate(
                op_t::generate(binary.left().subject(), binary.right())
            ).parse(scan);
        }
    };
    template <typename NestedT>
    struct refactor_unary_type {
        template <typename ParserT, typename ScannerT, typename BinaryT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &p, ScannerT const& scan, BinaryT const& binary,
            NestedT const& nested_d)
        {
            typedef
                typename BinaryT::left_t::parser_category_t
                parser_category_t;
            return refactor_unary_nested<parser_category_t>::
                    parse(p, scan, binary, nested_d);
        }
    };
    template <>
    struct refactor_unary_type<non_nested_refactoring> {
        template <typename ParserT, typename ScannerT, typename BinaryT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &p, ScannerT const& scan, BinaryT const& binary,
            non_nested_refactoring const&)
        {
            typedef
                typename BinaryT::left_t::parser_category_t
                parser_category_t;
            return refactor_unary_non_nested<parser_category_t>::
                    parse(p, scan, binary);
        }
    };
    template <>
    struct refactor_unary_type<self_nested_refactoring> {
        template <typename ParserT, typename ScannerT, typename BinaryT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &p, ScannerT const& scan, BinaryT const& binary,
            self_nested_refactoring const &nested_tag)
        {
            typedef
                typename BinaryT::left_t::parser_category_t
                parser_category_t;
            typedef typename ParserT::parser_generator_t parser_generator_t;
            parser_generator_t nested_d(nested_tag);
            return refactor_unary_nested<parser_category_t>::
                    parse(p, scan, binary, nested_d);
        }
    };
    template <typename CategoryT>
    struct refactor_action_nested {
        template <
            typename ParserT, typename ScannerT, typename BinaryT,
            typename NestedT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &, ScannerT const& scan, BinaryT const& binary,
            NestedT const& nested_d)
        {
            return nested_d[binary].parse(scan);
        }
    };
    template <>
    struct refactor_action_nested<action_parser_category> {
        template <
            typename ParserT, typename ScannerT, typename BinaryT,
            typename NestedT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &, ScannerT const& scan, BinaryT const& binary,
            NestedT const& nested_d)
        {
            typedef typename BinaryT::parser_generator_t binary_gen_t;
            return (
                nested_d[
                    binary_gen_t::generate(
                        binary.left().subject(),
                        binary.right()
                    )
                ][binary.left().predicate()]
            ).parse(scan);
        }
    };
    template <typename CategoryT>
    struct refactor_action_non_nested {
        template <typename ParserT, typename ScannerT, typename BinaryT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &, ScannerT const& scan, BinaryT const& binary)
        {
            return binary.parse(scan);
        }
    };
    template <>
    struct refactor_action_non_nested<action_parser_category> {
        template <typename ParserT, typename ScannerT, typename BinaryT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &, ScannerT const& scan, BinaryT const& binary)
        {
            typedef typename BinaryT::parser_generator_t binary_gen_t;
            return (
                binary_gen_t::generate(
                    binary.left().subject(),
                    binary.right()
                )[binary.left().predicate()]
            ).parse(scan);
        }
    };
    template <typename NestedT>
    struct refactor_action_type {
        template <typename ParserT, typename ScannerT, typename BinaryT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &p, ScannerT const& scan, BinaryT const& binary,
            NestedT const& nested_d)
        {
            typedef
                typename BinaryT::left_t::parser_category_t
                parser_category_t;
            return refactor_action_nested<parser_category_t>::
                    parse(p, scan, binary, nested_d);
        }
    };
    template <>
    struct refactor_action_type<non_nested_refactoring> {
        template <typename ParserT, typename ScannerT, typename BinaryT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &p, ScannerT const& scan, BinaryT const& binary,
            non_nested_refactoring const&)
        {
            typedef
                typename BinaryT::left_t::parser_category_t
                parser_category_t;
            return refactor_action_non_nested<parser_category_t>::
                parse(p, scan, binary);
        }
    };
    template <>
    struct refactor_action_type<self_nested_refactoring> {
        template <typename ParserT, typename ScannerT, typename BinaryT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &p, ScannerT const& scan, BinaryT const& binary,
            self_nested_refactoring const &nested_tag)
        {
            typedef typename ParserT::parser_generator_t parser_generator_t;
            typedef
                typename BinaryT::left_t::parser_category_t
                parser_category_t;
            parser_generator_t nested_d(nested_tag);
            return refactor_action_nested<parser_category_t>::
                    parse(p, scan, binary, nested_d);
        }
    };
    template <typename CategoryT>
    struct attach_action_nested {
        template <
            typename ParserT, typename ScannerT, typename ActionT,
            typename NestedT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &, ScannerT const& scan, ActionT const &action,
            NestedT const& nested_d)
        {
            return action.parse(scan);
        }
    };
    template <>
    struct attach_action_nested<binary_parser_category> {
        template <
            typename ParserT, typename ScannerT, typename ActionT,
            typename NestedT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &, ScannerT const& scan, ActionT const &action,
            NestedT const& nested_d)
        {
            typedef
                typename ActionT::subject_t::parser_generator_t
                binary_gen_t;
            return (
                binary_gen_t::generate(
                    nested_d[action.subject().left()[action.predicate()]],
                    nested_d[action.subject().right()[action.predicate()]]
                )
            ).parse(scan);
        }
    };
    template <typename CategoryT>
    struct attach_action_non_nested {
        template <typename ParserT, typename ScannerT, typename ActionT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &, ScannerT const& scan, ActionT const &action)
        {
            return action.parse(scan);
        }
    };
    template <>
    struct attach_action_non_nested<binary_parser_category> {
        template <typename ParserT, typename ScannerT, typename ActionT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &, ScannerT const& scan, ActionT const &action)
        {
            typedef
                typename ActionT::subject_t::parser_generator_t
                binary_gen_t;
            return (
                binary_gen_t::generate(
                    action.subject().left()[action.predicate()],
                    action.subject().right()[action.predicate()]
                )
            ).parse(scan);
        }
    };
    template <typename NestedT>
    struct attach_action_type {
        template <typename ParserT, typename ScannerT, typename ActionT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &p, ScannerT const& scan, ActionT const& action,
            NestedT const& nested_d)
        {
            typedef
                typename ActionT::subject_t::parser_category_t
                parser_category_t;
            return attach_action_nested<parser_category_t>::
                    parse(p, scan, action, nested_d);
        }
    };
    template <>
    struct attach_action_type<non_nested_refactoring> {
        template <typename ParserT, typename ScannerT, typename ActionT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &p, ScannerT const& scan, ActionT const &action,
            non_nested_refactoring const&)
        {
            typedef
                typename ActionT::subject_t::parser_category_t
                parser_category_t;
            return attach_action_non_nested<parser_category_t>::
                parse(p, scan, action);
        }
    };
    template <>
    struct attach_action_type<self_nested_refactoring> {
        template <typename ParserT, typename ScannerT, typename ActionT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const &p, ScannerT const& scan, ActionT const &action,
            self_nested_refactoring const& nested_tag)
        {
            typedef typename ParserT::parser_generator_t parser_generator_t;
            typedef
                typename ActionT::subject_t::parser_category_t
                parser_category_t;
            parser_generator_t nested_d(nested_tag);
            return attach_action_nested<parser_category_t>::
                    parse(p, scan, action, nested_d);
        }
    };
}
}}
namespace boost { namespace spirit {
template <typename NestedT = non_nested_refactoring>
class refactor_unary_gen;
template <typename BinaryT, typename NestedT = non_nested_refactoring>
class refactor_unary_parser :
    public parser<refactor_unary_parser<BinaryT, NestedT> > {
public:
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ( boost::is_convertible<typename BinaryT::parser_category_t, binary_parser_category>::value ) ) >)> boost_static_assert_typedef_59;
    refactor_unary_parser(BinaryT const& binary_, NestedT const& nested_)
    : binary(binary_), nested(nested_) {}
    typedef refactor_unary_parser<BinaryT, NestedT> self_t;
    typedef refactor_unary_gen<NestedT> parser_generator_t;
    typedef typename BinaryT::left_t::parser_category_t parser_category_t;
    template <typename ScannerT>
    typename parser_result<self_t, ScannerT>::type
    parse(ScannerT const& scan) const
    {
        return impl::refactor_unary_type<NestedT>::
            parse(*this, scan, binary, nested);
    }
private:
    typename as_parser<BinaryT>::type::embed_t binary;
    typename NestedT::embed_t nested;
};
template <typename NestedT>
class refactor_unary_gen {
public:
    typedef refactor_unary_gen<NestedT> embed_t;
    refactor_unary_gen(NestedT const& nested_ = non_nested_refactoring())
    : nested(nested_) {}
    template <typename ParserT>
    refactor_unary_parser<ParserT, NestedT>
    operator[](parser<ParserT> const& subject) const
    {
        return refactor_unary_parser<ParserT, NestedT>
            (subject.derived(), nested);
    }
private:
    typename NestedT::embed_t nested;
};
const refactor_unary_gen<> refactor_unary_d = refactor_unary_gen<>();
template <typename NestedT = non_nested_refactoring>
class refactor_action_gen;
template <typename BinaryT, typename NestedT = non_nested_refactoring>
class refactor_action_parser :
    public parser<refactor_action_parser<BinaryT, NestedT> > {
public:
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ( boost::is_convertible<typename BinaryT::parser_category_t, binary_parser_category>::value ) ) >)> boost_static_assert_typedef_142;
    refactor_action_parser(BinaryT const& binary_, NestedT const& nested_)
    : binary(binary_), nested(nested_) {}
    typedef refactor_action_parser<BinaryT, NestedT> self_t;
    typedef refactor_action_gen<NestedT> parser_generator_t;
    typedef typename BinaryT::left_t::parser_category_t parser_category_t;
    template <typename ScannerT>
    typename parser_result<self_t, ScannerT>::type
    parse(ScannerT const& scan) const
    {
        return impl::refactor_action_type<NestedT>::
            parse(*this, scan, binary, nested);
    }
private:
    typename as_parser<BinaryT>::type::embed_t binary;
    typename NestedT::embed_t nested;
};
template <typename NestedT>
class refactor_action_gen {
public:
    typedef refactor_action_gen<NestedT> embed_t;
    refactor_action_gen(NestedT const& nested_ = non_nested_refactoring())
    : nested(nested_) {}
    template <typename ParserT>
    refactor_action_parser<ParserT, NestedT>
    operator[](parser<ParserT> const& subject) const
    {
        return refactor_action_parser<ParserT, NestedT>
            (subject.derived(), nested);
    }
private:
    typename NestedT::embed_t nested;
};
const refactor_action_gen<> refactor_action_d = refactor_action_gen<>();
template <typename NestedT = non_nested_refactoring>
class attach_action_gen;
template <typename ActionT, typename NestedT = non_nested_refactoring>
class attach_action_parser :
    public parser<attach_action_parser<ActionT, NestedT> > {
public:
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ( boost::is_convertible<typename ActionT::parser_category_t, action_parser_category>::value ) ) >)> boost_static_assert_typedef_224;
    attach_action_parser(ActionT const& actor_, NestedT const& nested_)
    : actor(actor_), nested(nested_) {}
    typedef attach_action_parser<ActionT, NestedT> self_t;
    typedef attach_action_gen<NestedT> parser_generator_t;
    typedef typename ActionT::parser_category_t parser_category_t;
    template <typename ScannerT>
    typename parser_result<self_t, ScannerT>::type
    parse(ScannerT const& scan) const
    {
        return impl::attach_action_type<NestedT>::
            parse(*this, scan, actor, nested);
    }
private:
    typename as_parser<ActionT>::type::embed_t actor;
    typename NestedT::embed_t nested;
};
template <typename NestedT>
class attach_action_gen {
public:
    typedef attach_action_gen<NestedT> embed_t;
    attach_action_gen(NestedT const& nested_ = non_nested_refactoring())
    : nested(nested_) {}
    template <typename ParserT, typename ActionT>
    attach_action_parser<action<ParserT, ActionT>, NestedT>
    operator[](action<ParserT, ActionT> const& actor) const
    {
        return attach_action_parser<action<ParserT, ActionT>, NestedT>
            (actor, nested);
    }
private:
    typename NestedT::embed_t nested;
};
const attach_action_gen<> attach_action_d = attach_action_gen<>();
}}
namespace boost { namespace spirit {
struct is_nested {};
struct non_nested {};
struct is_lexeme {};
struct non_lexeme {};
namespace impl {
    template <typename LexemeT>
    struct select_confix_parse_lexeme;
    template <>
    struct select_confix_parse_lexeme<is_lexeme> {
        template <typename ParserT, typename ScannerT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const& p, ScannerT const& scan)
        {
            typedef typename parser_result<ParserT, ScannerT>::type result_t;
            return contiguous_parser_parse<result_t>(p, scan, scan);
        }
    };
    template <>
    struct select_confix_parse_lexeme<non_lexeme> {
        template <typename ParserT, typename ScannerT>
        static typename parser_result<ParserT, ScannerT>::type
        parse(ParserT const& p, ScannerT const& scan)
        {
            return p.parse(scan);
        }
    };
    template <typename NestedT>
    struct select_confix_parse_refactor;
    template <>
    struct select_confix_parse_refactor<is_nested> {
        template <
            typename LexemeT, typename ParserT, typename ScannerT,
            typename OpenT, typename ExprT, typename CloseT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(
            LexemeT const &, ParserT const& this_, ScannerT const& scan,
            OpenT const& open, ExprT const& expr, CloseT const& close)
        {
            typedef refactor_action_gen<refactor_unary_gen<> > refactor_t;
            const refactor_t refactor_body_d = refactor_t(refactor_unary_d);
            return select_confix_parse_lexeme<LexemeT>::parse((
                            open
                        >> (this_ | refactor_body_d[expr - close])
                        >> close
                    ), scan);
        }
    };
    template <>
    struct select_confix_parse_refactor<non_nested> {
        template <
            typename LexemeT, typename ParserT, typename ScannerT,
            typename OpenT, typename ExprT, typename CloseT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(
            LexemeT const &, ParserT const& , ScannerT const& scan,
            OpenT const& open, ExprT const& expr, CloseT const& close)
        {
            typedef refactor_action_gen<refactor_unary_gen<> > refactor_t;
            const refactor_t refactor_body_d = refactor_t(refactor_unary_d);
            return select_confix_parse_lexeme<LexemeT>::parse((
                            open
                        >> refactor_body_d[expr - close]
                        >> close
                    ), scan);
        }
    };
    template <typename NestedT>
    struct select_confix_parse_no_refactor;
    template <>
    struct select_confix_parse_no_refactor<is_nested> {
        template <
            typename LexemeT, typename ParserT, typename ScannerT,
            typename OpenT, typename ExprT, typename CloseT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(
            LexemeT const &, ParserT const& this_, ScannerT const& scan,
            OpenT const& open, ExprT const& expr, CloseT const& close)
        {
            return select_confix_parse_lexeme<LexemeT>::parse((
                            open
                        >> (this_ | (expr - close))
                        >> close
                    ), scan);
        }
    };
    template <>
    struct select_confix_parse_no_refactor<non_nested> {
        template <
            typename LexemeT, typename ParserT, typename ScannerT,
            typename OpenT, typename ExprT, typename CloseT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(
            LexemeT const &, ParserT const & , ScannerT const& scan,
            OpenT const& open, ExprT const& expr, CloseT const& close)
        {
            return select_confix_parse_lexeme<LexemeT>::parse((
                            open
                        >> (expr - close)
                        >> close
                    ), scan);
        }
    };
    template <typename CategoryT>
    struct confix_parser_type {
        template <
            typename NestedT, typename LexemeT,
            typename ParserT, typename ScannerT,
            typename OpenT, typename ExprT, typename CloseT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(
            NestedT const &, LexemeT const &lexeme,
            ParserT const& this_, ScannerT const& scan,
            OpenT const& open, ExprT const& expr, CloseT const& close)
        {
            return select_confix_parse_refactor<NestedT>::
                parse(lexeme, this_, scan, open, expr, close);
        }
    };
    template <>
    struct confix_parser_type<plain_parser_category> {
        template <
            typename NestedT, typename LexemeT,
            typename ParserT, typename ScannerT,
            typename OpenT, typename ExprT, typename CloseT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(
            NestedT const &, LexemeT const &lexeme,
            ParserT const& this_, ScannerT const& scan,
            OpenT const& open, ExprT const& expr, CloseT const& close)
        {
            return select_confix_parse_no_refactor<NestedT>::
                parse(lexeme, this_, scan, open, expr, close);
        }
    };
}
}}
namespace boost { namespace spirit {
template<typename NestedT = non_nested, typename LexemeT = non_lexeme>
struct confix_parser_gen;
template <
    typename OpenT, typename ExprT, typename CloseT, typename CategoryT,
    typename NestedT = non_nested, typename LexemeT = non_lexeme
>
struct confix_parser :
    public parser<
        confix_parser<OpenT, ExprT, CloseT, CategoryT, NestedT, LexemeT>
    >
{
    typedef
        confix_parser<OpenT, ExprT, CloseT, CategoryT, NestedT, LexemeT>
        self_t;
    confix_parser(OpenT const &open_, ExprT const &expr_, CloseT const &close_)
    : open(open_), expr(expr_), close(close_)
    {}
    template <typename ScannerT>
    typename parser_result<self_t, ScannerT>::type
    parse(ScannerT const& scan) const
    {
        return impl::confix_parser_type<CategoryT>::
            parse(NestedT(), LexemeT(), *this, scan, open, expr, close);
    }
private:
    typename as_parser<OpenT>::type::embed_t open;
    typename as_parser<ExprT>::type::embed_t expr;
    typename as_parser<CloseT>::type::embed_t close;
};
template<typename NestedT, typename LexemeT>
struct confix_parser_gen
{
    template<typename StartT, typename ExprT, typename EndT>
    confix_parser<
        typename as_parser<StartT>::type,
        typename as_parser<ExprT>::type,
        typename as_parser<EndT>::type,
        typename as_parser<ExprT>::type::parser_category_t,
        NestedT,
        LexemeT
    >
    operator()(
        StartT const &start_, ExprT const &expr_, EndT const &end_) const
    {
        typedef typename as_parser<StartT>::type start_t;
        typedef typename as_parser<ExprT>::type expr_t;
        typedef typename as_parser<EndT>::type end_t;
        typedef
            typename as_parser<ExprT>::type::parser_category_t
            parser_category_t;
        typedef
            confix_parser<
                start_t, expr_t, end_t, parser_category_t, NestedT, LexemeT
            >
            return_t;
        return return_t(
            as_parser<StartT>::convert(start_),
            as_parser<ExprT>::convert(expr_),
            as_parser<EndT>::convert(end_)
        );
    }
    template<typename StartT, typename ExprT, typename EndT>
    confix_parser<
        typename as_parser<StartT>::type,
        typename as_parser<ExprT>::type,
        typename as_parser<EndT>::type,
        plain_parser_category,
        NestedT,
        LexemeT
    >
    direct(StartT const &start_, ExprT const &expr_, EndT const &end_) const
    {
        typedef typename as_parser<StartT>::type start_t;
        typedef typename as_parser<ExprT>::type expr_t;
        typedef typename as_parser<EndT>::type end_t;
        typedef plain_parser_category parser_category_t;
        typedef
            confix_parser<
                start_t, expr_t, end_t, parser_category_t, NestedT, LexemeT
            >
            return_t;
        return return_t(
            as_parser<StartT>::convert(start_),
            as_parser<ExprT>::convert(expr_),
            as_parser<EndT>::convert(end_)
        );
    }
};
const confix_parser_gen<non_nested, non_lexeme> confix_p =
    confix_parser_gen<non_nested, non_lexeme>();
template<typename NestedT>
struct comment_parser_gen
{
    template<typename StartT>
    confix_parser<
        typename as_parser<StartT>::type,
        kleene_star<anychar_parser>,
        alternative<eol_parser, end_parser>,
        unary_parser_category,
        NestedT,
        is_lexeme
    >
    operator() (StartT const &start_) const
    {
        typedef typename as_parser<StartT>::type start_t;
        typedef kleene_star<anychar_parser> expr_t;
        typedef alternative<eol_parser, end_parser> end_t;
        typedef unary_parser_category parser_category_t;
        typedef
            confix_parser<
                start_t, expr_t, end_t, parser_category_t, NestedT, is_lexeme
            >
            return_t;
        return return_t(
            as_parser<StartT>::convert(start_),
            *anychar_p,
            eol_p | end_p
        );
    }
    template<typename StartT, typename EndT>
    confix_parser<
        typename as_parser<StartT>::type,
        kleene_star<anychar_parser>,
        typename as_parser<EndT>::type,
        unary_parser_category,
        NestedT,
        is_lexeme
    >
    operator() (StartT const &start_, EndT const &end_) const
    {
        typedef typename as_parser<StartT>::type start_t;
        typedef kleene_star<anychar_parser> expr_t;
        typedef typename as_parser<EndT>::type end_t;
        typedef unary_parser_category parser_category_t;
        typedef
            confix_parser<
                start_t, expr_t, end_t, parser_category_t, NestedT, is_lexeme
            >
            return_t;
        return return_t(
            as_parser<StartT>::convert(start_),
            *anychar_p,
            as_parser<EndT>::convert(end_)
        );
    }
};
const comment_parser_gen<non_nested> comment_p =
    comment_parser_gen<non_nested>();
template<typename OpenT, typename CloseT>
struct comment_nest_parser:
    public parser<comment_nest_parser<OpenT, CloseT> >
{
    typedef comment_nest_parser<OpenT, CloseT> self_t;
    comment_nest_parser(OpenT const &open_, CloseT const &close_):
        open(open_), close(close_)
    {}
    template<typename ScannerT>
    typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const &scan) const
    {
        return do_parse(
            open >> *(*this | (anychar_p - close)) >> close,
            scan);
    }
private:
    template<typename ParserT, typename ScannerT>
    typename parser_result<self_t, ScannerT>::type
        do_parse(ParserT const &p, ScannerT const &scan) const
    {
        return
            impl::contiguous_parser_parse<
                typename parser_result<ParserT, ScannerT>::type
            >(p, scan, scan);
    }
    typename as_parser<OpenT>::type::embed_t open;
    typename as_parser<CloseT>::type::embed_t close;
};
template<typename OpenT, typename CloseT>
inline
comment_nest_parser<
    typename as_parser<OpenT>::type,
    typename as_parser<CloseT>::type
>
    comment_nest_p(OpenT const &open, CloseT const &close)
{
    return
        comment_nest_parser<
            typename as_parser<OpenT>::type,
            typename as_parser<CloseT>::type
        >(
            as_parser<OpenT>::convert(open),
            as_parser<CloseT>::convert(close)
        );
}
}}
namespace boost { namespace spirit {
struct no_list_endtoken { typedef no_list_endtoken embed_t; };
namespace impl {
    template <typename EndT>
    struct select_list_parse_refactor {
        template <
            typename ParserT, typename ScannerT,
            typename ItemT, typename DelimT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(ScannerT const& scan, ParserT const& ,
            ItemT const &item, DelimT const &delim, EndT const &end)
        {
            typedef refactor_action_gen<refactor_unary_gen<> > refactor_t;
            const refactor_t refactor_item_d = refactor_t(refactor_unary_d);
            return (
                    refactor_item_d[item - (end | delim)]
                >> *(delim >> refactor_item_d[item - (end | delim)])
                >> !(delim >> end)
            ).parse(scan);
        }
    };
    template <>
    struct select_list_parse_refactor<no_list_endtoken> {
        template <
            typename ParserT, typename ScannerT,
            typename ItemT, typename DelimT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(ScannerT const& scan, ParserT const& ,
            ItemT const &item, DelimT const &delim, no_list_endtoken const&)
        {
            typedef refactor_action_gen<refactor_unary_gen<> > refactor_t;
            const refactor_t refactor_item_d = refactor_t(refactor_unary_d);
            return (
                    refactor_item_d[item - delim]
                >> *(delim >> refactor_item_d[item - delim])
            ).parse(scan);
        }
    };
    template <typename EndT>
    struct select_list_parse_no_refactor {
        template <
            typename ParserT, typename ScannerT,
            typename ItemT, typename DelimT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(ScannerT const& scan, ParserT const& ,
            ItemT const &item, DelimT const &delim, EndT const &end)
        {
            return (
                    (item - (end | delim))
                >> *(delim >> (item - (end | delim)))
                >> !(delim >> end)
            ).parse(scan);
        }
    };
    template <>
    struct select_list_parse_no_refactor<no_list_endtoken> {
        template <
            typename ParserT, typename ScannerT,
            typename ItemT, typename DelimT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(ScannerT const& scan, ParserT const& ,
            ItemT const &item, DelimT const &delim, no_list_endtoken const&)
        {
            return (
                    (item - delim)
                >> *(delim >> (item - delim))
            ).parse(scan);
        }
    };
    template <typename CategoryT>
    struct list_parser_type {
        template <
            typename ParserT, typename ScannerT,
            typename ItemT, typename DelimT, typename EndT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(ScannerT const& scan, ParserT const& p,
              ItemT const &item, DelimT const &delim, EndT const &end)
        {
            return select_list_parse_refactor<EndT>::
                parse(scan, p, item, delim, end);
        }
    };
    template <>
    struct list_parser_type<plain_parser_category> {
        template <
            typename ParserT, typename ScannerT,
            typename ItemT, typename DelimT, typename EndT
        >
        static typename parser_result<ParserT, ScannerT>::type
        parse(ScannerT const& scan, ParserT const& p,
              ItemT const &item, DelimT const &delim, EndT const &end)
        {
            return select_list_parse_no_refactor<EndT>::
                parse(scan, p, item, delim, end);
        }
    };
}
}}
namespace boost { namespace spirit {
template <
    typename ItemT, typename DelimT, typename EndT = no_list_endtoken,
    typename CategoryT = plain_parser_category
>
struct list_parser :
    public parser<list_parser<ItemT, DelimT, EndT, CategoryT> > {
    typedef list_parser<ItemT, DelimT, EndT, CategoryT> self_t;
    typedef CategoryT parser_category_t;
    list_parser(ItemT const &item_, DelimT const &delim_,
        EndT const& end_ = no_list_endtoken())
    : item(item_), delim(delim_), end(end_)
    {}
    template <typename ScannerT>
    typename parser_result<self_t, ScannerT>::type
    parse(ScannerT const& scan) const
    {
        return impl::list_parser_type<CategoryT>
            ::parse(scan, *this, item, delim, end);
    }
private:
    typename as_parser<ItemT>::type::embed_t item;
    typename as_parser<DelimT>::type::embed_t delim;
    typename as_parser<EndT>::type::embed_t end;
};
template <typename CharT = char>
struct list_parser_gen :
    public list_parser<kleene_star<anychar_parser>, chlit<CharT> >
{
    typedef list_parser_gen<CharT> self_t;
    list_parser_gen()
    : list_parser<kleene_star<anychar_parser>, chlit<CharT> >
        (*anychar_p, chlit<CharT>(','))
    {}
    template<typename DelimT>
    list_parser<
        kleene_star<anychar_parser>,
        typename as_parser<DelimT>::type,
        no_list_endtoken,
        unary_parser_category
    >
    operator()(DelimT const &delim_) const
    {
        typedef kleene_star<anychar_parser> item_t;
        typedef typename as_parser<DelimT>::type delim_t;
        typedef
            list_parser<item_t, delim_t, no_list_endtoken, unary_parser_category>
            return_t;
        return return_t(*anychar_p, as_parser<DelimT>::convert(delim_));
    }
    template<typename ItemT, typename DelimT>
    list_parser<
        typename as_parser<ItemT>::type,
        typename as_parser<DelimT>::type,
        no_list_endtoken,
        typename as_parser<ItemT>::type::parser_category_t
    >
    operator()(ItemT const &item_, DelimT const &delim_) const
    {
        typedef typename as_parser<ItemT>::type item_t;
        typedef typename as_parser<DelimT>::type delim_t;
        typedef list_parser<item_t, delim_t, no_list_endtoken,
                typename item_t::parser_category_t>
            return_t;
        return return_t(
            as_parser<ItemT>::convert(item_),
            as_parser<DelimT>::convert(delim_)
        );
    }
    template<typename ItemT, typename DelimT, typename EndT>
    list_parser<
        typename as_parser<ItemT>::type,
        typename as_parser<DelimT>::type,
        typename as_parser<EndT>::type
    >
    operator()(
        ItemT const &item_, DelimT const &delim_, EndT const &end_) const
    {
        typedef typename as_parser<ItemT>::type item_t;
        typedef typename as_parser<DelimT>::type delim_t;
        typedef typename as_parser<EndT>::type end_t;
        typedef list_parser<item_t, delim_t, end_t,
                typename item_t::parser_category_t>
            return_t;
        return return_t(
            as_parser<ItemT>::convert(item_),
            as_parser<DelimT>::convert(delim_),
            as_parser<EndT>::convert(end_)
        );
    }
    template<typename ItemT, typename DelimT>
    list_parser<
        typename as_parser<ItemT>::type,
        typename as_parser<DelimT>::type,
        no_list_endtoken,
        plain_parser_category
    >
    direct(ItemT const &item_, DelimT const &delim_) const
    {
        typedef typename as_parser<ItemT>::type item_t;
        typedef typename as_parser<DelimT>::type delim_t;
        typedef list_parser<item_t, delim_t, no_list_endtoken,
                plain_parser_category>
            return_t;
        return return_t(
            as_parser<ItemT>::convert(item_),
            as_parser<DelimT>::convert(delim_)
        );
    }
    template<typename ItemT, typename DelimT, typename EndT>
    list_parser<
        typename as_parser<ItemT>::type,
        typename as_parser<DelimT>::type,
        typename as_parser<EndT>::type,
        plain_parser_category
    >
    direct(
        ItemT const &item_, DelimT const &delim_, EndT const &end_) const
    {
        typedef typename as_parser<ItemT>::type item_t;
        typedef typename as_parser<DelimT>::type delim_t;
        typedef typename as_parser<EndT>::type end_t;
        typedef
            list_parser<item_t, delim_t, end_t, plain_parser_category>
            return_t;
        return return_t(
            as_parser<ItemT>::convert(item_),
            as_parser<DelimT>::convert(delim_),
            as_parser<EndT>::convert(end_)
        );
    }
};
const list_parser_gen<> list_p = list_parser_gen<>();
}}
namespace boost {
    namespace spirit {
template <typename CharT = char, typename TailT = chset<CharT> >
class distinct_parser
{
public:
    typedef
        contiguous<
            sequence<
                chseq<CharT const*>,
                negated_empty_match_parser<
                    TailT
                >
            >
        >
            result_t;
    distinct_parser()
    : tail(chset<CharT>())
    {
    }
    explicit distinct_parser(parser<TailT> const & tail_)
    : tail(tail_.derived())
    {
    }
    explicit distinct_parser(CharT const* letters)
    : tail(chset_p(letters))
    {
    }
    result_t operator()(CharT const* str) const
    {
        return lexeme_d[chseq_p(str) >> ~epsilon_p(tail)];
    }
    TailT tail;
};
template <typename CharT = char, typename TailT = chset<CharT> >
class distinct_directive
{
public:
    template<typename ParserT>
    struct result {
        typedef
            contiguous<
                sequence<
                    ParserT,
                    negated_empty_match_parser<
                        TailT
                    >
                >
            >
                type;
    };
    distinct_directive()
    : tail(chset<CharT>())
    {
    }
    explicit distinct_directive(CharT const* letters)
    : tail(chset_p(letters))
    {
    }
    explicit distinct_directive(parser<TailT> const & tail_)
    : tail(tail_.derived())
    {
    }
    template<typename ParserT>
    typename result<typename as_parser<ParserT>::type>::type
        operator[](ParserT const &subject) const
    {
        return
            lexeme_d[as_parser<ParserT>::convert(subject) >> ~epsilon_p(tail)];
    }
    TailT tail;
};
template <typename ScannerT = scanner<> >
class dynamic_distinct_parser
{
public:
    typedef typename ScannerT::value_t char_t;
    typedef
        rule<typename lexeme_scanner<ScannerT>::type>
            tail_t;
    typedef
        contiguous<
            sequence<
                chseq<char_t const*>,
                negated_empty_match_parser<
                    tail_t
                >
            >
        >
            result_t;
    dynamic_distinct_parser()
    : tail(nothing_p)
    {
    }
    template<typename ParserT>
    explicit dynamic_distinct_parser(parser<ParserT> const & tail_)
    : tail(tail_.derived())
    {
    }
    explicit dynamic_distinct_parser(char_t const* letters)
    : tail(chset_p(letters))
    {
    }
    result_t operator()(char_t const* str) const
    {
        return lexeme_d[chseq_p(str) >> ~epsilon_p(tail)];
    }
    tail_t tail;
};
template <typename ScannerT = scanner<> >
class dynamic_distinct_directive
{
public:
    typedef typename ScannerT::value_t char_t;
    typedef
        rule<typename lexeme_scanner<ScannerT>::type>
            tail_t;
    template<typename ParserT>
    struct result {
        typedef
            contiguous<
                sequence<
                    ParserT,
                    negated_empty_match_parser<
                        tail_t
                    >
                >
            >
                type;
    };
    dynamic_distinct_directive()
    : tail(nothing_p)
    {
    }
    template<typename ParserT>
    explicit dynamic_distinct_directive(parser<ParserT> const & tail_)
    : tail(tail_.derived())
    {
    }
    explicit dynamic_distinct_directive(char_t const* letters)
    : tail(chset_p(letters))
    {
    }
    template<typename ParserT>
    typename result<typename as_parser<ParserT>::type>::type
        operator[](ParserT const &subject) const
    {
        return
            lexeme_d[as_parser<ParserT>::convert(subject) >> ~epsilon_p(tail)];
    }
    tail_t tail;
};
    }
}
#include <deque>
namespace boost
{
  namespace detail {
   template <class Category, class T, class Distance, class Pointer, class Reference>
   struct iterator_base : std::iterator<Category, T, Distance, Pointer, Reference> {};
  }
  template <class Category, class T, class Distance = std::ptrdiff_t,
            class Pointer = T*, class Reference = T&>
   struct iterator : detail::iterator_base<Category, T, Distance, Pointer, Reference> {};
}
namespace boost {
namespace mpl {
template<
      typename C = void_
    , typename F1 = void_
    , typename F2 = void_
    >
struct apply_if
{
 private:
    typedef typename if_<C,F1,F2>::type nullary_func_;
 public:
    typedef typename nullary_func_::type type;
   
};
template<
      bool C
    , typename F1
    , typename F2
    >
struct apply_if_c
{
 private:
    typedef typename if_c<C,F1,F2>::type nullary_func_;
 public:
    typedef typename nullary_func_::type type;
};
template<> struct apply_if< void_,void_,void_ > { template< typename T1,typename T2,typename T3 , typename T4 =void_ ,typename T5 =void_ > struct apply : apply_if< T1,T2,T3 > { }; }; template<> struct lambda< apply_if< void_,void_,void_ > , void_ , int_<-1> > { typedef apply_if< void_,void_,void_ > type; }; namespace aux { template< typename T1,typename T2,typename T3 > struct template_arity< apply_if< T1,T2,T3 > > { static const int value = 3; }; template<> struct template_arity< apply_if< void_,void_,void_ > > { static const int value = -1; }; }
}
}
namespace boost {
namespace mpl {
template<
      typename T = void_
    >
struct identity
{
    typedef T type;
   
};
template<
      typename T = void_
    >
struct make_identity
{
    typedef identity<T> type;
   
};
template<> struct identity< void_ > { template< typename T1 , typename T2 =void_ ,typename T3 =void_ ,typename T4 =void_ ,typename T5 =void_ > struct apply : identity< T1 > { }; }; template<> struct lambda< identity< void_ > , void_ , int_<-1> > { typedef identity< void_ > type; }; namespace aux { template< typename T1 > struct template_arity< identity< T1 > > { static const int value = 1; }; template<> struct template_arity< identity< void_ > > { static const int value = -1; }; }
template<> struct make_identity< void_ > { template< typename T1 , typename T2 =void_ ,typename T3 =void_ ,typename T4 =void_ ,typename T5 =void_ > struct apply : make_identity< T1 > { }; }; template<> struct lambda< make_identity< void_ > , void_ , int_<-1> > { typedef make_identity< void_ > type; }; namespace aux { template< typename T1 > struct template_arity< make_identity< T1 > > { static const int value = 1; }; template<> struct template_arity< make_identity< void_ > > { static const int value = -1; }; }
}
}
namespace boost {
namespace mpl {
template<> struct arg<-1>
{
    static int const value = -1;
   
    template<
          typename U1 = void_, typename U2 = void_, typename U3 = void_
        , typename U4 = void_, typename U5 = void_
        >
    struct apply
    {
        typedef U1 type;
     private:
        static bool const nv = !is_void_<type>::value;
        typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( nv ) >)> boost_static_assert_typedef_22;
    };
};
template<> struct arg<1>
{
    static int const value = 1;
    typedef arg<2> next;
   
    template<
          typename U1 = void_, typename U2 = void_, typename U3 = void_
        , typename U4 = void_, typename U5 = void_
        >
    struct apply
    {
        typedef U1 type;
     private:
        static bool const nv = !is_void_<type>::value;
        typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( nv ) >)> boost_static_assert_typedef_43;
    };
};
template<> struct arg<2>
{
    static int const value = 2;
    typedef arg<3> next;
   
    template<
          typename U1 = void_, typename U2 = void_, typename U3 = void_
        , typename U4 = void_, typename U5 = void_
        >
    struct apply
    {
        typedef U2 type;
     private:
        static bool const nv = !is_void_<type>::value;
        typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( nv ) >)> boost_static_assert_typedef_64;
    };
};
template<> struct arg<3>
{
    static int const value = 3;
    typedef arg<4> next;
   
    template<
          typename U1 = void_, typename U2 = void_, typename U3 = void_
        , typename U4 = void_, typename U5 = void_
        >
    struct apply
    {
        typedef U3 type;
     private:
        static bool const nv = !is_void_<type>::value;
        typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( nv ) >)> boost_static_assert_typedef_85;
    };
};
template<> struct arg<4>
{
    static int const value = 4;
    typedef arg<5> next;
   
    template<
          typename U1 = void_, typename U2 = void_, typename U3 = void_
        , typename U4 = void_, typename U5 = void_
        >
    struct apply
    {
        typedef U4 type;
     private:
        static bool const nv = !is_void_<type>::value;
        typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( nv ) >)> boost_static_assert_typedef_106;
    };
};
template<> struct arg<5>
{
    static int const value = 5;
    typedef arg<6> next;
   
    template<
          typename U1 = void_, typename U2 = void_, typename U3 = void_
        , typename U4 = void_, typename U5 = void_
        >
    struct apply
    {
        typedef U5 type;
     private:
        static bool const nv = !is_void_<type>::value;
        typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( nv ) >)> boost_static_assert_typedef_127;
    };
};
}
}
namespace boost {
namespace mpl {
typedef arg< -1 > _;
namespace placeholders {
using boost::mpl::_;
}
typedef arg<1> _1;
namespace placeholders {
using boost::mpl::_1;
}
typedef arg<2> _2;
namespace placeholders {
using boost::mpl::_2;
}
typedef arg<3> _3;
namespace placeholders {
using boost::mpl::_3;
}
typedef arg<4> _4;
namespace placeholders {
using boost::mpl::_4;
}
typedef arg<5> _5;
namespace placeholders {
using boost::mpl::_5;
}
typedef arg<6> _6;
namespace placeholders {
using boost::mpl::_6;
}
}
}
namespace boost {
struct no_traversal_tag {};
struct incrementable_traversal_tag {};
struct single_pass_traversal_tag
  : incrementable_traversal_tag {};
struct forward_traversal_tag
  : single_pass_traversal_tag {};
struct bidirectional_traversal_tag
  : forward_traversal_tag {};
struct random_access_traversal_tag
  : bidirectional_traversal_tag {};
namespace detail
{
  template <class Cat>
  struct old_category_to_traversal
    : mpl::apply_if<
          is_convertible<Cat,std::random_access_iterator_tag>
        , mpl::identity<random_access_traversal_tag>
        , mpl::apply_if<
              is_convertible<Cat,std::bidirectional_iterator_tag>
            , mpl::identity<bidirectional_traversal_tag>
            , mpl::apply_if<
                  is_convertible<Cat,std::forward_iterator_tag>
                , mpl::identity<forward_traversal_tag>
                , mpl::apply_if<
                      is_convertible<Cat,std::input_iterator_tag>
                    , mpl::identity<single_pass_traversal_tag>
                    , mpl::apply_if<
                          is_convertible<Cat,std::output_iterator_tag>
                        , mpl::identity<incrementable_traversal_tag>
                        , void
                      >
                  >
              >
          >
      >
  {};
  template <class Traversal>
  struct pure_traversal_tag
    : mpl::apply_if<
          is_convertible<Traversal,random_access_traversal_tag>
        , mpl::identity<random_access_traversal_tag>
        , mpl::apply_if<
              is_convertible<Traversal,bidirectional_traversal_tag>
            , mpl::identity<bidirectional_traversal_tag>
            , mpl::apply_if<
                  is_convertible<Traversal,forward_traversal_tag>
                , mpl::identity<forward_traversal_tag>
                , mpl::apply_if<
                      is_convertible<Traversal,single_pass_traversal_tag>
                    , mpl::identity<single_pass_traversal_tag>
                    , mpl::apply_if<
                          is_convertible<Traversal,incrementable_traversal_tag>
                        , mpl::identity<incrementable_traversal_tag>
                        , void
                      >
                  >
              >
          >
      >
  {
  };
}
template <class Cat>
struct iterator_category_to_traversal
  : mpl::apply_if<
        is_convertible<Cat,incrementable_traversal_tag>
      , mpl::identity<Cat>
      , detail::old_category_to_traversal<Cat>
    >
{};
template <class Iterator = mpl::_1>
struct iterator_traversal
  : iterator_category_to_traversal<
        typename boost::detail::iterator_traits<Iterator>::iterator_category
    >
{};
}
namespace boost { namespace mpl {
namespace aux {
template< bool C_, typename T1, typename T2, typename T3, typename T4 >
struct or_impl
    : true_
{
};
template< typename T1, typename T2, typename T3, typename T4 >
struct or_impl< false,T1,T2,T3,T4 >
    : or_impl<
          ::boost::mpl::aux::nested_type_wknd<T1>::value
        , T2, T3, T4
        , false_
        >
{
};
template<>
struct or_impl<
          false
        , false_, false_, false_, false_
        >
    : false_
{
};
}
template<
      typename T1 = void_
    , typename T2 = void_
    , typename T3 = false_, typename T4 = false_, typename T5 = false_
    >
struct or_
    : aux::or_impl<
          ::boost::mpl::aux::nested_type_wknd<T1>::value
        , T2, T3, T4, T5
        >
{
   
};
template<> struct or_< void_,void_ > { template< typename T1,typename T2 , typename T3 =void_ ,typename T4 =void_ ,typename T5 =void_ > struct apply : or_< T1,T2 > { }; }; template<> struct lambda< or_< void_,void_ > , void_ , int_<-1> > { typedef or_< void_,void_ > type; }; namespace aux { template< typename T1,typename T2,typename T3,typename T4,typename T5 > struct template_arity< or_< T1,T2,T3,T4,T5 > > { static const int value = 5; }; template<> struct template_arity< or_< void_,void_ > > { static const int value = -1; }; }
}}
namespace boost
{
  template <typename A, typename B>
  struct is_interoperable
    : mpl::or_<
          is_convertible< A, B >
        , is_convertible< B, A > >
  {
  };
}
namespace boost { namespace mpl {
namespace aux {
template< bool C_, typename T1, typename T2, typename T3, typename T4 >
struct and_impl
    : false_
{
};
template< typename T1, typename T2, typename T3, typename T4 >
struct and_impl< true,T1,T2,T3,T4 >
    : and_impl<
          ::boost::mpl::aux::nested_type_wknd<T1>::value
        , T2, T3, T4
        , true_
        >
{
};
template<>
struct and_impl<
          true
        , true_, true_, true_, true_
        >
    : true_
{
};
}
template<
      typename T1 = void_
    , typename T2 = void_
    , typename T3 = true_, typename T4 = true_, typename T5 = true_
    >
struct and_
    : aux::and_impl<
          ::boost::mpl::aux::nested_type_wknd<T1>::value
        , T2, T3, T4, T5
        >
{
   
};
template<> struct and_< void_,void_ > { template< typename T1,typename T2 , typename T3 =void_ ,typename T4 =void_ ,typename T5 =void_ > struct apply : and_< T1,T2 > { }; }; template<> struct lambda< and_< void_,void_ > , void_ , int_<-1> > { typedef and_< void_,void_ > type; }; namespace aux { template< typename T1,typename T2,typename T3,typename T4,typename T5 > struct template_arity< and_< T1,T2,T3,T4,T5 > > { static const int value = 5; }; template<> struct template_arity< and_< void_,void_ > > { static const int value = -1; }; }
}}
namespace boost { struct use_default; }
namespace boost { namespace detail {
struct input_output_iterator_tag
  : std::input_iterator_tag
{
    operator std::output_iterator_tag() const
    {
        return std::output_iterator_tag();
    }
};
template <class ValueParam, class Reference>
struct iterator_writability_disabled
  : is_const<ValueParam>
{};
template <class Traversal, class ValueParam, class Reference>
struct iterator_facade_default_category
  : mpl::apply_if<
        mpl::and_<
            is_reference<Reference>
          , is_convertible<Traversal,forward_traversal_tag>
        >
      , mpl::apply_if<
            is_convertible<Traversal,random_access_traversal_tag>
          , mpl::identity<std::random_access_iterator_tag>
          , mpl::if_<
                is_convertible<Traversal,bidirectional_traversal_tag>
              , std::bidirectional_iterator_tag
              , std::forward_iterator_tag
            >
        >
      , typename mpl::apply_if<
            mpl::and_<
                is_convertible<Traversal, single_pass_traversal_tag>
              , is_convertible<Reference, ValueParam>
            >
          , mpl::identity<std::input_iterator_tag>
          , mpl::identity<Traversal>
        >
    >
{
};
template <class T>
struct is_iterator_category
  : mpl::or_<
        is_convertible<T,std::input_iterator_tag>
      , is_convertible<T,std::output_iterator_tag>
    >
{
};
template <class T>
struct is_iterator_traversal
  : is_convertible<T,incrementable_traversal_tag>
{};
template <class Category, class Traversal>
struct iterator_category_with_traversal
  : Category, Traversal
{
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( !(is_convertible< typename iterator_category_to_traversal<Category>::type , Traversal >::value) ) >)> boost_static_assert_typedef_158;
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( is_iterator_category<Category>::value ) >)> boost_static_assert_typedef_160;
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( !is_iterator_category<Traversal>::value ) >)> boost_static_assert_typedef_161;
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( !is_iterator_traversal<Category>::value ) >)> boost_static_assert_typedef_162;
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( is_iterator_traversal<Traversal>::value ) >)> boost_static_assert_typedef_164;
};
template <class Traversal, class ValueParam, class Reference>
struct facade_iterator_category_impl
{
    typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( !is_iterator_category<Traversal>::value ) >)> boost_static_assert_typedef_175;
    typedef typename iterator_facade_default_category<
        Traversal,ValueParam,Reference
    >::type category;
    typedef typename mpl::if_<
        is_same<
            Traversal
          , typename iterator_category_to_traversal<category>::type
        >
      , category
      , iterator_category_with_traversal<category,Traversal>
    >::type type;
};
template <class CategoryOrTraversal, class ValueParam, class Reference>
struct facade_iterator_category
  : mpl::apply_if<
        is_iterator_category<CategoryOrTraversal>
      , mpl::identity<CategoryOrTraversal>
      , facade_iterator_category_impl<CategoryOrTraversal,ValueParam,Reference>
    >
{
};
}}
namespace boost
{
  namespace iterators
  {
    template<bool>
    struct enabled
    {
      template<typename T>
      struct base
      {
        typedef T type;
      };
    };
    template<>
    struct enabled<false>
    {
      template<typename T>
      struct base
      {
      };
    };
    template <class Cond,
              class Return>
    struct enable_if
      : enabled<(Cond::value)>::template base<Return>
    {
    };
  }
}
namespace boost {
namespace detail {
template <typename T>
struct add_pointer_impl
{
    typedef typename remove_reference<T>::type no_ref_type;
    typedef no_ref_type* type;
};
}
template< typename T > struct add_pointer { typedef typename detail::add_pointer_impl<T>::type type; };
}
namespace boost {
namespace detail {
template <typename T, bool is_vol>
struct remove_const_helper
{
    typedef T type;
};
template <typename T>
struct remove_const_helper<T, true>
{
    typedef T volatile type;
};
template <typename T>
struct remove_const_impl
{
    typedef typename remove_const_helper<
          typename cv_traits_imp<T*>::unqualified_type
        , ::boost::is_volatile<T>::value
        >::type type;
};
}
template< typename T > struct remove_const { typedef typename detail::remove_const_impl<T>::type type; };
template< typename T > struct remove_const<T&> { typedef T& type; };
template< typename T, std::size_t N > struct remove_const<T const[N]> { typedef T type[N]; };
template< typename T, std::size_t N > struct remove_const<T const volatile[N]> { typedef T volatile type[N]; };
}
namespace boost
{
  template <class I, class V, class TC, class R, class D> class iterator_facade;
  namespace detail
  {
    template <
        class Facade1
      , class Facade2
      , class Return
    >
    struct enable_if_interoperable
      : ::boost::iterators::enable_if<
           mpl::or_<
               is_convertible<Facade1, Facade2>
             , is_convertible<Facade2, Facade1>
           >
         , Return
        >
    {
    };
    template <
        class ValueParam
      , class CategoryOrTraversal
      , class Reference
      , class Difference
    >
    struct iterator_facade_types
    {
        typedef typename facade_iterator_category<
            CategoryOrTraversal, ValueParam, Reference
        >::type iterator_category;
        typedef typename remove_const<ValueParam>::type value_type;
        typedef typename mpl::apply_if<
            detail::iterator_writability_disabled<ValueParam,Reference>
          , add_pointer<typename add_const<value_type>::type>
          , add_pointer<value_type>
        >::type pointer;
    };
    template <class T>
    struct operator_arrow_proxy
    {
        operator_arrow_proxy(T const* px) : m_value(*px) {}
        const T* operator->() const { return &m_value; }
        operator const T*() const { return &m_value; }
        T m_value;
    };
    template <class Value, class Reference, class Pointer>
    struct operator_arrow_result
    {
        typedef typename mpl::if_<
            is_reference<Reference>
          , Pointer
          , operator_arrow_proxy<Value>
        >::type type;
        static type make(Reference x)
        {
            return type(&x);
        }
    };
    template <class Iterator>
    class operator_brackets_proxy
    {
        typedef typename Iterator::reference reference;
        typedef typename Iterator::value_type value_type;
     public:
        operator_brackets_proxy(Iterator const& iter)
          : m_iter(iter)
        {}
        operator reference() const
        {
            return *m_iter;
        }
        operator_brackets_proxy& operator=(value_type const& val)
        {
            *m_iter = val;
            return *this;
        }
     private:
        Iterator m_iter;
    };
    template <class Value, class Reference>
    struct use_operator_brackets_proxy
      : mpl::and_<
            boost::is_POD<Value>
          , iterator_writability_disabled<Value,Reference>
        >
    {};
    template <class Iterator, class Value, class Reference>
    struct operator_brackets_result
    {
        typedef typename mpl::if_<
            use_operator_brackets_proxy<Value,Reference>
          , Value
          , operator_brackets_proxy<Iterator>
        >::type type;
    };
    template <class Iterator>
    operator_brackets_proxy<Iterator> make_operator_brackets_result(Iterator const& iter, mpl::false_)
    {
        return operator_brackets_proxy<Iterator>(iter);
    }
    template <class Iterator>
    typename Iterator::value_type make_operator_brackets_result(Iterator const& iter, mpl::true_)
    {
      return *iter;
    }
  }
  class iterator_core_access
  {
      template <class I, class V, class TC, class R, class D> friend class iterator_facade;
      template < class Derived1, class V1, class TC1, class R1, class D1 , class Derived2, class V2, class TC2, class R2, class D2 > friend typename detail::enable_if_interoperable< Derived1, Derived2, bool >::type operator ==( iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs);
      template < class Derived1, class V1, class TC1, class R1, class D1 , class Derived2, class V2, class TC2, class R2, class D2 > friend typename detail::enable_if_interoperable< Derived1, Derived2, bool >::type operator !=( iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs);
      template < class Derived1, class V1, class TC1, class R1, class D1 , class Derived2, class V2, class TC2, class R2, class D2 > friend typename detail::enable_if_interoperable< Derived1, Derived2, bool >::type operator <( iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs);
      template < class Derived1, class V1, class TC1, class R1, class D1 , class Derived2, class V2, class TC2, class R2, class D2 > friend typename detail::enable_if_interoperable< Derived1, Derived2, bool >::type operator >( iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs);
      template < class Derived1, class V1, class TC1, class R1, class D1 , class Derived2, class V2, class TC2, class R2, class D2 > friend typename detail::enable_if_interoperable< Derived1, Derived2, bool >::type operator <=( iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs);
      template < class Derived1, class V1, class TC1, class R1, class D1 , class Derived2, class V2, class TC2, class R2, class D2 > friend typename detail::enable_if_interoperable< Derived1, Derived2, bool >::type operator >=( iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs);
      template < class Derived1, class V1, class TC1, class R1, class D1 , class Derived2, class V2, class TC2, class R2, class D2 > friend typename detail::enable_if_interoperable< Derived1, Derived2, typename Derived1::difference_type >::type operator -( iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs)
      ;
      template <class Derived, class V, class TC, class R, class D> friend Derived operator+ (iterator_facade<Derived, V, TC, R, D> const& , typename Derived::difference_type)
      ;
      template <class Derived, class V, class TC, class R, class D> friend Derived operator+ (typename Derived::difference_type , iterator_facade<Derived, V, TC, R, D> const&)
      ;
      template <class Facade>
      static typename Facade::reference dereference(Facade const& f)
      {
          return f.dereference();
      }
      template <class Facade>
      static void increment(Facade& f)
      {
          f.increment();
      }
      template <class Facade>
      static void decrement(Facade& f)
      {
          f.decrement();
      }
      template <class Facade1, class Facade2>
      static bool equal(Facade1 const& f1, Facade2 const& f2)
      {
          return f1.equal(f2);
      }
      template <class Facade>
      static void advance(Facade& f, typename Facade::difference_type n)
      {
          f.advance(n);
      }
      template <class Facade1, class Facade2>
      static typename Facade1::difference_type distance_to(
                                                           Facade1 const& f1, Facade2 const& f2)
      {
          return f1.distance_to(f2);
      }
   private:
      iterator_core_access();
  };
  template <
      class Derived
    , class Value
    , class CategoryOrTraversal
    , class Reference = Value&
    , class Difference = std::ptrdiff_t
  >
  class iterator_facade
  {
   private:
      typedef Derived derived_t;
      Derived& derived()
      {
          return static_cast<Derived&>(*this);
      }
      Derived const& derived() const
      {
          return static_cast<Derived const&>(*this);
      }
      typedef detail::iterator_facade_types<
         Value, CategoryOrTraversal, Reference, Difference
      > associated_types;
   public:
      typedef typename associated_types::value_type value_type;
      typedef Reference reference;
      typedef Difference difference_type;
      typedef typename associated_types::pointer pointer;
      typedef typename associated_types::iterator_category iterator_category;
      reference operator*() const
      {
          return iterator_core_access::dereference(this->derived());
      }
      typename detail::operator_arrow_result<
          value_type
        , reference
        , pointer
      >::type
      operator->() const
      {
          return detail::operator_arrow_result<
              value_type
            , reference
            , pointer
          >::make(*this->derived());
      }
      typename detail::operator_brackets_result<Derived,Value,Reference>::type
      operator[](difference_type n) const
      {
          typedef detail::use_operator_brackets_proxy<Value,Reference> use_proxy;
          return detail::make_operator_brackets_result<Derived>(
              this->derived() + n
            , use_proxy()
          );
      }
      Derived& operator++()
      {
          iterator_core_access::increment(this->derived());
          return this->derived();
      }
      Derived operator++(int)
      {
          Derived tmp(this->derived());
          ++*this;
          return tmp;
      }
      Derived& operator--()
      {
          iterator_core_access::decrement(this->derived());
          return this->derived();
      }
      Derived operator--(int)
      {
          Derived tmp(this->derived());
          --*this;
          return tmp;
      }
      Derived& operator+=(difference_type n)
      {
          iterator_core_access::advance(this->derived(), n);
          return this->derived();
      }
      Derived& operator-=(difference_type n)
      {
          iterator_core_access::advance(this->derived(), -n);
          return this->derived();
      }
      Derived operator-(difference_type x) const
      {
          Derived result(this->derived());
          return result -= x;
      }
  };
  template < class Derived1, class V1, class TC1, class R1, class D1 , class Derived2, class V2, class TC2, class R2, class D2 > inline typename detail::enable_if_interoperable< Derived1, Derived2, bool >::type operator ==( iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs) { typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ( is_interoperable< Derived1, Derived2 >::value && true ) ) >)> boost_static_assert_typedef_555; return iterator_core_access::equal( static_cast<Derived2 const&>(rhs), static_cast<Derived1 const&>(lhs)); }
  template < class Derived1, class V1, class TC1, class R1, class D1 , class Derived2, class V2, class TC2, class R2, class D2 > inline typename detail::enable_if_interoperable< Derived1, Derived2, bool >::type operator !=( iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs) { typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ( is_interoperable< Derived1, Derived2 >::value && true ) ) >)> boost_static_assert_typedef_556; return ! iterator_core_access::equal( static_cast<Derived2 const&>(rhs), static_cast<Derived1 const&>(lhs)); }
  template < class Derived1, class V1, class TC1, class R1, class D1 , class Derived2, class V2, class TC2, class R2, class D2 > inline typename detail::enable_if_interoperable< Derived1, Derived2, bool >::type operator <( iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs) { typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ( is_interoperable< Derived1, Derived2 >::value && true ) ) >)> boost_static_assert_typedef_558; return 0 > iterator_core_access::distance_to( static_cast<Derived2 const&>(rhs), static_cast<Derived1 const&>(lhs)); }
  template < class Derived1, class V1, class TC1, class R1, class D1 , class Derived2, class V2, class TC2, class R2, class D2 > inline typename detail::enable_if_interoperable< Derived1, Derived2, bool >::type operator >( iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs) { typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ( is_interoperable< Derived1, Derived2 >::value && true ) ) >)> boost_static_assert_typedef_559; return 0 < iterator_core_access::distance_to( static_cast<Derived2 const&>(rhs), static_cast<Derived1 const&>(lhs)); }
  template < class Derived1, class V1, class TC1, class R1, class D1 , class Derived2, class V2, class TC2, class R2, class D2 > inline typename detail::enable_if_interoperable< Derived1, Derived2, bool >::type operator <=( iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs) { typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ( is_interoperable< Derived1, Derived2 >::value && true ) ) >)> boost_static_assert_typedef_560; return 0 >= iterator_core_access::distance_to( static_cast<Derived2 const&>(rhs), static_cast<Derived1 const&>(lhs)); }
  template < class Derived1, class V1, class TC1, class R1, class D1 , class Derived2, class V2, class TC2, class R2, class D2 > inline typename detail::enable_if_interoperable< Derived1, Derived2, bool >::type operator >=( iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs) { typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ( is_interoperable< Derived1, Derived2 >::value && true ) ) >)> boost_static_assert_typedef_561; return 0 <= iterator_core_access::distance_to( static_cast<Derived2 const&>(rhs), static_cast<Derived1 const&>(lhs)); }
  template < class Derived1, class V1, class TC1, class R1, class D1 , class Derived2, class V2, class TC2, class R2, class D2 > inline typename detail::enable_if_interoperable< Derived1, Derived2, typename Derived1::difference_type >::type operator -( iterator_facade<Derived1, V1, TC1, R1, D1> const& lhs , iterator_facade<Derived2, V2, TC2, R2, D2> const& rhs) { typedef ::boost::static_assert_test< sizeof(::boost::STATIC_ASSERTION_FAILURE< (bool)( ( is_interoperable< Derived1, Derived2 >::value && (is_same< typename Derived1::difference_type , typename Derived2::difference_type >::value) ) ) >)> boost_static_assert_typedef_573; return iterator_core_access::distance_to( static_cast<Derived2 const&>(rhs), static_cast<Derived1 const&>(lhs)); }
template <class Derived, class V, class TC, class R, class D> inline Derived operator+ ( iterator_facade<Derived, V, TC, R, D> const& i , typename Derived::difference_type n ) { Derived tmp(static_cast<Derived const&>(i)); return tmp += n; }
template <class Derived, class V, class TC, class R, class D> inline Derived operator+ ( typename Derived::difference_type n , iterator_facade<Derived, V, TC, R, D> const& i ) { Derived tmp(static_cast<Derived const&>(i)); return tmp += n; }
}
namespace boost {
template <class Iterator>
struct iterator_value
{
    typedef typename detail::iterator_traits<Iterator>::value_type type;
};
template <class Iterator>
struct iterator_reference
{
    typedef typename detail::iterator_traits<Iterator>::reference type;
};
template <class Iterator>
struct iterator_pointer
{
    typedef typename detail::iterator_traits<Iterator>::pointer type;
};
template <class Iterator>
struct iterator_difference
{
    typedef typename detail::iterator_traits<Iterator>::difference_type type;
};
template <class Iterator>
struct iterator_category
{
    typedef typename detail::iterator_traits<Iterator>::iterator_category type;
};
}
namespace boost
{
  struct use_default;
  template<class To>
  struct is_convertible<use_default,To>
    : mpl::false_ {};
  namespace detail
  {
    struct enable_type;
  }
  template<typename From, typename To>
  struct enable_if_convertible
    : iterators::enable_if<
          is_convertible<From, To>
        , detail::enable_type
      >
  {};
  namespace detail
  {
    template <class T, class DefaultNullaryFn>
    struct ia_dflt_help
      : mpl::apply_if<
            is_same<T, use_default>
          , DefaultNullaryFn
          , mpl::identity<T>
        >
    {
    };
    template <
        class Derived
      , class Base
      , class Value
      , class Traversal
      , class Reference
      , class Difference
    >
    struct iterator_adaptor_base
    {
        typedef iterator_facade<
            Derived
          , typename detail::ia_dflt_help<
                Value, iterator_value<Base>
            >::type
          , typename detail::ia_dflt_help<
                Traversal
              , iterator_traversal<Base>
            >::type
          , typename detail::ia_dflt_help<
                Reference
              , mpl::apply_if<
                    is_same<Value,use_default>
                  , iterator_reference<Base>
                  , add_reference<Value>
                >
            >::type
          , typename detail::ia_dflt_help<
                Difference, iterator_difference<Base>
            >::type
        >
        type;
    };
    template <class T> int static_assert_convertible_to(T);
  }
  template <
      class Derived
    , class Base
    , class Value = use_default
    , class Traversal = use_default
    , class Reference = use_default
    , class Difference = use_default
  >
  class iterator_adaptor
    : public detail::iterator_adaptor_base<
        Derived, Base, Value, Traversal, Reference, Difference
      >::type
  {
      friend class iterator_core_access;
      typedef typename detail::iterator_adaptor_base<
          Derived, Base, Value, Traversal, Reference, Difference
      >::type super_t;
   public:
      iterator_adaptor() {}
      explicit iterator_adaptor(Base const &iter)
          : m_iterator(iter)
      {
      }
      Base base() const
        { return m_iterator; }
   protected:
      Base const& base_reference() const
        { return m_iterator; }
      Base& base_reference()
        { return m_iterator; }
   private:
      typename super_t::reference dereference() const
        { return *m_iterator; }
      template <
      class OtherDerived, class OtherIterator, class V, class C, class R, class D
      >
      bool equal(iterator_adaptor<OtherDerived, OtherIterator, V, C, R, D> const& x) const
      {
          return m_iterator == x.base();
      }
      typedef typename iterator_category_to_traversal<
          typename super_t::iterator_category
      >::type my_traversal;
      void advance(typename super_t::difference_type n)
      {
          typedef int assertion[sizeof(detail::static_assert_convertible_to<random_access_traversal_tag>(my_traversal()))];
          m_iterator += n;
      }
      void increment() { ++m_iterator; }
      void decrement()
      {
          typedef int assertion[sizeof(detail::static_assert_convertible_to<bidirectional_traversal_tag>(my_traversal()))];
           --m_iterator;
      }
      template <
          class OtherDerived, class OtherIterator, class V, class C, class R, class D
      >
      typename super_t::difference_type distance_to(
          iterator_adaptor<OtherDerived, OtherIterator, V, C, R, D> const& y) const
      {
          typedef int assertion[sizeof(detail::static_assert_convertible_to<random_access_traversal_tag>(my_traversal()))];
          return y.base() - m_iterator;
      }
   private:
      Base m_iterator;
  };
}
namespace boost { namespace spirit {
namespace impl {
template <typename QueueT, typename T, typename PointerT>
class fsq_iterator
: public boost::iterator_adaptor<
        fsq_iterator<QueueT, T, PointerT>,
        PointerT,
        T,
        std::random_access_iterator_tag
    >
{
public:
    typedef typename QueueT::position_t position;
    typedef boost::iterator_adaptor<
            fsq_iterator<QueueT, T, PointerT>, PointerT, T,
            std::random_access_iterator_tag
        > base_t;
    fsq_iterator() {}
    fsq_iterator(position const &p_) : p(p_) {}
    position const &get_position() const { return p; }
private:
    friend class boost::iterator_core_access;
    typename base_t::reference dereference() const
    {
        return p.self->m_queue[p.pos];
    }
    void increment()
    {
        ++p.pos;
        if (p.pos == QueueT::MAX_SIZE+1)
            p.pos = 0;
    }
    void decrement()
    {
        if (p.pos == 0)
            p.pos = QueueT::MAX_SIZE;
        else
            --p.pos;
    }
    template <
        typename OtherDerivedT, typename OtherIteratorT,
        typename V, typename C, typename R, typename D
    >
    bool equal(iterator_adaptor<OtherDerivedT, OtherIteratorT, V, C, R, D>
        const &x) const
    {
        position const &rhs_pos =
            static_cast<OtherDerivedT const &>(x).get_position();
        return (p.self == rhs_pos.self) && (p.pos == rhs_pos.pos);
    }
    template <
        typename OtherDerivedT, typename OtherIteratorT,
        typename V, typename C, typename R, typename D
    >
    typename base_t::difference_type distance_to(
        iterator_adaptor<OtherDerivedT, OtherIteratorT, V, C, R, D>
        const &x) const
    {
        typedef typename base_t::difference_type diff_t;
        position const &p2 =
            static_cast<OtherDerivedT const &>(x).get_position();
        std::size_t pos1 = p.pos;
        std::size_t pos2 = p2.pos;
        ;
        if (pos1 < p.self->m_head)
            pos1 += QueueT::MAX_SIZE;
        if (pos2 < p2.self->m_head)
            pos2 += QueueT::MAX_SIZE;
        if (pos2 > pos1)
            return diff_t(pos2 - pos1);
        else
            return -diff_t(pos1 - pos2);
    }
    void advance(typename base_t::difference_type n)
    {
        if (n < 0)
        {
            n = -n;
            if (p.pos < (std::size_t)n)
                p.pos = QueueT::MAX_SIZE+1 - (n - p.pos);
            else
                p.pos -= n;
        }
        else
        {
            p.pos += n;
            if (p.pos >= QueueT::MAX_SIZE+1)
                p.pos -= QueueT::MAX_SIZE+1;
        }
    }
private:
    position p;
};
}
template <typename T, std::size_t N>
class fixed_size_queue
{
private:
    struct position
    {
        fixed_size_queue* self;
        std::size_t pos;
        position() : self(0), pos(0) {}
        position(const fixed_size_queue* s, std::size_t p)
            : self(const_cast<fixed_size_queue*>(s)), pos(p)
        {}
    };
public:
    typedef impl::fsq_iterator<fixed_size_queue<T, N>, T, T*> iterator;
    typedef impl::fsq_iterator<fixed_size_queue<T, N>, T const, T const*>
        const_iterator;
    typedef position position_t;
    friend class impl::fsq_iterator<fixed_size_queue<T, N>, T, T*>;
    friend class impl::fsq_iterator<fixed_size_queue<T, N>, T const, T const*>;
    fixed_size_queue();
    fixed_size_queue(const fixed_size_queue& x);
    fixed_size_queue& operator=(const fixed_size_queue& x);
    ~fixed_size_queue();
    void push_back(const T& e);
    void push_front(const T& e);
    void serve(T& e);
    void pop_front();
    bool empty() const
    {
        return m_size == 0;
    }
    bool full() const
    {
        return m_size == N;
    }
    iterator begin()
    {
        return iterator(position(this, m_head));
    }
    const_iterator begin() const
    {
        return const_iterator(position(this, m_head));
    }
    iterator end()
    {
        return iterator(position(this, m_tail));
    }
    const_iterator end() const
    {
        return const_iterator(position(this, m_tail));
    }
    std::size_t size() const
    {
        return m_size;
    }
    T& front()
    {
        return m_queue[m_head];
    }
    const T& front() const
    {
        return m_queue[m_head];
    }
private:
    static const std::size_t MAX_SIZE = N;
    std::size_t m_head;
    std::size_t m_tail;
    std::size_t m_size;
    T m_queue[N+1];
};
template <typename T, std::size_t N>
inline
fixed_size_queue<T, N>::fixed_size_queue()
    : m_head(0)
    , m_tail(0)
    , m_size(0)
{
    ;
    ;
    ;
    ;
}
template <typename T, std::size_t N>
inline
fixed_size_queue<T, N>::fixed_size_queue(const fixed_size_queue& x)
    : m_head(x.m_head)
    , m_tail(x.m_tail)
    , m_size(x.m_size)
{
    copy(x.begin(), x.end(), begin());
    ;
    ;
    ;
    ;
}
template <typename T, std::size_t N>
inline fixed_size_queue<T, N>&
fixed_size_queue<T, N>::operator=(const fixed_size_queue& x)
{
    if (this != &x)
    {
        m_head = x.m_head;
        m_tail = x.m_tail;
        m_size = x.m_size;
        copy(x.begin(), x.end(), begin());
    }
    ;
    ;
    ;
    ;
    return *this;
}
template <typename T, std::size_t N>
inline
fixed_size_queue<T, N>::~fixed_size_queue()
{
    ;
    ;
    ;
    ;
}
template <typename T, std::size_t N>
inline void
fixed_size_queue<T, N>::push_back(const T& e)
{
    ;
    ;
    ;
    ;
    ;
    m_queue[m_tail] = e;
    ++m_size;
    ++m_tail;
    if (m_tail == N+1)
        m_tail = 0;
    ;
    ;
    ;
    ;
}
template <typename T, std::size_t N>
inline void
fixed_size_queue<T, N>::push_front(const T& e)
{
    ;
    ;
    ;
    ;
    ;
    if (m_head == 0)
        m_head = N;
    else
        --m_head;
    m_queue[m_head] = e;
    ++m_size;
    ;
    ;
    ;
    ;
}
template <typename T, std::size_t N>
inline void
fixed_size_queue<T, N>::serve(T& e)
{
    ;
    ;
    ;
    ;
    e = m_queue[m_head];
    pop_front();
}
template <typename T, std::size_t N>
inline void
fixed_size_queue<T, N>::pop_front()
{
    ;
    ;
    ;
    ;
    ++m_head;
    if (m_head == N+1)
        m_head = 0;
    --m_size;
    ;
    ;
    ;
    ;
}
}}
namespace boost { namespace spirit {
namespace impl {
    template <typename T>
    inline void mp_swap(T& t1, T& t2);
}
namespace multi_pass_policies
{
class ref_counted
{
    protected:
        ref_counted()
            : count(new std::size_t(1))
        {}
        ref_counted(ref_counted const& x)
            : count(x.count)
        {}
        void clone()
        {
            ++*count;
        }
        bool release()
        {
            if (!--*count)
            {
                delete count;
                count = 0;
                return true;
            }
            return false;
        }
        void swap(ref_counted& x)
        {
            impl::mp_swap(count, x.count);
        }
    public:
        bool unique() const
        {
            return *count == 1;
        }
    private:
        std::size_t* count;
};
class first_owner
{
    protected:
        first_owner()
            : first(true)
        {}
        first_owner(first_owner const&)
            : first(false)
        {}
        void clone()
        {
        }
        bool release()
        {
            return first;
        }
        void swap(first_owner&)
        {
        }
    public:
        bool unique() const
        {
            return false;
        }
    private:
        bool first;
};
class illegal_backtracking : public std::exception
{
public:
    illegal_backtracking() throw() {}
    ~illegal_backtracking() throw() {}
    virtual const char*
    what() const throw()
    { return "boost::spirit::illegal_backtracking"; }
};
class buf_id_check
{
    protected:
        buf_id_check()
            : shared_buf_id(new unsigned long(0))
            , buf_id(0)
        {}
        buf_id_check(buf_id_check const& x)
            : shared_buf_id(x.shared_buf_id)
            , buf_id(x.buf_id)
        {}
        void destroy()
        {
            delete shared_buf_id;
            shared_buf_id = 0;
        }
        void swap(buf_id_check& x)
        {
            impl::mp_swap(shared_buf_id, x.shared_buf_id);
            impl::mp_swap(buf_id, x.buf_id);
        }
        void check() const
        {
            if (buf_id != *shared_buf_id)
            {
                boost::throw_exception(illegal_backtracking());
            }
        }
        void clear_queue()
        {
            ++*shared_buf_id;
            ++buf_id;
        }
    private:
        unsigned long* shared_buf_id;
        unsigned long buf_id;
};
class no_check
{
    protected:
        no_check() {}
        no_check(no_check const&) {}
        void destroy() {}
        void swap(no_check&) {}
        void check() const {}
        void clear_queue() {}
};
class std_deque
{
    public:
template <typename ValueT>
class inner
{
    private:
        typedef std::deque<ValueT> queue_type;
        queue_type* queuedElements;
        mutable typename queue_type::size_type queuePosition;
    protected:
        inner()
            : queuedElements(new queue_type)
            , queuePosition(0)
        {}
        inner(inner const& x)
            : queuedElements(x.queuedElements)
            , queuePosition(x.queuePosition)
        {}
        void destroy()
        {
            ;
            delete queuedElements;
            queuedElements = 0;
        }
        void swap(inner& x)
        {
            impl::mp_swap(queuedElements, x.queuedElements);
            impl::mp_swap(queuePosition, x.queuePosition);
        }
        template <typename MultiPassT>
        static typename MultiPassT::reference dereference(MultiPassT const& mp)
        {
            if (mp.queuePosition == mp.queuedElements->size())
            {
                if (mp.unique())
                {
                    if (mp.queuedElements->size() > 0)
                    {
                        mp.queuedElements->clear();
                        mp.queuePosition = 0;
                    }
                }
                return mp.get_input();
            }
            else
            {
                return (*mp.queuedElements)[mp.queuePosition];
            }
        }
        template <typename MultiPassT>
        static void increment(MultiPassT& mp)
        {
            if (mp.queuePosition == mp.queuedElements->size())
            {
                if (mp.unique())
                {
                    if (mp.queuedElements->size() > 0)
                    {
                        mp.queuedElements->clear();
                        mp.queuePosition = 0;
                    }
                }
                else
                {
                    mp.queuedElements->push_back(mp.get_input());
                    ++mp.queuePosition;
                }
                mp.advance_input();
            }
            else
            {
                ++mp.queuePosition;
            }
        }
        void clear_queue()
        {
            queuedElements->clear();
            queuePosition = 0;
        }
        template <typename MultiPassT>
        static bool is_eof(MultiPassT const& mp)
        {
            return mp.queuePosition == mp.queuedElements->size() &&
                mp.input_at_eof();
        }
        bool equal_to(inner const& x) const
        {
            return queuePosition == x.queuePosition;
        }
        bool less_than(inner const& x) const
        {
            return queuePosition < x.queuePosition;
        }
};
};
template < std::size_t N>
class fixed_size_queue
{
    public:
template <typename ValueT>
class inner
{
    private:
        typedef boost::spirit::fixed_size_queue<ValueT, N> queue_type;
        queue_type * queuedElements;
        mutable typename queue_type::iterator queuePosition;
    protected:
        inner()
            : queuedElements(new queue_type)
            , queuePosition(queuedElements->begin())
        {}
        inner(inner const& x)
            : queuedElements(x.queuedElements)
            , queuePosition(x.queuePosition)
        {}
        void destroy()
        {
            ;
            delete queuedElements;
            queuedElements = 0;
        }
        void swap(inner& x)
        {
            impl::mp_swap(queuedElements, x.queuedElements);
            impl::mp_swap(queuePosition, x.queuePosition);
        }
        template <typename MultiPassT>
        static typename MultiPassT::reference dereference(MultiPassT const& mp)
        {
            if (mp.queuePosition == mp.queuedElements->end())
            {
                return mp.get_input();
            }
            else
            {
                return *mp.queuePosition;
            }
        }
        template <typename MultiPassT>
        static void increment(MultiPassT& mp)
        {
            if (mp.queuePosition == mp.queuedElements->end())
            {
                if (mp.queuedElements->size() >= N)
                    mp.queuedElements->pop_front();
                mp.queuedElements->push_back(mp.get_input());
                mp.advance_input();
            }
            ++mp.queuePosition;
        }
        void clear_queue()
        {}
        template <typename MultiPassT>
        static bool is_eof(MultiPassT const& mp)
        {
            return mp.queuePosition == mp.queuedElements->end() &&
                mp.input_at_eof();
        }
        bool equal_to(inner const& x) const
        {
            return queuePosition == x.queuePosition;
        }
        bool less_than(inner const& x) const
        {
            return queuePosition < x.queuePosition;
        }
};
};
class input_iterator
{
    public:
template <typename InputT>
class inner
{
    public:
        typedef
            typename boost::detail::iterator_traits<InputT>::value_type
            value_type;
        typedef
            typename boost::detail::iterator_traits<InputT>::difference_type
            difference_type;
        typedef
            typename boost::detail::iterator_traits<InputT>::pointer
            pointer;
        typedef
            typename boost::detail::iterator_traits<InputT>::reference
            reference;
    protected:
        inner()
            : input(new InputT())
            , val(new value_type)
        {}
        inner(InputT x)
            : input(new InputT(x))
            , val(new value_type(**input))
        {}
        inner(inner const& x)
            : input(x.input)
            , val(x.val)
        {}
        void destroy()
        {
            delete input;
            input = 0;
            delete val;
            val = 0;
        }
        bool same_input(inner const& x) const
        {
            return input == x.input;
        }
        typedef
            typename boost::detail::iterator_traits<InputT>::value_type
            value_t;
        void swap(inner& x)
        {
            impl::mp_swap(input, x.input);
            impl::mp_swap(val, x.val);
        }
    public:
        reference get_input() const
        {
            return *val;
        }
        void advance_input()
        {
            ++*input;
            *val = **input;
        }
        bool input_at_eof() const
        {
            return *input == InputT();
        }
    private:
        InputT* input;
        value_type* val;
};
};
class lex_input
{
    public:
template <typename InputT>
class inner
{
    public:
        typedef int value_type;
    typedef std::ptrdiff_t difference_type;
        typedef int* pointer;
        typedef int& reference;
    protected:
        inner()
            : curtok(new int(0))
        {}
        inner(InputT x)
            : curtok(new int(x))
        {}
        inner(inner const& x)
            : curtok(x.curtok)
        {}
        void destroy()
        {
            delete curtok;
            curtok = 0;
        }
        bool same_input(inner const& x) const
        {
            return curtok == x.curtok;
        }
        void swap(inner& x)
        {
            impl::mp_swap(curtok, x.curtok);
        }
    public:
        reference get_input() const
        {
            return *curtok;
        }
        void advance_input()
        {
            extern int yylex();
            *curtok = yylex();
        }
        bool input_at_eof() const
        {
            return *curtok == 0;
        }
    private:
        int* curtok;
};
};
class functor_input
{
    public:
template <typename FunctorT>
class inner
{
    typedef typename FunctorT::result_type result_type;
    public:
        typedef result_type value_type;
    typedef std::ptrdiff_t difference_type;
        typedef result_type* pointer;
        typedef result_type& reference;
    protected:
        inner()
            : ftor(0)
            , curtok(0)
        {}
        inner(FunctorT const& x)
            : ftor(new FunctorT(x))
            , curtok(new result_type((*ftor)()))
        {}
        inner(inner const& x)
            : ftor(x.ftor)
            , curtok(x.curtok)
        {}
        void destroy()
        {
            delete ftor;
            ftor = 0;
            delete curtok;
            curtok = 0;
        }
        bool same_input(inner const& x) const
        {
            return ftor == x.ftor;
        }
        void swap(inner& x)
        {
            impl::mp_swap(curtok, x.curtok);
            impl::mp_swap(ftor, x.ftor);
        }
    public:
        reference get_input() const
        {
            return *curtok;
        }
        void advance_input()
        {
            if (curtok) {
                *curtok = (*ftor)();
            }
        }
        bool input_at_eof() const
        {
            return !curtok || *curtok == ftor->eof;
        }
        FunctorT& get_functor() const
        {
            return *ftor;
        }
    private:
        FunctorT* ftor;
        result_type* curtok;
};
};
}
namespace iterator_ { namespace impl {
template <typename InputPolicyT, typename InputT>
struct iterator_base_creator
{
    typedef typename InputPolicyT::template inner<InputT> input_t;
    typedef boost::iterator
    <
        std::forward_iterator_tag,
        typename input_t::value_type,
        typename input_t::difference_type,
        typename input_t::pointer,
        typename input_t::reference
    > type;
};
}}
template
<
    typename InputT,
    typename InputPolicy = multi_pass_policies::input_iterator,
    typename OwnershipPolicy = multi_pass_policies::ref_counted,
    typename CheckingPolicy = multi_pass_policies::buf_id_check,
    typename StoragePolicy = multi_pass_policies::std_deque
>
class multi_pass;
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
class multi_pass
    : public OwnershipPolicy
    , public CheckingPolicy
    , public StoragePolicy::template inner<
                typename InputPolicy::template inner<InputT>::value_type>
    , public InputPolicy::template inner<InputT>
    , public iterator_::impl::iterator_base_creator<InputPolicy, InputT>::type
{
        typedef OwnershipPolicy OP;
        typedef CheckingPolicy CHP;
        typedef typename StoragePolicy::template inner<
            typename InputPolicy::template inner<InputT>::value_type> SP;
        typedef typename InputPolicy::template inner<InputT> IP;
        typedef typename
            iterator_::impl::iterator_base_creator<InputPolicy, InputT>::type
            IB;
    public:
        typedef typename IB::value_type value_type;
        typedef typename IB::difference_type difference_type;
        typedef typename IB::reference reference;
        typedef typename IB::pointer pointer;
        typedef InputT iterator_type;
        multi_pass();
        explicit multi_pass(InputT input);
        ~multi_pass();
        multi_pass(multi_pass const&);
        multi_pass& operator=(multi_pass const&);
        void swap(multi_pass& x);
        reference operator*() const;
        pointer operator->() const;
        multi_pass& operator++();
        multi_pass operator++(int);
        void clear_queue();
        bool operator==(const multi_pass& y) const;
        bool operator<(const multi_pass& y) const;
    private:
        bool is_eof() const;
};
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>::
multi_pass()
    : OP()
    , CHP()
    , SP()
    , IP()
{
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>::
multi_pass(InputT input)
    : OP()
    , CHP()
    , SP()
    , IP(input)
{
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>::
~multi_pass()
{
    if (OP::release())
    {
        CHP::destroy();
        SP::destroy();
        IP::destroy();
    }
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>::
multi_pass(
        multi_pass const& x)
    : OP(x)
    , CHP(x)
    , SP(x)
    , IP(x)
{
    OP::clone();
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>&
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>::
operator=(
        multi_pass const& x)
{
    multi_pass temp(x);
    temp.swap(*this);
    return *this;
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline void
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>::
swap(multi_pass& x)
{
    OP::swap(x);
    CHP::swap(x);
    SP::swap(x);
    IP::swap(x);
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline
typename multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>::
reference
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>::
operator*() const
{
    CHP::check();
    return SP::dereference(*this);
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline
typename multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>::
pointer
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>::
operator->() const
{
    return &(operator*());
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>&
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>::
operator++()
{
    CHP::check();
    SP::increment(*this);
    return *this;
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>::
operator++(int)
{
    multi_pass
    <
        InputT,
        InputPolicy,
        OwnershipPolicy,
        CheckingPolicy,
        StoragePolicy
    > tmp(*this);
    ++*this;
    return tmp;
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline void
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>::
clear_queue()
{
    SP::clear_queue();
    CHP::clear_queue();
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline bool
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>::
is_eof() const
{
    return SP::is_eof(*this);
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline bool
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>::
operator==(const multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy,
        StoragePolicy>& y) const
{
    if (is_eof() && y.is_eof())
    {
        return true;
    }
    else if (is_eof() ^ y.is_eof())
    {
        return false;
    }
    else if (!IP::same_input(y))
    {
        return false;
    }
    else
    {
        return SP::equal_to(y);
    }
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline bool
multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy>::
operator<(const multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy,
        StoragePolicy>& y) const
{
    return SP::less_than(y);
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline
bool operator!=(
        const multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy,
                        StoragePolicy>& x,
        const multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy,
                        StoragePolicy>& y)
{
    return !(x == y);
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline
bool operator>(
        const multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy,
                        StoragePolicy>& x,
        const multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy,
                        StoragePolicy>& y)
{
    return y < x;
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline
bool operator>=(
        const multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy,
                        StoragePolicy>& x,
        const multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy,
                        StoragePolicy>& y)
{
    return !(x < y);
}
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
inline
bool operator<=(
        const multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy,
                        StoragePolicy>& x,
        const multi_pass<InputT, InputPolicy, OwnershipPolicy, CheckingPolicy,
                        StoragePolicy>& y)
{
    return !(y < x);
}
template <typename InputT>
inline multi_pass<InputT>
make_multi_pass(InputT i)
{
    return multi_pass<InputT>(i);
}
template <typename InputT, std::size_t N>
class look_ahead :
    public multi_pass<
        InputT,
        multi_pass_policies::input_iterator,
        multi_pass_policies::first_owner,
        multi_pass_policies::no_check,
        multi_pass_policies::fixed_size_queue<N> >
{
        typedef multi_pass<
            InputT,
            multi_pass_policies::input_iterator,
            multi_pass_policies::first_owner,
            multi_pass_policies::no_check,
            multi_pass_policies::fixed_size_queue<N> > base_t;
    public:
        look_ahead()
            : base_t() {}
        explicit look_ahead(InputT x)
            : base_t(x) {}
        look_ahead(look_ahead const& x)
            : base_t(x) {}
};
template
<
    typename InputT,
    typename InputPolicy,
    typename OwnershipPolicy,
    typename CheckingPolicy,
    typename StoragePolicy
>
void swap(
    multi_pass<
        InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy
    > &x,
    multi_pass<
        InputT, InputPolicy, OwnershipPolicy, CheckingPolicy, StoragePolicy
    > &y)
{
    x.swap(y);
}
namespace impl {
    template <typename T>
    inline void mp_swap(T& t1, T& t2)
    {
        using std::swap;
        using boost::spirit::swap;
        swap(t1, t2);
    }
}
}}
namespace boost { namespace spirit {
    namespace impl {
        template <typename T>
        void flush_iterator(T &) {}
        template <typename T1, typename T2, typename T3, typename T4>
        void flush_iterator(boost::spirit::multi_pass<
            T1, T2, T3, T4, boost::spirit::multi_pass_policies::std_deque> &i)
        {
            i.clear_queue();
        }
    }
    class flush_multi_pass_parser
    : public parser<flush_multi_pass_parser>
    {
    public:
        typedef flush_multi_pass_parser this_t;
        template <typename ScannerT>
        typename parser_result<this_t, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            impl::flush_iterator(scan.first);
            return scan.empty_match();
        }
    };
    flush_multi_pass_parser const
        flush_multi_pass_p = flush_multi_pass_parser();
}}
namespace boost { namespace spirit {
    template <typename MutexT, typename ParserT>
    struct scoped_lock_parser
        : public unary< ParserT, parser< scoped_lock_parser<MutexT, ParserT> > >
    {
        typedef scoped_lock_parser<MutexT, ParserT> self_t;
        typedef MutexT mutex_t;
        typedef ParserT parser_t;
        template <typename ScannerT>
        struct result
        {
            typedef typename parser_result<parser_t, ScannerT>::type type;
        };
        scoped_lock_parser(mutex_t &m, parser_t const &p)
            : unary< ParserT, parser< scoped_lock_parser<MutexT, ParserT> > >(p)
            , mutex(m)
        {}
        template <typename ScannerT>
        typename parser_result<self_t, ScannerT>::type
        parse(ScannerT const &scan) const
        {
            typedef typename mutex_t::scoped_lock scoped_lock_t;
            scoped_lock_t lock(mutex);
            return this->subject().parse(scan);
        }
        mutex_t &mutex;
    };
    template <typename MutexT>
    struct scoped_lock_parser_gen
    {
        typedef MutexT mutex_t;
        explicit scoped_lock_parser_gen(mutex_t &m) : mutex(m) {}
        template<typename ParserT>
        scoped_lock_parser
        <
            MutexT,
            typename as_parser<ParserT>::type
        >
        operator[](ParserT const &p) const
        {
            typedef ::boost::spirit::as_parser<ParserT> as_parser_t;
            typedef typename as_parser_t::type parser_t;
            return scoped_lock_parser<mutex_t, parser_t>
                (mutex, as_parser_t::convert(p));
        }
        mutex_t &mutex;
    };
    template <typename MutexT>
    scoped_lock_parser_gen<MutexT>
    scoped_lock_d(MutexT &mutex)
    {
        return scoped_lock_parser_gen<MutexT>(mutex);
    }
}}
namespace boost { namespace spirit {
namespace impl
{
    template <typename CharT, typename T>
    class tst;
}
template <typename T, typename SetT>
class symbol_inserter;
template
<
    typename T = int,
    typename CharT = char,
    typename SetT = impl::tst<T, CharT>
>
class symbols
: private SetT
, public parser<symbols<T, CharT, SetT> >
{
public:
    typedef parser<symbols<T, CharT, SetT> > parser_base_t;
    typedef symbols<T, CharT, SetT> self_t;
    typedef self_t const& embed_t;
    typedef T symbol_data_t;
    typedef boost::reference_wrapper<T> symbol_ref_t;
    symbols();
    symbols(symbols const& other);
    ~symbols();
    symbols&
    operator=(symbols const& other);
    symbol_inserter<T, SetT> const&
    operator=(CharT const* str);
    template <typename ScannerT>
    struct result
    {
        typedef typename match_result<ScannerT, symbol_ref_t>::type type;
    };
    template <typename ScannerT>
    typename parser_result<self_t, ScannerT>::type
    parse_main(ScannerT const& scan) const
    {
        typedef typename ScannerT::iterator_t iterator_t;
        iterator_t first = scan.first;
        typename SetT::search_info result = SetT::find(scan);
        if (result.data)
            return scan.
                create_match(
                    result.length,
                    symbol_ref_t(*result.data),
                    first,
                    scan.first);
        else
            return scan.no_match();
    }
    template <typename ScannerT>
    typename parser_result<self_t, ScannerT>::type
    parse(ScannerT const& scan) const
    {
        typedef typename parser_result<self_t, ScannerT>::type result_t;
        return impl::implicit_lexeme_parse<result_t>
            (*this, scan, scan);
    }
    template < typename ScannerT >
    T* find(ScannerT const& scan) const
    { return SetT::find(scan).data; }
    symbol_inserter<T, SetT> const add;
};
template <typename T, typename CharT, typename SetT>
T* add(symbols<T, CharT, SetT>& table, CharT const* sym, T const& data = T());
template <typename T, typename CharT, typename SetT>
T* find(symbols<T, CharT, SetT> const& table, CharT const* sym);
template <typename T, typename SetT>
class symbol_inserter
{
public:
    symbol_inserter(SetT& set_)
    : set(set_) {}
    template <typename IteratorT>
    symbol_inserter const&
    operator()(IteratorT first, IteratorT const& last, T const& data = T()) const
    {
        set.add(first, last, data);
        return *this;
    }
    template <typename CharT>
    symbol_inserter const&
    operator()(CharT const* str, T const& data = T()) const
    {
        CharT const* last = str;
        while (*last)
            last++;
        set.add(str, last, data);
        return *this;
    }
    template <typename CharT>
    symbol_inserter const&
    operator,(CharT const* str) const
    {
        CharT const* last = str;
        while (*last)
            last++;
        set.add(str, last, T());
        return *this;
    }
private:
    SetT& set;
};
}}
namespace boost { namespace spirit {
    namespace impl
    {
    template <typename T, typename CharT>
    struct tst_node
    {
        tst_node(CharT value_)
        : value(value_)
        , left(0)
        , right(0)
        { middle.link = 0; }
        ~tst_node()
        {
            delete left;
            delete right;
            if (value)
                delete middle.link;
            else
                delete middle.data;
        }
        tst_node*
        clone() const
        {
            std::auto_ptr<tst_node> copy(new tst_node(value));
            if (left)
                copy->left = left->clone();
            if (right)
                copy->right = right->clone();
            if (value && middle.link)
            {
                copy->middle.link = middle.link->clone();
            }
            else
            {
                std::auto_ptr<T> mid_data(new T(*middle.data));
                copy->middle.data = mid_data.release();
            }
            return copy.release();
        }
        union center {
            tst_node* link;
            T* data;
        };
        CharT value;
        tst_node* left;
        center middle;
        tst_node* right;
    };
    template <typename T, typename CharT>
    class tst
    {
    public:
        struct search_info
        {
            T* data;
            std::size_t length;
        };
        tst()
        : root(0) {}
        tst(tst const& other)
        : root(other.root ? other.root->clone() : 0) {}
        ~tst()
        { delete root; }
        tst&
        operator=(tst const& other)
        {
            if (this != &other)
            {
                node_t* new_root = other.root ? other.root->clone() : 0;
                delete root;
                root = new_root;
            }
            return *this;
        }
        template <typename IteratorT>
        T* add(IteratorT first, IteratorT const& last, T const& data)
        {
            if (first == last)
                return 0;
            node_t** np = &root;
            CharT ch = *first;
            for (;;)
            {
                if (*np == 0 || ch == 0)
                {
                    node_t* right = 0;
                    if (np != 0)
                        right = *np;
                    *np = new node_t(ch);
                    if (right)
                        (**np).right = right;
                }
                if (ch < (**np).value)
                {
                    np = &(**np).left;
                }
                else
                {
                    if (ch == (**np).value)
                    {
                        if (ch == 0)
                        {
                            if ((**np).middle.data == 0)
                            {
                                (**np).middle.data = new T(data);
                                return (**np).middle.data;
                            }
                            else
                            {
                                return 0;
                            }
                       }
                        ++first;
                        ch = (first == last) ? 0 : *first;
                        np = &(**np).middle.link;
                    }
                    else
                    {
                        np = &(**np).right;
                    }
                }
            }
        }
        template <typename ScannerT>
        search_info find(ScannerT const& scan) const
        {
            search_info result = { 0, 0 };
            if (scan.at_end()) {
                return result;
            }
            typedef typename ScannerT::iterator_t iterator_t;
            node_t* np = root;
            CharT ch = *scan;
            iterator_t save = scan.first;
            iterator_t latest = scan.first;
            std::size_t latest_len = 0;
            while (np)
            {
                if (ch < np->value)
                {
                    if (np->value == 0)
                    {
                        result.data = np->middle.data;
                        if (result.data)
                        {
                            latest = scan.first;
                            latest_len = result.length;
                        }
                    }
                    np = np->left;
                }
                else
                {
                    if (ch == np->value)
                    {
                        if (scan.at_end())
                        {
                            result.data = np->middle.data;
                            if (result.data)
                            {
                                latest = scan.first;
                                latest_len = result.length;
                            }
                            break;
                        }
                        ++scan;
                        ch = scan.at_end() ? 0 : *scan;
                        np = np->middle.link;
                        ++result.length;
                    }
                    else
                    {
                        if (np->value == 0)
                        {
                            result.data = np->middle.data;
                            if (result.data)
                            {
                                latest = scan.first;
                                latest_len = result.length;
                            }
                        }
                        np = np->right;
                     }
                }
            }
            if (result.data == 0)
            {
                scan.first = save;
            }
            else
            {
                scan.first = latest;
                result.length = latest_len;
            }
            return result;
        }
    private:
        typedef tst_node<T, CharT> node_t;
        node_t* root;
    };
    }
}}
namespace boost { namespace spirit {
template <typename T, typename CharT, typename SetT>
inline symbols<T, CharT, SetT>::symbols()
: SetT()
, add(*this)
{
}
template <typename T, typename CharT, typename SetT>
symbols<T, CharT, SetT>::symbols(symbols const& other)
: SetT(other)
, parser<symbols<T, CharT, SetT> >()
, add(*this)
{
}
template <typename T, typename CharT, typename SetT>
inline symbols<T, CharT, SetT>::~symbols()
{}
template <typename T, typename CharT, typename SetT>
inline symbols<T, CharT, SetT>&
symbols<T, CharT, SetT>::operator=(symbols const& other)
{
    SetT::operator=(other);
    return *this;
}
template <typename T, typename CharT, typename SetT>
inline symbol_inserter<T, SetT> const&
symbols<T, CharT, SetT>::operator=(CharT const* str)
{
    return add, str;
}
template <typename T, typename CharT, typename SetT>
inline T*
find(symbols<T, CharT, SetT> const& table, CharT const* sym)
{
    CharT const* last = sym;
    while (*last)
        last++;
    scanner<CharT const *> scan(sym, last);
    T* result = table.find(scan);
    return scan.at_end()? result: 0;
}
template <typename T, typename CharT, typename SetT>
inline T*
add(symbols<T, CharT, SetT>& table, CharT const* sym, T const& data)
{
    CharT const* last = sym;
    while (*last)
        last++;
    scanner<CharT const *> scan(sym, last);
    if (table.find(scan))
        return 0;
    table.add(sym, last, data);
    return table.find(scan);
}
}}
using namespace boost::spirit;
using namespace std;
struct skip_grammar : public grammar<skip_grammar>
{
    template <typename ScannerT>
    struct definition
    {
        definition(skip_grammar const& )
        {
            skip
                = space_p
                | "//" >> *(anychar_p - '\n') >> '\n'
                | "/*" >> *(anychar_p - "*/") >> "*/"
                | "#line" >> *(anychar_p - '\n') >> '\n'
                | "#pragma" >> *(anychar_p - '\n') >> '\n'
                ;
        }
        rule<ScannerT> skip;
        rule<ScannerT> const&
        start() const { return skip; }
    };
};
template<typename GrammarT>
bool
parse(GrammarT const& g, char const* text, unsigned int len)
{
    skip_grammar skip;
    parse_info<const char *> result =
        parse(text, text+len, g, skip);
    return !!result.full;
}
struct c_grammar : public grammar<c_grammar>
{
    template <typename ScannerT>
    struct definition
    {
        definition(c_grammar const& ) :
            ELLIPSIS("..."), RIGHT_ASSIGN(">>="), LEFT_ASSIGN("<<="),
            ADD_ASSIGN("+="), SUB_ASSIGN("-="), MUL_ASSIGN("*="),
            DIV_ASSIGN("/="), MOD_ASSIGN("%="), AND_ASSIGN("&="),
            XOR_ASSIGN("^="), OR_ASSIGN("|="), RIGHT_OP(">>"), LEFT_OP("<<"),
            INC_OP("++"), DEC_OP("--"), PTR_OP("->"), AND_OP("&&"),
            OR_OP("||"), LE_OP("<="), GE_OP(">="), EQ_OP("=="), NE_OP("!="),
            SEMICOLON(';'),
            COMMA(','), COLON(':'), ASSIGN('='), LEFT_PAREN('('),
            RIGHT_PAREN(')'), DOT('.'), ADDROF('&'), BANG('!'), TILDE('~'),
            MINUS('-'), PLUS('+'), STAR('*'), SLASH('/'), PERCENT('%'),
            LT_OP('<'), GT_OP('>'), XOR('^'), OR('|'), QUEST('?')
        {
            keywords =
                "auto", "break", "case", "char", "const", "continue", "default",
                "do", "double", "else", "enum", "extern", "float", "for",
                "goto", "if", "int", "long", "register", "return", "short",
                "signed", "sizeof", "static", "struct", "switch", "typedef",
                "union", "unsigned", "void", "volatile", "while";
            LEFT_BRACE = chlit<>('{') | strlit<>("<%");
            RIGHT_BRACE = chlit<>('}') | strlit<>("%>");
            LEFT_BRACKET = chlit<>('[') | strlit<>("<:");
            RIGHT_BRACKET = chlit<>(']') | strlit<>(":>");
            AUTO = strlit<>("auto");
            BREAK = strlit<>("break");
            CASE = strlit<>("case");
            CHAR = strlit<>("char");
            CONST = strlit<>("const");
            CONTINUE = strlit<>("continue");
            DEFAULT = strlit<>("default");
            DO = strlit<>("do");
            DOUBLE = strlit<>("double");
            ELSE = strlit<>("else");
            ENUM = strlit<>("enum");
            EXTERN = strlit<>("extern");
            FOR = strlit<>("for");
            FLOAT = strlit<>("float");
            GOTO = strlit<>("goto");
            IF = strlit<>("if");
            INT = strlit<>("int");
            LONG = strlit<>("long");
            REGISTER = strlit<>("register");
            RETURN = strlit<>("return");
            SHORT = strlit<>("short");
            SIGNED = strlit<>("signed");
            SIZEOF = strlit<>("sizeof");
            STATIC = strlit<>("static");
            STRUCT = strlit<>("struct");
            SWITCH = strlit<>("switch");
            TYPEDEF = strlit<>("typedef");
            UNION = strlit<>("union");
            UNSIGNED = strlit<>("unsigned");
            VOID = strlit<>("void");
            VOLATILE = strlit<>("volatile");
            WHILE = strlit<>("while");
            IDENTIFIER =
                lexeme_d[
                    ((alpha_p | '_' | '$') >> *(alnum_p | '_' | '$'))
                    - (keywords >> anychar_p - (alnum_p | '_' | '$'))
                ]
                ;
            STRING_LITERAL_PART =
                lexeme_d[
                    !chlit<>('L') >> chlit<>('\"') >>
                    *( strlit<>("\\\"") | anychar_p - chlit<>('\"') ) >>
                    chlit<>('\"')
                ]
                ;
            STRING_LITERAL = +STRING_LITERAL_PART;
            INT_CONSTANT_HEX
                = lexeme_d[
                    chlit<>('0')
                    >> as_lower_d[chlit<>('x')]
                    >> +xdigit_p
                    >> !as_lower_d[chlit<>('l') | chlit<>('u')]
                ]
                ;
            INT_CONSTANT_OCT
                = lexeme_d[
                    chlit<>('0')
                    >> +range<>('0', '7')
                    >> !as_lower_d[chlit<>('l') | chlit<>('u')]
                ]
                ;
            INT_CONSTANT_DEC
                = lexeme_d[
                    +digit_p
                    >> !as_lower_d[chlit<>('l') | chlit<>('u')]
                ]
                ;
            INT_CONSTANT_CHAR
                = lexeme_d[
                    !chlit<>('L') >> chlit<>('\'') >>
                    longest_d[
                            anychar_p
                        | ( chlit<>('\\')
                                >> chlit<>('0')
                                >> repeat_p(0, 2)[range<>('0', '7')]
                            )
                        | (chlit<>('\\') >> anychar_p)
                    ] >>
                    chlit<>('\'')
                ]
                ;
            INT_CONSTANT =
                    INT_CONSTANT_HEX
                | INT_CONSTANT_OCT
                | INT_CONSTANT_DEC
                | INT_CONSTANT_CHAR
                ;
            FLOAT_CONSTANT_1
                = lexeme_d[
                    +digit_p
                    >> (chlit<>('e') | chlit<>('E'))
                    >> !(chlit<>('+') | chlit<>('-'))
                    >> +digit_p
                    >> !as_lower_d[chlit<>('l') | chlit<>('f')]
                ]
                ;
            FLOAT_CONSTANT_2
                = lexeme_d[
                    *digit_p
                    >> chlit<>('.')
                    >> +digit_p
                    >> !( (chlit<>('e') | chlit<>('E'))
                            >> !(chlit<>('+') | chlit<>('-'))
                            >> +digit_p
                        )
                    >> !as_lower_d[chlit<>('l') | chlit<>('f')]
                ]
                ;
            FLOAT_CONSTANT_3
                = lexeme_d[
                    +digit_p
                    >> chlit<>('.')
                    >> *digit_p
                    >> !( (chlit<>('e') | chlit<>('E'))
                            >> !(chlit<>('+') | chlit<>('-'))
                            >> +digit_p
                        )
                    >> !as_lower_d[chlit<>('l') | chlit<>('f')]
                ]
                ;
            FLOAT_CONSTANT =
                    FLOAT_CONSTANT_1
                | FLOAT_CONSTANT_2
                | FLOAT_CONSTANT_3
                ;
            CONSTANT = longest_d[FLOAT_CONSTANT | INT_CONSTANT];
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            ;
            primary_expression
                = IDENTIFIER
                | CONSTANT
                | STRING_LITERAL
                | LEFT_PAREN >> expression >> RIGHT_PAREN
                ;
            postfix_expression
                = primary_expression >> postfix_expression_helper
                ;
            postfix_expression_helper
                = (
                        LEFT_BRACKET >> expression >> RIGHT_BRACKET
                    | LEFT_PAREN >> !argument_expression_list >> RIGHT_PAREN
                    | DOT >> IDENTIFIER
                    | PTR_OP >> IDENTIFIER
                    | INC_OP
                    | DEC_OP
                    ) >>
                    postfix_expression_helper
                | epsilon_p
                ;
            argument_expression_list
                = assignment_expression >> *(COMMA >> assignment_expression)
                ;
            unary_expression
                = postfix_expression
                | INC_OP >> unary_expression
                | DEC_OP >> unary_expression
                | unary_operator >> cast_expression
                | SIZEOF >>
                    (
                        unary_expression
                    | LEFT_PAREN >> type_name >> RIGHT_PAREN
                    )
                ;
            unary_operator
                = ADDROF
                | STAR
                | PLUS
                | MINUS
                | TILDE
                | BANG
                ;
            cast_expression
                = LEFT_PAREN >> type_name >> RIGHT_PAREN >> cast_expression
                | unary_expression
                ;
            multiplicative_expression
                = cast_expression >> multiplicative_expression_helper
                ;
            multiplicative_expression_helper
                = (
                        STAR >> cast_expression
                    | SLASH >> cast_expression
                    | PERCENT >> cast_expression
                    ) >>
                    multiplicative_expression_helper
                | epsilon_p
                ;
            additive_expression
                = multiplicative_expression >> additive_expression_helper
                ;
            additive_expression_helper
                = (
                        PLUS >> multiplicative_expression
                    | MINUS >> multiplicative_expression
                    ) >>
                    additive_expression_helper
                | epsilon_p
                ;
            shift_expression
                = additive_expression >> shift_expression_helper
                ;
            shift_expression_helper
                = (
                        LEFT_OP >> additive_expression
                    | RIGHT_OP >> additive_expression
                    ) >>
                    shift_expression_helper
                | epsilon_p
                ;
            relational_expression
                = shift_expression >> relational_expression_helper
                ;
            relational_expression_helper
                = (
                        LT_OP >> shift_expression
                    | GT_OP >> shift_expression
                    | LE_OP >> shift_expression
                    | GE_OP >> shift_expression
                    ) >>
                    relational_expression_helper
                | epsilon_p
                ;
            equality_expression
                = relational_expression >> equality_expression_helper
                ;
            equality_expression_helper
                = (
                        EQ_OP >> relational_expression
                    | NE_OP >> relational_expression
                    ) >>
                    equality_expression_helper
                | epsilon_p
                ;
            and_expression
                = equality_expression >> and_expression_helper
                ;
            and_expression_helper
                = ADDROF >> equality_expression >> and_expression_helper
                | epsilon_p
                ;
            exclusive_or_expression
                = and_expression >> exclusive_or_expression_helper
                ;
            exclusive_or_expression_helper
                = XOR >> and_expression >> exclusive_or_expression_helper
                | epsilon_p
                ;
            inclusive_or_expression
                = exclusive_or_expression >> inclusive_or_expression_helper
                ;
            inclusive_or_expression_helper
                = OR >> exclusive_or_expression >> inclusive_or_expression_helper
                | epsilon_p
                ;
            logical_and_expression
                = inclusive_or_expression >> logical_and_expression_helper
                ;
            logical_and_expression_helper
                = AND_OP >> inclusive_or_expression >> logical_and_expression_helper
                | epsilon_p
                ;
            logical_or_expression
                = logical_and_expression >> logical_or_expression_helper
                ;
            logical_or_expression_helper
                = OR_OP >> logical_and_expression >> logical_or_expression_helper
                | epsilon_p
                ;
            conditional_expression
                = logical_or_expression >> conditional_expression_helper
                ;
            conditional_expression_helper
                = QUEST >> expression >> COLON
                    >> conditional_expression >> conditional_expression_helper
                | epsilon_p
                ;
            assignment_expression
                = unary_expression >> assignment_operator >> assignment_expression
                | conditional_expression
                ;
            assignment_operator
                = ASSIGN
                | MUL_ASSIGN
                | DIV_ASSIGN
                | MOD_ASSIGN
                | ADD_ASSIGN
                | SUB_ASSIGN
                | LEFT_ASSIGN
                | RIGHT_ASSIGN
                | AND_ASSIGN
                | XOR_ASSIGN
                | OR_ASSIGN
                ;
            expression
                = assignment_expression >> expression_helper
                ;
            expression_helper
                = COMMA >> assignment_expression >> expression_helper
                | epsilon_p
                ;
            constant_expression
                = conditional_expression
                ;
            declaration
                = declaration_specifiers >> !init_declarator_list >> SEMICOLON
                ;
            declaration_specifiers
                = (
                        storage_class_specifier
                    | type_specifier
                    | type_qualifier
                    ) >>
                    !declaration_specifiers
                ;
            init_declarator_list
                = init_declarator >> *(COMMA >> init_declarator)
                ;
            init_declarator
                = declarator >> !(ASSIGN >> initializer)
                ;
            storage_class_specifier
                = TYPEDEF
                | EXTERN
                | STATIC
                | AUTO
                | REGISTER
                ;
            type_specifier
                = VOID
                | CHAR
                | SHORT
                | INT
                | LONG
                | FLOAT
                | DOUBLE
                | SIGNED
                | UNSIGNED
                | struct_or_union_specifier
                | enum_specifier
                ;
            struct_or_union_specifier
                = struct_or_union >>
                    (
                        IDENTIFIER >> !(LEFT_BRACE >> struct_declaration_list
                            >> RIGHT_BRACE)
                    | LEFT_BRACE >> struct_declaration_list >> RIGHT_BRACE
                    )
                ;
            struct_or_union
                = STRUCT
                | UNION
                ;
            struct_declaration_list
                = +struct_declaration
                ;
            struct_declaration
                = specifier_qualifier_list >> struct_declarator_list >> SEMICOLON
                ;
            specifier_qualifier_list
                = (
                        type_specifier
                    | type_qualifier
                    ) >>
                    !specifier_qualifier_list
                ;
            struct_declarator_list
                = struct_declarator >> *(COMMA >> struct_declarator)
                ;
            struct_declarator
                = declarator || (COLON >> constant_expression)
                ;
            enum_specifier
                = ENUM >> !IDENTIFIER >> LEFT_BRACE >> enumerator_list >> RIGHT_BRACE
                ;
            enumerator_list
                = enumerator >> *(COMMA >> enumerator)
                ;
            enumerator
                = IDENTIFIER >> !(ASSIGN >> constant_expression)
                ;
            type_qualifier
                = CONST
                | VOLATILE
                ;
            declarator
                = !pointer >> direct_declarator
                ;
            direct_declarator
                = (
                        IDENTIFIER
                    | LEFT_PAREN >> declarator >> RIGHT_PAREN
                    ) >>
                    direct_declarator_helper
                ;
            direct_declarator_helper
                = (
                        LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
                    | LEFT_PAREN >>
                    ! (
                            parameter_type_list
                        | identifier_list
                        ) >> RIGHT_PAREN
                    ) >> direct_declarator_helper
                | epsilon_p
                ;
            pointer
                = STAR >> !(type_qualifier_list || pointer)
                ;
            type_qualifier_list
                = +type_qualifier
                ;
            parameter_type_list
                = parameter_list >> !(COMMA >> ELLIPSIS)
                ;
            parameter_list
                = parameter_declaration >> *(COMMA >> parameter_declaration)
                ;
            parameter_declaration
                = declaration_specifiers >>
                   !(
                        declarator
                    | abstract_declarator
                    )
                ;
            identifier_list
                = IDENTIFIER >> *(COMMA >> IDENTIFIER)
                ;
            type_name
                = specifier_qualifier_list >> !abstract_declarator
                ;
            abstract_declarator
                = pointer || direct_abstract_declarator
                ;
            direct_abstract_declarator
                = (
                        LEFT_PAREN >>
                        (
                            abstract_declarator >> RIGHT_PAREN
                        | !parameter_type_list >> RIGHT_PAREN
                        )
                    | LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
                    ) >>
                    direct_abstract_declarator_helper
                ;
            direct_abstract_declarator_helper
                = (
                        LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
                    | LEFT_PAREN >> !parameter_type_list >> RIGHT_PAREN
                    ) >>
                    direct_abstract_declarator_helper
                | epsilon_p
                ;
            initializer
                = assignment_expression
                | LEFT_BRACE >> initializer_list >> !COMMA >> RIGHT_BRACE
                ;
            initializer_list
                = initializer >> *(COMMA >> initializer)
                ;
            statement
                = labeled_statement
                | compound_statement
                | expression_statement
                | selection_statement
                | iteration_statement
                | jump_statement
                ;
            labeled_statement
                = IDENTIFIER >> COLON >> statement
                | CASE >> constant_expression >> COLON >> statement
                | DEFAULT >> COLON >> statement
                ;
            compound_statement
                = LEFT_BRACE >> !(declaration_list || statement_list) >> RIGHT_BRACE
                ;
            declaration_list
                = +declaration
                ;
            statement_list
                = +statement
                ;
            expression_statement
                = !expression >> SEMICOLON
                ;
            selection_statement
                = IF >> LEFT_PAREN >> expression >> RIGHT_PAREN >> statement
                    >> !(ELSE >> statement)
                | SWITCH >> LEFT_PAREN >> expression >> RIGHT_PAREN >> statement
                ;
            iteration_statement
                = WHILE >> LEFT_PAREN >> expression >> RIGHT_PAREN >> statement
                | DO >> statement >> WHILE >> LEFT_PAREN >> expression
                    >> RIGHT_PAREN >> SEMICOLON
                | FOR >> LEFT_PAREN >> expression_statement >> expression_statement
                    >> !expression >> RIGHT_PAREN >> statement
                ;
            jump_statement
                = GOTO >> IDENTIFIER >> SEMICOLON
                | CONTINUE >> SEMICOLON
                | BREAK >> SEMICOLON
                | RETURN >> !expression >> SEMICOLON
                ;
            function_definition
                = !declaration_specifiers
                >> declarator
                >> !declaration_list
                >> compound_statement
                ;
            external_declaration
                = function_definition
                | declaration
                ;
            translation_unit
                = *external_declaration
                ;
        }
        symbols<> keywords;
        strlit<>
                ELLIPSIS, RIGHT_ASSIGN, LEFT_ASSIGN, ADD_ASSIGN, SUB_ASSIGN,
                MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN, AND_ASSIGN, XOR_ASSIGN,
                OR_ASSIGN, RIGHT_OP, LEFT_OP, INC_OP, DEC_OP, PTR_OP, AND_OP,
                OR_OP, LE_OP, GE_OP, EQ_OP, NE_OP;
        chlit<>
                SEMICOLON, COMMA, COLON, ASSIGN, LEFT_PAREN, RIGHT_PAREN,
                DOT, ADDROF, BANG, TILDE, MINUS, PLUS, STAR, SLASH, PERCENT,
                LT_OP, GT_OP, XOR, OR, QUEST;
        rule<ScannerT>
                LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET;
        rule<ScannerT>
                AUTO, BREAK, CASE, CHAR, CONST, CONTINUE, DEFAULT, DO, DOUBLE,
                ELSE, ENUM, EXTERN, FOR, FLOAT, GOTO, IF, INT, LONG, REGISTER,
                RETURN, SHORT, SIGNED, SIZEOF, STATIC, STRUCT, SWITCH, TYPEDEF,
                UNION, UNSIGNED, VOID, VOLATILE, WHILE, IDENTIFIER,
                STRING_LITERAL_PART, STRING_LITERAL, INT_CONSTANT_HEX, INT_CONSTANT,
                INT_CONSTANT_OCT, INT_CONSTANT_DEC, INT_CONSTANT_CHAR,
                FLOAT_CONSTANT,FLOAT_CONSTANT_1, FLOAT_CONSTANT_2, FLOAT_CONSTANT_3,
                CONSTANT;
        rule<ScannerT>
                primary_expression, postfix_expression, postfix_expression_helper,
                argument_expression_list, unary_expression, unary_operator,
                cast_expression,
                multiplicative_expression, multiplicative_expression_helper,
                additive_expression, additive_expression_helper,
                shift_expression, shift_expression_helper,
                relational_expression, relational_expression_helper,
                equality_expression, equality_expression_helper,
                and_expression, and_expression_helper,
                exclusive_or_expression, exclusive_or_expression_helper,
                inclusive_or_expression, inclusive_or_expression_helper,
                logical_and_expression, logical_and_expression_helper,
                logical_or_expression, logical_or_expression_helper,
                conditional_expression, conditional_expression_helper,
                assignment_expression, assignment_operator,
                expression, expression_helper, constant_expression, declaration,
                declaration_specifiers, init_declarator_list, init_declarator,
                storage_class_specifier, type_specifier, struct_or_union_specifier,
                struct_or_union, struct_declaration_list, struct_declaration,
                specifier_qualifier_list, struct_declarator_list,
                struct_declarator, enum_specifier, enumerator_list, enumerator,
                type_qualifier, declarator,
                direct_declarator, direct_declarator_helper, pointer,
                type_qualifier_list, parameter_type_list, parameter_list,
                parameter_declaration, identifier_list, type_name,
                abstract_declarator,
                direct_abstract_declarator, direct_abstract_declarator_helper,
                initializer, initializer_list, statement, labeled_statement,
                compound_statement, declaration_list, statement_list,
                expression_statement, selection_statement, iteration_statement,
                jump_statement, translation_unit, external_declaration,
                function_definition;
        rule<ScannerT> const&
        start() const { return translation_unit; }
    };
};
extern const char *g_code;
int
main(int argc, char* argv[])
{
  c_grammar g;
#ifdef SMALL_PROBLEM_SIZE
  unsigned long n = 4;
#else
  unsigned long n = 40;
#endif
  if (argc > 1)
    n = atol(argv[1]);
  for (unsigned long i=0;i<n;i++)
  {
    if (parse(g, g_code, strlen(g_code))) {
      std::cerr << "parse OK!\n";
    } else {
      std::cerr << "parse bad.\n";
    }
  }
  return 0;
}
const char *g_code =
" extern int debug_key_pressed; \n"
" void (*dsp32ops[])(void); \n"
"  \n"
"  \n"
"  \n"
"  \n"
"  \n"
"  \n"
"  \n"
" union int_double \n"
" { \n"
"         double d; \n"
"         unsigned long i[2]; \n"
" }; \n"
"  \n"
" static void illegal(void) \n"
" { \n"
" } \n"
"  \n"
"  \n"
" static void unimplemented(void) \n"
" { \n"
"         exit(1); \n"
" } \n"
"  \n"
"  \n"
"  void execute_one(void) \n"
" { \n"
"         if (dsp32.mbufaddr[++dsp32.mbuf_index & 3] != 1) { int bufidx = dsp32.mbuf_index & 3; if (dsp32.mbufaddr[bufidx] >= 0) WLONG(dsp32.mbufaddr[bufidx], dsp32.mbufdata[bufidx]); else WWORD(-dsp32.mbufaddr[bufidx], dsp32.mbufdata[bufidx]); dsp32.mbufaddr[bufidx] = 1; }; \n"
"         CALL_MAME_DEBUG; \n"
"         OP = ROPCODE(dsp32.PC); \n"
"         dsp32_icount -= 4; \n"
"         dsp32.PC += 4; \n"
"         if (OP) \n"
"                 (*dsp32ops[OP >> 21])(); \n"
" } \n"
"  \n"
" static unsigned long cau_read_pi_special(unsigned char i) \n"
" { \n"
"         switch (i) \n"
"         { \n"
"                 case 4: return dsp32.ibuf; \n"
"                 case 5: return dsp32.obuf; \n"
"                 case 6: update_pcr(dsp32.pcr & ~PCR_PDFs); return dsp32.pdr; \n"
"                 case 14: return dsp32.piop; \n"
"                 case 20: return dsp32.pdr2; \n"
"                 case 22: update_pcr(dsp32.pcr & ~PCR_PIFs); return dsp32.pir; \n"
"                 case 30: return dsp32.pcw; \n"
"         } \n"
"         return 0; \n"
" } \n"
"  \n"
"  \n"
" static void cau_write_pi_special(unsigned char i, unsigned long val) \n"
" { \n"
"         switch (i) \n"
"         { \n"
"                 case 4: dsp32.ibuf = val; break; \n"
"                 case 5: dsp32.obuf = val; break; \n"
"                 case 6: dsp32.pdr = val; update_pcr(dsp32.pcr | PCR_PDFs); break; \n"
"                 case 14: dsp32.piop = val; break; \n"
"                 case 20: dsp32.pdr2 = val; break; \n"
"                 case 22: dsp32.pir = val; update_pcr(dsp32.pcr | PCR_PIFs); break; \n"
"                 case 30: dsp32.pcw = val; break; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
"  unsigned char cau_read_pi_1byte(int pi) \n"
" { \n"
"         int p = (pi >> 5) & 0x1f; \n"
"         int i = (pi >> 0) & 0x1f; \n"
"         if (p) \n"
"         { \n"
"                 unsigned long result = RBYTE(dsp32.r[p]); \n"
"                 dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i]) & 0xffffff); \n"
"                 return result; \n"
"         } \n"
"         else \n"
"                 return cau_read_pi_special(i); \n"
" } \n"
"  \n"
"  \n"
"  unsigned short cau_read_pi_2byte(int pi) \n"
" { \n"
"         int p = (pi >> 5) & 0x1f; \n"
"         int i = (pi >> 0) & 0x1f; \n"
"         if (p) \n"
"         { \n"
"                 unsigned long result = RWORD(dsp32.r[p]); \n"
"                 if (i < 22 || i > 23) \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i]) & 0xffffff); \n"
"                 else \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i] * 2) & 0xffffff); \n"
"                 return result; \n"
"         } \n"
"         else \n"
"                 return cau_read_pi_special(i); \n"
" } \n"
"  \n"
"  \n"
"  unsigned long cau_read_pi_4byte(int pi) \n"
" { \n"
"         int p = (pi >> 5) & 0x1f; \n"
"         int i = (pi >> 0) & 0x1f; \n"
"         if (p) \n"
"         { \n"
"                 unsigned long result = RLONG(dsp32.r[p]); \n"
"                 if (i < 22 || i > 23) \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i]) & 0xffffff); \n"
"                 else \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i] * 4) & 0xffffff); \n"
"                 return result; \n"
"         } \n"
"         else \n"
"                 return cau_read_pi_special(i); \n"
" } \n"
"  \n"
"  \n"
"  void cau_write_pi_1byte(int pi, unsigned char val) \n"
" { \n"
"         int p = (pi >> 5) & 0x1f; \n"
"         int i = (pi >> 0) & 0x1f; \n"
"         if (p) \n"
"         { \n"
"                 WBYTE(dsp32.r[p], val); \n"
"                 dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i]) & 0xffffff); \n"
"         } \n"
"         else \n"
"                 cau_write_pi_special(i, val); \n"
" } \n"
"  \n"
"  \n"
"  void cau_write_pi_2byte(int pi, unsigned short val) \n"
" { \n"
"         int p = (pi >> 5) & 0x1f; \n"
"         int i = (pi >> 0) & 0x1f; \n"
"         if (p) \n"
"         { \n"
"                 WWORD(dsp32.r[p], val); \n"
"                 if (i < 22 || i > 23) \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i]) & 0xffffff); \n"
"                 else \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i] * 2) & 0xffffff); \n"
"         } \n"
"         else \n"
"                 cau_write_pi_special(i, val); \n"
" } \n"
"  \n"
"  \n"
"  void cau_write_pi_4byte(int pi, unsigned long val) \n"
" { \n"
"         int p = (pi >> 5) & 0x1f; \n"
"         int i = (pi >> 0) & 0x1f; \n"
"         if (p) \n"
"         { \n"
"                 WLONG(dsp32.r[p], (long)(val << 8) >> 8); \n"
"                 if (i < 22 || i > 23) \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i]) & 0xffffff); \n"
"                 else \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i] * 4) & 0xffffff); \n"
"         } \n"
"         else \n"
"                 cau_write_pi_special(i, val); \n"
" } \n"
"  double dau_get_amult(int aidx) \n"
" { \n"
"         int bufidx = (dsp32.abuf_index - 1) & 3; \n"
"         double val = dsp32.a[aidx]; \n"
"         while (dsp32_icount >= dsp32.abufcycle[bufidx] - 2 * 4) \n"
"         { \n"
"                 if (dsp32.abufreg[bufidx] == aidx) \n"
"                         val = dsp32.abuf[bufidx]; \n"
"                 bufidx = (bufidx - 1) & 3; \n"
"         } \n"
"         return val; \n"
" } \n"
"  \n"
"  \n"
"  double dau_get_anzflags(void) \n"
" { \n"
"         int bufidx = (dsp32.abuf_index - 1) & 3; \n"
"         double nzflags = dsp32.NZflags; \n"
"         while (dsp32_icount >= dsp32.abufcycle[bufidx] - 3 * 4) \n"
"         { \n"
"                 nzflags = dsp32.abufNZflags[bufidx]; \n"
"                 bufidx = (bufidx - 1) & 3; \n"
"         } \n"
"         return nzflags; \n"
" } \n"
"  \n"
"  \n"
"  unsigned char dau_get_avuflags(void) \n"
" { \n"
"  \n"
"         int bufidx = (dsp32.abuf_index - 1) & 3; \n"
"         unsigned char vuflags = dsp32.VUflags; \n"
"         while (dsp32_icount >= dsp32.abufcycle[bufidx] - 3 * 4) \n"
"         { \n"
"                 vuflags = dsp32.abufVUflags[bufidx]; \n"
"                 bufidx = (bufidx - 1) & 3; \n"
"         } \n"
"         return vuflags; \n"
"  \n"
"  \n"
"  \n"
" } \n"
"  \n"
"  \n"
"  void remember_last_dau(int aidx) \n"
" { \n"
"  \n"
"         int bufidx = dsp32.abuf_index++ & 3; \n"
"         dsp32.abuf[bufidx] = dsp32.a[aidx]; \n"
"         dsp32.abufreg[bufidx] = aidx; \n"
"         dsp32.abufNZflags[bufidx] = dsp32.NZflags; \n"
"  \n"
"         dsp32.abufVUflags[bufidx] = dsp32.VUflags; \n"
"  \n"
"         dsp32.abufcycle[bufidx] = dsp32_icount; \n"
"  \n"
" } \n"
"  \n"
"  \n"
"  void dau_set_val_noflags(int aidx, double res) \n"
" { \n"
"         remember_last_dau(aidx); \n"
"         dsp32.a[aidx] = res; \n"
" } \n"
"  \n"
"  \n"
"  void dau_set_val_flags(int aidx, double res) \n"
" { \n"
"         remember_last_dau(aidx); \n"
"  \n"
" { \n"
"         double absres = (res < 0) ? -res : res; \n"
"         dsp32.VUflags = 0; \n"
"         if (absres < 5.87747e-39) \n"
"         { \n"
"                 if (absres != 0) \n"
"                         dsp32.VUflags = UFLAGBIT; \n"
"                 res = 0.0; \n"
"         } \n"
"         else if (absres > 3.40282e38) \n"
"         { \n"
"                 dsp32.VUflags = VFLAGBIT; \n"
"  \n"
"  \n"
"                 res = (res < 0) ? -3.40282e38 : 3.40282e38; \n"
"         } \n"
" } \n"
"  \n"
"         dsp32.NZflags = res; \n"
"         dsp32.a[aidx] = res; \n"
" } \n"
"  \n"
"  \n"
" double dsp_to_double(unsigned long val) \n"
" { \n"
"         union int_double id; \n"
"  \n"
"         if (val == 0) \n"
"                 return 0; \n"
"         else if ((int)val > 0) \n"
"         { \n"
"                 int exponent = ((val & 0xff) - 128 + 1023) << 20; \n"
"                 id.i[BYTE_XOR_BE(0)] = exponent + (val >> 11); \n"
"                 id.i[BYTE_XOR_BE(1)] = (val << 21) & 0xe0000000; \n"
"         } \n"
"         else \n"
"         { \n"
"                 int exponent = ((val & 0xff) - 128 + 1023) << 20; \n"
"                 val = -(val & 0xffffff00); \n"
"                 id.i[BYTE_XOR_BE(0)] = 0x80000000 + exponent + ((val >> 11) & 0x001fffff); \n"
"                 id.i[BYTE_XOR_BE(1)] = (val << 21) & 0xe0000000; \n"
"         } \n"
"         return id.d; \n"
" } \n"
"  \n"
"  \n"
" unsigned long ddd(double val) \n"
" { \n"
"         int mantissa, exponent; \n"
"         union int_double id; \n"
"         id.d = val; \n"
"         mantissa = ((id.i[BYTE_XOR_BE(0)] & 0x000fffff) << 11) | ((id.i[BYTE_XOR_BE(1)] & 0xe0000000) >> 21); \n"
"         exponent = ((id.i[BYTE_XOR_BE(0)] & 0x7ff00000) >> 20) - 1023 + 128; \n"
"         if (exponent < 0) \n"
"                 return 0x00000000; \n"
"         else if (exponent > 255) \n"
"         { \n"
"  \n"
"  \n"
"                 return ((long)id.i[BYTE_XOR_BE(0)] >= 0) ? 0x7fffffff : 0x800000ff; \n"
"         } \n"
"         else if ((long)id.i[BYTE_XOR_BE(0)] >= 0) \n"
"                 return exponent | mantissa; \n"
"         else \n"
"         { \n"
"                 mantissa = -mantissa; \n"
"                 if (mantissa == 0) { mantissa = 0x80000000; exponent--; } \n"
"                 return 0x80000000 | exponent | (mantissa & 0xffffff00); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static double dau_read_pi_special(int i) \n"
" { \n"
"         exit(1); \n"
"         return 0; \n"
" } \n"
"  \n"
"  \n"
" static void dau_write_pi_special(int i, double val) \n"
" { \n"
"         exit(1); \n"
" } \n"
"  \n"
"  \n"
" static int lastp; \n"
"  \n"
"  double dau_read_pi_double_1st(int pi, int multiplier) \n"
" { \n"
"         int p = (pi >> 3) & 15; \n"
"         int i = (pi >> 0) & 7; \n"
"  \n"
"         lastp = p; \n"
"         if (p) \n"
"         { \n"
"                 unsigned long result = RLONG(dsp32.r[p]); \n"
"                 if (i < 6) \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i+16]) & 0xffffff); \n"
"                 else \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i+16] * 4) & 0xffffff); \n"
"                 return dsp_to_double(result); \n"
"         } \n"
"         else if (i < 4) \n"
"                 return multiplier ? dau_get_amult(i) : dsp32.a[i]; \n"
"         else \n"
"                 return dau_read_pi_special(i); \n"
" } \n"
"  \n"
"  \n"
"  double dau_read_pi_double_2nd(int pi, int multiplier, double xval) \n"
" { \n"
"         int p = (pi >> 3) & 15; \n"
"         int i = (pi >> 0) & 7; \n"
"  \n"
"         if (p == 15) p = lastp; \n"
"         lastp = p; \n"
"         if (p) \n"
"         { \n"
"                 unsigned long result; \n"
"                 result = RLONG(dsp32.r[p]); \n"
"                 if (i < 6) \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i+16]) & 0xffffff); \n"
"                 else \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i+16] * 4) & 0xffffff); \n"
"                 return dsp_to_double(result); \n"
"         } \n"
"         else if (i < 4) \n"
"                 return multiplier ? dau_get_amult(i) : dsp32.a[i]; \n"
"         else \n"
"                 return dau_read_pi_special(i); \n"
" } \n"
"  \n"
"  \n"
"  unsigned long dau_read_pi_4bytes(int pi) \n"
" { \n"
"         int p = (pi >> 3) & 15; \n"
"         int i = (pi >> 0) & 7; \n"
"  \n"
"         lastp = p; \n"
"         if (p) \n"
"         { \n"
"                 unsigned long result = RLONG(dsp32.r[p]); \n"
"                 if (i < 6) \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i+16]) & 0xffffff); \n"
"                 else \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i+16] * 4) & 0xffffff); \n"
"                 return result; \n"
"         } \n"
"         else if (i < 4) \n"
"                 return double_to_dsp(dsp32.a[i]); \n"
"         else \n"
"                 return dau_read_pi_special(i); \n"
" } \n"
"  \n"
"  \n"
"  unsigned short dau_read_pi_2bytes(int pi) \n"
" { \n"
"         int p = (pi >> 3) & 15; \n"
"         int i = (pi >> 0) & 7; \n"
"  \n"
"         lastp = p; \n"
"         if (p) \n"
"         { \n"
"                 unsigned long result = RWORD(dsp32.r[p]); \n"
"                 if (i < 6) \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i+16]) & 0xffffff); \n"
"                 else \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i+16] * 2) & 0xffffff); \n"
"                 return result; \n"
"         } \n"
"         else if (i < 4) \n"
"                 return double_to_dsp(dsp32.a[i]); \n"
"         else \n"
"                 return dau_read_pi_special(i); \n"
" } \n"
"  \n"
"  \n"
"  void dau_write_pi_double(int pi, double val) \n"
" { \n"
"         int p = (pi >> 3) & 15; \n"
"         int i = (pi >> 0) & 7; \n"
"  \n"
"         if (p == 15) p = lastp; \n"
"         if (p) \n"
"         { \n"
"                 do { int bufidx = dsp32.mbuf_index & 3; dsp32.mbufaddr[bufidx] = (dsp32.r[p]); dsp32.mbufdata[bufidx] = (double_to_dsp(val)); } while (0); \n"
"                 if (i < 6) \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i+16]) & 0xffffff); \n"
"                 else \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i+16] * 4) & 0xffffff); \n"
"         } \n"
"         else if (i < 4) \n"
"                 dau_set_val_noflags(i, val); \n"
"         else \n"
"                 dau_write_pi_special(i, val); \n"
" } \n"
"  \n"
"  \n"
"  void dau_write_pi_4bytes(int pi, unsigned long val) \n"
" { \n"
"         int p = (pi >> 3) & 15; \n"
"         int i = (pi >> 0) & 7; \n"
"  \n"
"         if (p == 15) p = lastp; \n"
"         if (p) \n"
"         { \n"
"                 lastp = p; \n"
"                 do { int bufidx = dsp32.mbuf_index & 3; dsp32.mbufaddr[bufidx] = (dsp32.r[p]); dsp32.mbufdata[bufidx] = (val); } while (0); \n"
"                 if (i < 6) \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i+16]) & 0xffffff); \n"
"                 else \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i+16] * 4) & 0xffffff); \n"
"         } \n"
"         else if (i < 4) \n"
"                 dau_set_val_noflags(i, dsp_to_double(val)); \n"
"         else \n"
"                 dau_write_pi_special(i, val); \n"
" } \n"
"  \n"
"  \n"
"  void dau_write_pi_2bytes(int pi, unsigned short val) \n"
" { \n"
"         int p = (pi >> 3) & 15; \n"
"         int i = (pi >> 0) & 7; \n"
"  \n"
"         if (p == 15) p = lastp; \n"
"         if (p) \n"
"         { \n"
"                 lastp = p; \n"
"                 do { int bufidx = dsp32.mbuf_index & 3; dsp32.mbufaddr[bufidx] = -(dsp32.r[p]); dsp32.mbufdata[bufidx] = (val); } while (0); \n"
"                 if (i < 6) \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i+16]) & 0xffffff); \n"
"                 else \n"
"                         dsp32.r[p] = ((dsp32.r[p] + dsp32.r[i+16] * 2) & 0xffffff); \n"
"         } \n"
"         else if (i < 4) \n"
"                 dau_set_val_noflags(i, dsp_to_double(val << 16)); \n"
"         else \n"
"                 dau_write_pi_special(i, val); \n"
" } \n"
" static int condition(int cond) \n"
" { \n"
"         switch (cond) \n"
"         { \n"
"                 case 0: \n"
"                         return 0; \n"
"                 case 1: \n"
"                         return 1; \n"
"                 case 2: \n"
"                         return !nFLAG; \n"
"                 case 3: \n"
"                         return nFLAG; \n"
"                 case 4: \n"
"                         return !zFLAG; \n"
"                 case 5: \n"
"                         return zFLAG; \n"
"                 case 6: \n"
"                         return !vFLAG; \n"
"                 case 7: \n"
"                         return vFLAG; \n"
"                 case 8: \n"
"                         return !cFLAG; \n"
"                 case 9: \n"
"                         return cFLAG; \n"
"                 case 10: \n"
"                         return !(nFLAG ^ cFLAG); \n"
"                 case 11: \n"
"                         return (nFLAG ^ cFLAG); \n"
"                 case 12: \n"
"                         return !(zFLAG | (nFLAG ^ vFLAG)); \n"
"                 case 13: \n"
"                         return (zFLAG | (nFLAG ^ vFLAG)); \n"
"                 case 14: \n"
"                         return !(cFLAG | zFLAG); \n"
"                 case 15: \n"
"                         return (cFLAG | zFLAG); \n"
"  \n"
"                 case 16: \n"
"                         return !(dau_get_avuflags() & UFLAGBIT); \n"
"                 case 17: \n"
"                         return (dau_get_avuflags() & UFLAGBIT); \n"
"                 case 18: \n"
"                         return !(dau_get_anzflags() < 0); \n"
"                 case 19: \n"
"                         return (dau_get_anzflags() < 0); \n"
"                 case 20: \n"
"                         return !(dau_get_anzflags() == 0); \n"
"                 case 21: \n"
"                         return (dau_get_anzflags() == 0); \n"
"                 case 22: \n"
"                         return !(dau_get_avuflags() & VFLAGBIT); \n"
"                 case 23: \n"
"                         return (dau_get_avuflags() & VFLAGBIT); \n"
"                 case 24: \n"
"                         return !(dau_get_anzflags() <= 0); \n"
"                 case 25: \n"
"                         return (dau_get_anzflags() <= 0); \n"
"  \n"
"                 case 32: \n"
"                 case 33: \n"
"                 case 34: \n"
"                 case 35: \n"
"                 case 36: \n"
"                 case 37: \n"
"                 case 38: \n"
"                 case 39: \n"
"                 case 40: \n"
"                 case 41: \n"
"                 case 42: \n"
"                 case 43: \n"
"                 case 44: \n"
"                 case 45: \n"
"                 case 46: \n"
"                 case 47: \n"
"                 default: \n"
"                         exit(1); \n"
"         } \n"
" } \n"
"  \n"
" static void nop(void) \n"
" { \n"
"         unsigned long op = 123; \n"
"         if (op == 0) \n"
"                 return; \n"
"         execute_one(); \n"
"         dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"         cpu_setopbase24ledw(dsp32.PC); \n"
"  \n"
" } \n"
"  \n"
" static void goto_t(void) \n"
" { \n"
"         unsigned long op = OP; \n"
"         execute_one(); \n"
"         dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"         cpu_setopbase24ledw(dsp32.PC); \n"
" } \n"
"  \n"
"  \n"
" static void goto_pl(void) \n"
" { \n"
"         if (!nFLAG) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_mi(void) \n"
" { \n"
"         if (nFLAG) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_ne(void) \n"
" { \n"
"         if (!zFLAG) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_eq(void) \n"
" { \n"
"         if (zFLAG) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_vc(void) \n"
" { \n"
"         if (!vFLAG) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_vs(void) \n"
" { \n"
"         if (vFLAG) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_cc(void) \n"
" { \n"
"         if (!cFLAG) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_cs(void) \n"
" { \n"
"         if (cFLAG) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_ge(void) \n"
" { \n"
"         if (!(nFLAG ^ vFLAG)) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_lt(void) \n"
" { \n"
"         if (nFLAG ^ vFLAG) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_gt(void) \n"
" { \n"
"         if (!(zFLAG | (nFLAG ^ vFLAG))) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_le(void) \n"
" { \n"
"         if (zFLAG | (nFLAG ^ vFLAG)) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_hi(void) \n"
" { \n"
"         if (!cFLAG && !zFLAG) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_ls(void) \n"
" { \n"
"         if (cFLAG || zFLAG) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_auc(void) \n"
" { \n"
"         if (!(dau_get_avuflags() & UFLAGBIT)) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_aus(void) \n"
" { \n"
"         if (dau_get_avuflags() & UFLAGBIT) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_age(void) \n"
" { \n"
"         if (dau_get_anzflags() >= 0) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_alt(void) \n"
" { \n"
"         if (dau_get_anzflags() < 0) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_ane(void) \n"
" { \n"
"         if (dau_get_anzflags() != 0) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_aeq(void) \n"
" { \n"
"         if (dau_get_anzflags() == 0) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_avc(void) \n"
" { \n"
"         if (!(dau_get_avuflags() & VFLAGBIT)) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_avs(void) \n"
" { \n"
"         if (dau_get_avuflags() & VFLAGBIT) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_agt(void) \n"
" { \n"
"         if (dau_get_anzflags() > 0) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_ale(void) \n"
" { \n"
"         if (dau_get_anzflags() <= 0) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void goto_ibe(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void goto_ibf(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void goto_obf(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void goto_obe(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void goto_pde(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void goto_pdf(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void goto_pie(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void goto_pif(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void goto_syc(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void goto_sys(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void goto_fbc(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void goto_fbs(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void goto_irq1lo(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void goto_irq1hi(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void goto_irq2lo(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void goto_irq2hi(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void dec_goto(void) \n"
" { \n"
"         int hr = (OP >> 21) & 0x1f; \n"
"         int old = (short)dsp32.r[hr]; \n"
"         dsp32.r[hr] = (((long)(short)(dsp32.r[hr] - 1)) & 0xffffff); \n"
"         if (old >= 0) \n"
"         { \n"
"                 unsigned long op = OP; \n"
"                 execute_one(); \n"
"                 dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"                 cpu_setopbase24ledw(dsp32.PC); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void call(void) \n"
" { \n"
"         unsigned long op = OP; \n"
"         int mr = (op >> 21) & 0x1f; \n"
"         if (((0x6f3efffe) & (1 << (mr)))) \n"
"                 dsp32.r[mr] = dsp32.PC + 4; \n"
"         execute_one(); \n"
"         dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (short)op) & 0xffffff); \n"
"         cpu_setopbase24ledw(dsp32.PC); \n"
" } \n"
"  \n"
"  \n"
" static void goto24(void) \n"
" { \n"
"         unsigned long op = OP; \n"
"         execute_one(); \n"
"         dsp32.PC = (((dsp32.r[(op >> 16) & 0x1f]) + (op & 0xffff) + ((op >> 5) & 0xff0000)) & 0xffffff); \n"
"         cpu_setopbase24ledw(dsp32.PC); \n"
" } \n"
"  \n"
"  \n"
" static void call24(void) \n"
" { \n"
"         unsigned long op = OP; \n"
"         int mr = (op >> 16) & 0x1f; \n"
"         if (((0x6f3efffe) & (1 << (mr)))) \n"
"                 dsp32.r[mr] = dsp32.PC + 4; \n"
"         execute_one(); \n"
"         dsp32.PC = (op & 0xffff) + ((op >> 5) & 0xff0000); \n"
"         cpu_setopbase24ledw(dsp32.PC); \n"
" } \n"
"  \n"
"  \n"
" static void do_i(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void do_r(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
" static void add_si(void) \n"
" { \n"
"         int dr = (OP >> 21) & 0x1f; \n"
"         int hrval = ((unsigned short)dsp32.r[(OP >> 16) & 0x1f]); \n"
"         int res = hrval + (unsigned short)OP; \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"         dsp32.nzcflags = ((res) << 8); dsp32.vflags = (((hrval) ^ (OP) ^ (res) ^ ((res) >> 1)) << 8); \n"
" } \n"
"  \n"
"  \n"
" static void add_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = ((unsigned short)dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int s2rval = (OP & 0x800) ? ((unsigned short)dsp32.r[(OP >> 0) & 0x1f]) : ((unsigned short)dsp32.r[dr]); \n"
"                 int res = s2rval + s1rval; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"                 dsp32.nzcflags = ((res) << 8); dsp32.vflags = (((s1rval) ^ (s2rval) ^ (res) ^ ((res) >> 1)) << 8); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void mul2_s(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = ((unsigned short)dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int res = s1rval * 2; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"                 dsp32.nzcflags = ((res) << 8); dsp32.vflags = (((s1rval) ^ (0) ^ (res) ^ ((res) >> 1)) << 8); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void subr_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = ((unsigned short)dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int s2rval = (OP & 0x800) ? ((unsigned short)dsp32.r[(OP >> 0) & 0x1f]) : ((unsigned short)dsp32.r[dr]); \n"
"                 int res = s1rval - s2rval; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"                 dsp32.nzcflags = ((res) << 8); dsp32.vflags = (((s1rval) ^ (s2rval) ^ (res) ^ ((res) >> 1)) << 8); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void addr_ss(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void sub_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = ((unsigned short)dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int s2rval = (OP & 0x800) ? ((unsigned short)dsp32.r[(OP >> 0) & 0x1f]) : ((unsigned short)dsp32.r[dr]); \n"
"                 int res = s2rval - s1rval; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"                 dsp32.nzcflags = ((res) << 8); dsp32.vflags = (((s1rval) ^ (s2rval) ^ (res) ^ ((res) >> 1)) << 8); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void neg_s(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = ((unsigned short)dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int res = -s1rval; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"                 dsp32.nzcflags = ((res) << 8); dsp32.vflags = (((s1rval) ^ (0) ^ (res) ^ ((res) >> 1)) << 8); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void andc_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = ((unsigned short)dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int s2rval = (OP & 0x800) ? ((unsigned short)dsp32.r[(OP >> 0) & 0x1f]) : ((unsigned short)dsp32.r[dr]); \n"
"                 int res = s2rval & ~s1rval; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"                 dsp32.nzcflags = (((res) << 8) & 0xffffff); dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void cmp_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int drval = ((unsigned short)dsp32.r[(OP >> 16) & 0x1f]); \n"
"                 int s1rval = ((unsigned short)dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int res = drval - s1rval; \n"
"                 dsp32.nzcflags = ((res) << 8); dsp32.vflags = (((drval) ^ (s1rval) ^ (res) ^ ((res) >> 1)) << 8); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void xor_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = ((unsigned short)dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int s2rval = (OP & 0x800) ? ((unsigned short)dsp32.r[(OP >> 0) & 0x1f]) : ((unsigned short)dsp32.r[dr]); \n"
"                 int res = s2rval ^ s1rval; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"                 dsp32.nzcflags = (((res) << 8) & 0xffffff); dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void rcr_s(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = ((unsigned short)dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int res = ((dsp32.nzcflags >> 9) & 0x8000) | (s1rval >> 1); \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"                 dsp32.nzcflags = ((res & 0xffff) << 8) | ((s1rval & 1) << 24); \n"
"                 dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void or_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = ((unsigned short)dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int s2rval = (OP & 0x800) ? ((unsigned short)dsp32.r[(OP >> 0) & 0x1f]) : ((unsigned short)dsp32.r[dr]); \n"
"                 int res = s2rval | s1rval; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"                 dsp32.nzcflags = (((res) << 8) & 0xffffff); dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void rcl_s(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = ((unsigned short)dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int res = ((dsp32.nzcflags >> 24) & 0x0001) | (s1rval << 1); \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"                 dsp32.nzcflags = ((res & 0xffff) << 8) | ((s1rval & 0x8000) << 9); \n"
"                 dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void shr_s(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = ((unsigned short)dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int res = s1rval >> 1; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"                 dsp32.nzcflags = ((res & 0xffff) << 8) | ((s1rval & 1) << 24); \n"
"                 dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void div2_s(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = ((unsigned short)dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int res = (s1rval & 0x8000) | (s1rval >> 1); \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"                 dsp32.nzcflags = ((res & 0xffff) << 8) | ((s1rval & 1) << 24); \n"
"                 dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void and_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = ((unsigned short)dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int s2rval = (OP & 0x800) ? ((unsigned short)dsp32.r[(OP >> 0) & 0x1f]) : ((unsigned short)dsp32.r[dr]); \n"
"                 int res = s2rval & s1rval; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"                 dsp32.nzcflags = (((res) << 8) & 0xffffff); dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void test_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int drval = ((unsigned short)dsp32.r[(OP >> 16) & 0x1f]); \n"
"                 int s1rval = ((unsigned short)dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int res = drval & s1rval; \n"
"                 dsp32.nzcflags = (((res) << 8) & 0xffffff); dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void add_di(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         int drval = ((unsigned short)dsp32.r[dr]); \n"
"         int res = drval + (unsigned short)OP; \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"         dsp32.nzcflags = ((res) << 8); dsp32.vflags = (((drval) ^ (OP) ^ (res) ^ ((res) >> 1)) << 8); \n"
" } \n"
"  \n"
"  \n"
" static void subr_di(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         int drval = ((unsigned short)dsp32.r[dr]); \n"
"         int res = (unsigned short)OP - drval; \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"         dsp32.nzcflags = ((res) << 8); dsp32.vflags = (((drval) ^ (OP) ^ (res) ^ ((res) >> 1)) << 8); \n"
" } \n"
"  \n"
"  \n"
" static void addr_di(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void sub_di(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         int drval = ((unsigned short)dsp32.r[dr]); \n"
"         int res = drval - (unsigned short)OP; \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"         dsp32.nzcflags = ((res) << 8); dsp32.vflags = (((drval) ^ (OP) ^ (res) ^ ((res) >> 1)) << 8); \n"
" } \n"
"  \n"
"  \n"
" static void andc_di(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         int drval = ((unsigned short)dsp32.r[dr]); \n"
"         int res = drval & ~(unsigned short)OP; \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"         dsp32.nzcflags = (((res) << 8) & 0xffffff); dsp32.vflags = 0; \n"
" } \n"
"  \n"
"  \n"
" static void cmp_di(void) \n"
" { \n"
"         int drval = ((unsigned short)dsp32.r[(OP >> 16) & 0x1f]); \n"
"         int res = drval - (unsigned short)OP; \n"
"         dsp32.nzcflags = ((res) << 8); dsp32.vflags = (((drval) ^ (OP) ^ (res) ^ ((res) >> 1)) << 8); \n"
" } \n"
"  \n"
"  \n"
" static void xor_di(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         int drval = ((unsigned short)dsp32.r[dr]); \n"
"         int res = drval ^ (unsigned short)OP; \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"         dsp32.nzcflags = (((res) << 8) & 0xffffff); dsp32.vflags = 0; \n"
" } \n"
"  \n"
"  \n"
" static void or_di(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         int drval = ((unsigned short)dsp32.r[dr]); \n"
"         int res = drval | (unsigned short)OP; \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"         dsp32.nzcflags = (((res) << 8) & 0xffffff); dsp32.vflags = 0; \n"
" } \n"
"  \n"
"  \n"
" static void and_di(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         int drval = ((unsigned short)dsp32.r[dr]); \n"
"         int res = drval & (unsigned short)OP; \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"         dsp32.nzcflags = (((res) << 8) & 0xffffff); dsp32.vflags = 0; \n"
" } \n"
"  \n"
"  \n"
" static void test_di(void) \n"
" { \n"
"         int drval = ((unsigned short)dsp32.r[(OP >> 16) & 0x1f]); \n"
"         int res = drval & (unsigned short)OP; \n"
"         dsp32.nzcflags = (((res) << 8) & 0xffffff); dsp32.vflags = 0; \n"
" } \n"
" static void adde_si(void) \n"
" { \n"
"         int dr = (OP >> 21) & 0x1f; \n"
"         int hrval = (dsp32.r[(OP >> 16) & 0x1f]); \n"
"         int res = hrval + (((long)(short)(OP)) & 0xffffff); \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = ((res) & 0xffffff); \n"
"         dsp32.nzcflags = (res); dsp32.vflags = ((hrval) ^ (OP << 8) ^ (res) ^ ((res) >> 1)); \n"
" } \n"
"  \n"
"  \n"
" static void adde_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = (dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int s2rval = (OP & 0x800) ? (dsp32.r[(OP >> 0) & 0x1f]) : (dsp32.r[dr]); \n"
"                 int res = s2rval + s1rval; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = ((res) & 0xffffff); \n"
"                 dsp32.nzcflags = (res); dsp32.vflags = ((s1rval) ^ (s2rval) ^ (res) ^ ((res) >> 1)); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void mul2e_s(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = (dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int res = s1rval * 2; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = ((res) & 0xffffff); \n"
"                 dsp32.nzcflags = (res); dsp32.vflags = ((s1rval) ^ (0) ^ (res) ^ ((res) >> 1)); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void subre_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = (dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int s2rval = (OP & 0x800) ? (dsp32.r[(OP >> 0) & 0x1f]) : (dsp32.r[dr]); \n"
"                 int res = s1rval - s2rval; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = ((res) & 0xffffff); \n"
"                 dsp32.nzcflags = (res); dsp32.vflags = ((s1rval) ^ (s2rval) ^ (res) ^ ((res) >> 1)); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void addre_ss(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void sube_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = (dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int s2rval = (OP & 0x800) ? (dsp32.r[(OP >> 0) & 0x1f]) : (dsp32.r[dr]); \n"
"                 int res = s2rval - s1rval; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = ((res) & 0xffffff); \n"
"                 dsp32.nzcflags = (res); dsp32.vflags = ((s1rval) ^ (s2rval) ^ (res) ^ ((res) >> 1)); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void nege_s(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = (dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int res = -s1rval; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = ((res) & 0xffffff); \n"
"                 dsp32.nzcflags = (res); dsp32.vflags = ((s1rval) ^ (0) ^ (res) ^ ((res) >> 1)); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void andce_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = (dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int s2rval = (OP & 0x800) ? (dsp32.r[(OP >> 0) & 0x1f]) : (dsp32.r[dr]); \n"
"                 int res = s2rval & ~s1rval; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = res; \n"
"                 dsp32.nzcflags = ((res) & 0xffffff); dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void cmpe_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int drval = (dsp32.r[(OP >> 16) & 0x1f]); \n"
"                 int s1rval = (dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int res = drval - s1rval; \n"
"                 dsp32.nzcflags = (res); dsp32.vflags = ((drval) ^ (s1rval) ^ (res) ^ ((res) >> 1)); \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void xore_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = (dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int s2rval = (OP & 0x800) ? (dsp32.r[(OP >> 0) & 0x1f]) : (dsp32.r[dr]); \n"
"                 int res = s2rval ^ s1rval; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = res; \n"
"                 dsp32.nzcflags = ((res) & 0xffffff); dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void rcre_s(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = (dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int res = ((dsp32.nzcflags >> 1) & 0x800000) | (s1rval >> 1); \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = ((res) & 0xffffff); \n"
"                 dsp32.nzcflags = res | ((s1rval & 1) << 24); \n"
"                 dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void ore_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = (dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int s2rval = (OP & 0x800) ? (dsp32.r[(OP >> 0) & 0x1f]) : (dsp32.r[dr]); \n"
"                 int res = s2rval | s1rval; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = res; \n"
"                 dsp32.nzcflags = ((res) & 0xffffff); dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void rcle_s(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = (dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int res = ((dsp32.nzcflags >> 24) & 0x000001) | (s1rval << 1); \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = ((res) & 0xffffff); \n"
"                 dsp32.nzcflags = res | ((s1rval & 0x800000) << 1); \n"
"                 dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void shre_s(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = (dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int res = s1rval >> 1; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = res; \n"
"                 dsp32.nzcflags = res | ((s1rval & 1) << 24); \n"
"                 dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void div2e_s(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = (dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int res = (s1rval & 0x800000) | (s1rval >> 1); \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = ((res) & 0xffffff); \n"
"                 dsp32.nzcflags = res | ((s1rval & 1) << 24); \n"
"                 dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void ande_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 int s1rval = (dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int s2rval = (OP & 0x800) ? (dsp32.r[(OP >> 0) & 0x1f]) : (dsp32.r[dr]); \n"
"                 int res = s2rval & s1rval; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = res; \n"
"                 dsp32.nzcflags = ((res) & 0xffffff); dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void teste_ss(void) \n"
" { \n"
"         if ((!(OP & 0x400) || (condition((OP >> 12) & 15)))) \n"
"         { \n"
"                 int drval = (dsp32.r[(OP >> 16) & 0x1f]); \n"
"                 int s1rval = (dsp32.r[(OP >> 5) & 0x1f]); \n"
"                 int res = drval & s1rval; \n"
"                 dsp32.nzcflags = ((res) & 0xffffff); dsp32.vflags = 0; \n"
"         } \n"
" } \n"
"  \n"
"  \n"
" static void adde_di(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         int drval = (dsp32.r[dr]); \n"
"         int res = drval + (((long)(short)(OP)) & 0xffffff); \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = ((res) & 0xffffff); \n"
"         dsp32.nzcflags = (res); dsp32.vflags = ((drval) ^ (OP << 8) ^ (res) ^ ((res) >> 1)); \n"
" } \n"
"  \n"
"  \n"
" static void subre_di(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         int drval = (dsp32.r[dr]); \n"
"         int res = (((long)(short)(OP)) & 0xffffff) - drval; \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = ((res) & 0xffffff); \n"
"         dsp32.nzcflags = (res); dsp32.vflags = ((drval) ^ (OP << 8) ^ (res) ^ ((res) >> 1)); \n"
" } \n"
"  \n"
"  \n"
" static void addre_di(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void sube_di(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         int drval = (dsp32.r[dr]); \n"
"         int res = drval - (((long)(short)(OP)) & 0xffffff); \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = ((res) & 0xffffff); \n"
"         dsp32.nzcflags = (res); dsp32.vflags = ((drval) ^ (OP << 8) ^ (res) ^ ((res) >> 1)); \n"
" } \n"
"  \n"
"  \n"
" static void andce_di(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         int drval = (dsp32.r[dr]); \n"
"         int res = drval & ~(((long)(short)(OP)) & 0xffffff); \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = res; \n"
"         dsp32.nzcflags = ((res) & 0xffffff); dsp32.vflags = 0; \n"
" } \n"
"  \n"
"  \n"
" static void cmpe_di(void) \n"
" { \n"
"         int drval = (dsp32.r[(OP >> 16) & 0x1f]); \n"
"         int res = drval - (((long)(short)(OP)) & 0xffffff); \n"
"         dsp32.nzcflags = (res); dsp32.vflags = ((drval) ^ (OP << 8) ^ (res) ^ ((res) >> 1)); \n"
" } \n"
"  \n"
"  \n"
" static void xore_di(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         int drval = (dsp32.r[dr]); \n"
"         int res = drval ^ (((long)(short)(OP)) & 0xffffff); \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = res; \n"
"         dsp32.nzcflags = ((res) & 0xffffff); dsp32.vflags = 0; \n"
" } \n"
"  \n"
"  \n"
" static void ore_di(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         int drval = (dsp32.r[dr]); \n"
"         int res = drval | (((long)(short)(OP)) & 0xffffff); \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = res; \n"
"         dsp32.nzcflags = ((res) & 0xffffff); dsp32.vflags = 0; \n"
" } \n"
"  \n"
"  \n"
" static void ande_di(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         int drval = (dsp32.r[dr]); \n"
"         int res = drval & (((long)(short)(OP)) & 0xffffff); \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = res; \n"
"         dsp32.nzcflags = ((res) & 0xffffff); dsp32.vflags = 0; \n"
" } \n"
"  \n"
"  \n"
" static void teste_di(void) \n"
" { \n"
"         int drval = (dsp32.r[(OP >> 16) & 0x1f]); \n"
"         int res = drval & (((long)(short)(OP)) & 0xffffff); \n"
"         dsp32.nzcflags = ((res) & 0xffffff); dsp32.vflags = 0; \n"
" } \n"
" static void load_hi(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         unsigned long res = RBYTE((((long)(short)(OP)) & 0xffffff)); \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"         dsp32.nzcflags = res << 8; \n"
"         dsp32.vflags = 0; \n"
" } \n"
"  \n"
"  \n"
" static void load_li(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         unsigned long res = RBYTE((((long)(short)(OP)) & 0xffffff)); \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = res; \n"
"         dsp32.nzcflags = res << 8; \n"
"         dsp32.vflags = 0; \n"
" } \n"
"  \n"
"  \n"
" static void load_i(void) \n"
" { \n"
"         unsigned long res = RWORD((((long)(short)(OP)) & 0xffffff)); \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"         dsp32.nzcflags = res << 8; \n"
"         dsp32.vflags = 0; \n"
" } \n"
"  \n"
"  \n"
" static void load_ei(void) \n"
" { \n"
"         unsigned long res = ((RLONG((((long)(short)(OP)) & 0xffffff))) & 0xffffff); \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = res; \n"
"         dsp32.nzcflags = res; \n"
"         dsp32.vflags = 0; \n"
" } \n"
"  \n"
"  \n"
" static void store_hi(void) \n"
" { \n"
"         WBYTE((((long)(short)(OP)) & 0xffffff), dsp32.r[(OP >> 16) & 0x1f] >> 8); \n"
" } \n"
"  \n"
"  \n"
" static void store_li(void) \n"
" { \n"
"         WBYTE((((long)(short)(OP)) & 0xffffff), dsp32.r[(OP >> 16) & 0x1f]); \n"
" } \n"
"  \n"
"  \n"
" static void store_i(void) \n"
" { \n"
"         WWORD((((long)(short)(OP)) & 0xffffff), ((unsigned short)dsp32.r[(OP >> 16) & 0x1f])); \n"
" } \n"
"  \n"
"  \n"
" static void store_ei(void) \n"
" { \n"
"         WLONG((((long)(short)(OP)) & 0xffffff), (long)((dsp32.r[(OP >> 16) & 0x1f]) << 8) >> 8); \n"
" } \n"
"  \n"
"  \n"
" static void load_hr(void) \n"
" { \n"
"         if (!(OP & 0x400)) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 unsigned long res = cau_read_pi_1byte(OP) << 8; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"                 dsp32.nzcflags = res << 8; \n"
"                 dsp32.vflags = 0; \n"
"         } \n"
"         else \n"
"                 unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void load_lr(void) \n"
" { \n"
"         if (!(OP & 0x400)) \n"
"         { \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 unsigned long res = cau_read_pi_1byte(OP); \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = res; \n"
"                 dsp32.nzcflags = res << 8; \n"
"                 dsp32.vflags = 0; \n"
"         } \n"
"         else \n"
"                 unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void load_r(void) \n"
" { \n"
"         if (!(OP & 0x400)) \n"
"         { \n"
"                 unsigned long res = cau_read_pi_2byte(OP); \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = (((long)(short)(res)) & 0xffffff); \n"
"                 dsp32.nzcflags = res << 8; \n"
"                 dsp32.vflags = 0; \n"
"         } \n"
"         else \n"
"                 unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void load_er(void) \n"
" { \n"
"         if (!(OP & 0x400)) \n"
"         { \n"
"                 unsigned long res = ((cau_read_pi_4byte(OP)) & 0xffffff); \n"
"                 int dr = (OP >> 16) & 0x1f; \n"
"                 if (((0x6f3efffe) & (1 << (dr)))) \n"
"                         dsp32.r[dr] = res; \n"
"                 dsp32.nzcflags = res; \n"
"                 dsp32.vflags = 0; \n"
"         } \n"
"         else \n"
"                 unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void store_hr(void) \n"
" { \n"
"         if (!(OP & 0x400)) \n"
"                 cau_write_pi_1byte(OP, dsp32.r[(OP >> 16) & 0x1f] >> 8); \n"
"         else \n"
"                 unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void store_lr(void) \n"
" { \n"
"         if (!(OP & 0x400)) \n"
"                 cau_write_pi_1byte(OP, dsp32.r[(OP >> 16) & 0x1f]); \n"
"         else \n"
"                 unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void store_r(void) \n"
" { \n"
"         if (!(OP & 0x400)) \n"
"                 cau_write_pi_2byte(OP, ((unsigned short)dsp32.r[(OP >> 16) & 0x1f])); \n"
"         else \n"
"                 unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void store_er(void) \n"
" { \n"
"         if (!(OP & 0x400)) \n"
"                 cau_write_pi_4byte(OP, (dsp32.r[(OP >> 16) & 0x1f])); \n"
"         else \n"
"                 unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void load24(void) \n"
" { \n"
"         int dr = (OP >> 16) & 0x1f; \n"
"         unsigned long res = (OP & 0xffff) + ((OP >> 5) & 0xff0000); \n"
"         if (((0x6f3efffe) & (1 << (dr)))) \n"
"                 dsp32.r[dr] = res; \n"
" } \n"
" static void d1_aMpp(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 0, xval); \n"
"         double res = yval + dau_get_amult((OP >> 26) & 7) * xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d1_aMpm(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 0, xval); \n"
"         double res = yval - dau_get_amult((OP >> 26) & 7) * xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d1_aMmp(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 0, xval); \n"
"         double res = -yval + dau_get_amult((OP >> 26) & 7) * xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d1_aMmm(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 0, xval); \n"
"         double res = -yval - dau_get_amult((OP >> 26) & 7) * xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d1_0px(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 0, xval); \n"
"         double res = yval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
"         (void)xval; \n"
" } \n"
"  \n"
"  \n"
" static void d1_0mx(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 0, xval); \n"
"         double res = -yval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
"         (void)xval; \n"
" } \n"
"  \n"
"  \n"
" static void d1_1pp(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 0, xval); \n"
"         double res = yval + xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d1_1pm(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 0, xval); \n"
"         double res = yval - xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d1_1mp(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 0, xval); \n"
"         double res = -yval + xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d1_1mm(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 0, xval); \n"
"         double res = -yval - xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d1_aMppr(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void d1_aMpmr(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void d1_aMmpr(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void d1_aMmmr(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
" static void d2_aMpp(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 1, xval); \n"
"         double res = dsp32.a[(OP >> 26) & 7] + yval * xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, yval); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d2_aMpm(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 1, xval); \n"
"         double res = dsp32.a[(OP >> 26) & 7] - yval * xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, yval); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d2_aMmp(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 1, xval); \n"
"         double res = -dsp32.a[(OP >> 26) & 7] + yval * xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, yval); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d2_aMmm(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 1, xval); \n"
"         double res = -dsp32.a[(OP >> 26) & 7] - yval * xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, yval); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d2_aMppr(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void d2_aMpmr(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void d2_aMmpr(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void d2_aMmmr(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
" static void d3_aMpp(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 1, xval); \n"
"         double res = dsp32.a[(OP >> 26) & 7] + yval * xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d3_aMpm(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 1, xval); \n"
"         double res = dsp32.a[(OP >> 26) & 7] - yval * xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d3_aMmp(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 1, xval); \n"
"         double res = -dsp32.a[(OP >> 26) & 7] + yval * xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d3_aMmm(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 1, xval); \n"
"         double res = -dsp32.a[(OP >> 26) & 7] - yval * xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d3_aMppr(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void d3_aMpmr(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void d3_aMmpr(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void d3_aMmmr(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
" static void d4_pp(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 0, xval); \n"
"         double res = yval + xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, yval); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d4_pm(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 0, xval); \n"
"         double res = yval - xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, yval); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d4_mp(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 0, xval); \n"
"         double res = -yval + xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, yval); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d4_mm(void) \n"
" { \n"
"         double xval = dau_read_pi_double_1st(OP >> 14, 1); \n"
"         double yval = dau_read_pi_double_2nd(OP >> 7, 0, xval); \n"
"         double res = -yval - xval; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, yval); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d4_ppr(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void d4_pmr(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void d4_mpr(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void d4_mmr(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
" static void d5_ic(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void d5_oc(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void d5_float(void) \n"
" { \n"
"         double res = (double)(short)dau_read_pi_2bytes(OP >> 7); \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d5_int(void) \n"
" { \n"
"         double val = dau_read_pi_double_1st(OP >> 7, 0); \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         short res; \n"
"         if (!(dsp32.DAUC & 0x10)) val = floor(val + 0.5); \n"
"         else val = ceil(val - 0.5); \n"
"         res = (short)val; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_2bytes(zpi, res); \n"
"         dau_set_val_noflags((OP >> 21) & 3, dsp_to_double(res << 16)); \n"
" } \n"
"  \n"
"  \n"
" static void d5_round(void) \n"
" { \n"
"         double res = (double)(float)dau_read_pi_double_1st(OP >> 7, 0); \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d5_ifalt(void) \n"
" { \n"
"         int ar = (OP >> 21) & 3; \n"
"         double res = dsp32.a[ar]; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (NFLAG) \n"
"                 res = dau_read_pi_double_1st(OP >> 7, 0); \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_noflags(ar, res); \n"
" } \n"
"  \n"
"  \n"
" static void d5_ifaeq(void) \n"
" { \n"
"         int ar = (OP >> 21) & 3; \n"
"         double res = dsp32.a[ar]; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (ZFLAG) \n"
"                 res = dau_read_pi_double_1st(OP >> 7, 0); \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_noflags(ar, res); \n"
" } \n"
"  \n"
"  \n"
" static void d5_ifagt(void) \n"
" { \n"
"         int ar = (OP >> 21) & 3; \n"
"         double res = dsp32.a[ar]; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (!NFLAG && !ZFLAG) \n"
"                 res = dau_read_pi_double_1st(OP >> 7, 0); \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_noflags(ar, res); \n"
" } \n"
"  \n"
"  \n"
" static void d5_float24(void) \n"
" { \n"
"         double res = (double)((long)(dau_read_pi_4bytes(OP >> 7) << 8) >> 8); \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_double(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, res); \n"
" } \n"
"  \n"
"  \n"
" static void d5_int24(void) \n"
" { \n"
"         double val = dau_read_pi_double_1st(OP >> 7, 0); \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         long res; \n"
"         if (!(dsp32.DAUC & 0x10)) val = floor(val + 0.5); \n"
"         else val = ceil(val - 0.5); \n"
"         res = (long)val; \n"
"         if (res > 0x7fffff) res = 0x7fffff; \n"
"         else if (res < -0x800000) res = -0x800000; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_4bytes(zpi, (long)(res << 8) >> 8); \n"
"         dau_set_val_noflags((OP >> 21) & 3, dsp_to_double(res << 8)); \n"
" } \n"
"  \n"
"  \n"
" static void d5_ieee(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void d5_dsp(void) \n"
" { \n"
"         unimplemented(); \n"
" } \n"
"  \n"
"  \n"
" static void d5_seed(void) \n"
" { \n"
"         unsigned long val = dau_read_pi_4bytes(OP >> 7); \n"
"         long res = val ^ 0x7fffffff; \n"
"         int zpi = (OP >> 0) & 0x7f; \n"
"         if (zpi != 7) \n"
"                 dau_write_pi_4bytes(zpi, res); \n"
"         dau_set_val_flags((OP >> 21) & 3, dsp_to_double((long)res)); \n"
" } \n"
" void (*dsp32ops[])(void) = \n"
" { \n"
"         nop, goto_t, goto_pl, goto_mi, goto_ne, goto_eq, goto_vc, goto_vs, \n"
"         goto_cc, goto_cs, goto_ge, goto_lt, goto_gt, goto_le, goto_hi, goto_ls, \n"
"         goto_auc, goto_aus, goto_age, goto_alt, goto_ane, goto_aeq, goto_avc, goto_avs, \n"
"         goto_agt, goto_ale, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         goto_ibe, goto_ibf, goto_obf, goto_obe, goto_pde, goto_pdf, goto_pie, goto_pif, \n"
"         goto_syc, goto_sys, goto_fbc, goto_fbs, goto_irq1lo,goto_irq1hi,goto_irq2lo,goto_irq2hi, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"  \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         dec_goto, dec_goto, dec_goto, dec_goto, dec_goto, dec_goto, dec_goto, dec_goto, \n"
"         dec_goto, dec_goto, dec_goto, dec_goto, dec_goto, dec_goto, dec_goto, dec_goto, \n"
"         dec_goto, dec_goto, dec_goto, dec_goto, dec_goto, dec_goto, dec_goto, dec_goto, \n"
"         dec_goto, dec_goto, dec_goto, dec_goto, dec_goto, dec_goto, dec_goto, dec_goto, \n"
"  \n"
"         call, call, call, call, call, call, call, call, \n"
"         call, call, call, call, call, call, call, call, \n"
"         call, call, call, call, call, call, call, call, \n"
"         call, call, call, call, call, call, call, call, \n"
"         add_si, add_si, add_si, add_si, add_si, add_si, add_si, add_si, \n"
"         add_si, add_si, add_si, add_si, add_si, add_si, add_si, add_si, \n"
"         add_si, add_si, add_si, add_si, add_si, add_si, add_si, add_si, \n"
"         add_si, add_si, add_si, add_si, add_si, add_si, add_si, add_si, \n"
"  \n"
"         add_ss, mul2_s, subr_ss, addr_ss, sub_ss, neg_s, andc_ss, cmp_ss, \n"
"         xor_ss, rcr_s, or_ss, rcl_s, shr_s, div2_s, and_ss, test_ss, \n"
"         add_di, illegal, subr_di, addr_di, sub_di, illegal, andc_di, cmp_di, \n"
"         xor_di, illegal, or_di, illegal, illegal, illegal, and_di, test_di, \n"
"         load_hi, load_hi, load_li, load_li, load_i, load_i, load_ei, load_ei, \n"
"         store_hi, store_hi, store_li, store_li, store_i, store_i, store_ei, store_ei, \n"
"         load_hr, load_hr, load_lr, load_lr, load_r, load_r, load_er, load_er, \n"
"         store_hr, store_hr, store_lr, store_lr, store_r, store_r, store_er, store_er, \n"
"  \n"
"         d1_aMpp, d1_aMpp, d1_aMpp, d1_aMpp, d1_aMpm, d1_aMpm, d1_aMpm, d1_aMpm, \n"
"         d1_aMmp, d1_aMmp, d1_aMmp, d1_aMmp, d1_aMmm, d1_aMmm, d1_aMmm, d1_aMmm, \n"
"         d1_aMppr, d1_aMppr, d1_aMppr, d1_aMppr, d1_aMpmr, d1_aMpmr, d1_aMpmr, d1_aMpmr, \n"
"         d1_aMmpr, d1_aMmpr, d1_aMmpr, d1_aMmpr, d1_aMmmr, d1_aMmmr, d1_aMmmr, d1_aMmmr, \n"
"         d1_aMpp, d1_aMpp, d1_aMpp, d1_aMpp, d1_aMpm, d1_aMpm, d1_aMpm, d1_aMpm, \n"
"         d1_aMmp, d1_aMmp, d1_aMmp, d1_aMmp, d1_aMmm, d1_aMmm, d1_aMmm, d1_aMmm, \n"
"         d1_aMppr, d1_aMppr, d1_aMppr, d1_aMppr, d1_aMpmr, d1_aMpmr, d1_aMpmr, d1_aMpmr, \n"
"         d1_aMmpr, d1_aMmpr, d1_aMmpr, d1_aMmpr, d1_aMmmr, d1_aMmmr, d1_aMmmr, d1_aMmmr, \n"
"  \n"
"         d1_aMpp, d1_aMpp, d1_aMpp, d1_aMpp, d1_aMpm, d1_aMpm, d1_aMpm, d1_aMpm, \n"
"         d1_aMmp, d1_aMmp, d1_aMmp, d1_aMmp, d1_aMmm, d1_aMmm, d1_aMmm, d1_aMmm, \n"
"         d1_aMppr, d1_aMppr, d1_aMppr, d1_aMppr, d1_aMpmr, d1_aMpmr, d1_aMpmr, d1_aMpmr, \n"
"         d1_aMmpr, d1_aMmpr, d1_aMmpr, d1_aMmpr, d1_aMmmr, d1_aMmmr, d1_aMmmr, d1_aMmmr, \n"
"         d1_aMpp, d1_aMpp, d1_aMpp, d1_aMpp, d1_aMpm, d1_aMpm, d1_aMpm, d1_aMpm, \n"
"         d1_aMmp, d1_aMmp, d1_aMmp, d1_aMmp, d1_aMmm, d1_aMmm, d1_aMmm, d1_aMmm, \n"
"         d1_aMppr, d1_aMppr, d1_aMppr, d1_aMppr, d1_aMpmr, d1_aMpmr, d1_aMpmr, d1_aMpmr, \n"
"         d1_aMmpr, d1_aMmpr, d1_aMmpr, d1_aMmpr, d1_aMmmr, d1_aMmmr, d1_aMmmr, d1_aMmmr, \n"
"  \n"
"         d1_0px, d1_0px, d1_0px, d1_0px, d1_0px, d1_0px, d1_0px, d1_0px, \n"
"         d1_0mx, d1_0mx, d1_0mx, d1_0mx, d1_0mx, d1_0mx, d1_0mx, d1_0mx, \n"
"         d1_aMppr, d1_aMppr, d1_aMppr, d1_aMppr, d1_aMpmr, d1_aMpmr, d1_aMpmr, d1_aMpmr, \n"
"         d1_aMmpr, d1_aMmpr, d1_aMmpr, d1_aMmpr, d1_aMmmr, d1_aMmmr, d1_aMmmr, d1_aMmmr, \n"
"         d1_1pp, d1_1pp, d1_1pp, d1_1pp, d1_1pm, d1_1pm, d1_1pm, d1_1pm, \n"
"         d1_1mp, d1_1mp, d1_1mp, d1_1mp, d1_1mm, d1_1mm, d1_1mm, d1_1mm, \n"
"         d1_aMppr, d1_aMppr, d1_aMppr, d1_aMppr, d1_aMpmr, d1_aMpmr, d1_aMpmr, d1_aMpmr, \n"
"         d1_aMmpr, d1_aMmpr, d1_aMmpr, d1_aMmpr, d1_aMmmr, d1_aMmmr, d1_aMmmr, d1_aMmmr, \n"
"  \n"
"         d4_pp, d4_pp, d4_pp, d4_pp, d4_pm, d4_pm, d4_pm, d4_pm, \n"
"         d4_mp, d4_mp, d4_mp, d4_mp, d4_mm, d4_mm, d4_mm, d4_mm, \n"
"         d4_ppr, d4_ppr, d4_ppr, d4_ppr, d4_pmr, d4_pmr, d4_pmr, d4_pmr, \n"
"         d4_mpr, d4_mpr, d4_mpr, d4_mpr, d4_mmr, d4_mmr, d4_mmr, d4_mmr, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"  \n"
"         d2_aMpp, d2_aMpp, d2_aMpp, d2_aMpp, d2_aMpm, d2_aMpm, d2_aMpm, d2_aMpm, \n"
"         d2_aMmp, d2_aMmp, d2_aMmp, d2_aMmp, d2_aMmm, d2_aMmm, d2_aMmm, d2_aMmm, \n"
"         d2_aMppr, d2_aMppr, d2_aMppr, d2_aMppr, d2_aMpmr, d2_aMpmr, d2_aMpmr, d2_aMpmr, \n"
"         d2_aMmpr, d2_aMmpr, d2_aMmpr, d2_aMmpr, d2_aMmmr, d2_aMmmr, d2_aMmmr, d2_aMmmr, \n"
"         d2_aMpp, d2_aMpp, d2_aMpp, d2_aMpp, d2_aMpm, d2_aMpm, d2_aMpm, d2_aMpm, \n"
"         d2_aMmp, d2_aMmp, d2_aMmp, d2_aMmp, d2_aMmm, d2_aMmm, d2_aMmm, d2_aMmm, \n"
"         d2_aMppr, d2_aMppr, d2_aMppr, d2_aMppr, d2_aMpmr, d2_aMpmr, d2_aMpmr, d2_aMpmr, \n"
"         d2_aMmpr, d2_aMmpr, d2_aMmpr, d2_aMmpr, d2_aMmmr, d2_aMmmr, d2_aMmmr, d2_aMmmr, \n"
"  \n"
"         d2_aMpp, d2_aMpp, d2_aMpp, d2_aMpp, d2_aMpm, d2_aMpm, d2_aMpm, d2_aMpm, \n"
"         d2_aMmp, d2_aMmp, d2_aMmp, d2_aMmp, d2_aMmm, d2_aMmm, d2_aMmm, d2_aMmm, \n"
"         d2_aMppr, d2_aMppr, d2_aMppr, d2_aMppr, d2_aMpmr, d2_aMpmr, d2_aMpmr, d2_aMpmr, \n"
"         d2_aMmpr, d2_aMmpr, d2_aMmpr, d2_aMmpr, d2_aMmmr, d2_aMmmr, d2_aMmmr, d2_aMmmr, \n"
"         d2_aMpp, d2_aMpp, d2_aMpp, d2_aMpp, d2_aMpm, d2_aMpm, d2_aMpm, d2_aMpm, \n"
"         d2_aMmp, d2_aMmp, d2_aMmp, d2_aMmp, d2_aMmm, d2_aMmm, d2_aMmm, d2_aMmm, \n"
"         d2_aMppr, d2_aMppr, d2_aMppr, d2_aMppr, d2_aMpmr, d2_aMpmr, d2_aMpmr, d2_aMpmr, \n"
"         d2_aMmpr, d2_aMmpr, d2_aMmpr, d2_aMmpr, d2_aMmmr, d2_aMmmr, d2_aMmmr, d2_aMmmr, \n"
"  \n"
"         d2_aMpp, d2_aMpp, d2_aMpp, d2_aMpp, d2_aMpm, d2_aMpm, d2_aMpm, d2_aMpm, \n"
"         d2_aMmp, d2_aMmp, d2_aMmp, d2_aMmp, d2_aMmm, d2_aMmm, d2_aMmm, d2_aMmm, \n"
"         d2_aMppr, d2_aMppr, d2_aMppr, d2_aMppr, d2_aMpmr, d2_aMpmr, d2_aMpmr, d2_aMpmr, \n"
"         d2_aMmpr, d2_aMmpr, d2_aMmpr, d2_aMmpr, d2_aMmmr, d2_aMmmr, d2_aMmmr, d2_aMmmr, \n"
"         d2_aMpp, d2_aMpp, d2_aMpp, d2_aMpp, d2_aMpm, d2_aMpm, d2_aMpm, d2_aMpm, \n"
"         d2_aMmp, d2_aMmp, d2_aMmp, d2_aMmp, d2_aMmm, d2_aMmm, d2_aMmm, d2_aMmm, \n"
"         d2_aMppr, d2_aMppr, d2_aMppr, d2_aMppr, d2_aMpmr, d2_aMpmr, d2_aMpmr, d2_aMpmr, \n"
"         d2_aMmpr, d2_aMmpr, d2_aMmpr, d2_aMmpr, d2_aMmmr, d2_aMmmr, d2_aMmmr, d2_aMmmr, \n"
"  \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"  \n"
"         d3_aMpp, d3_aMpp, d3_aMpp, d3_aMpp, d3_aMpm, d3_aMpm, d3_aMpm, d3_aMpm, \n"
"         d3_aMmp, d3_aMmp, d3_aMmp, d3_aMmp, d3_aMmm, d3_aMmm, d3_aMmm, d3_aMmm, \n"
"         d3_aMppr, d3_aMppr, d3_aMppr, d3_aMppr, d3_aMpmr, d3_aMpmr, d3_aMpmr, d3_aMpmr, \n"
"         d3_aMmpr, d3_aMmpr, d3_aMmpr, d3_aMmpr, d3_aMmmr, d3_aMmmr, d3_aMmmr, d3_aMmmr, \n"
"         d3_aMpp, d3_aMpp, d3_aMpp, d3_aMpp, d3_aMpm, d3_aMpm, d3_aMpm, d3_aMpm, \n"
"         d3_aMmp, d3_aMmp, d3_aMmp, d3_aMmp, d3_aMmm, d3_aMmm, d3_aMmm, d3_aMmm, \n"
"         d3_aMppr, d3_aMppr, d3_aMppr, d3_aMppr, d3_aMpmr, d3_aMpmr, d3_aMpmr, d3_aMpmr, \n"
"         d3_aMmpr, d3_aMmpr, d3_aMmpr, d3_aMmpr, d3_aMmmr, d3_aMmmr, d3_aMmmr, d3_aMmmr, \n"
"  \n"
"         d3_aMpp, d3_aMpp, d3_aMpp, d3_aMpp, d3_aMpm, d3_aMpm, d3_aMpm, d3_aMpm, \n"
"         d3_aMmp, d3_aMmp, d3_aMmp, d3_aMmp, d3_aMmm, d3_aMmm, d3_aMmm, d3_aMmm, \n"
"         d3_aMppr, d3_aMppr, d3_aMppr, d3_aMppr, d3_aMpmr, d3_aMpmr, d3_aMpmr, d3_aMpmr, \n"
"         d3_aMmpr, d3_aMmpr, d3_aMmpr, d3_aMmpr, d3_aMmmr, d3_aMmmr, d3_aMmmr, d3_aMmmr, \n"
"         d3_aMpp, d3_aMpp, d3_aMpp, d3_aMpp, d3_aMpm, d3_aMpm, d3_aMpm, d3_aMpm, \n"
"         d3_aMmp, d3_aMmp, d3_aMmp, d3_aMmp, d3_aMmm, d3_aMmm, d3_aMmm, d3_aMmm, \n"
"         d3_aMppr, d3_aMppr, d3_aMppr, d3_aMppr, d3_aMpmr, d3_aMpmr, d3_aMpmr, d3_aMpmr, \n"
"         d3_aMmpr, d3_aMmpr, d3_aMmpr, d3_aMmpr, d3_aMmmr, d3_aMmmr, d3_aMmmr, d3_aMmmr, \n"
"  \n"
"         d3_aMpp, d3_aMpp, d3_aMpp, d3_aMpp, d3_aMpm, d3_aMpm, d3_aMpm, d3_aMpm, \n"
"         d3_aMmp, d3_aMmp, d3_aMmp, d3_aMmp, d3_aMmm, d3_aMmm, d3_aMmm, d3_aMmm, \n"
"         d3_aMppr, d3_aMppr, d3_aMppr, d3_aMppr, d3_aMpmr, d3_aMpmr, d3_aMpmr, d3_aMpmr, \n"
"         d3_aMmpr, d3_aMmpr, d3_aMmpr, d3_aMmpr, d3_aMmmr, d3_aMmmr, d3_aMmmr, d3_aMmmr, \n"
"         d3_aMpp, d3_aMpp, d3_aMpp, d3_aMpp, d3_aMpm, d3_aMpm, d3_aMpm, d3_aMpm, \n"
"         d3_aMmp, d3_aMmp, d3_aMmp, d3_aMmp, d3_aMmm, d3_aMmm, d3_aMmm, d3_aMmm, \n"
"         d3_aMppr, d3_aMppr, d3_aMppr, d3_aMppr, d3_aMpmr, d3_aMpmr, d3_aMpmr, d3_aMpmr, \n"
"         d3_aMmpr, d3_aMmpr, d3_aMmpr, d3_aMmpr, d3_aMmmr, d3_aMmmr, d3_aMmmr, d3_aMmmr, \n"
"  \n"
"         d5_ic, d5_ic, d5_ic, d5_ic, d5_oc, d5_oc, d5_oc, d5_oc, \n"
"         d5_float, d5_float, d5_float, d5_float, d5_int, d5_int, d5_int, d5_int, \n"
"         d5_round, d5_round, d5_round, d5_round, d5_ifalt, d5_ifalt, d5_ifalt, d5_ifalt, \n"
"         d5_ifaeq, d5_ifaeq, d5_ifaeq, d5_ifaeq, d5_ifagt, d5_ifagt, d5_ifagt, d5_ifagt, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         d5_float24, d5_float24, d5_float24, d5_float24, d5_int24, d5_int24, d5_int24, d5_int24, \n"
"         d5_ieee, d5_ieee, d5_ieee, d5_ieee, d5_dsp, d5_dsp, d5_dsp, d5_dsp, \n"
"         d5_seed, d5_seed, d5_seed, d5_seed, illegal, illegal, illegal, illegal, \n"
"  \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"  \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         do_i, do_r, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"  \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         adde_si, adde_si, adde_si, adde_si, adde_si, adde_si, adde_si, adde_si, \n"
"         adde_si, adde_si, adde_si, adde_si, adde_si, adde_si, adde_si, adde_si, \n"
"         adde_si, adde_si, adde_si, adde_si, adde_si, adde_si, adde_si, adde_si, \n"
"         adde_si, adde_si, adde_si, adde_si, adde_si, adde_si, adde_si, adde_si, \n"
"  \n"
"         adde_ss, mul2e_s, subre_ss, addre_ss, sube_ss, nege_s, andce_ss, cmpe_ss, \n"
"         xore_ss, rcre_s, ore_ss, rcle_s, shre_s, div2e_s, ande_ss, teste_ss, \n"
"         adde_di, illegal, subre_di, addre_di, sube_di, illegal, andce_di, cmpe_di, \n"
"         xore_di, illegal, ore_di, illegal, illegal, illegal, ande_di, teste_di, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"         illegal, illegal, illegal, illegal, illegal, illegal, illegal, illegal, \n"
"  \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"  \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"  \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"  \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"         goto24, goto24, goto24, goto24, goto24, goto24, goto24, goto24, \n"
"  \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"  \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"  \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"  \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"         load24, load24, load24, load24, load24, load24, load24, load24, \n"
"  \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"  \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"  \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"  \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24, \n"
"         call24, call24, call24, call24, call24, call24, call24, call24 \n"
" }; \n";
