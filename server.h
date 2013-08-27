#ifndef SERVER_H
#define SERVER_H

#include "point.h"
#include <queue>            // std::queue


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
    void refine(Server* t);   // calculate the new cell after intercection of the halfspace between the
                                        // current server and new point
    void send_msg(Server t, Point p);   // Enqueues the point to the Server t's que
//    Point third(Vertex v, Point a, Point b);    // Returns the point assosiated with v
                                                // that is not a or b
    void updateCell(Point a);
    void recalculateCell(Server* t, Point intersect[]);
    Point* findIntercets(Line line);
    Point* vertsToArray();

    // Geometry functions
    Point mid(Point a, Point b);
    bool isLeftOrOn(Point a, Point b, Point c);
    double grad(Point a, Point b);
    double recip(double m);
    Line getLine(Point a, Point b);
    Line getPerpendic(Line line, Point at);
    Point intersect(Line line1, Line line2);
    bool pointInPolygon(Point p);
    bool ccw(Point p[], int n);

// Params
    Point loc;
    Cell cell;
    std::queue<Point> que;

};

#endif // SERVER_H
