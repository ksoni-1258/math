//  Copyright John Maddock 2006.
//  Use, modification and distribution are subject to the
//  Boost Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_STATS_DERIVED_HPP
#define BOOST_STATS_DERIVED_HPP

// This file implements various common properties of distributions
// that can be implemented in terms of other properties:
// variance OR standard deviation (see note below),
// hazard, cumulative hazard (chf), coefficient_of_variation.
//
// Note that while both variance and standard_deviation are provided
// here, each distribution MUST SPECIALIZE AT LEAST ONE OF THESE
// otherwise these two versions will just call each other over and over
// until stack space runs out ...

// Of course there may be more efficient means of implementing these
// that are specific to a particular distribution, but these generic
// versions give these properties "for free" with most distributions.
//
// In order to make use of this header, it must be included AT THE END
// of the distribution header, AFTER the distribution and its core
// property accessors have been defined: this is so that compilers
// that implement 2-phase lookup and early-type-checking of templates
// can find the definitions refered to herein.
//

#include <boost/type_traits/is_same.hpp>
#include <boost/static_assert.hpp>

namespace boost{ namespace math{

template <class Distribution>
typename Distribution::value_type variance(const Distribution& dist);

template <class Distribution>
inline typename Distribution::value_type standard_deviation(const Distribution& dist)
{
   using namespace std;  // ADL of sqrt.
   return sqrt(variance(dist));
}

template <class Distribution>
inline typename Distribution::value_type variance(const Distribution& dist)
{
   typename Distribution::value_type result = standard_deviation(dist);
   return result * result;
}

template <class Distribution, class RealType>
typename Distribution::value_type hazard(const Distribution& dist, const RealType& x)
{ // hazard function
  // http://www.itl.nist.gov/div898/handbook/eda/section3/eda362.htm#HAZ
   typedef typename Distribution::value_type value_type;
   value_type p = cdf(complement(dist, x));
   value_type d = pdf(dist, x);
   if(d > p * tools::max_value<value_type>())
      return tools::overflow_error<value_type>(
         BOOST_CURRENT_FUNCTION, 0);
   if(d == 0)
   {
      // This protects against 0/0, but is it the right thing to do?
      return 0;
   }
   return d / p;
}

template <class Distribution, class RealType>
inline typename Distribution::value_type chf(const Distribution& dist, const RealType& x)
{ // cumulative hazard function.
  // http://www.itl.nist.gov/div898/handbook/eda/section3/eda362.htm#HAZ
   using namespace std;
   return -log(cdf(complement(dist, x)));
}

template <class Distribution>
typename Distribution::value_type coefficient_of_variation(const Distribution& dist)
{
   typedef typename Distribution::value_type value_type;
   value_type m = mean(dist);
   value_type d = standard_deviation(dist);
   if((m < 1) && (d > m * tools::max_value<value_type>()))
      return tools::overflow_error<value_type>(
         BOOST_CURRENT_FUNCTION, 0);
   return d / m;
}
//
// Next follow overloads of some of the standard accessors with mixed
// argument types. We just use a typecast to forward on to the "real"
// implementation with all arguments of the same type:
//
template <class Distribution, class RealType>
inline typename Distribution::value_type pdf(const Distribution& dist, const RealType& x)
{
   typedef typename Distribution::value_type value_type;
   BOOST_STATIC_ASSERT((0 == ::boost::is_same<value_type, RealType>::value));
   return pdf(dist, static_cast<value_type>(x));
}
template <class Distribution, class RealType>
inline typename Distribution::value_type cdf(const Distribution& dist, const RealType& x)
{
   typedef typename Distribution::value_type value_type;
   BOOST_STATIC_ASSERT((0 == ::boost::is_same<value_type, RealType>::value));
   return cdf(dist, static_cast<value_type>(x));
}
template <class Distribution, class RealType>
inline typename Distribution::value_type quantile(const Distribution& dist, const RealType& x)
{
   typedef typename Distribution::value_type value_type;
   BOOST_STATIC_ASSERT((0 == ::boost::is_same<value_type, RealType>::value));
   return quantile(dist, static_cast<value_type>(x));
}
/*
template <class Distribution, class RealType>
inline typename Distribution::value_type chf(const Distribution& dist, const RealType& x)
{
   typedef typename Distribution::value_type value_type;
   BOOST_STATIC_ASSERT((0 == ::boost::is_same<value_type, RealType>::value));
   return chf(dist, static_cast<value_type>(x));
}
*/
template <class Distribution, class RealType>
inline typename Distribution::value_type cdf(const complemented2_type<Distribution, RealType>& c)
{
   typedef typename Distribution::value_type value_type;
   BOOST_STATIC_ASSERT((0 == ::boost::is_same<value_type, RealType>::value));
   return cdf(complement(c.dist, static_cast<value_type>(c.param)));
}

template <class Distribution, class RealType>
inline typename Distribution::value_type quantile(const complemented2_type<Distribution, RealType>& c)
{
   typedef typename Distribution::value_type value_type;
   BOOST_STATIC_ASSERT((0 == ::boost::is_same<value_type, RealType>::value));
   return quantile(complement(c.dist, static_cast<value_type>(c.param)));
}

template <class Dist>
typename Dist::value_type median(const Dist& d)
{ // median - default definition for those distributions for which a
  // simple closed form is not known,
  // and for which a domain_error and/or NaN generating function is NOT defined.
  typedef typename Dist::value_type value_type;
  return quantile(d, static_cast<value_type>(0.5f));
}

} // namespace math
} // namespace boost

#endif // BOOST_STATS_DERIVED_HPP
