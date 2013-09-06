#include "quadwindow.h"
#include "ui_quadwindow.h"
#include <sstream>
#include <stdio.h>
#include "time.h"
#include "stdlib.h"

QPushButton *button;
int clientCount =0;
QuadWindow::QuadWindow(QWidget *parent) :QMainWindow(parent),ui(new Ui::QuadWindow)
{
    srand(time(NULL));
    sCount = 0;
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
    c[10] = new QColor(Qt::red);
    c[11] = new QColor(Qt::blue);
    c[12] = new QColor(Qt::black);
    c[13] = new QColor(Qt::magenta);
    c[14] = new QColor(Qt::cyan);
    c[15] = new QColor(Qt::lightGray);

    ui->setupUi(this);

    button = new QPushButton("&AddClient", this);
    connect(button, SIGNAL(clicked()), this, SLOT(addClient()));
    setup();
    update();

    //Update timer
    updateTimer = new QTimer();
    updateTimer->setInterval(50);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(serverUpdate()));
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(handleAreas()));
    updateTimer->start();

    //client add timer
    clientAddTimer = new QTimer();
    clientAddTimer->setInterval(2000);
//    connect(clientAddTimer, SIGNAL(timeout()), this, SLOT(addClient()));
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

        if (servers[i]->lvl == -1) {
            continue;
        }
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

void QuadWindow::remove(Server* s) {
    for (int i=0; i<sCount;i++) {
        if (servers[i]->loc.equal(s->loc)) {
            for (int j=i;j<sCount-1;j++) {
                servers[j] = servers[j+1];
            }
            sCount--;
            break;
        }
    }
}

void QuadWindow::setup() {
    servers[sCount] = new Server(500,500, Point(0,0), Point(1000,1000));
    servers[sCount]->myClients.insert(new Client(servers[sCount]->loc,1000));
    servers[sCount]->myClients.insert(new Client(servers[sCount]->loc,1000));
    clientCount++;
    clientCount++;
    sCount++;
}


void QuadWindow::addClient() {
    std::string s;
    std::stringstream out;

    if (!servers[sCount-1]->isLoaded()) {
        servers[sCount-1]->myClients.insert(new Client(servers[sCount-1]->loc,1000));
        out << ++clientCount;
        s = out.str();
        button->setText(QString(s.c_str()));
    }

    if (clientCount >=6) {
        clientAddTimer->stop();
    }

}

void QuadWindow::serverUpdate() {
    set <Client*>::iterator it;
    set <Client*>::iterator tmp;
    int numServers = sCount;
    for (int s=0;s <numServers;s++) {
        for (it = servers[s]->myClients.begin(); it != servers[s]->myClients.end();) {
            tmp = it;
            (*it)->move();
            ++tmp;
            it = tmp;
        }

        servers[s]->checkOwership();

    }
    update();
}

void QuadWindow::handleAreas() {

    for (int s=0;s<sCount;s++) {
        if (servers[s]->underLoaded()) {
            if (servers[s]->returnArea()) {
                this->remove(servers[s]);
                break;
            }

        }

        if (servers[s]->isLoaded()) {
            servers[sCount] = new Server();
            if (servers[s]->transfer(servers[sCount])) {
                sCount++;
                break;
            }
        }
    }
    update();
}
