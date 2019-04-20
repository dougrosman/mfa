#pragma once

#include "ofMain.h"

class Dot{

	public:
    
    // constructor
    Dot();
    
    // functions
    void update();
    void checkWalls();
    void melt(float xAccel, float lowerY, float upperY);

    // attributes
    glm::vec3 pos;
    glm::vec3 vel;
    glm::vec3 accel;
    
    float drag = 0.99;
    float size;
    float age;
    ofColor color;
};
