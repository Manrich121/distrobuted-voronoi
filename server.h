#ifndef SERVER_H
#define SERVER_H

#include <set>
#include "point.h"

/*****************************************************************************
 *  Server class contains methods used in both the distributed Voronoi
 *  and QuadTree implementations.
 *
 *  Description:
 *  Params:
 *  - Point loc => The current location of the server within the virtual
 *                  environment
 *  - Cell => A cell struct that contians the following for each implementation
 *      Voronoi:
 *      - int n => The number of Vertex's
 *      - Vertex* origin => Pointer to first Vertex defining a polygon
 *
 *      QuadTree:
 *      - int n => The index of Rectangles owned by the current server (#Rects = n/2)
 *      - rect[8] => a Point array, size 8 containing sets of two Points, each
 *                  defining a rectangle. (Max of four Rectangles)
 *                  ie. rec[0:1], rec[2:3], rec[4:5], rec[6:7]
 *  - neighbor => stdlib set of neighbours of the current server
 ******************************************************************************/

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
    Vertex* origin;         // pointer to origin of polygon Assume counter clockwise sequence
    Point rect[8];           // Sets of two points each defining a rectangle
};

class Server
{
public:
    Server();
    Server(double x, double y);

    /**********************
     *  Distributed Voronoi
     **********************/
    void refine(Server *t);     // calculate the new cell after intercection of the halfspace between
                                // the current server and new point
    void updateCell(Point a);
    void splitCell(Server *t, Point intersect[]);
    void findIntercets(Line line, Point tp[]);
    void vertsToArray(Point arr[]);

    // Polygon functions
    bool pointInPolygon(Point p);
    bool ccw(Point p[], int n);

    /**********************
     *  QuadTree
     **********************/
    Server(double x, double y, Point p1, Point p2);
    void addRect(Point p1, Point p2);
    void devide();            // Devide current rectangle into four and move location to top left rect
    void transfer(Server* t); // Transfer one of the current server's most loaded rectangles to t

// Params
    Point loc;
    Cell cell;
    std::set<Server*> neighbor;
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
