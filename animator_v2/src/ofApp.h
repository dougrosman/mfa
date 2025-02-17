#pragma once

#include "ofMain.h"
#include "Dot.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
    
    void drawDotFrame(std::vector<Dot> dotFrame, int nDots);
    //void drawDotFrame(std::vector<Dot> dotFrame, std::vector<Dot> proxyFrame, bool addProxy);
    void cycle();
    void score();
    
    // used to store and prune incoming dot data
    struct dotFrame {
        float x1;   // smallest X
        float y1;   // smallest Y
        float x2;   // largest X
        float y2;   // largest Y
        std::vector<glm::vec3> fourteenDots;
    };
    
    // for importing and storing dot data at the proper scale
    dotFrame df;
    float drawScale;
    float drawX;
    float drawY;
    float xMin;
    float xMax;
    float yMin;
    float yMax;
    float dotSize;
    float resetTime;
    
    float outputWidth;
    float outputHeight;
    
    int numBodies;
    int numDots;
    
    std::vector<dotFrame> originalDotFrames;
    std::vector<std::vector<Dot>> allDotFramesReference;
    std::vector<std::vector<Dot>> allDotFramesProxy;
    
    std::vector<int> numDotsInFrame;
    std::vector<glm::vec3> translateVals;
    
    std::vector<Dot> proxyFrame;
    std::vector<std::vector<Dot>> proxyFrameUgh;
    
    std::vector<int> dotIndexMods;
    
    // for saving images
    ofFbo fbo;
    ofPixels savePixels;
    
    // loading dot data
    string dataSet;
    
    // void cycle
    float dotFrameIndex;
    
    // void melt
    float meltX;
    float meltY1;
    float meltY2;
    
    float explodeX;
    float explodeY;
    
    float stopCycleTime;
    float tempFc;
    
    bool shouldCycle = false;
    bool shouldMelt = false;
    bool shouldExplode = false;
    bool shouldReset = false;
    bool shouldClear = false;
    float currFrame;
    
    bool shouldFade2 = true;
    bool shouldFade3 = true;
    
    int tempDotsInFrameIndex = 9;
    
    //record
    bool record = false;
    string batchName = "";
    string saveName = "";
    int saveCount = 0;
    
};
