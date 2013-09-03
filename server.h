#ifndef SERVER_H
#define SERVER_H

#include "point.h"

class Vertex {
public:
    Vertex(Point a){
        loc = a;
        nxt = NULL;
    }

    Point loc;
    Vertex* nxt;            // Pointer to next vertex in polygon
};

struct Cell {
    int n;
    Vertex* origin;         // pointer to origin of polygon
                            // Assume counter clockwise sequence
};

class Server
{
public:
    Server();
    Server(double x, double y);
    void refine(Server *t);   // calculate the new cell after intercection of the halfspace between the
                                        // current server and new point
    void updateCell(Point a);
    void splitCell(Server *t, Point intersect[]);
    void findIntercets(Line line, Point tp[]);
    void vertsToArray(Point arr[]);

    // Polygon functions
    bool pointInPolygon(Point p);
    bool ccw(Point p[], int n);

// Params
    Point loc;
    Cell cell;
};

// Geometry functions
Point middle(Point a, Point b);
bool isLeft(Point a, Point b, Point c);
bool isOnLine(Point a, Line line);
bool collinear(Point a, Point b, Point c);
double grad(Point a, Point b);
double recip(double m);
Line getLine(Point a, Point b);
Line getPerpendic(Line line, Point at);
Point intersect(Line line1, Line line2);
bool get_line_intersection(Point p0, Point p1, Point p2, Point p3, Point* inter);


#endif // SERVER_H
