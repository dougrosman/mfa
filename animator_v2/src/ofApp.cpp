#include "ofApp.h"

///// TO DO /////

// reset function that returns dots to original frames
// re-order amalg_02.txt so white bod comes first
// fix ughIndex so melting can work properly (there's always one body that doesn't melt)
// make it possible to fade in the number of dots
// for multiple bodies, make the frames that are pulled from constant
// when new bodies are drawn on screen, give them a random x translation so they don't all bunch up in the center
// create score where timing can control everything

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
    dataSet = "amalg_02";
    dotFrameIndex = 0;
    ofBackground(0);
    numBodies = 1;
    numDots = 0;
    
    
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
                yMin = 80.;
                yMax = 590.;
                
                // if we parsed 14 dots of data, push back the frames that
                // are within our desired size
                if (df.fourteenDots.size() == 14 && (df.y2 - df.y1) < 380 && df.x1 > xMin && df.x2 < xMax && df.y1 > yMin && df.y2 < yMax)
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
    
    
    // okay, so now we have a vector or vectors that contain 14 Dots, which each have a position, velocity, acceleration, etc...
}

////////// UPDATE ///////////////////// UPDATE //////////

void ofApp::update()
{
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
                proxy.melt(0.07, 0.06, 0.09);
                tempProxyFrame.push_back(proxy);
            }
            proxyFrameUgh.push_back(tempProxyFrame);
        }
        shouldMelt = !shouldMelt;
    }

    if(shouldCycle)
    {
        cycle();
    }
    

    
    
    
    // if there are proxy frames, add that info to the dot-frame info.
    if(proxyFrameUgh.size() > 0)
    {
        for(int q = 0; q < allDotFramesProxy.size(); q+=allDotFramesProxy.size()/numBodies)
        {
            int ughIndex = ofMap(q, 0, allDotFramesProxy.size(), 0, numBodies);
            for(int i = 0; i < allDotFramesProxy[dotFrameIndex].size()-numDots; i++)
            {
                allDotFramesProxy[ofWrap(dotFrameIndex + q, 0, allDotFramesProxy.size())][i].pos = allDotFramesReference[ofWrap(dotFrameIndex + q, 0, allDotFramesProxy.size())][i].pos + proxyFrameUgh[ughIndex][i].pos;
                proxyFrameUgh[ughIndex][i].checkWalls(true, allDotFramesReference[ofWrap(dotFrameIndex + q, 0, allDotFramesProxy.size())][i]);
                
                if(shouldReset && ofGetFrameNum() - currFrame < 90)
                {
                // lerps between current and original dot frame positions
                allDotFramesProxy[ofWrap(dotFrameIndex + q, 0, allDotFramesProxy.size())][i].pos = glm::mix(allDotFramesProxy[ofWrap(dotFrameIndex + q, 0, allDotFramesProxy.size())][i].pos, allDotFramesReference[ofWrap(dotFrameIndex + q, 0, allDotFramesProxy.size())][i].pos, ofMap(ofGetFrameNum() - currFrame, 0, 90, 0., 1.));
                    std::cout << ofMap(ofGetFrameNum() - currFrame, 0, 89, 0., 1.) << std::endl;
                    std::cout << ofGetFrameNum() - currFrame << std::endl;
                } if(ofGetFrameNum() - currFrame == 89) {
                    shouldCycle = true;
                    shouldReset = false;
                    proxyFrameUgh.clear();
                }
            }
        }
    
    
        for(int q = 0; q < allDotFramesProxy.size(); q+=allDotFramesProxy.size()/numBodies)
        {
            int ughIndex = ofMap(q, 0, allDotFramesProxy.size(), 0, numBodies);
            //std::cout << ughIndex << std::endl;
            for(int i = 0; i < 14; i++)
            {
                proxyFrameUgh[ughIndex][i].update();
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
    
        for(int q = 0; q < allDotFramesProxy.size(); q+=allDotFramesProxy.size()/numBodies)
        {
            drawDotFrame(allDotFramesProxy[ofWrap(dotFrameIndex+q, 0, allDotFramesProxy.size())]);
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
    
}

////////// FUNCTIONS ///////////////////// FUNCTIONS //////////


// draws the dot frame
void ofApp::drawDotFrame(std::vector<Dot> dotFrame)
{
    
    for(int i = 0; i < 14-numDots; i++)
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
            
        case '-': if(numDots < 14) { numDots++; }
            break;
            
        case '=': if (numDots > 0) { numDots--; }
            break;
            
        case '9': if(numBodies > 0) { numBodies--; }
            break;
            
        case '0': if (numBodies > 0) { numBodies++; }
            break;
            
    }
}
