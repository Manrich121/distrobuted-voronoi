#ifndef SERVER_H
#define SERVER_H

#include <set>          // std::set
#include <list>         // std::list
#include <vector>       // std::vector
#include <algorithm>    // std::sort
#include "point.h"
#include "client.h"
#include "ConvexHullAlgs.h"

#include "VoronoiDiagramGenerator.h"

// Defines
#define MAXCLIENTS 5
#define MINCLIENTS 1
#define WIDTH 600
#define EPS 0.1

#define _DEBUG

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
 *      - rect => a List of Rectangles
 *  - neighbor => stdlib set of neighbours of the current server
 ******************************************************************************/

class Vertex {
public:
    Vertex(Point a){
        loc = a;
        next = NULL;
    }

    Point loc;
    Vertex* next;            // Pointer to next vertex in polygon
    Vertex* prev;
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
    double rmax;
    Vertex* origin;             // pointer to origin of polygon Assume counter clockwise sequence
    std::list<Rectangle*> rect;  // A Set of point Rectangle objects each defining a rectangle
};

class Server
{
public:
    Server();
    Server(double x, double y);

    void printNeighbourLocs();
    bool isLoaded();
    bool underLoaded();

    /**********************
     *  Distributed Voronoi
     **********************/
    void generateVoronoi(std::vector<Point> *points, std::vector<Point> *lineseg);

    void GrahamSort(std::vector<Point> points);
    void GrahamScan(std::vector<Point> p);
    void vertsToVector(std::vector<Point> *v);
    void findIntersects(Line line, std::vector<Point> *ip);
    void checkNeighbours();
    bool isNeigh(Server* t);

    void refine(Server *t);     // calculate the new cell after intercection of the halfspace between
                                // the current server and new point
    void transferPoints(std::vector<Point> iPoints, std::vector<Point> *tPoints);

    // Polygon functions
    void addVertex(Point a);
    void clearCell();
    void deleteVertex(Vertex* v);
    bool pointInPolygon(Point p);
    std::vector<Point> *RemoveDup(std::vector<Point> v);

    /**********************
     *  QuadTree
     **********************/
    Server(double x, double y, Point p1, Point p2);
    void addRect(Point p1, Point p2);
    void addRect(Rectangle *r);
    bool devide();              // Devide current rectangle into four and move location to top left rect
    bool merge();
    bool transfer(Server* t);   // Transfer one of the current server's most loaded rectangles to t
    bool returnArea();          // Selects less loaded neighbour in same lvl, else on lvl up;
    bool insideArea(Point* tp);  // Determines if the test point tp is inside the area of current server
    void addAdjacent(Server* t); // Determines if the Server t, and all its neighbours is adjacent to this, thus is a neighbour
    void ownership(Client* c);
    void checkOwership();

// Params
    Point loc;
    int lvl;
    Cell cell;
    Server* parent;
    int childCount;
    std::set<Server*> neighbours;
    std::set<Client*> myClients;


#ifdef _DEBUG
    bool master;
#endif

};

bool ccw(Point p[], int n);
bool ccw(Point p1, Point p2, Point p3);
bool inRect(Point* tp, Rectangle r);
void insertIntoVec(std::vector<Point> *v, Point p);

#endif // SERVER_H
