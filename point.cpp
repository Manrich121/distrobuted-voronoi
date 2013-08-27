#include "point.h"

Point::Point(){}

Point::Point(double x = 0.0, double y = 0.0) {
        xval = x;
        yval = y;
}
// Extractors
double Point::x() { return xval; }
double Point::y() { return yval; }

// Distance to another point.  Pythagorean thm.
double Point::dist(Point other) {
    double xd = xval - other.xval;
    double yd = yval - other.yval;
    return sqrt(xd*xd + yd*yd);
}

// Add or subtract two points.
Point Point::operator+(Point b)
{
    return Point(xval + b.xval, yval + b.yval);
}
Point Point::operator-(Point b)
{
    return Point(xval - b.xval, yval - b.yval);
}

// Move the existing point.
void Point::move(double a, double b)
{
    xval += a;
    yval += b;
}

// Print the point on the stream.  The class ostream is a base class
// for output streams of various types.
void Point::print(ostream &strm)
{
    strm << "(" << xval << "," << yval << ")";
}

// Retruns true if the two points are equal
bool Point::equal(Point p) {
    return (this->x() == p.x()) && (this->y() == p.y());
}
