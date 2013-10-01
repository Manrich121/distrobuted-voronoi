#ifndef VORONOIDISPLAYER_H
#define VORONOIDISPLAYER_H

#include <QWidget>
#include <QtGui>
#include <vector>
namespace Ui {
class VoronoiDisplayer;
}

class Point;

class VoronoiDisplayer : public QWidget
{
    Q_OBJECT
    
public:
    explicit VoronoiDisplayer(QWidget *parent = 0);
    VoronoiDisplayer(QWidget *parent, std::vector<Point> vPoints);
    ~VoronoiDisplayer();
    void paintEvent(QPaintEvent*);
    QPoint pointToQp(Point p);
    
private:
    Ui::VoronoiDisplayer *ui;
    std::vector<Point> nPoints;
    std::vector<Point> vPoints;
};

#endif // VORONOIDISPLAYER_H
