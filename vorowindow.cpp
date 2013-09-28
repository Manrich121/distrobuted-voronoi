#include "vorowindow.h"
#include "ui_vorowindow.h"
#include <sstream>
#include <stdio.h>
#include "time.h"

int cCount =0;
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

    //Update timer
    this->updateTimer = new QTimer();
    this->updateTimer->setInterval(100);
    connect(this->updateTimer, SIGNAL(timeout()), this, SLOT(clientUpdate()));
//    connect(updateTimer, SIGNAL(timeout()), this, SLOT(checkLoad()));
    updateTimer->start();

    //client add timer
    loadTimer = new QTimer();
    loadTimer->setInterval(500);
    connect(loadTimer, SIGNAL(timeout()), this, SLOT(checkLoad()));
    loadTimer->start();
}

VoroWindow::~VoroWindow()
{
    delete ui;
}

void VoroWindow::paintEvent(QPaintEvent*) {
    int n;
    Vertex* pointer;
    set <Client*>::iterator it;

    QPainter painter(this);
    // Pen
    QPen pen;
    painter.setPen(pen);

    for (int i=0;i<servers.size();i++) {    //loop over servers
        pen.setColor(*c[i]);

        pen.setWidth(4);
        pen.setColor(*c[i]);
        painter.setPen(pen);

        n = servers[i]->cell.n;
        std::vector<Point> vpoints;
        QPoint polyPoints[n];

        painter.drawPoint(pointToQp(servers[i]->loc));

        for (it = servers[i]->myClients.begin(); it != servers[i]->myClients.end(); it++) {
            painter.drawPoint(pointToQp((*it)->loc));
        }

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

    // Insert first server
    servers.push_back(new Server(300,300));
    servers.front()->myClients.insert(new Client((*servers.rbegin())->loc,WIDTH));
    cCount++;
    sCount++;
    p.push_back(Point(WIDTH,0));
    p.push_back(Point(WIDTH,WIDTH));
    p.push_back(Point(0,0));
    p.push_back(Point(0,WIDTH));

    servers.front()->GrahamScan(p);
}

void VoroWindow::addClient() {
    std::string s;
    std::stringstream out;

    if (!(*servers.rbegin())->isLoaded()) {
        (*servers.rbegin())->myClients.insert(new Client((*servers.rbegin())->loc,WIDTH));
        out << ++cCount;
        s = out.str();
        ui->addClients->setText(QString(s.c_str()));
    }

}

void VoroWindow::clientUpdate() {
    vector <Server*>::iterator sit;
    set <Client*>::iterator it;
    set <Client*>::iterator tmp;

    for (sit = this->servers.begin(); sit != this->servers.end(); sit++) {
        for (it = (*sit)->myClients.begin(); it != (*sit)->myClients.end();) {
            tmp = it;
            (*it)->move();
            ++tmp;
            it = tmp;
        }
        (*sit)->checkOwnership();
    }
    update();
}

bool retFlag = false;
void VoroWindow::checkLoad() {
    vector <Server*>::iterator sit;
    Server* curServ;

    retFlag = !retFlag;
    for (int i=0;i<sCount;i++) {    //loop over servers
        curServ = servers[i];

        if (retFlag && curServ->underLoaded()) {
            curServ->returnThisSite();
            servers.erase(servers.begin()+i);
            sCount--;
            break;
        }

        if (curServ->isLoaded() && servers.size() <10) {
            Server* newServer = new Server();
            if(curServ->refine(newServer)){
                servers.push_back(newServer);
                sCount++;
            }

        }
    }
    update();
}

void VoroWindow::on_pushButton_clicked() {
    servers.push_back(new Server(rand()%WIDTH+1,rand()%WIDTH+1));
    servers.front()->refine(servers.back());
    sCount++;
//    setup();
    update();

}

void VoroWindow::on_addClients_clicked()
{
    this->addClient();
}
