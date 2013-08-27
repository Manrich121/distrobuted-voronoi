#ifndef POINT_H
#define POINT_H

#include <iostream>
#include <cmath>

using namespace std;

struct Line {
    // The coeffience to form the line Ay + Bx = C
    double a;
    double b;
    double c;
};

class Point {
private:
    double xval, yval;
public:
    Point();
    Point(double x, double y);
    double x();
    double y();
    double dist(Point other);
    Point operator+(Point b);
    Point operator-(Point b);
    void move(double a, double b);
    bool equal(Point p);

    void print(ostream &strm);
};

#endif // POINT_H
