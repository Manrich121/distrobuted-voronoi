#include <cstdio>
#include "server.h"
#include "point.h"


Server::Server(){
    loc = Point(0,0);
    lvl = -1;           // Set lvl to -1 indicationg that it has not been assigned
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
    std::vector<Point> iPoints;
    std::vector<Point> tPoints;

    if (this->isNeigh(t)) {


        // Redefine take a new point to be evaluated and calculates the new Cell
        Point mid = middle(this->loc, t->loc);        // Calulate midpoint
        Line line = getLine(this->loc, t->loc);
        line = getPerpendic(line, mid);
        this->findIntersects(line, &iPoints);

        this->transferPoints(iPoints, &tPoints);

        set <Server*>::iterator it;
        set <Server*> tmpNeig = this->neighbours;
        for(it = tmpNeig.begin(); it != tmpNeig.end(); it++) {
            Server* curServ = (*it);
            if (curServ->isNeigh(t)) {
                iPoints.clear();
                curServ->neighbours.insert(t);
                t->neighbours.insert(curServ);

                mid = middle(curServ->loc, t->loc);        // Calulate midpoint
                Line line = getLine(curServ->loc, t->loc);
                line = getPerpendic(line, mid);
                curServ->findIntersects(line, &iPoints);

                curServ->transferPoints(iPoints, &tPoints);
            }
        }

        this->neighbours.insert(t);
        t->neighbours.insert(this);

        t->clearCell();
//        t->GrahamSort(tPoints);
        t->GrahamScan(tPoints);
    }
}

void Server::transferPoints(std::vector<Point> iPoints, std::vector<Point> *tPoints) {
    std::vector<Point> myPoints;
    if (iPoints.size() == 2){
        if (ccw(this->loc, iPoints[0], iPoints[1])){
            Point tmp = iPoints[0];
            iPoints[0] = iPoints[1];
            iPoints[1] = tmp;
        }
        tPoints->push_back(iPoints[0]);
        tPoints->push_back(iPoints[1]);
        myPoints.push_back(iPoints[0]);
        myPoints.push_back(iPoints[1]);

        // Loop though this.cell and transfer points to myPoints and tPoints
        Vertex* pointer = this->cell.origin;
        bool iamLeft = isLeft(this->loc, iPoints[0], iPoints[1]);
        while (pointer != NULL) {
            if (iamLeft && isLeft(pointer->loc, iPoints[0], iPoints[1])) {
                myPoints.push_back(pointer->loc);
            }else{
                tPoints->push_back(pointer->loc);
            }
            pointer = pointer->next;
        }

        this->clearCell();
//        this->GrahamSort(myPoints);
        this->GrahamScan(myPoints);
    }
}

/*
 *  Adds a Vertex to this.cell incrementing cell.n and calculate cell.rmax as the maximum distance
 *  from this.loc and the new Vertex
 */
void Server::addVertex(Point a) {
    Vertex* pointer = this->cell.origin;
    Vertex* vNode = new Vertex(a);
    vNode->next = NULL;

    if (pointer == NULL) {          // Cell not yet init
        this->cell.origin = vNode;
        this->cell.origin->prev = NULL;
        this->cell.n = 0;
        this->cell.rmax = 0;
    }else{
        if (pointer->prev == NULL) {    // Cell only contains one Vertex=origin
            if (pointer->loc.equal(vNode->loc)) {
                return;
            }
            pointer->next = vNode;      // Add new Vertex
            vNode->prev = pointer;
        }else{
            pointer = pointer->prev;    // Jump to end of polygon
            if (collinear(pointer->prev->loc, pointer->loc, vNode->loc)){
                pointer->prev->next = vNode;
                vNode->prev = pointer->prev;
                delete pointer;
            }
            if (pointer->loc.equal(vNode->loc)) {
                return;
            }

            pointer->next = vNode;      // Add new Vertex
            vNode->prev = pointer;
        }
        this->cell.origin->prev = vNode;
    }
    this->cell.n++;

    // Recalc rmax
    double newR = this->loc.dist(a);    // calculate radius to new point
    if (newR > this->cell.rmax) {
        this->cell.rmax = newR;
    }
}

void Server::clearCell(){
    if (this->cell.origin == NULL) {
        return;
    }
    deleteVertex(this->cell.origin);
    this->cell.origin = NULL;
    this->cell.n = 0;
}

void Server::deleteVertex(Vertex* v) {
    if (v->next == NULL) {
        delete(v);
    }else{
        deleteVertex(v->next);
    }
}

void Server::checkNeighbours() {
    set <Server*>::iterator it;
    set <Server*> tmpNeig = this->neighbours;
    for(it = tmpNeig.begin(); it != tmpNeig.end(); it++) {
        if (!this->isNeigh(*it)) {              // If not neigbour anymore
            this->neighbours.erase(*it);
            (*it)->neighbours.erase(this);
        }
    }
}

