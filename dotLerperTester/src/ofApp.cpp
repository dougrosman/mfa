/* This app creates an environment to manipulate x,y coordinates that make
 * up a skeleton body.
 *
 * 1. Take in the x, y data
 */

#include "ofApp.h"


void ofApp::setup()
{
    fbo.allocate(512, 512, GL_RGB);
    savePixels.allocate(512, 512, GL_RGB);
    fbo.begin();
    ofClear(0);
    fbo.end();
    
    //ofSetFrameRate(30);
    
    float dotSize = 10;
    ofBackground(0);
    ofBuffer buffer = ofBufferFromFile("dot_data/amalg_02.txt");
    
    gMin.x = (77.0 + (2*dotSize));//*1.832;
    gMax.x = (648.0 + (2*dotSize));//*1.832;
    gMin.y = (85.0 - (2*dotSize));//*1.832;
    gMax.y = (626.0 - (2*dotSize));//*1.832;
    //ofScale(.5459, .5459);
    //1.8318
    std::cout << buffer.size() << std::endl;
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
                dotFrame.clear();
                //std::cout << "SCREEEEAAM" << std::endl;
            }
            
            else
            {
                // store the x+y values in a glm::vec2
                glm::vec2 tempXY = {ofToFloat(parsedLine[0]), ofToFloat(parsedLine[1])};
                dotParticle dp;
                
                dp.pos = {(tempXY.x)*1.21 + 106, (tempXY.y)*1.21 - (150), 0};
                dp.vel = {0, 0, 0};
                dp.accel = {0, 0, 0};
                dp.size = 10.2;
                dp.color = ofColor::fromHsb(ofMap(j, 0, 14, 10, 245), 255, 255);
                j++;
                
                dotFrame.push_back(dp);
                //std::cout << dotFrame.size() << std::endl;
                
                if (dotFrame.size() == 14)
                {
                    dotParticles.push_back(dotFrame);
                    j = 0;
                }
                
            }
        }
    }
    //std::cout << dotParticles.size() << std::endl;
    
}


void ofApp::update()
{
//    for(auto& dp : dotParticles)
//    {
//
//        if(ofGetKeyPressed(' '))
//        {
//            dp.applyForce();
//        }
//        dp.checkWalls();
//        dp.update();
//
//    }
    
    if(exploMode)
    {
        for(int i = 0; i < dotParticles[0].size(); i++)
        {
            if(apply)
            {
                dotParticles[currFigure][i].applyForce();
                
                std::cout << "test" << std::endl;
                
                apply = false;
                
            }
            dotParticles[currFigure][i].update();
            
        }
    }
}


void ofApp::draw()
{
    // LERPING STUFF
    
    if(record)
    {
        fbo.begin();
        ofClear(0);
    }

    if(lerpMode)
    {

    float t = ofGetElapsedTimef();
    for(int i = 0; i < dotParticles[0].size(); i++)
    {
        
        float initX = dotParticles[prevFigure][i].pos.x;  // 0 1
        float initY = dotParticles[prevFigure][i].pos.y;
        float endX = dotParticles[currFigure][i].pos.x;   // 1 2
        float endY = dotParticles[currFigure][i].pos.y;
        float increment = ofMap(sin(t/lerpRate), -1, 1, 0., 1.);
        ofColor currColor = dotParticles[currFigure][i].color;
        
        float resultX = 0;
        float resultY = 0;
        
        if(currDx > 0)
        {
            resultX = ofLerp(initX, endX, increment);
            resultY = ofLerp(initY, endY, increment);
        }
        
        else
        {
            resultX = ofLerp(endX, initX, increment);
            resultY = ofLerp(endY, initY, increment);
        }
        ofSetColor(currColor);
        
        ofPushMatrix();
        ofTranslate(xPos, yPos);
        ofScale(scaler, scaler);
        ofDrawCircle(resultX, resultY, 10.2);
        ofPopMatrix();
        
    }
    if(record)
    {
        fbo.end();
        fbo.readToPixels(savePixels);
    
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
    
        ofSaveImage(savePixels, "exports/lerps" + saveName);
        saveCount++;
    }



    // Deal with incrementing to the next dot arrangement
    currInc = ofMap(sin(t/lerpRate), -1, 1, .0, 1.);

    if(currInc - prevInc > 0)
    {
        currDx = 1;
    }
    else
    {
        currDx = -1;
    }
    prevInc = currInc;

    if(ofGetFrameNum() % 400 == 0)
    {
        changeSpeed = true;
        
        //std::cout << "shoot" << std::endl;
    }

    if(currDx != prevDx)
    {
        //k++;
        prevFigure = currFigure;
        currFigure = ofRandom(startIndex, endIndex);
        //std::cout << "bang" << std::endl;
        
        if(changeSpeed)
        {
            
            lerpRate = ofRandom(0.1, .5);
            currInc = 0;
            changeSpeed = false;
            std::cout << "speed changed, lerpRate = " << lerpRate << std::endl;
        }
    }

    prevDx = currDx;
}
    if(exploMode)
    {
        for(int i = 0; i < dotParticles[0].size(); i++)
        {
            glm::vec3 currDot = dotParticles[currFigure][i].pos;
            dotParticles[currFigure][i].applyForce();
            ofColor currColor = dotParticles[currFigure][i].color;
            ofSetColor(currColor);
            
            ofPushMatrix();
            ofTranslate(xPos, yPos);
            ofScale(scaler, scaler);
            ofDrawCircle(currDot, 10.2);
            ofPopMatrix();
        }
    }
    
    
    
    
}



void ofApp::keyPressed(int key)
{
    if(key == 'e')
    {
        std::exit(1);
    }
    
    else if (key == 'r')
    {
        record = !record;
    }
    
    else if (key == '-' && scaler > .125)
    {
        scaler-=.03125;
    }
    else if (key == '=')
    {
        scaler+=.03125;
    }
    
    else if (key == 'w')
    {
        yPos-=10;
    }
    else if (key == 's')
    {
        yPos+=10;
    }
    
    else if (key == 'a')
    {
        xPos-=10;
    }
    else if (key == 'd')
    {
        xPos+=10;
    }
    // swap modes
    else if (key == ' ')
    {
        lerpMode = !lerpMode;
        exploMode = !exploMode;
        apply = !apply;
    }
    
    
    switch(key)
    {
        case OF_KEY_RIGHT:
        if(lerpRate < 64) lerpRate+=.1;
        break;
        case OF_KEY_LEFT:
        if(lerpRate > 0.1) lerpRate-=.1;
        break;
    }
}
