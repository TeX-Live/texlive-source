// -*- related-file-name: "../../liblcdf/point.cc" -*-
#ifndef LCDF_POINT_HH
#define LCDF_POINT_HH
#include <math.h>

struct Point {

    double x;
    double y;

    Point()				{ }
    Point(double xx, double yy)		: x(xx), y(yy) { }
    // Point(const Point &)		use compiler default
    Point(const Point &p, double dx, double dy) : x(p.x + dx), y(p.y + dy) { }
    // ~Point()				use compiler default

    inline double squared_length() const throw ();
    inline double length() const throw ();
    inline double magnitude() const throw ();
    static inline double distance(const Point &, const Point &) throw ();
    static inline double dot(const Point &, const Point &) throw ();
    static Point midpoint(const Point &, const Point &) throw ();

    inline double angle() const throw ();

    void shift(double dx, double dy)	{ x += dx; y += dy; }

    inline Point shifted(double dx, double dy) const throw ();
    Point rotated(double) const throw ();
    inline Point normal() const throw ();

    bool on_line(const Point &, const Point &, double) const throw ();
    bool on_segment(const Point &, const Point &, double) const throw ();

    inline Point &operator+=(const Point &) throw ();
    inline Point &operator-=(const Point &) throw ();
    inline Point &operator*=(double) throw ();
    inline Point &operator/=(double) throw ();

    // Point operator+(Point, const Point &);
    // Point operator-(Point, const Point &);
    // Point operator*(Point, double);
    // Point operator/(Point, double);
    // Point operator-(const Point &);

    // bool operator==(const Point &, const Point &);
    // bool operator!=(const Point &, const Point &);

};

inline double
Point::squared_length() const throw ()
{
    return x*x + y*y;
}

inline double
Point::length() const throw ()
{
    return sqrt(x*x + y*y);
}

inline double
Point::magnitude() const throw ()
{
    return length();
}

inline double
Point::angle() const throw ()
{
    return atan2(y, x);
}

inline Point
Point::shifted(double dx, double dy) const throw ()
{
    return Point(x + dx, y + dy);
}

inline Point
Point::normal() const throw ()
{
    double l = length();
    return (l ? Point(x/l, y/l) : *this);
}

inline Point &
Point::operator+=(const Point &p) throw ()
{
    x += p.x;
    y += p.y;
    return *this;
}

inline Point &
Point::operator-=(const Point &p) throw ()
{
    x -= p.x;
    y -= p.y;
    return *this;
}

inline Point &
Point::operator*=(double d) throw ()
{
    x *= d;
    y *= d;
    return *this;
}

inline Point &
Point::operator/=(double d) throw ()
{
    x /= d;
    y /= d;
    return *this;
}

inline bool
operator==(const Point &a, const Point &b) throw ()
{
    return a.x == b.x && a.y == b.y;
}

inline bool
operator!=(const Point &a, const Point &b) throw ()
{
    return a.x != b.x || a.y != b.y;
}

inline Point
operator+(Point a, const Point &b) throw ()
{
    a += b;
    return a;
}

inline Point
operator-(Point a, const Point &b) throw ()
{
    a -= b;
    return a;
}

inline Point
operator-(const Point &a) throw ()
{
    return Point(-a.x, -a.y);
}

inline Point
operator*(Point a, double scale) throw ()
{
    a *= scale;
    return a;
}

inline Point
operator*(double scale, Point a) throw ()
{
    a *= scale;
    return a;
}

inline Point
operator/(Point a, double scale) throw ()
{
    a /= scale;
    return a;
}

inline double
Point::distance(const Point &a, const Point &b) throw ()
{
    return (a - b).length();
}

inline double
Point::dot(const Point &a, const Point &b) throw ()
{
    return a.x*b.x + a.y*b.y;
}

#endif
