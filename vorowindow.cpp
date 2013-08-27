#include "vorowindow.h"
#include "ui_vorowindow.h"


VoroWindow::VoroWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::VoroWindow)
{
    servers = new Server[5];
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
    QPainter painter(this);

    // Pen
    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::blue);
    painter.setPen(pen);

    QPointF points[sCount];
    for (int i=0;i<sCount;i++) {
        int n = servers[i].cell.n;
        points[i] = pointToQp(servers[i].loc);

        // Draw current server cell
//        QPoint polyPoints[n];
//        for (int j=0; j<n; j++) {
//            polyPoints[j] = pointToQp(servers[i].cell.verts[j].loc);
//        }
//        painter.drawPolygon(polyPoints,4);

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
    // Insert first server
    servers[sCount] = Server(150,150);

    // Construct cell ccw and assign cell to 1st server
    servers[sCount].updateCell(Point(0,0));
    servers[sCount].updateCell(Point(400,0));
    servers[sCount].updateCell(Point(400,400));
    servers[sCount].updateCell(Point(0,400));

    sCount++;

    servers[sCount] = Server(100,100);

//    servers[sCount-1].refine(servers[sCount].loc);
    sCount++;
//    servers[sCount-1].refine(servers[sCount].loc);
//    sCount++;


}
