
//  (C) Copyright John Maddock 2006.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_SPECIAL_HERMITE_HPP
#define BOOST_MATH_SPECIAL_HERMITE_HPP

#include <boost/math/special_functions/math_fwd.hpp>
#include <boost/math/tools/config.hpp>
#include <boost/math/tools/evaluation_type.hpp>

namespace boost{
namespace math{

// Recurrance relation for Hermite polynomials:
template <class T1, class T2, class T3>
inline typename tools::promote_args<T1, T2, T3>::type 
   hermite_next(unsigned n, T1 x, T2 Hn, T3 Hnm1)
{
   return (2 * x * Hn - 2 * n * Hnm1);
}

namespace detail{

// Implement Hermite polynomials via recurrance:
template <class T>
T hermite_imp(unsigned n, T x)
{
   T p0 = 1;
   T p1 = 2 * x;

   if(n == 0)
      return p0;

   unsigned c = 1;

   while(c < n)
   {
      std::swap(p0, p1);
      p1 = hermite_next(c, x, p0, p1);
      ++c;
   }
   return p1;
}

} // namespace detail

template <class T>
inline typename tools::promote_args<T>::type 
   hermite(unsigned n, T x)
{
   typedef typename tools::promote_args<T>::type result_type;
   typedef typename tools::evaluation<result_type>::type value_type;
   return tools::checked_narrowing_cast<result_type>(detail::hermite_imp(n, static_cast<value_type>(x)), BOOST_CURRENT_FUNCTION);
}

} // namespace math
} // namespace boost

#endif // BOOST_MATH_SPECIAL_HERMITE_HPP

