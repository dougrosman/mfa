#include "Dot.h"


// default constructor
Dot::Dot()
{
    pos = {0, 0, 0};
    vel = {0, 0, 0};
    accel = {0, 0, 0};
    
    color = ofColor::fromHsb(ofMap((int)ofRandom(14), 0, 14, 10, 245), 255, 255);
    size = 9;
    age = 0;
}


void Dot::update()
{
    vel += accel;
    vel *= drag;
    pos += vel;
    
    age++;
}



