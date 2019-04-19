#pragma once

#include "ofMain.h"
#include "dotParticle.h"
//#include "ofApp.h"

class dotParticle {
    
public:
    // functions
    void update();
    void checkWalls();
    
    
    void melt(float xAccel, float lowerY, float upperY);
    
    
    // variables
    glm::vec2 pos;
    glm::vec2 vel;
    glm::vec2 accel;
    float drag = 0.99;
    
    // bounding box
    glm::vec2 upperBound;
    glm::vec2 lowerBound;
    
    
    
    ofColor color;
    float size;
    
    
};
