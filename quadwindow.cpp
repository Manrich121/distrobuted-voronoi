#include "quadwindow.h"
#include "ui_quadwindow.h"

QuadWindow::QuadWindow(QWidget *parent) :QMainWindow(parent),ui(new Ui::QuadWindow)
{
    sCount = 0;
    c[0] = new QColor(Qt::red);
    c[1] = new QColor(Qt::blue);
    c[2] = new QColor(Qt::black);
    c[3] = new QColor(Qt::yellow);
    c[4] = new QColor(Qt::green);
    ui->setupUi(this);
    setup();
    update();
}

QuadWindow::~QuadWindow()
{
    delete ui;
}


void QuadWindow::paintEvent(QPaintEvent*) {
    int n;
    QPainter painter(this);
    // Pen
    QPen pen;
    for (int i=0;i<sCount;i++) {

        pen.setWidth(5);
        pen.setColor(*c[i]);
        painter.setPen(pen);

        painter.drawPoint(pointToQp(servers[i]->loc));
        n = servers[i]->cell.n;
        for (int j=0;j<n;j+=2) {
            QPoint p1 = pointToQp(servers[i]->cell.rect[j]);
            QPoint p2 = pointToQp(servers[i]->cell.rect[j+1]);

            pen.setWidth(2);
            painter.setPen(pen);
            painter.drawRect(QRect(p1,p2));
        }


    }

//    QRect r = QRect(QPoint(400,400),QPoint(0,0));

//    painter.drawRect(r);
}

QPoint QuadWindow::pointToQp(Point p) {
    return QPoint(p.x(), p.y());
}

void QuadWindow::setup() {
    servers[sCount] = new Server(200,200, Point(0,0), Point(400,400));
    servers[sCount]->devide();
    sCount++;

    servers[sCount] = new Server();
    servers[sCount-1]->transfer(servers[sCount]);
    servers[sCount]->devide();
    sCount++;

    servers[sCount] = new Server();
    servers[sCount-2]->transfer(servers[sCount]);
    sCount++;

}
