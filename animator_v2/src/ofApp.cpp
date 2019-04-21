#include "ofApp.h"

///// TO DO /////

// reset function that returns dots to original frames
// re-order amalg_02.txt so white bod comes first
// fix ughIndex so melting can work properly (there's always one body that doesn't melt)
// make it possible to fade in the number of dots
// for multiple bodies, make the frames that are pulled from constant
// when new bodies are drawn on screen, give them a random x translation so they don't all bunch up in the center
// create score where timing can control everything

///// TO DO II /////
// fix txt file
// fix q intervals to be constant
// write out final score (1-2-3-2-1)

////////// SETUP ///////////////////// SETUP //////////

void ofApp::setup()
{
    // constants to set at the beginning
    batchName = "saturday_test_03";
    outputWidth = 1024;
    outputHeight = 512;
    ofSetFrameRate(60);
    
    drawScale = 2.21874;
    drawX = 116;
    drawY = -94;
    dotSize = 9 * (drawScale * outputWidth/2048);
    ofSetCircleResolution(80);
    dataSet = "amalg_03";
    dotFrameIndex = 0;
    ofBackground(0);
    numBodies = 10;
    numDots = 0;
    resetTime = 120;
    
    translateVals.push_back({0, 0, 0});
    translateVals.push_back({-200, 0, 0});
    translateVals.push_back({200, 0, 0});
    
    translateVals.push_back({0, 0, 0});
    translateVals.push_back({-500, 0, 0});
    translateVals.push_back({300, 0, 0});
    translateVals.push_back({-300, 0, 0});
    translateVals.push_back({100, 0, 0});
    translateVals.push_back({500, 0, 0});
    translateVals.push_back({-100, 0, 0});
    
    dotIndexMods.push_back(0);
    dotIndexMods.push_back(9200);
    dotIndexMods.push_back(18400);
    
    dotIndexMods.push_back(5000);
    dotIndexMods.push_back(8000);
    dotIndexMods.push_back(11000);
    dotIndexMods.push_back(14000);
    dotIndexMods.push_back(17000);
    dotIndexMods.push_back(20000);
    dotIndexMods.push_back(23000);
    
    for(int i = 0; i < 10; i++)
    {
        numDotsInFrame.push_back(0);
    }
    
    
    // allocate fbo and savePixels for saving frames later
    fbo.allocate(outputWidth, outputHeight, GL_RGB);
    savePixels.allocate(outputWidth, outputHeight, GL_RGB);
    
    // clear the fbo
    fbo.begin();
    ofClear(0);
    fbo.end();
    
    // create a buffer to load from text file
    ofBuffer buffer = ofBufferFromFile("dot_data/" + dataSet + ".txt");
    
    // parse the text file and store all the dot frame data in vector
    if(buffer.size())
    {
        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end(); it != end; ++it)
        {
            // grab the line
            string line = *it;
            
            // split the line by the comma to separate the x+y values
            std::vector<string> parsedLine = ofSplitString(line, ",");
            
            // Clear the dotFrame whenever there's an empty line
            if (line.empty())
            {
                df.fourteenDots.clear();
                
                // reset the x + y coordinates of the current dotFrame
                df.x1 = 10000;
                df.y1 = 10000;
                df.x2 = 0;
                df.y2 = 0;
            }
            
            // if there's data to parse...
            else
            {
                // store the x+y values in a temporary glm::vec2
                glm::vec3 tempXY = {ofToFloat(parsedLine[0]), ofToFloat(parsedLine[1]), 0};
                
                // store the largest and smallest x/y-values of each frame
                if(tempXY.x < df.x1)
                {
                    df.x1 = tempXY.x;
                }
                if(tempXY.y < df.y1)
                {
                    df.y1 = tempXY.y;
                }
                if(tempXY.x > df.x2)
                {
                    df.x2 = tempXY.x;
                }
                if(tempXY.y > df.y2)
                {
                    df.y2 = tempXY.y;
                }
                
                tempXY.x *= drawScale * (outputWidth/2048);
                tempXY.y *= drawScale * (outputWidth/2048);
                tempXY.x += (drawScale * outputWidth/2048) * drawX;
                tempXY.y += (drawScale * outputWidth/2048) * drawY;
                
                // push the glm::vec2 into the temporary vector
                df.fourteenDots.push_back(tempXY);
                
                xMin = 50.;
                xMax = 685.;
                yMin = 70.;
                yMax = 590.;
                
                // if we parsed 14 dots of data, push back the frames that
                // are within our desired size
                if (df.fourteenDots.size() == 14 && (df.y2 - df.y1) < 390 && df.x1 > xMin && df.x2 < xMax && df.y1 > yMin && df.y2 < yMax)
                {
                    originalDotFrames.push_back(df);
                }
            }
        }
    }
    
    // create a vector of dot frames (with Dot objects) with the information from originalDotFrames
    for(int i = 0; i < originalDotFrames.size(); i++)
    {
        // create a temporary dots vector
        std::vector<Dot> dots;
        for(int j = 0; j < 14; j++)
        {
            Dot d;
            d.pos = originalDotFrames[i].fourteenDots[j];
            d.color = ofColor::fromHsb(ofMap(j, 0, 14, 10, 245), 255, 255);
            d.size = dotSize;
            dots.push_back(d);
        }
        
        // push temporary dots vector into allDotsFrames
        allDotFramesReference.push_back(dots);
    }
    

    allDotFramesProxy = allDotFramesReference;
    std::cout<< allDotFramesReference.size() << std::endl;
    
    
    // okay, so now we have a vector or vectors that contain 14 Dots, which each have a position, velocity, acceleration, etc...
}

