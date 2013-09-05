#include "client.h"
#include "stdlib.h"

Client::Client() {
    loc = Point();
}

Client::Client(Point p, int max) {
    loc = p;
    vx = rand()%5 +1;
    vy = rand()%5 +3;
    edge = max;
}

void Client::move() {
    loc.setX(loc.x() + vx);
    loc.setY(loc.y() + vy);

    if (loc.x() >= edge || loc.x() <= 0 ) {
        vx = -vx;
    }

    if (loc.y() >= edge || loc.y() <= 0) {
        vy = -vy;
    }
}
