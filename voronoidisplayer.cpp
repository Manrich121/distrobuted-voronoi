#include "voronoidisplayer.h"
#include "ui_voronoidisplayer.h"
#include "point.h"

VoronoiDisplayer::VoronoiDisplayer(QWidget *parent) :QWidget(parent),ui(new Ui::VoronoiDisplayer)
{
    ui->setupUi(this);
}

VoronoiDisplayer::VoronoiDisplayer(QWidget *parent, std::vector<Point> vPoints):QWidget(parent),ui(new Ui::VoronoiDisplayer)
{
    parent = NULL;
    ui->setupUi(this);
    this->vPoints = vPoints;
    update();
}

VoronoiDisplayer::~VoronoiDisplayer()
{
    delete ui;
}

void VoronoiDisplayer::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    // Pen
    QPen pen;
    pen.setWidth(5);
    painter.setPen(pen);

    for (int i=0;i<vPoints.size();i++) {    //loop over servers
        painter.drawPoint(pointToQp(vPoints[i]));

    }

    pen.setWidth(2);
    painter.setPen(pen);
    for (int i=0;i<vPoints.size()-1;i++){
        painter.drawLine(vPoints[i].x(), vPoints[i].y(), vPoints[i+1].x(), vPoints[i+1].y());
    }
}

QPoint VoronoiDisplayer::pointToQp(Point p) {
    return QPoint(p.x(), p.y());
}