////////// UPDATE ///////////////////// UPDATE //////////

void ofApp::update()
{
    std::cout << ofGetFrameNum() << std::endl;
    score();
    //melt test
    if(shouldMelt)
    {
        for(int q = 0; q < numBodies; q++)
        {
            std::vector<Dot> tempProxyFrame;
            for(int i = 0; i < 14; i++)
            {
                Dot proxy;
                proxy.pos = {0, 0, 0};
                proxy.vel = {0, 0, 0};
                proxy.accel = {0, 0, 0};
                proxy.melt(meltX, meltY1, meltY2);
                tempProxyFrame.push_back(proxy);
            }
            proxyFrameUgh.push_back(tempProxyFrame);
        }
        shouldMelt = !shouldMelt;
    }
    
    if(shouldExplode)
    {
        for(int q = 0; q < numBodies; q++)
        {
            std::vector<Dot> tempProxyFrame;
            for(int i = 0; i < 14; i++)
            {
                Dot proxy;
                proxy.pos = {0, 0, 0};
                proxy.vel = {0, 0, 0};
                proxy.accel = {0, 0, 0};
                proxy.explode(explodeX, explodeY);
                tempProxyFrame.push_back(proxy);
            }
            proxyFrameUgh.push_back(tempProxyFrame);
        }
        shouldExplode = !shouldExplode;
    }

    if(shouldCycle)
    {
        cycle();
    }

    // if there are proxy frames, add that info to the dot-frame info.
    if(proxyFrameUgh.size() > 0)
    {
        // for(int q = 0; q < allDotFramesProxy.size(); q+=(allDotFramesProxy.size()/numBodies))
        for(int q = 0; q < numBodies; q++)
        {
            //int ughIndex = ofMap(q, 0, allDotFramesProxy.size(), 0, numBodies);
            int ughIndex = q;
            for(int i = 0; i < allDotFramesProxy[dotFrameIndex].size()-numDots; i++)
            {
                proxyFrameUgh[ughIndex][i].checkWalls(true, allDotFramesReference[ofWrap(dotFrameIndex + q, 0, allDotFramesProxy.size())][i]);
                allDotFramesProxy[ofWrap(dotFrameIndex + dotIndexMods[q], 0, allDotFramesProxy.size())][i].pos = allDotFramesReference[ofWrap(dotFrameIndex + dotIndexMods[q], 0, allDotFramesProxy.size())][i].pos + proxyFrameUgh[ughIndex][i].pos;
                
                
                if(shouldReset && ofGetFrameNum() - currFrame < resetTime)
                {
                    
                // lerps between current and original dot frame positions
                    allDotFramesProxy[ofWrap(dotFrameIndex + dotIndexMods[q], 0, allDotFramesProxy.size())][i].pos = glm::mix(allDotFramesProxy[ofWrap(dotFrameIndex + dotIndexMods[q], 0, allDotFramesProxy.size())][i].pos, allDotFramesReference[ofWrap(dotFrameIndex + dotIndexMods[q], 0, allDotFramesProxy.size())][i].pos, ofMap(ofGetFrameNum() - currFrame, 0, resetTime, 0., 1.));
                
                    
                    if(ofGetFrameNum() - currFrame == resetTime-1) {
                        shouldCycle = true;
                        shouldClear = true;
                    }
                }
                
                //std::cout << glm::mix(allDotFramesProxy[ofWrap(dotFrameIndex + q, 0, allDotFramesProxy.size())][i].pos, allDotFramesReference[ofWrap(dotFrameIndex + q, 0, allDotFramesProxy.size())][i].pos, ofMap(ofGetFrameNum() - currFrame, 0, resetTime, 0., 1.)) << std::endl;
                
                if(shouldClear)
                {
                    proxyFrameUgh.clear();
                    shouldClear = false;
                    shouldReset = false;
                }
            }
        }
    
        if(proxyFrameUgh.size() > 0)
        {
            // for(int q = 0; q < allDotFramesProxy.size(); q+=(allDotFramesProxy.size()/numBodies))
            for(int q = 0; q < numBodies; q++)
            {
                //int ughIndex = ofMap(q, 0, allDotFramesProxy.size(), 0, numBodies);
                int ughIndex = q;
                for(int i = 0; i < 14; i++)
                {
                    proxyFrameUgh[ughIndex][i].update();
                }
            }
        }
    }
    
    //numBodies = ofMap(ofGetMouseX(), 0, ofGetWidth(), 1, 4);
    //numDots = ofMap(ofGetMouseY(), 0, ofGetHeight(), 2, 10);
}

