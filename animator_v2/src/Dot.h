#pragma once

#include "ofMain.h"

class Dot{

	public:
    
    // constructor
    Dot();
    
    // functions
    void update();
    void checkWalls(bool proxy, Dot dot);
    void melt(float xAccel, float lowerY, float upperY);
    void explode(float xVel, float yVel);
    void reset();

    // attributes
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 accel;
    
    float drag = 0.99;
    float size;
    float age;
    ofColor color;
    
};
