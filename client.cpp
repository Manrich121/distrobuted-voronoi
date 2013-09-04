#include "client.h"
#include "stdlib.h"

Client::Client() {
    loc = Point();
}

Client::Client(int max) {
    int x = rand()%100;
    int y = rand()%150;
    loc = Point(x, y);
    vx = rand()%6 +2;
    vy = rand()%5 +2;
    edge = max;
}

Point Client::getLocation() {
    return loc;
}

void Client::move(double x, double y) {
    loc.setX(x + vx);
    loc.setY(y + vy);

    if (x >= edge || x <= 0 ) {
        vx = -vx;
    }

    if (y >= edge || y <= 0) {
        vy = -vy;
    }
}
