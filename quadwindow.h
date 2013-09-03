#ifndef QUADWINDOW_H
#define QUADWINDOW_H

#include <QMainWindow>

namespace Ui {
class QuadWindow;
}

class QuadWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit QuadWindow(QWidget *parent = 0);
    ~QuadWindow();
    
private:
    Ui::QuadWindow *ui;
};

#endif // QUADWINDOW_H
