#include "vorowindow.h"
#include "ui_vorowindow.h"


VoroWindow::VoroWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::VoroWindow)
{
    sCount = 0;
    srand(time(NULL));
    c[0] = new QColor(Qt::red);
    c[1] = new QColor(Qt::blue);
    c[2] = new QColor(Qt::black);
    c[3] = new QColor(Qt::magenta);
    c[4] = new QColor(Qt::cyan);
    c[5] = new QColor(Qt::lightGray);
    c[6] = new QColor(Qt::darkMagenta);
    c[7] = new QColor(Qt::darkYellow);
    c[8] = new QColor(Qt::darkRed);
    c[9] = new QColor(Qt::yellow);


    ui->setupUi(this);
    setup();
    update();

}

VoroWindow::~VoroWindow()
{
    delete ui;
}

void VoroWindow::paintEvent(QPaintEvent*) {
    int n;
    Vertex* pointer;

    QPainter painter(this);
    // Pen
    QPen pen;
    painter.setPen(pen);

    for (int i=0;i<sCount;i++) {    //loop over servers
        pen.setColor(*c[i]);

        pen.setWidth(4);
        pen.setColor(*c[i]);
        painter.setPen(pen);

        n = servers[i]->cell.n;
        std::vector<Point> vpoints;
        QPoint polyPoints[n];

        painter.drawPoint(pointToQp(servers[i]->loc));

        // Draw current server cell
        servers[i]->vertsToVector(&vpoints);

        pointer = servers[i]->cell.origin;
        if (pointer != NULL) {
            for (int j=0;j<n;j++) {
                polyPoints[j] = this->pointToQp(vpoints[j]);


//                printf("Server %d: (%g,%g)\n",i,vpoints[j].x(),vpoints[j].y());
            }
            pen.setWidth(1);
            painter.drawPolygon(polyPoints,n);
        }
    }
}

QPoint VoroWindow::pointToQp(Point p) {
    return QPoint(p.x(), p.y());
}

void VoroWindow::setup() {
    std::vector<Point> p;
    vector<Vector> *points = new vector<Vector>;
    vector<Vector> *convexHull = new vector<Vector>;
    float znew = 0.0f;
    float wnew = 1.0f;

    // Insert first server
    servers.push_back(new Server(rand()%WIDTH+1,rand()%WIDTH+1));
    p.push_back(Point(WIDTH,0));
    p.push_back(Point(WIDTH,WIDTH));
    p.push_back(Point(0,0));
    p.push_back(Point(0,WIDTH));
    p.push_back(Point(0,WIDTH/2));

//    points->push_back(Vector(600,0,znew,wnew));
//    points->push_back(Vector(0,0,znew,wnew));
//    points->push_back(Vector(0,600,znew,wnew));
//    points->push_back(Vector(600,600,znew,wnew));
//    points->push_back(Vector(100,600,znew,wnew));

    if(!ConvexHullAlgs::GrahamsScan(convexHull, points))
        printf("error computing convex hull\n");

    servers[sCount]->GrahamScan(p);
    sCount++;

    servers.push_back(new Server(rand()%WIDTH+1,rand()%WIDTH+1));
    servers[sCount-1]->refine(servers.back());
    sCount++;

    servers.push_back(new Server(rand()%WIDTH+1,rand()%WIDTH+1));
    servers[sCount-1]->refine(servers.back());
    sCount++;

//    servers.push_back(new Server(rand()%WIDTH+1,rand()%WIDTH+1));
//    servers[sCount-1]->refine(servers.back());
//    sCount++;

//    servers.push_back(new Server(350,100));
//    servers[sCount-3]->refine(servers[sCount]);
//    servers[sCount-2]->refine(servers[sCount]);
//    servers[sCount-1]->refine(servers[sCount]);
//    sCount++;
}

void VoroWindow::on_pushButton_clicked() {
    sCount =0;
    servers.clear();
    setup();
    update();
}
