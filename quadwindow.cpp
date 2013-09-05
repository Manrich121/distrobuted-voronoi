#include "quadwindow.h"
#include "ui_quadwindow.h"
#include <sstream>
#include <stdio.h>
#include "time.h"
#include "stdlib.h"

QuadWindow::QuadWindow(QWidget *parent) :QMainWindow(parent),ui(new Ui::QuadWindow)
{
    srand(time(NULL));
    sCount = 0;
    c[0] = new QColor(Qt::red);
    c[1] = new QColor(Qt::blue);
    c[2] = new QColor(Qt::black);
    c[3] = new QColor(Qt::yellow);
    c[4] = new QColor(Qt::cyan);
    c[5] = new QColor(Qt::lightGray);
    c[6] = new QColor(Qt::darkMagenta);
    c[7] = new QColor(Qt::darkYellow);
    c[8] = new QColor(Qt::darkRed);
    c[9] = new QColor(Qt::magenta);
    ui->setupUi(this);
    setup();
    update();

    //Update timer
    updateTimer = new QTimer();
    updateTimer->setInterval(50);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(serverUpdate()));
    updateTimer->start();

    //client add timer
    clientAddTimer = new QTimer();
    clientAddTimer->setInterval(2000);
    connect(clientAddTimer, SIGNAL(timeout()), this, SLOT(addClient()));
    clientAddTimer->start();
}

QuadWindow::~QuadWindow()
{
    delete ui;
}


void QuadWindow::paintEvent(QPaintEvent*) {
    int n;
    std::string s;

    set <Client*>::iterator it;

    QPainter painter(this);
    // Pen
    QPen pen;
    for (int i=0;i<sCount;i++) {
        std::stringstream out;
        out << i;
        s = out.str();

        pen.setWidth(5);
        pen.setColor(*c[i]);
        painter.setPen(pen);

        painter.drawText(pointToQp(servers[i]->loc),QString(s.c_str()));
        for (it = servers[i]->myClients.begin(); it != servers[i]->myClients.end(); it++) {
            painter.drawPoint(pointToQp((*it)->loc));
        }


        n = servers[i]->cell.n;
        for (int j=0;j<n;j++) {
            QPoint p1 = pointToQp(servers[i]->cell.rect[j]->topLeft);
            QPoint p2 = pointToQp(servers[i]->cell.rect[j]->botRight);

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
    sCount++;


//    bool in = servers[sCount-1]->insideArea(Point(100,200));
//    printf("Inside %d",in);

//    servers[sCount] = new Server();
//    servers[sCount-1]->transfer(servers[sCount]);
//    sCount++;

//    servers[sCount] = new Server();
//    servers[sCount-1]->transfer(servers[sCount]);
//    sCount++;

//    servers[sCount] = new Server();
//    servers[sCount-3]->transfer(servers[sCount]);
//    sCount++;

//    servers[sCount] = new Server();
//    servers[sCount-3]->transfer(servers[sCount]);
//    sCount++;

//    for(int i=0; i<sCount;i++) {
//        servers[i]->printNeighbourLocs();
//        printf("\n");
//    }

}

int clientCount =0;
void QuadWindow::addClient() {
    if (!servers[sCount-1]->isLoaded()) {
        servers[sCount-1]->myClients.insert(new Client(servers[sCount-1]->loc,400));
        clientCount++;
    }

    if (clientCount >=10) {
        clientAddTimer->stop();
    }

}

void QuadWindow::serverUpdate() {
    set <Client*>::iterator it;
    int numServers = sCount;
    for (int s=0;s <numServers;s++) {
        for (it = servers[s]->myClients.begin(); it != servers[s]->myClients.end(); it++) {
            (*it)->move();
            servers[s]->ownership(*it);
        }

        if (servers[s]->isLoaded()) {
            servers[sCount] = new Server();
            servers[s]->transfer(servers[sCount]);
            sCount++;
        }
    }
    update();
}