////////// DRAW ///////////////////// DRAW //////////
void ofApp::draw()
{
    fbo.begin();
        ofClear(0);
    
        //for(int q = 0; q < allDotFramesProxy.size(); q+=(allDotFramesProxy.size()/numBodies))
        for(int q = 0; q < numBodies; q++)
        {
            ofPushMatrix();
            ofTranslate(translateVals[q]);
            drawDotFrame(allDotFramesProxy[ofWrap(dotFrameIndex+dotIndexMods[q], 0, allDotFramesProxy.size())], numDotsInFrame[q]);
        }
    
        fbo.end();
    fbo.draw(0, 0);
    
    if(record)
    {
        fbo.readToPixels(savePixels);
        
        // create names for output files
        if(saveCount < 10)
        {
            saveName = "0000" + ofToString(saveCount) + ".png";
        }
        else if(saveCount < 100 && saveCount > 9)
        {
            saveName = "000" + ofToString(saveCount) + ".png";
        }
        else if(saveCount < 1000 && saveCount > 99)
        {
            saveName = "00" + ofToString(saveCount) + ".png";
        }
        else if(saveCount < 10000 && saveCount > 999)
        {
            saveName = "0" + ofToString(saveCount) + ".png";
        }
        else
        {
            saveName = ofToString(saveCount) + ".png";
        }
        saveCount++;
        
        ofSaveImage(savePixels, "../exports/" + batchName + "_" + ofToString(outputWidth) + "/" + batchName + "_" + saveName, OF_IMAGE_QUALITY_BEST);
        std::cout << saveName << std::endl;
    }
    
    ofDrawBitmapString(ofToString(ofGetFrameNum()), 10, 20);
    
}

////////// FUNCTIONS ///////////////////// FUNCTIONS //////////


