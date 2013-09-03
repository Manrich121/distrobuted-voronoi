#ifndef CLIENT_H
#define CLIENT_H

#include <Vector2D.h>

class Client {

public:
    Client();   // constructor
    Client(int max); //contructor with init pos
    ~Client();  //Destructer
    Vector2D* getLocation();
    void move(double x, double y);

protected:
    Vector2D* location;
    int vx;
    int vy;
    int edge;
};

#endif // CLIENT_H
