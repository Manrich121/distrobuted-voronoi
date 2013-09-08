#ifndef QUADWINDOW_H
#define QUADWINDOW_H

#include <set>
#include <QMainWindow>
#include <QtGui>
#include <QGraphicsItem>
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
    void removeClient();
    void clientUpdate();
    void checkLoad();
private slots:
    void on_addClients_clicked();
    void on_removeClient_clicked();

private:

    Ui::QuadWindow *ui;
    QTimer* updateTimer;
    QTimer* loadTimer;
    std::set<Server*> servers;
    QColor* c[16];
};

#endif // QUADWINDOW_H