bool Server::isNeigh(Server* t) {
    Point mid = middle(this->loc, t->loc);        // Calulate midpoint
//    if (this->pointInPolygon(mid)) {
//        return true;
//    }
//    double midDist = mid.dist(t->loc);
//    Vertex* pointer = this->cell.origin;

//    while (pointer != NULL) {
//        if (pointer->loc.dist(t->loc) <= midDist) {
//            return true;
//        }
//        pointer = pointer->next;
//    }

//    return false;
    return (this->loc.dist(mid) <= this->cell.rmax);
}

void Server::findIntersects(Line line, std::vector<Point> *ip) {
    Line cellLine;
    Vertex* pointer = this->cell.origin;
    Point* tp;

    while(pointer->next != NULL){                   // While not at end of polygon
        cellLine = getLine(pointer->loc, pointer->next->loc);
        tp = intersect(line, cellLine);
        if (tp == NULL ) {
            pointer = pointer->next;
            continue;
        }
        // (a.dist(b) <= a.dist(c) && c.dist(b) <= c.dist(a))
        if (pointer->loc.dist(*tp) <= pointer->loc.dist(pointer->next->loc) &&
                pointer->next->loc.dist(*tp) <= pointer->next->loc.dist(pointer->loc)) {
            if (ip->size() <2) {
                ip->push_back(*tp);
            }else{
                return;
            }
        }
        pointer = pointer->next;
    }
    if (ip->size() <2) {
        // Last line segment
        cellLine = getLine(pointer->loc,this->cell.origin->loc);
        tp = intersect(line, cellLine);
        if (tp != NULL) {
            if (pointer->loc.dist(*tp) <= pointer->loc.dist(this->cell.origin->loc) &&
                    this->cell.origin->loc.dist(*tp) <= this->cell.origin->loc.dist(pointer->loc)) {
                ip->push_back(*tp);
            }
        }
    }
}

/*
 *  Construct a simple polygon by ustilising the sorting step of the Graham scan algorithm.
 *  Given a array of points, and sets this.cell equal to a simple ccw polygon
 *  First select the bottom-leftmost point l then sorts all following points ccw around L
 *  **Ref:  J. Erickson, “Lecture: Convex Hulls,” 2008.
 *          Available: www.cs.uiuc.edu/jeffe/teaching/compgeom/notes/01-convexhull.pdf
 */
void Server::GrahamSort(std::vector<Point> points) {
    int lpos = 0;
    Point l = points[lpos];
    Point tmp, p1, p2;

    // Find top-leftmost point
    for (unsigned int i=1;i<points.size();i++) {
        if (points[i].x() <= l.x()) {     // Second mininum x
            l = points[i];
            lpos = i;
        }
    }
    // Remove l from vector points
    points.erase(points.begin()+lpos);

    this->addVertex(l);

    unsigned i;
    // Sort
    for (i = 0;i <points.size()-1;i++) {
        for(unsigned j = i+1; j<points.size(); j++) {
            p1 = points[i];
            p2 = points[j];
            if (!ccw(l, points[i], points[j])){     // If not ccw, swap so that it is counter clock wise;
                tmp = points[i];
                points[i] = points[j];
                points[j] = tmp;
            }
        }
        this->addVertex(points[i]);
    }
    this->addVertex(points[i]);
}

void Server::GrahamScan(std::vector<Point> p) {
    vector<Vector> *points = new vector<Vector>;
    vector<Vector> *convexHull = new vector<Vector>;
    float znew = 0.0f;
    float wnew = 1.0f;

    for (unsigned int i=0; i< p.size();i++) {
        points->push_back(Vector(p[i].x(),p[i].y(), znew, wnew));
    }

    if(!ConvexHullAlgs::GrahamsScan(convexHull, points))
        printf("error computing convex hull\n");
    for (unsigned int i=0;i<convexHull->size();i++){
        this->addVertex(Point(convexHull->at(i).x,convexHull->at(i).y));
    }

}

