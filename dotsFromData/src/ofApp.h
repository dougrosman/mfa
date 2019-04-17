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
    
    ofTrueTypeFont font;
    
    std::vector<dotFrame> dotData;
    string dataSet;
    
    //maroon: startIndex = 82, endIndex = 4482
    //uw: startIndex = 70, endIndex = 5240
    
    int startIndex = 0;
    int endIndex = 0;
    float dotSize;
    bool print = true;
    int incRate = 4;
    
    int dotIndex = startIndex;
    
    glm::vec2 gMin = {10000, 10000};
    glm::vec2 gMax = {0, 0};
    
    ofFbo fbo;
    ofPixels savePixels;
    string saveName = "";
    int saveCount = 0;
    float exportSize = 1080;
    float exportWidth = 1024;
    float exportHeight = 512;
    
    bool debug = false;
    bool HD = true;
    bool twoBods = false;
    
    
    
    // I need:
    // maybe I should just create a struct
    // for an entire 14-dot frame that contains
    // 1. the 4 min/max dot info
    // 2. the std::vector<glm::vec2>
    
    // I also need:
    // A struct instance that keeps track of the
    // 4 min/max dot info for the ENTIRE sequence
    
};
