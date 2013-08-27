#include <cstdio>
#include "server.h"
#include "point.h"

Server::Server(){
}

Server::Server(double x, double y)
{
    loc = Point(x,y);
    cell.n = 0;
    cell.origin = NULL;
}

void Server::refine(Server* t) {
// Redefine take a new point to be evaluated and calculates the new Cell
    Point mid = this->mid(this->loc, t->loc);        // Calulate midpoint

    printf("Mid X: %g Y:%g\n",mid.x(),mid.y());

    // Test insidePolygon()
    if (this->pointInPolygon(mid)){
// TODO Insert t into this->neighbourList
        printf("Inside polygon? YES\n");
    }else{
         printf("Inside polygon? NO\n");
         return;
    }

    Line line = this->getLine(this->loc, t->loc);
    printf("Line from (%g,%g) to (%g,%g) = %gy + %gx = %g\n",this->loc.x(),this->loc.y(),t->loc.x(),t->loc.y(),line.a,line.b,line.c);

    line = this->getPerpendic(line, mid);
    printf("PLine from (%g,%g) to (%g,%g) = %gy + %gx = %g\n",this->loc.x(),this->loc.y(),t->loc.x(),t->loc.y(),line.a,line.b,line.c);

    Point* intersects = this->findIntercets(line);

}

void Server::send_msg(Server t, Point p){       // enqueue the next point to be evaluated
    t.que.push(p);
}

//Point Server::third(Vertex v, Point a, Point b) {
//    for (int i=0; i < 3; i++) {
//        if (!v.points[i].equal(a) && !v.points[i].equal(b)) {
//            return v.points[i];
//        }
//    }
//    return Point();
//}

// Addes the two new  Vertexes
void Server::updateCell(Point a) {
    Vertex* pointer = this->cell.origin;
    Vertex *vNode = new Vertex(a);
    if (pointer == NULL) {
        this->cell.origin = vNode;
    }else{
        while (pointer->nxt != NULL) {           // Loop till end of list
            pointer = pointer->nxt;
        }
        pointer->nxt = vNode;                   // Add new vertex
    }
    this->cell.n++;
}

void Server::recalculateCell(Server* t, Point intersect[2]) {
    Cell oldCell = this->cell;
    this->cell.n=0;
    this->cell.origin = NULL;

    // Test if server is left of intersect-line, used to test ccw
    bool left = this->isLeftOrOn(this->loc, intersect[0], intersect[1]);

    // Loop
    if (this->isLeftOrOn(oldCell.origin->loc, intersect[0], intersect[1]) == left) {

    }
}

Point* Server::findIntercets(Line line) {
    int i = 0;
    Line cellLine;
    Vertex* pointer = this->cell.origin;
    Point* tp = new Point[2];

    while(pointer->nxt != NULL){
        cellLine = this->getLine(pointer->loc,pointer->nxt->loc);       // Get line segment of polygon
        tp[i] = this->intersect(line, cellLine);
        if (this->pointInPolygon(tp[i])) {
            printf("Interc (%g,%g) \n", tp[i].x(), tp[i].y());
            i++;
        }
        pointer = pointer->nxt;
    }

    if (i<2) {
        // Test last cell line segment
        cellLine = this->getLine(pointer->loc,this->cell.origin->loc);       // Get line segment of polygon
        tp[i] = this->intersect(line, cellLine);
        printf("Interc (%g,%g)\n", tp[i].x(), tp[i].y());
    }
    return tp;
}

Point* Server::vertsToArray() {
    int i =0;
    Point *arr = new Point[this->cell.n];       // create new pointer of array, memalloc with new
    Vertex* pointer = this->cell.origin;

    while (pointer != NULL) {
        arr[i] = pointer->loc;
        pointer = pointer->nxt;
        i++;
    }
    return arr;
}


/****************************************
 *  Geometry functions
 ****************************************/

// Returns the midpoint between two points
Point Server::mid(Point a, Point b) {
    return Point((a.x() + b.x()) / 2.0,
                 (a.y() + b.y()) / 2.0);
}

