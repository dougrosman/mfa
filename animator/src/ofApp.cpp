#include "ofApp.h"

/**
 * This sketch takes in X+Y position data from a .txt file, which was
 * generated from the Kinect motion tracking, and generates dots.
 * Dot frames are rendered to an FBO and exported frame by frame.
 */

void ofApp::setup()
{
    ofBackground(0);
    
    // pix2pixHD requires either 2048x1024 or 1024x512
    outputWidth = 1024;
    outputHeight = 512;
    
    refImage.load("/Users/dougrosman/openFrameworks/of_v20190323_osx_release/apps/mfa/dotsFromData/bin/reference/birthday_dots-0052.png");
    
    
    // allocate fbo and savePixels for saving frames later
    fbo.allocate(outputWidth, outputHeight, GL_RGB);
    savePixels.allocate(outputWidth, outputHeight, GL_RGB);
    
    // clear the fbo
    fbo.begin();
    ofClear(0);
    fbo.end();
    
    // set up the proxy particle frame
    for(int i = 0; i < 14; i++)
    {
        dotParticle proxy;
        proxy.pos = {0, 0};
        proxy.vel = {0, 0};
        proxy.accel = {0, 0};
        proxy.size = 9 * (drawScale * outputWidth/2048);
        proxyFrame.push_back(proxy);
        proxyFrame2.push_back(proxy);
    }
    
    dataSet = "amalg_02";
    startIndex = 0;
    
    ofBuffer buffer = ofBufferFromFile("colors/" + dataSet + ".txt");
    
    ofSetCircleResolution(80);
    
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
                glm::vec2 tempXY = {ofToFloat(parsedLine[0]), ofToFloat(parsedLine[1])};
                
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
                
                df.dotSize = 9 * (drawScale * outputWidth/2048);
                
                // if we parsed 14 dots of data, push back the frames that
                // are within our desired size
                if (df.fourteenDots.size() == 14 && (df.y2 - df.y1) < 380 && df.x1 > xMin && df.x2 < xMax && df.y1 > yMin && df.y2 < yMax)
                {
                    dotData.push_back(df);
                }
            }
        }
    }
    std::cout << "# Dot Frames: " << dotData.size() << std::endl;
    
    for(int i = 0; i < dotData.size(); i++)
    {
        std::vector<dotParticle> dotParticles;
        for(int j = 0; j < dotData[i].fourteenDots.size(); j++)
        {
            dotParticle dp;
            dp.pos = dotData[i].fourteenDots[j];
            dp.vel = {0, 0};
            dp.accel = {0, 0};
            dp.color = ofColor::fromHsb(ofMap(j, 0, dotData[0].fourteenDots.size(), 10, 245), 255, 255);
            dp.size = 9 * (drawScale * (outputWidth/2048));
            dotParticles.push_back(dp);
        }
        allProxyFrames.push_back(dotParticles);
    }

}


void ofApp::update()
{
    if(incIndex)
    {
        if(dotIndex < dotData.size()-1)
        {
            dotIndex++;
        }
        else
        {
            dotIndex = 0;
        }
        
    }
    for(int i = 0; i < 14; i++)
    {
        if(ofGetKeyPressed('m'))
        {
            currIndex = dotIndex;
            shouldStop = true;
            for(int j = 0; j < 14; j++)
            {
                proxyFrame[j].melt(0.02, 0.04, 0.02);
            }
        }
        
        if(ofGetKeyPressed('r'))
        {
            // currIndex = dotIndex;
            // shouldStop = true;
            for(int j = 0; j < 14; j++)
            {
                proxyFrame[j].melt(0.02, 0.04, 0.02);
            }
        }
        
        
        
        // the position of the dots, with their original + proxy values.
        glm::vec2 proxyPos = dotData[dotIndex].fourteenDots[i] + proxyFrame[i].pos;
        
        
        // check walls
        
        if(proxyPos.x > ofGetWidth()-dotSize)
        {
            //std::cout << proxyPos.x << std::endl;
            proxyFrame[i].pos.x = ofGetWidth()-dotSize - dotData[dotIndex].fourteenDots[i].x;
            proxyFrame[i].vel.x*=-.99;
        }
        
        if(proxyPos.x < dotSize)
        {
            proxyFrame[i].pos.x = dotSize - dotData[dotIndex].fourteenDots[i].x;
            proxyFrame[i].vel.x*=-.99;
        }
        
        if(proxyPos.y < dotSize)
        {
            proxyFrame[i].pos.y = dotSize - dotData[dotIndex].fourteenDots[i].y;
            proxyFrame[i].vel.y*=-.99;
        }
        
        if(proxyPos.y > ofGetHeight() - dotSize)
        {
            proxyFrame[i].pos.y = ofGetHeight()-dotSize - dotData[dotIndex].fourteenDots[i].y;
            proxyFrame[i].vel.y*=-.99;
        }
        
        proxyFrame[i].update();
    }
    
    if(dotIndex - currIndex > 90 && shouldStop)
    {
        incIndex = false;
        shouldStop = false;
    }
    
}

