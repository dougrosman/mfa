#pragma once

#include "ofMain.h"
#include "dotParticle.h"

class dotParticle {
    
public:
    // functions
    void update();
    void checkWalls();
    void melt();
    
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