// Returns true if the point is left or on a ccw line
bool Server::isLeftOrOn(Point a, Point b, Point c) {
    return ((b.x() - a.x())*(c.y() - a.y()) - (b.y() - a.y())*(c.x() - a.x())) >= 0;
}

// Calculates and return the gradient between two points
double Server::grad(Point a, Point b) {
    return 1.0 * (a.y() - b.y()) / (a.x() - b.x());
}

// Return the reciprocal of the grad so that m1*m2=-1
double Server::recip(double m) {
    return -1/m;
}

// Calculates the equation of the line between two lines
Line Server::getLine(Point v, Point w) {
    Line line;
    line.a = 1.0;                       // A is always chose as one
    line.b = -1*grad(v,w);              // B is equal to m, the gradiant of the two lines
    if (line.b == INFINITY || line.b == -INFINITY) {
        line.a = 0;
        line.b = 1;
    }
    line.c =  line.a*w.y() + line.b*v.x();     // C is eqaul to m(-x1) + y1

    return line;
}

Line Server::getPerpendic(Line line, Point at) {
    Line newLine;
    newLine.a = 1.0;                    // A is agian one
    newLine.b = -1*recip(-1*line.b);    // Find the reciprocal of line1
    newLine.c = at.y() + newLine.b*at.x();       // Calculate the equation through the point 'at'

    return newLine;
}

// Calculate the intersection of two lines
// A1x + B1y = C1
// A2x + B2y = C2
Point Server::intersect(Line line1, Line line2) {
    double det = line1.a*line2.b - line2.a*line1.b;     // Calculate the determinant
    double x;
    double y;
    if (det == 0){                      // Lines are parallel
        x = NAN;
        y = NAN;
    }else{                              // Calculate intersection point
        y = (line2.b*line1.c - line1.b*line2.c)/det;
        x = (line1.a*line2.c - line2.a*line1.c)/det;
    }

    return Point(x,y);
}

//  Globals which should be set before calling this function:
//
//  int    polySides  =  how many corners the polygon has
//  float  polyX[]    =  horizontal coordinates of corners
//  float  polyY[]    =  vertical coordinates of corners
//  float  x, y       =  point to be tested
//
//  (Globals are used in this example for purposes of speed.  Change as
//  desired.)
//
//  The function will return YES if the point x,y is inside OR ON the polygon, or
//  NO if it is not.  If the point is exactly on the edge of the polygon,
//  then the function may return YES or NO.
//
//  Note that division by zero is avoided because the division is protected
//  by the "if" clause which surrounds it.

bool Server::pointInPolygon(Point p) {
    int polySides = this->cell.n;
    float x = p.x();
    float y = p.y();
    float polyYi, polyYj;
    float polyXi, polyXj;
    int i, j = polySides-1;
    bool oddNodes = false;

    Point* verts = vertsToArray();

    for (i=0; i<polySides; i++) {
        polyYi = verts[i].y();
        polyXi = verts[i].x();
        polyYj = verts[j].y();
        polyXj = verts[j].x();

        if (((polyYi<= y && polyYj>=y) ||   (polyYj<= y && polyYi>=y)) &&  (polyXi<=x || polyXj<=x)) {
            if (polyXi+(y-polyYi)/(polyYj-polyYi)*(polyXj-polyXi)<=x) {
                oddNodes=!oddNodes;
            }
        }

        j=i;
    }

    return oddNodes;
}

// Takes an array of atleast 3 points and returns if they are ccw
// **Ref: Stackoverflow, Beta: Math - How to determine if a list of polygon points are in clockwise order?
// http://stackoverflow.com/questions/1165647/how-to-determine-if-a-list-of-polygon-points-are-in-clockwise-order
bool Server::ccw(Point p[], int n) {
    double sum = 0;

    // Sum over (x2-x1)(y2+y1)
    for (int i=0;i<n-1;i++) {
        sum += (p[i+1].x()-p[i].x())*(p[i+1].y()+p[i].y());
    }
    return sum <=0;
}



