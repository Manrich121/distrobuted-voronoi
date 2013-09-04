#ifndef QUADWINDOW_H
#define QUADWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include "server.h"

namespace Ui {
class QuadWindow;
}

class QuadWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit QuadWindow(QWidget *parent = 0);
    ~QuadWindow();
    void paintEvent(QPaintEvent*);
    QPoint pointToQp(Point p);
    void setup();
    
private:
    Ui::QuadWindow *ui;
    Server* servers[10];
    int sCount;
    QColor* c[10];
};

#endif // QUADWINDOW_H
