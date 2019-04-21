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

void Dot::checkWalls(bool proxy, Dot dot)
{
    if(proxy)
    {
        if(pos.x + dot.pos.x > ofGetWidth()-size)
        {
            pos.x = ofGetWidth()-size - dot.pos.x;
            vel.x*=-.9;
        }
        
        if(pos.x + dot.pos.x < size)
        {
            pos.x = size - dot.pos.x;
            vel.x*=-.9;
        }
        
        if(pos.y + dot.pos.y > ofGetHeight()-size)
        {
            pos.y = ofGetHeight()-size - dot.pos.y;
            vel.y*=-.6;
            accel.x*=.5;
        }
        
        if(pos.y + dot.pos.y < size)
        {
            pos.y = size - dot.pos.y;
            vel.y*=-.6;
        }
    }
    else
    {
        if(pos.x > ofGetWidth()-size)
        {
            pos.x = ofGetWidth()-size;
            vel.x*=-.9;
        }
        
        if(pos.x < size)
        {
            pos.x = size;
            vel.x*=-.9;
        }
        
        if(pos.y > ofGetHeight()-size)
        {
            pos.y = ofGetHeight()-size;
            vel.y*=-.6;
        }
        
        if(pos.y < size)
        {
            pos.y = size*2;
            vel.y*=-.6;
        }
    }
}

void Dot::update()
{
    vel += accel;
    vel *= drag;
    pos += vel;
    
    age++;
}

void Dot::melt(float xAccel, float lowerY, float upperY)
{
    accel = {ofRandom(-xAccel, xAccel), ofRandom(lowerY, upperY), 0};
}

void Dot::explode(float xVel, float yVel)
{
    accel = {0, .5, 0};
    vel = {ofRandom(-xVel, xVel), ofRandom(-yVel, yVel), 0};
}

void Dot::reset()
{
    accel = {0, 0, 0};
    vel = {0, 0, 0};
    pos = {0, 0, 0};
}



