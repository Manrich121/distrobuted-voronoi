#include "quadwindow.h"
#include "ui_quadwindow.h"
#include <sstream>
#include <stdio.h>
#include "time.h"
#include "stdlib.h"

int clientCount =0;
QuadWindow::QuadWindow(QWidget *parent) :QMainWindow(parent),ui(new Ui::QuadWindow)
{
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
    c[10] = new QColor(Qt::red);
    c[11] = new QColor(Qt::blue);
    c[12] = new QColor(Qt::black);
    c[13] = new QColor(Qt::magenta);
    c[14] = new QColor(Qt::cyan);
    c[15] = new QColor(Qt::lightGray);

    ui->setupUi(this);

    setup();
    update();

    //Update timer
    updateTimer = new QTimer();
    updateTimer->setInterval(50);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(clientUpdate()));
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
    int i= 0;
    std::string s;

    set <Client*>::iterator it;
    set <Server*>::iterator sit;

    QPainter painter(this);
    // Pen
    QPen pen;
    for (sit = this->servers.begin(); sit != this->servers.end(); sit++) {
        std::stringstream out;
        out << (*sit)->lvl;
        s = out.str();

        pen.setWidth(5);
        pen.setColor(*c[i]);
        painter.setPen(pen);

        if ((*sit)->lvl == -1) {
            continue;
        }
        painter.drawText(pointToQp((*sit)->loc),QString(s.c_str()));

        for (it = (*sit)->myClients.begin(); it != (*sit)->myClients.end(); it++) {
            painter.drawPoint(pointToQp((*it)->loc));
        }


        n = (*sit)->cell.n;
        for (int j=0;j<n;j++) {
            QPoint p1 = pointToQp((*sit)->cell.rect[j]->topLeft);
            QPoint p2 = pointToQp((*sit)->cell.rect[j]->botRight);

            pen.setWidth(2);
            painter.setPen(pen);
            painter.drawRect(QRect(p1,p2));
        }
        i++;
    }

//    QRect r = QRect(QPoint(400,400),QPoint(0,0));

//    painter.drawRect(r);
}

QPoint QuadWindow::pointToQp(Point p) {
    return QPoint(p.x(), p.y());
}

void QuadWindow::setup() {
    Server* curServ = new Server(500,500);
    curServ->master = true;
    curServ->addRect(Point(0,0), Point(1000,1000));
    curServ->myClients.insert(new Client(curServ->loc,1000));
    clientCount++;
    curServ->myClients.insert(new Client(curServ->loc,1000));
    clientCount++;
    servers.insert(curServ);
}


void QuadWindow::addClient() {
    std::string s;
    std::stringstream out;

    if (!(*servers.rbegin())->isLoaded()) {
        (*servers.rbegin())->myClients.insert(new Client((*servers.rbegin())->loc,1000));
        out << ++clientCount;
        s = out.str();
        ui->addClients->setText(QString(s.c_str()));
    }

    if (clientCount >=6) {
        clientAddTimer->stop();
    }

}

void QuadWindow::removeClient() {
    set <Server*>::iterator sit;
    Server* curServ;
    std::string s;
    std::stringstream out;

    for (sit = this->servers.begin(); sit != this->servers.end(); sit++) {
        curServ = (*sit);
        if (!curServ->myClients.empty()) {
            curServ->myClients.erase(*curServ->myClients.rbegin());
            out << --clientCount;
            s = out.str();
            ui->addClients->setText(QString(s.c_str()));
        }
    }
}


void QuadWindow::clientUpdate() {
    set <Server*>::iterator sit;
    set <Client*>::iterator it;
    set <Client*>::iterator tmp;

    for (sit = this->servers.begin(); sit != this->servers.end(); sit++) {
        for (it = (*sit)->myClients.begin(); it != (*sit)->myClients.end();) {
            tmp = it;
            (*it)->move();
            ++tmp;
            it = tmp;
        }

        (*sit)->checkOwership();

    }
    update();
}

void QuadWindow::handleAreas() {
    set <Server*>::iterator sit;
    Server* curServ;

    for (sit = this->servers.begin(); sit != this->servers.end(); sit++) {
        curServ = (*sit);

        if (curServ->underLoaded()) {
            if (curServ->returnArea()) {
                servers.erase((curServ));
                break;
            }
        }
//        if (servers[s]->underLoaded()) {
//            if (servers[s]->returnArea()) {
//                this->remove(servers[s]);
//                break;
//            }
//        }

        if (curServ->isLoaded() && servers.size() <16) {
            Server* newServ = new Server();
            if (curServ->transfer(newServ)) {
                servers.insert(newServ);
                break;
            }
        }
    }
    update();
}

void QuadWindow::on_addClients_clicked()
{
    this->addClient();

}

void QuadWindow::on_removeClient_clicked()
{
    this->removeClient();
}
