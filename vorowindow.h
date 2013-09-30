#ifndef VOROWINDOW_H
#define VOROWINDOW_H


#include <set>
#include <QMainWindow>
#include <QtGui>
#include <QGraphicsItem>
#include "server.h"
#include "client.h"

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

public slots:
    void clientUpdate();
    void addClient();
    void checkLoad();
private slots:
    void on_pushButton_clicked();
    void on_addClients_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::VoroWindow *ui;
    std::vector<Server*> servers;
    int sCount;
    QColor* c[10];
    QTimer* updateTimer;
    QTimer* loadTimer;
};

#endif // VOROWINDOW_H
