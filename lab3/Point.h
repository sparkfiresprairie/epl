#if !(_Point_h)
#define _Point_h 1

#include <cmath>

#ifndef HUGE /* a useful constant (a large floating point number) */
# define HUGE MAXFLOAT
#endif /* HUGE */

#ifndef M_PI
#	define M_PI ((double) 3.1415926535897932)
#endif 

class Point {

public:
  static const double tolerance;

  double xpos, ypos;
  Point(void);
  Point(const Point& p);
  Point(double,double);
  Point& operator=(const Point& p);
  Point operator+(const Point& p) const;
  Point& operator+=(const Point& p);
  Point& operator-=(const Point& p);
  Point& operator*=(double s);
  Point& operator/=(double s);
  bool operator==(const Point& p) const;
  bool operator!=(const Point& p) const { return !operator==(p); }
  double distance(const Point& p) const;
  double bearing(const Point& p) const; // the direction from 'this' to 'p'
};

inline 
Point& Point::operator=(const Point& p)
{ 
  xpos = p.xpos; 
  ypos = p.ypos; 
  return *this;
}

inline
Point::Point(const Point& p) { xpos = p.xpos; ypos = p.ypos; }

inline
Point::Point(void) { xpos = ypos = 0.0; }

inline 
Point::Point(double x, double y) { xpos = x; ypos = y; }

inline
double Point::distance(const Point& p) const 
{
  double xdist = (xpos - p.xpos);
  double xdist_sqrd = xdist*xdist;
  
  double ydist = (ypos - p.ypos);
  double ydist_sqrd = ydist*ydist;
  
  return sqrt(xdist_sqrd + ydist_sqrd);
}

inline
Point Point::operator+(const Point& p) const
{
	Point temp(xpos, ypos) ;
	temp += p ;
	return temp;
}
				
inline
Point& Point::operator+=(const Point& p)
{
  xpos += p.xpos;
  ypos += p.ypos;
  return *this;
}

inline
Point& Point::operator-=(const Point& p)
{
  xpos -= p.xpos;
  ypos -= p.ypos;
  return *this;
}

inline
Point& Point::operator*=(double s)
{
  xpos *= s;
  ypos *= s;
  return *this;
}

inline
Point& Point::operator/=(double s)
{
  xpos /= s;
  ypos /= s;
  return *this;
}

inline
bool Point::operator==(const Point& p) const
{
  return distance(p) < tolerance;
}

inline
double Point::bearing(const Point& p) const
{
  assert(*this != p);
  double del_x = p.xpos - xpos;
  double del_y = p.ypos - ypos;
  if (fabs(del_y) <= fabs(1.0e-10 * del_x)) { // delta y is essentially 0
    if (del_x > 0.0) return 0.0;
    else return M_PI;
  }
  else { /* atan returns the correct result if delta x is positive */
    if (del_x > 0.0)
      return atan(del_y / del_x);
    else
      return atan(del_y / del_x) + M_PI;
  }
}


/* NOTE: this ifdef is correct only for g++.
   This is an implementation-dependent hack */
#ifdef _IOSTREAM_H
inline 
ostream& operator<<(ostream& ost, const Point& p)
{
  ost << "(" << p.xpos << "," << p.ypos << ")";
  return ost;
}
#endif /* _IOSTREAM_H */

#endif /* !(_Point_h) */
