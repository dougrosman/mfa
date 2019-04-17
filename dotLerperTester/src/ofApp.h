#pragma once

#include "ofMain.h"
#include "dotParticle.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
		void keyPressed(int key);
        void applyForce();
    
    std::vector<dotParticle> dotFrame;
    std::vector<std::vector<dotParticle>> dotParticles;
    
    glm::vec2 gMin;
    glm::vec2 gMax;
    int j = 0;
    
    ofFbo fbo;
    ofPixels savePixels;
    
    float scaler = .75;
    float xPos = -150;
    float yPos = 40;
    
    bool record = false;
    
    // LERP STUFF
    int saveCount = 0;
    float lerpRate = 0.5;
    float currDx = 1;
    float prevDx = 1;
    int currFigure = 2;
    int prevFigure = 0;
    float currInc = 0;
    float prevInc = 0;
    int k = 0;
    int randomIndex = 0;
    
    int startIndex = 10;
    int endIndex = 30000;
    string saveName = "";
    
    bool changeSpeed;
		
    
    // MODES
    
    bool lerpMode = true;
    bool exploMode = false;
    bool apply = true;
};
