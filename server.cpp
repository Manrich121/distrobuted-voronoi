#include <cstdio>
#include "server.h"
#include "point.h"


Server::Server(){
    loc = Point(0,0);
    lvl = -1;           // Set lvl to -1 indicationg that it has not been assignedS
    cell.n = 0;
    childCount = 0;
    cell.origin = NULL;
    parent = NULL;
    master = false;
}

Server::Server(double x, double y)
{
    loc = Point(x,y);
    lvl = 0;
    cell.n = 0;
    childCount = 0;
    cell.origin = NULL;
    parent = NULL;
    master = false;
}

bool Server::isLoaded() {
    return this->myClients.size()>MAXCLIENTS;
}

bool Server::underLoaded() {
    return (this->lvl !=-1 && this->myClients.size()<MINCLIENTS && this->cell.n==1);
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

/*
 *  Adds a new Rectangle, enforcing topLeft and botRight Rectangle format
 *  p1-------
 *  |       |
 *  |       |
 *  -------p2
 */
void Server::addRect(Point p1, Point p2) {
    double tempx;
    double tempy;
    if (cell.n == 4) {
        return;
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

    this->cell.rect.insert(new Rectangle(p1,p2));
    cell.n++;
}

void Server::addRect(Rectangle* r) {
    this->addRect(r->topLeft, r->botRight);
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

    if (this->lvl == 2) {
        return false;
    }
        // Get Rect
        Point p1 = (*cell.rect.begin())->topLeft;
        Point p2 = (*cell.rect.begin())->botRight;
        this->cell.n = 0;
        this->cell.rect.clear();

        // Devide into four rects and add to this.cell
        Point p3 = Point(p2.x(), p1.y());
        Point p4 = Point(p1.x(), p2.y());
        Point p5 = Point((p2.x() + p1.x())/2,(p2.y() + p1.y())/2);

        // Add all four rectangles
        this->addRect(p1,p5);
        this->addRect(p5,p3);
        this->addRect(p5,p2);
        this->addRect(p4,p5);

        // Set location in topLeft rect and increase lvl
        this->loc = Point((p5.x()+p1.x())/2, (p5.y()+p1.y())/2);
        this->lvl++;
        return true;
}

/*
 *  Determines if four rectangles can merge into one. Returns true if successful, else false
 */

bool Server::merge() {
    std::set<Rectangle*>::iterator it;
    Rectangle* curRect;
    Rectangle* newR = (*cell.rect.begin());

    if (this->cell.n != 4) {
        return false;
    }

    // Find the rectangle that is the largest
    for (it = this->cell.rect.begin(); it != cell.rect.end();it++) {
        curRect = (*it);
        if (newR->topLeft.x() >= curRect->topLeft.x() && newR->topLeft.y() >= curRect->topLeft.y()) {
            newR->topLeft = curRect->topLeft;
        }
        if (newR->botRight.x() <= curRect->botRight.x() && newR->botRight.y() <= curRect->botRight.y()) {
            newR->botRight = curRect->botRight;
        }
    }

#ifdef _DEBUG
    if (this->master) {
        if (!(newR->topLeft.equal(Point(0,0)) && newR->botRight.equal(Point(500,500)))) {
            printf("bug");
        }
    }
#endif

    this->cell.n=0;
    this->cell.rect.clear();
    this->addRect(newR);
    this->lvl--;
    this->loc = Point((newR->topLeft.x()+newR->botRight.x())/2, (newR->topLeft.y()+newR->botRight.y())/2);
    return true;
}

/*
 *  Transfers the a Rectangle to t, returns true is successful.
 */

bool Server::transfer(Server *t) {
    if (this->cell.n == 1) {
        if (!this->devide()) {
            return false;
        }
    }

    Rectangle* curRect = (*this->cell.rect.begin());
    Point p1 = curRect->topLeft;
    Point p2 = curRect->botRight;

    this->cell.rect.erase(curRect);
    this->cell.n--;

    t->addRect(p1,p2);
    t->lvl = this->lvl;

    t->loc = Point((p1.x()+p2.x())/2, (p1.y()+p2.y())/2);

    t->parent = this;
    this->childCount++;

    t->addAdjacent(this);

    this->checkOwership();

//    while (this->isLoaded()) {
//        curRect = (*this->cell.rect.rbegin());
//        this->cell.rect.erase(curRect);
//        this->cell.n--;
//        t->addRect(curRect);
//        t->addAdjacent(this);
//        this->checkOwership();
//    }


    // test all neighbours possible adjacent
    set <Server*>::iterator it;
    for(it = this->neighbor.begin(); it != this->neighbor.end(); it++) {
        if ((*it)!=t) {
            t->addAdjacent(*it);
            (*it)->neighbor.erase(this);
            (*it)->addAdjacent(this);
        }
    }

    return true;
}

bool Server::returnArea() {
    set <Server*>::iterator it;
    set <Client*>::iterator cit;

    if (this->parent!=NULL && this->parent->lvl == this->lvl && childCount == 0 ) {
        Rectangle* curR = (*this->cell.rect.begin());

        // Remove self from parent
        this->parent->childCount--;
        this->parent->addRect(curR);

        this->parent->merge();


        // Transfer all myClients
        for (cit = this->myClients.begin(); cit != this->myClients.end();cit++) {
            this->parent->myClients.insert(*cit);
        }

        this->parent->checkOwership();

        // Remove me from all neighbour lists
        for(it = this->neighbor.begin(); it != this->neighbor.end(); it++) {
            this->parent->addAdjacent(*it);
            (*it)->neighbor.erase(this);
        }

        // Set lvl be deleted
        this->lvl = -1;
        return true;
    }
    return false;
}

/*
 *  Determines id the test point tp is inside or on the border of the area owned
 */

bool Server::insideArea(Point* tp) {
    std::set<Rectangle*>::iterator it;
    for (it = this->cell.rect.begin(); it != cell.rect.end();it++) {
        if (inRect(tp,(*it))) {
            return true;
        }
    }
    return false;
}

/*
 *  Tests if the Server t is adjacent and adds to neigbour list
 */

void Server::addAdjacent(Server* t) {
    std::set<Rectangle*>::iterator it;
    Rectangle* curRect;

    for (it = this->cell.rect.begin(); it != cell.rect.end();it++) {
        curRect = (*it);
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
    bool found = false;

    if (this->insideArea(&c->loc)) {
        return;
    }

    set <Server*>::iterator it;
    for(it = this->neighbor.begin(); it != this->neighbor.end(); it++) {
        if ((*it)->insideArea(&c->loc)) {
            found = true;
            (*it)->myClients.insert(c);
            this->myClients.erase(c);
        }
    }

    if (!found) {
        printf("bug");
    }
}


void Server::checkOwership() {
    set <Client*>::iterator it;
    set <Client*> tmp = myClients;

    for (it = tmp.begin(); it != tmp.end();) {
        this->ownership(*it);
        ++it;
    }
}

void Server::printNeighbourLocs(){
    set <Server*>::iterator it;
    printf("My loc (%g,%g)\n",this->loc.x(),this->loc.y());
    for(it = this->neighbor.begin(); it != this->neighbor.end(); it++) {
        printf("N's loc (%g,%g)\n",(*it)->loc.x(),(*it)->loc.y());
    }
}
