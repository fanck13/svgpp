// Copyright Daniel Wallin, David Abrahams 2010. Use, modification and
// distribution is subject to the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef EXBOOST_PARAMETER_IS_MAYBE_050329_HPP_EXBOOST
#define EXBOOST_PARAMETER_IS_MAYBE_050329_HPP_EXBOOST

#include <boost/type_traits/is_base_and_derived.hpp>

namespace exboost { using namespace boost;
namespace parameter {
namespace aux {

struct maybe_base {};

template <class T>
struct is_maybe
  : is_base_and_derived<maybe_base, T>
{};

} // namespace aux
} // namespace parameter
} // namespace exboost

#endif // EXBOOST_PARAMETER_IS_MAYBE_050329_HPP_EXBOOST