// draws the dot frame
void ofApp::drawDotFrame(std::vector<Dot> dotFrame, int nDots)
{
    
    for(int i = 0; i < nDots; i++)
    {
        ofSetColor(dotFrame[i].color);
        ofDrawCircle(dotFrame[i].pos, dotFrame[i].size);
    }
    
}

// plays back frames in their original order;
void ofApp::cycle()
{
    dotFrameIndex++;
}



void ofApp::keyPressed(int key)
{
    switch(key) {
        case 'c':
        case 'C':
            shouldCycle = !shouldCycle;
            break;
        
        case 'r':
        case 'R':
            record = !record;
            break;
            
        case 'm':
        case 'M':
            shouldMelt = !shouldMelt;
            //proxyFrameUgh.clear();
            break;
            
        case ' ':
            //proxyFrameUgh.clear();
            currFrame = ofGetFrameNum();
            shouldReset = !shouldReset;
            break;
            
        case '=': if(numDotsInFrame[0] < 14) { numDotsInFrame[0]++; }//numDots++; }
            break;
            
        case '-': if (numDotsInFrame[0] > 0) { numDotsInFrame[0]--; }//numDots--; }
            break;
            
        case '9': if(numBodies > 1 && proxyFrameUgh.size() == 0)
                {
                    //numDotsInFrame.erase(numDotsInFrame.end());
                    //translateVals.erase(translateVals.end());
                    numBodies--;
                }
            break;
            
        case '0': if (numBodies < 50 && proxyFrameUgh.size() == 0)
                {
                    numBodies++;
                    numDotsInFrame.push_back(0);
                    translateVals.push_back({ofRandom(-150, 150), 0, 0});
                }
            break;
            
    }
}

////////// SCORE ///////////////////// SCORE //////////

