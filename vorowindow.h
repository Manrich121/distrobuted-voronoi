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
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::VoroWindow *ui;
    std::vector<Server*> servers;
    int sCount;
    QColor* c[10];
};

#endif // VOROWINDOW_H
