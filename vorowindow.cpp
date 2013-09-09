#include "vorowindow.h"
#include "ui_vorowindow.h"


VoroWindow::VoroWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::VoroWindow)
{
    sCount = 0;

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
    pen.setWidth(2);
    pen.setColor(Qt::blue);
    painter.setPen(pen);

    QPoint points[sCount];


    for (int i=0;i<sCount;i++) {    //loop over servers

        n = servers[i]->cell.n;
        Point vpoints[n];
        QPoint polyPoints[n];
        points[i] = pointToQp(servers[i]->loc);

        // Draw current server cell
        servers[i]->vertsToArray(vpoints);

        pointer = servers[i]->cell.origin;
        if (pointer != NULL) {
            for (int j=0;j<n;j++) {
                polyPoints[j] = this->pointToQp(vpoints[j]);


//                printf("Server %d: (%g,%g)\n",i,vpoints[j].x(),vpoints[j].y());
            }
            painter.drawPolygon(polyPoints,n);
        }
    }

    pen.setWidth(5);
    pen.setColor(Qt::red);
    painter.setPen(pen);

    // Draw server locs
    painter.drawPoints(points,sCount);

}

QPoint VoroWindow::pointToQp(Point p) {
    return QPoint(p.x(), 400-p.y());
}



void VoroWindow::setup() {
    std::vector<Point> p;
    // Insert first server
    servers[sCount] = new Server(150,150);
    p.push_back(Point(400,0));
    p.push_back(Point(0,0));
    p.push_back(Point(200,200));
    p.push_back(Point(400,400));
    p.push_back(Point(0,400));

    servers[sCount]->GrahamSort(p);
    sCount++;

}
