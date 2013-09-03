#include "client.h"
#include "stdlib.h"

Client::Client() {
    location = new Vector2D();
}

Client::Client(int max) {
    int x = rand()%100;
    int y = rand()%150;
    location = new Vector2D(x, y);
    vx = rand()%6 +2;
    vy = rand()%5 +2;
    edge = max;
}

Vector2D* Client::getLocation() {
    return location;
}

void Client::move(double x, double y) {
    location->x = x + vx;
    location->y = y + vy;

    if (x >= edge || x <= 0 ) {
        vx = -vx;
    }

    if (y >= edge || y <= 0) {
        vy = -vy;
    }
}
