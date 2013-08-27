#include "vorowindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VoroWindow w;
    w.show();
    
    return a.exec();
}
