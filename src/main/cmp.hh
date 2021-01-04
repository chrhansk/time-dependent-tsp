#ifndef CMP_HH
#define CMP_HH

#include <math.h>

namespace cmp
{
  extern double eps;

  inline bool eq(double x, double y)
  {
    return fabs(x - y) <= eps;
  }

  inline bool gt(double x, double y)
  {
    return ((x) - (y)) > eps;
  }

  inline bool ge(double x, double y)
  {
    return ((x) - (y)) >= -eps;
  }

  inline bool le(double x, double y)
  {
    return ((x) - (y)) <= eps;
  }

  inline bool lt(double x, double y)
  {
    return ((x) - (y)) < -eps;
  }

  inline bool zero(double x)
  {
    return fabs(x) <= eps;
  }

  inline bool pos(double x)
  {
    return (x) > eps;
  }

  inline bool neg(double x)
  {
    return (x) < -eps;
  }
}


#endif /* CMP_HH */
