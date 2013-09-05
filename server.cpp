#include <cstdio>
#include "server.h"
#include "point.h"

#define MAXCLIENTS 5;

Server::Server(){
    loc = Point(0,0);
    lvl = 0;
    cell.n = 0;
    cell.origin = NULL;
    maxClients = MAXCLIENTS;
}

Server::Server(double x, double y)
{
    loc = Point(x,y);
    lvl = 0;
    cell.n = 0;
    cell.origin = NULL;
    maxClients = MAXCLIENTS;
}

bool Server::isLoaded() {
    return this->myClients.size()>this->maxClients;
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
bool ccw(Point p[], int n) {
    double sum = 0;

    // Sum over (x2-x1)(y2+y1)
    for (int i=0;i<n-1;i++) {
        sum += (p[i+1].x()-p[i].x())*(p[i+1].y()+p[i].y());
    }
    return sum <=0;
}

bool inRect(Point* tp, Rectangle* r) {
    if (tp->x() >= r->topLeft.x() && tp->x() <= r->botRight.x()) {
        if (tp->y() >= r->topLeft.y() && tp->y() <= r->botRight.y()) {
            return true;
        }
    }
    return false;
}

/***************************************
 *  QuadTree
 **************************************/
Server::Server(double x, double y, Point p1, Point p2) {
    loc = Point(x,y);
    cell.n = 0;
    lvl = 0;
    maxClients=MAXCLIENTS;
    this->addRect(p1,p2);
}

void Server::addRect(Point p1, Point p2) {
    double tempx;
    double tempy;
    if (cell.n + 1 > 4) {
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

    this->cell.rect[cell.n++] = new Rectangle(p1,p2);
}

/*
 *  Devide the current rectangle into four and return if true if successful.
 *  |  1  |  2  |
 *  -------------
 *  |  4  |  3  |
 *
 *  Can only devide upto level 2.
 */

bool Server::devide() {

    if (this->lvl < 2) {
        // Get Rect
        Point p1 = cell.rect[0]->topLeft;
        Point p2 = cell.rect[0]->botRight;
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
        this->lvl++;
        return true;
    }
    return false;
}

/*
 *  Transfers the last owned Rectangle to t, returns true is successful.
 */

bool Server::transfer(Server *t) {
    if (this->cell.n -1 <= 0) {
        bool success = this->devide();
        if (!success) {
            return false;
        }
    }
    int n = --this->cell.n;
    Point p1 = this->cell.rect[n]->topLeft;
    Point p2 = this->cell.rect[n]->botRight;
    t->loc = Point((p1.x()+p2.x())/2, (p1.y()+p2.y())/2);
    t->lvl = this->lvl;
    t->addRect(p1,p2);

    t->addAdjacent(this);

    this->checkOwership();

    // test all neighbours possible adjacent
    set <Server*>::iterator it;
    for(it = this->neighbor.begin(); it != this->neighbor.end(); it++) {
        if ((*it)!=t) {
            t->addAdjacent(*it);
        }
    }
    return true;
}

/*
 *  Determines id the test point tp is inside or on the border of the area owned
 */

bool Server::insideArea(Point* tp) {
    for (int i = 0; i < this->cell.n;i++){
        if (inRect(tp,this->cell.rect[i])) {
            return true;
        }
    }
    return false;
}

/*
 *  Tests if the Server t is adjacent and adds to neigbour list
 */

void Server::addAdjacent(Server* t) {
    int n = t->cell.n;          // get t's number of rects
    Rectangle* curRect;

    for (int i=0; i <n;i++) {
        curRect = t->cell.rect[i];
        if (this->insideArea(&curRect->topLeft) || this->insideArea(&curRect->botRight) ||
                this->insideArea(new Point(curRect->topLeft.x(),curRect->botRight.y())) ||
                this->insideArea(new Point(curRect->botRight.x(),curRect->topLeft.y()))) {
            this->neighbor.insert(t);
            t->neighbor.insert(this);
            break;
        }
    }
}

void Server::ownership(Client* c) {

    if (this->insideArea(&c->loc)) {
        return;
    }

    set <Server*>::iterator it;
    for(it = this->neighbor.begin(); it != this->neighbor.end(); it++) {
        if ((*it)->insideArea(&c->loc)) {
            (*it)->myClients.insert(c);
            this->myClients.erase(c);
        }
    }

}


void Server::checkOwership() {
    set <Client*>::iterator it;
    for (it = this->myClients.begin(); it != this->myClients.end(); it++) {
        this->ownership(*it);
    }
}

void Server::printNeighbourLocs(){
    set <Server*>::iterator it;
    printf("My loc (%g,%g)\n",this->loc.x(),this->loc.y());
    for(it = this->neighbor.begin(); it != this->neighbor.end(); it++) {
        printf("N's loc (%g,%g)\n",(*it)->loc.x(),(*it)->loc.y());
    }
}
