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

/************************************
 *  Distibuted Voronoi
 ***********************************/

void Server::refine(Server* t) {
// Redefine take a new point to be evaluated and calculates the new Cell
    Point mid = middle(this->loc, t->loc);        // Calulate midpoint

    printf("Mid X: %g Y:%g\n",mid.x(),mid.y());

    // Test insidePolygon()
    if (this->pointInPolygon(mid)){
// TODO Insert t into this->neighbourList
        printf("Inside polygon? YES\n");
    }else{
         printf("Inside polygon? NO\n");
         return;
    }

    Line line = getLine(this->loc, t->loc);
    printf("Line from (%g,%g) to (%g,%g) = %gy + %gx = %g\n",this->loc.x(),this->loc.y(),t->loc.x(),t->loc.y(),line.a,line.b,line.c);

    line = getPerpendic(line, mid);
    printf("PLine from (%g,%g) to (%g,%g) = %gy + %gx = %g\n",this->loc.x(),this->loc.y(),t->loc.x(),t->loc.y(),line.a,line.b,line.c);

    Point intersects[2];
    this->findIntercets(line,intersects);
    this->splitCell(t, intersects);
}

// Addes the two new  Vertexes
void Server::updateCell(Point a) {
    Vertex* pointer = this->cell.origin;
    Vertex *vNode = new Vertex(a);
    vNode->nxt = NULL;

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

void Server::splitCell(Server* t, Point intersect[]) {
    Cell oldCell = this->cell;
    Vertex* pointer = oldCell.origin;
    bool swapped = false;
    // Determine if s i left of intersect-line
    bool Left = isLeft(this->loc, intersect[0], intersect[1]);

    // Clear s.cell
    this->cell.n=0;
    this->cell.origin = NULL;

    // insert the intersects head first
    if (Left) {
        t->updateCell(intersect[1]);
        t->updateCell(intersect[0]);
    }else{
        t->updateCell(intersect[0]);
        t->updateCell(intersect[1]);
    }

    while (pointer != NULL) {
        // Test if the current point is on same side as s
        if (Left == isLeft(pointer->loc, intersect[0], intersect[1])) {
            this->updateCell(pointer->loc);

//            printf("New s vertex (%g,%g)\n",pointer->loc.x(),pointer->loc.y());

        }else{
            // Check to see if it will be the first swap of server
            if (!swapped) {
                if (Left) {
                    this->updateCell(intersect[0]);
                    this->updateCell(intersect[1]);
                }else{
                    this->updateCell(intersect[1]);
                    this->updateCell(intersect[0]);
                }
                swapped = true;
            }
            t->updateCell(pointer->loc);
//            printf("New t vertex (%g,%g)\n",pointer->loc.x(),pointer->loc.y());
        }
        // Point to next vertex
        pointer = pointer->nxt;
    }
}

void Server::findIntercets(Line line, Point tp[]) {
    int i = 0;
    Line cellLine;
    Vertex* pointer = this->cell.origin;    

    while(pointer->nxt != NULL){
        cellLine = getLine(pointer->loc,pointer->nxt->loc);       // Get line segment of polygon

//        get_line_intersection(pointer->loc, pointer->nxt->loc, p0, p1,&tp[i]);
//        printf("Interc (%g,%g) \n", tp[i].x(), tp[i].y());
        tp[i] = intersect(line, cellLine);
        if (collinear(pointer->loc, tp[i], pointer->nxt->loc)) {
            printf("Interc (%g,%g) \n", tp[i].x(), tp[i].y());
            i++;
        }
        pointer = pointer->nxt;
    }

    if (i<2) {
        // Test last cell line segment
        cellLine = getLine(pointer->loc,this->cell.origin->loc);       // Get line segment of polygon
        tp[i] = intersect(line, cellLine);
        printf("Last Interc (%g,%g)\n", tp[i].x(), tp[i].y());
    }
}

void Server::vertsToArray(Point arr[]) {
    int i =0;
    Vertex* pointer = this->cell.origin;

    while (pointer != NULL) {
        arr[i] = pointer->loc;
        pointer = pointer->nxt;
        i++;
    }
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

    Point verts[polySides];
    vertsToArray(verts);

    for (i=0; i<polySides; i++) {
        polyYi = verts[i].y();
        polyXi = verts[i].x();
        polyYj = verts[j].y();
        polyXj = verts[j].x();

        if ( ((polyYi>y) != (polyYj>y)) &&
             (x < (polyXj-polyXi) * (y-polyYi) / (polyYj-polyYi) + polyXi) ) {
            oddNodes =! oddNodes;
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

/***************************************
 *  QuadTree
 **************************************/
Server::Server(double x, double y, Point p1, Point p2) {
    loc = Point(x,y);
    cell.n = 0;
    cell.rect[cell.n++] = p1;
    cell.rect[cell.n++] = p2;
}

void Server::addRect(Point p1, Point p2) {
    double tempx;
    double tempy;
    if (cell.n + 2 > 8) {
        cell.n = 0;
    }

    if (p1.x() > p2.x()) {
        tempx = p1.x();
        p1.setX(p2.x());
        p2.setX(tempx);
    }
    if (p1.y() > p2.y()) {
        tempy = p2.y();
        p2.setY(p1.y());
        p1.setY(tempy);
    }

    cell.rect[cell.n++] = p1;
    cell.rect[cell.n++] = p2;
}

void Server::devide() {
    // Get Rect
    Point p1 = cell.rect[0];
    Point p2 = cell.rect[1];
    this->cell.n = 0;

    // Devide into four rects and add to this.cell
    Point p3 = Point(p2.x(), p1.y());
    Point p4 = Point(p1.x(), p2.y());
    Point p5 = Point((p2.x() + p1.x())/2,(p2.y() + p1.y())/2);

    this->addRect(p1,p5);
    this->addRect(p5,p3);
    this->addRect(p5,p2);
    this->addRect(p4,p5);

    this->loc = Point((p5.x()+p1.x())/2, (p5.y()+p1.y())/2);

}

void Server::transfer(Server *t) {
    if (this->cell.n -2 <= 0) {
        this->devide();
    }
    Point p1 = this->cell.rect[--this->cell.n];
    Point p2 = this->cell.rect[--this->cell.n];
    t->addRect(p1,p2);
}


/****************************************
 *  Geometry functions
 ****************************************/

// Returns the midpoint between two points
Point middle(Point a, Point b) {
    return Point((a.x() + b.x()) / 2.0,
                 (a.y() + b.y()) / 2.0);
}

// Returns true if the point is left or on a ccw line
bool isLeft(Point a, Point b, Point c) {
    return ((b.x() - a.x())*(c.y() - a.y()) - (b.y() - a.y())*(c.x() - a.x())) > 0;
}

bool isOnLine(Point p, Line line) {
    return (p.y() == (line.c-line.b*p.x()));
}

// Returns whether 3 points are collinear and in order a->b->c
bool collinear(Point a, Point b, Point c) {
    if ((a.y() - b.y()) * (a.x() - c.x()) == (a.y() - c.y()) * (a.x() - b.x())) {
        return (a.dist(b) <= a.dist(c) && c.dist(b) <= c.dist(a));
    }
    return false;
}

// Calculates and return the gradient between two points
double grad(Point a, Point b) {
    return 1.0 * (a.y() - b.y()) / (a.x() - b.x());
}

// Return the reciprocal of the grad so that m1*m2=-1
double recip(double m) {
    return -1/m;
}

// Calculates the equation of the line between two lines
Line getLine(Point v, Point w) {
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

Line getPerpendic(Line line, Point at) {
    Line newLine;
    newLine.a = 1.0;                    // A is agian one
    newLine.b = -1*recip(-1*line.b);    // Find the reciprocal of line1
    newLine.c = at.y() + newLine.b*at.x();       // Calculate the equation through the point 'at'

    return newLine;
}

// Calculate the intersection of two lines
// A1x + B1y = C1
// A2x + B2y = C2
Point intersect(Line line1, Line line2) {
    double det = line1.a*line2.b - line2.a*line1.b;     // Calculate the determinant
    double x;
    double y;
    if (det == 0){                      // Lines are parallel
        x = NAN;
        y = NAN;
    }else{                              // Calculate intersection point
        y = (line2.b*line1.c - line1.b*line2.c)/det;
        x = (line1.a*line2.c - line2.a*line1.c)/det;

        if(x==0.0) {
            x = 0.0;
        }
        if (y == 0.0) {
            y = 0.0;
        }
    }

    return Point(x,y);
}


bool get_line_intersection(Point p0, Point p1, Point p2, Point p3, Point* inter) {
        //float p0_x, float p0_y, float p1_x, float p1_y,
    //float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y)

    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1.x() - p0.x();
    s1_y = p1.y() - p0.y();
    s2_x = p3.x() - p2.x();
    s2_y = p3.y() - p2.y();

    float s, t;
    s = (-s1_y * (p0.x() - p2.x()) + s1_x * (p0.y() - p2.y())) / (-s2_x * s1_y + s1_x * s2_y);
    t = ( s2_x * (p0.y() - p2.y()) - s2_y * (p0.x() - p2.x())) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        // Collision detected
        if (inter != NULL)
            inter->setX(p0.x() + (t * s1_x));
            inter->setY(p0.y() + (t * s1_y));
        return true;
    }

    return false; // No collision
}

