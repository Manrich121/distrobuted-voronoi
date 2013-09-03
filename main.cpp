#include "vorowindow.h"
#include "quadwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    VoroWindow w;
    QuadWindow w;
    w.show();
    
    return a.exec();
}