void ofApp::draw()
{
    dotFrame currDf = dotData[dotIndex];
    dotSize = currDf.dotSize;
    ofSetColor(255);
    ofPushMatrix();
    ofScale(outputWidth/2048, outputWidth/2048);
    refImage.draw(0, 0);
    ofPopMatrix();
    
    ///////////////////////////////// OUTPUT ////////////////////////////////////////////
    ///////////////////////////////// OUTPUT ////////////////////////////////////////////
    ///////////////////////////////// OUTPUT ////////////////////////////////////////////
    ///////////////////////////////// OUTPUT ////////////////////////////////////////////
    ///////////////////////////////// OUTPUT ////////////////////////////////////////////
    // The image to output
    if(outputTest)
    {
        for(int j = 0; j < dotData[0].fourteenDots.size(); j++)
        {
            ofColor dotColor = ofColor::fromHsb(ofMap(j, 0, dotData[0].fourteenDots.size(), 10, 245), 255, 255);
            ofSetColor(dotColor);
            ofFill();
            ofDrawCircle(dotData[dotIndex].fourteenDots[j] + proxyFrame[j].pos, dotSize);
        }
        
    }

    // if record
    else
    {
        fbo.begin();
            ofClear(0);
            for(int j = 0; j < dotData[0].fourteenDots.size(); j++)
            {
                ofColor dotColor = ofColor::fromHsb(ofMap(j, 0, dotData[0].fourteenDots.size(), 10, 245), 255, 255);
                ofSetColor(dotColor);
                ofDrawCircle(dotData[dotIndex].fourteenDots[j] + proxyFrame[j].pos, dotSize);
            }
        fbo.end();
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
        
        ofSaveImage(savePixels, "../exports/colors/" + dataSet + "_" + ofToString(outputWidth) + "/dots_" + saveName, OF_IMAGE_QUALITY_BEST);
        std::cout << saveName << std::endl;
        
    }
}

void ofApp::keyPressed(int key)
{
    switch(key)
    {
    // advance dot frame during debug
    case '.':
        if(dotIndex < dotData.size() - incRate - 1) dotIndex+=incRate;
        break;
        
    case ',':
        if(dotIndex > 0) dotIndex-=incRate;
        break;
    
    // change incRate
    case '[':
        if(incRate > 0) incRate--;
        break;
        
    case ']':
        if(incRate < 100) incRate++;
        break;
    
    // change drawScale
    case '-':
        if(drawScale > 0) drawScale-=.015625;
            std::cout << "------------------------" << std::endl;
            std::cout << "drawX =       " << drawX << std::endl;
            std::cout << "drawY =       " << drawY << std::endl;
            std::cout << "drawScale =   " << drawScale << std::endl;
            std::cout << "" << std::endl;
        break;
    
    case '=':
        if(drawScale < 4) drawScale+=.015625;
            std::cout << "------------------------" << std::endl;
            std::cout << "drawX =       " << drawX << std::endl;
            std::cout << "drawY =       " << drawY << std::endl;
            std::cout << "drawScale =   " << drawScale << std::endl;
            std::cout << "" << std::endl;
        break;
    
    // switch modes (outputTest, record)
    case 'o':
    case 'O':
        record = false;
        outputTest = true;
        break;
            
    case 'r':
    case 'R':
        outputTest = true;
        record = true;
        break;
            
    case '2':
        twoBods =! twoBods;
        break;
    
    // draw position
    case 'w':
    case 'W':
        drawY-=1;
        std::cout << "------------------------" << std::endl;
        std::cout << "drawX =       " << drawX << std::endl;
        std::cout << "drawY =       " << drawY << std::endl;
        std::cout << "drawScale =   " << drawScale << std::endl;
        std::cout << "" << std::endl;
        break;
        
    case 's':
    case 'S':
        dotSize+=1;
        std::cout << "------------------------" << std::endl;
        std::cout << "drawX =       " << drawX << std::endl;
        std::cout << "drawY =       " << drawY << std::endl;
        std::cout << "drawScale =   " << drawScale << std::endl;
        std::cout << "dotSize =   " << dotSize << std::endl;
        std::cout << "" << std::endl;
        break;
        
    case 'a':
    case 'A':
        dotSize-=1;
        std::cout << "------------------------" << std::endl;
        std::cout << "drawX =       " << drawX << std::endl;
        std::cout << "drawY =       " << drawY << std::endl;
        std::cout << "drawScale =   " << drawScale << std::endl;
        std::cout << "dotSize =   " << dotSize << std::endl;
        std::cout << "" << std::endl;
        break;
        
    case 'd':
    case 'D':
        drawX+=1;
        std::cout << "------------------------" << std::endl;
        std::cout << "drawX =       " << drawX << std::endl;
        std::cout << "drawY =       " << drawY << std::endl;
        std::cout << "drawScale =   " << drawScale << std::endl;
        std::cout << "" << std::endl;
        break;
            
    case OF_KEY_RETURN:
        incIndex = !incIndex;
        break;
    }
}