void ofApp::score()
{
    //shouldCycle = true;
    float fc = ofGetFrameNum();
    
    /// PHASE 1 /// PHASE 1 /// PHASE 1 /// PHASE 1 /// PHASE 1
    
    // 1a. start cycling
    if(fc == 0)
    {
        shouldCycle = true;
    }
    
    // 1b. start black screen (0-1680)
    if(fc > 0 && (int)fc % 120 == 0 && numDotsInFrame[0] < 14)
    {
        numDotsInFrame[0]++;
    }
    
    // cycle for 2000 frames
    
    // 2. melt single bod
    if(fc == 3680)
    {
        stopCycleTime = 60;
        tempFc = fc;
        meltX = .06;
        meltY1 = .02;
        meltY2 = .05;
        shouldMelt = true;
    }
    
    // 3. stop cycle
    if(fc > 3680 && fc - tempFc == stopCycleTime)
    {
        shouldCycle = false;
    }
    
    // stay melted for 500 frames
    
    // 4. reset single bod
    if(fc == 4180)
    {
        resetTime = 420;
        currFrame = ofGetFrameNum();
        shouldReset = !shouldReset;
    }
    
    /// PHASE 2 /// PHASE 2 /// PHASE 2 /// PHASE 2 /// PHASE 2
    
    // 5. fade in second bod
    if(fc > 4179 && (int)fc % 30 == 0 && numDotsInFrame[1] < 14)
    {
        numDotsInFrame[1]++;
    }
    
    // cycle for 3600 frames
    
    // 6. melt double bods
    
    if(fc == 7780)
    {
        stopCycleTime = 30;
        tempFc = fc;
        meltX = .09;
        meltY1 = .1;
        meltY2 = .2;
        shouldMelt = true;
    }
    
    // 7. stop cycle
    if(fc > 7780 && fc - tempFc == stopCycleTime)
    {
        shouldCycle = false;
    }
    
    // stay melted for 400 frames
    
    // 8. reset double bods
    if(fc == 8180)
    {
        resetTime = 210;
        currFrame = ofGetFrameNum();
        shouldReset = !shouldReset;
    }
    
    /// PHASE 3 /// PHASE 3 /// PHASE 3 /// PHASE 3 /// PHASE 3
    
    // 9. fade in third bod
    if(fc > 8179 && (int)fc % 15 == 0 && numDotsInFrame[2] < 14)
    {
        numDotsInFrame[2]++;
    }
    
    // cycle for 3000 frames
    
    // 10. add in remaining 7 bods
    
    if(fc == 11800) { numDotsInFrame[3] = 14; }
    if(fc == 12300) { numDotsInFrame[4] = 14; }
    if(fc == 12700) { numDotsInFrame[5] = 14; }
    if(fc == 13000) { numDotsInFrame[6] = 14; }
    if(fc == 13200) { numDotsInFrame[7] = 14; }
    if(fc == 13300) { numDotsInFrame[8] = 14; }
    if(fc == 13350) { numDotsInFrame[9] = 14; }
    
    // 11 EXPLODE
    
    if(fc == 13800)
    {
        stopCycleTime = 1;
        tempFc = fc;
        explodeX = 30;
        explodeY = 30;
        shouldExplode = true;
    }
    
     // 12 stop cycling during EXPLODE
    if(fc > 13800 && fc - tempFc == stopCycleTime)
    {
        shouldCycle = false;
    }
    
    /// EPILOGUE /// EPILOGUE /// EPILOGUE /// EPILOGUE /// EPILOGUE
    
    // 13 reset all bods
    if(fc == 14300)
    {
        resetTime = 630;
        currFrame = ofGetFrameNum();
        shouldReset = !shouldReset;
    }
    
    // 14 fade away bods dot by dot
    if(fc > 14299)
    {
        if((int) fc % (60/tempDotsInFrameIndex) == 0)
        {
            if(tempDotsInFrameIndex > 0)
            {
                numDotsInFrame[tempDotsInFrameIndex]--;
                
                if(numDotsInFrame[tempDotsInFrameIndex] == 0)
                {
                    tempDotsInFrameIndex--;
                }
            }
            else
            {
                if(numDotsInFrame[tempDotsInFrameIndex] > 1)
                {
                    numDotsInFrame[tempDotsInFrameIndex]--;
                }
            }
        }
        
    }
    
    
    
    
    
    
//    // after one minute, start fading in second body
//    if(fc == 600)
//    {
//
//    }
//
//    // Fade in second body, one dot every 2 seconds
//    if(fc > 600 && (int)fc % 60 == 0 && numDotsInFrame[1] < 14)
//    {
//        numDotsInFrame[1]++;
//    }
//
//    // start fading in third body
//    if(fc == 900)
//    {
//        numBodies++;
//        numDotsInFrame.push_back(0);
//        translateVals.push_back({300, 0, 0});
//    }
//
//    // Fade in second body, one dot every 2 seconds
//    if(fc > 900 && (int)fc % 60 == 0 && numDotsInFrame[2] < 14)
//    {
//        numDotsInFrame[2]++;
//    }
//
//    // melt bods
//    if(fc == 1000)
//    {
//        stopCycleTime = 60;
//        tempFc = fc;
//        meltX = .09;
//        meltY1 = .1;
//        meltY2 = .14;
//        shouldMelt = true;
//    }
//
//    // stop cycling during melt
//    if(fc > 1000 && fc - tempFc == stopCycleTime)
//    {
//        shouldCycle = false;
//    }
//
//    if(fc == 1200)
//    {
//        resetTime = 120;
//        currFrame = ofGetFrameNum();
//        shouldReset = !shouldReset;
//    }
//
//    // explode bods
//    if(fc == 1400)
//    {
//        stopCycleTime = 1;
//        tempFc = fc;
//        explodeX = 30;
//        explodeY = 30;
//        shouldExplode = true;
//    }
//
//    // stop cycling during melt
//    if(fc > 1400 && fc - tempFc == stopCycleTime)
//    {
//        shouldCycle = false;
//    }
//
//    if(fc == 1600)
//    {
//        resetTime = 120;
//        currFrame = ofGetFrameNum();
//        shouldReset = !shouldReset;
//    }
    
    
}
