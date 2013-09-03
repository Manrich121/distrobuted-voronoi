#include "quadwindow.h"
#include "ui_quadwindow.h"

QuadWindow::QuadWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QuadWindow)
{
    ui->setupUi(this);
}

QuadWindow::~QuadWindow()
{
    delete ui;
}
