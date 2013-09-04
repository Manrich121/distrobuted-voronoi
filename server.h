#ifndef SERVER_H
#define SERVER_H

#include <set>
#include "point.h"
#include "client.h"

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
 *      - int n => The index of Rectangles owned by the current server
 *      - rect[4] => array of rectangle structs
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

class Rectangle {
public:
    Rectangle(Point tL, Point bR) {
        topLeft = tL;
        botRight = bR;
    }

    Point topLeft;
    Point botRight;
};

struct Cell {
    int n;
    Vertex* origin;         // pointer to origin of polygon Assume counter clockwise sequence
    Rectangle* rect[4];           // Sets of two points each defining a rectangle
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
    bool insideArea(Point tp);

// Params
    Point loc;
    int lvl;
    Cell cell;
    std::set<Server*> neighbor;

    Client* myClients[];
    int maxClients;
};

bool inRect(Point tp, Rectangle r);

#endif // SERVER_H
