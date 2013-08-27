#ifndef VOROWINDOW_H
#define VOROWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include "server.h"

namespace Ui {
class VoroWindow;
}

class VoroWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit VoroWindow(QWidget *parent = 0);
    ~VoroWindow();
    void paintEvent(QPaintEvent*);
    QPoint pointToQp(Point p);
    void setup();
    
private:
    Ui::VoroWindow *ui;
    Server* servers;
    int sCount;
};

#endif // VOROWINDOW_H
