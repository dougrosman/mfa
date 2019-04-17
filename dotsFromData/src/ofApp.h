#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

        void keyPressed(int key);
    
    struct dotFrame {
        float x1;   // smallest X
        float y1;   // smallest Y
        float x2;   // largest X
        float y2;   // largest Y
        std::vector<glm::vec2> fourteenDots;
    };
    
    dotFrame df;
    
    // a vector for storing dot frames
    std::vector<dotFrame> dotData;
    string dataSet;
    
    // the upper and lower bounds of our dot frames,
    // this allows us to only accept "reasonable" dot frames
    float xMin;
    float xMax;
    float yMin;
    float yMax;
    
    int startIndex;
    int endIndex;
    float dotSize;
    bool print = false;
    int incRate = 4;
    
    int dotIndex;
    
    // The Global min and max x+y-values
    glm::vec2 gMin = {10000, 10000};
    glm::vec2 gMax = {0, 0};
    
    ofFbo fbo;
    ofPixels savePixels;
    string saveName = "";
    int saveCount = 0;
    float outputWidth;
    float outputHeight;
    
    float drawScale = 2.20312;
    
    float drawX = 0;
    float drawY = 0;
    
    bool debug = false;
    bool outputTest = false;
    bool record = false;
    
    bool HD = true;
    bool twoBods = false;
    
    
    // misc global variables
    ofTrueTypeFont font;
    
};
