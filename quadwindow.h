#ifndef QUADWINDOW_H
#define QUADWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include "server.h"
#include "client.h"

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
public slots:
    void addClient();
    void serverUpdate();
    
private:
    Ui::QuadWindow *ui;
    QTimer* updateTimer;
    QTimer* clientAddTimer;
    Server* servers[16];
    int sCount;
    QColor* c[16];
};

#endif // QUADWINDOW_H
