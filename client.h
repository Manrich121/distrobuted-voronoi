#ifndef CLIENT_H
#define CLIENT_H

#include "point.h"
class Client {

public:
    Client();   // constructor
    Client(int max); //contructor with init pos
    ~Client();  //Destructer
    Point getLocation();
    void move(double x, double y);

protected:
    Point loc;
    int vx;
    int vy;
    int edge;
};

#endif // CLIENT_H
