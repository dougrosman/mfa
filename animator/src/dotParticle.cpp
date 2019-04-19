/* This app creates an environment to manipulate x,y coordinates that make
 * up a skeleton body.
 *
 * 1. Take in the x, y data
 */

#include "dotParticle.h"

void dotParticle::update()
{
    vel += accel;
    vel *= drag;
    pos += vel;
        
}

void dotParticle::checkWalls()
{
    if(pos.x > ofGetWidth()-size*2)
    {
        pos.x = ofGetWidth()-size*2;
        vel.x*=-.9;
    }
    
    if(pos.x < size*2)
    {
        pos.x = size*2;
        vel.x*=-.9;
    }
    
    if(pos.y < size*2)
    {
        pos.y = size*2;
        vel.y*=-.9;
    }
    
    if(pos.y > ofGetHeight()-size*2)
    {
        pos.y = ofGetHeight()-size*2;
        vel.y*=-.9;
    }
}

void dotParticle::melt(float xAccel, float lowerY, float upperY)
{
    accel = {ofRandom(-xAccel, xAccel), ofRandom(lowerY, upperY)};
}