void Server::vertsToVector(std::vector<Point> *v) {
    Vertex* pointer = this->cell.origin;

    while (pointer != NULL) {
        v->push_back(pointer->loc);
        pointer = pointer->next;
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

    std::vector<Point> verts;
    vertsToVector(&verts);

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

/*
 *  Determines if the tree points p1(a,b), p2(c,d) p3(e,f) are ccw, by looking at the slop of
 *  line (a,b)(c,d) vs line (a,b)(e,f)
 *  ccw <=> (f - b)(c - a)<(d - b)(e - a)
 *  **Ref: J. Erickson, “Lecture: Convex Hulls,” 2008.
 *  www.cs.uiuc.edu/~jeffe/teaching/compgeom/notes/01-convexhull.pdf
 */
bool ccw(Point p1, Point p2, Point p3) {
    return ((p3.y() - p1.y())*(p2.x() - p1.x())) < ((p2.y() - p1.y())*(p3.x() - p1.x()));
}

bool inRect(Point* tp, Rectangle* r) {
    if (tp->x() >= r->topLeft.x() && tp->x() <= r->botRight.x()) {
        if (tp->y() >= r->topLeft.y() && tp->y() <= r->botRight.y()) {
            return true;
        }
    }
    return false;
}


void insertIntoVec(std::vector<Point> *v, Point p) {
    Point tmp;
    for (unsigned int i=0;i<v->size();i++) {
        tmp = v->at(i);
        if (tmp.equal(p)) {
            return;
        }
    }

    v->push_back(p);
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

    this->cell.rect.push_back(new Rectangle(p1,p2));
    cell.n++;
}

void Server::addRect(Rectangle* r) {
    this->addRect(r->topLeft, r->botRight);
}

/*
 *  Devide the current rectangle into four and return true if successful.
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
    std::list<Rectangle*>::iterator it;
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
            printf("Master bug");
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

    Rectangle* curRect = (*this->cell.rect.rbegin());
    Point p1 = curRect->topLeft;
    Point p2 = curRect->botRight;

    this->cell.rect.pop_back();
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
//        this->cell.rect.pop_back();
//        this->cell.n--;
//        t->addRect(curRect);
//        t->addAdjacent(this);
//        this->checkOwership();
//    }


    // test all neighbours possible adjacent
    set <Server*>::iterator it;
    set <Server*> tmpNeig = this->neighbours;
    for(it = tmpNeig.begin(); it != tmpNeig.end(); it++) {
        if ((*it)!=t) {
            t->addAdjacent(*it);
            (*it)->neighbours.erase(this);
            (*it)->addAdjacent(this);
            this->neighbours.erase(*it);
            this->addAdjacent(*it);
        }
    }

#ifdef _DEBUG
    if (this->neighbours.size() > 8) {
        this->printNeighbourLocs();
    }
#endif

    return true;
}

bool Server::returnArea() {
    set <Server*>::iterator it;
    set <Client*>::iterator cit;

    if (this->parent!=NULL && this->parent->lvl == this->lvl && this->childCount == 0 ) {
        Rectangle* curR = (*this->cell.rect.rbegin());

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
        for(it = this->neighbours.begin(); it != this->neighbours.end(); it++) {
            this->parent->addAdjacent(*it);
            (*it)->neighbours.erase(this);
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
    std::list<Rectangle*>::iterator it;
    for (it = this->cell.rect.begin(); it != cell.rect.end();it++) {
        if (inRect(tp,(*it))) {
            return true;
        }
    }
    return false;
}

/*
 *  Tests if the Server t is adjacent and adds to neigbour list
 *   a  |   b   |   c
 *      |       |
 *  ----p1-----p3----
 *  h   |   t   |   d
 *      |       |
 *  ----p4-----p2----
 *  g   |   f   |   e
 *      |       |
 */

void Server::addAdjacent(Server* t) {
    std::list<Rectangle*>::iterator rit;
    Rectangle* tRect;
    bool neigh = false;

    if (this == t) {
        return;
    }

    Point *p3, *p4;
    for (rit = t->cell.rect.begin(); rit != t->cell.rect.end(); rit++) {
        tRect = (*rit);
        p3 = new Point(tRect->botRight.x(),tRect->topLeft.y());
        p4 = new Point(tRect->topLeft.x(), tRect->botRight.y());

        if (this->insideArea(&tRect->topLeft) || this->insideArea(&tRect->botRight) ||
                this->insideArea(p3) || this->insideArea(p4)) {
            neigh = true;
            break;
        }

    }

    if (neigh) {
        this->neighbours.insert(t);
        t->neighbours.insert(this);
    }
}

void Server::ownership(Client* c) {
    bool found = false;

    if (this->insideArea(&c->loc)) {
        return;
    }

    set <Server*>::iterator it;
    for(it = this->neighbours.begin(); it != this->neighbours.end(); it++) {
        if ((*it)->insideArea(&c->loc)) {
            found = true;
            (*it)->myClients.insert(c);
            this->myClients.erase(c);
        }
    }
#ifdef _DEBUG
    if (!found) {
        printf("Client bug");
    }
#endif
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
    for(it = this->neighbours.begin(); it != this->neighbours.end(); it++) {
        printf("N's loc (%g,%g)\n",(*it)->loc.x(),(*it)->loc.y());
    }
}

