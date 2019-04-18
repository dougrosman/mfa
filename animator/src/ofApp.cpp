#include "ofApp.h"

/**
 * This sketch takes in X+Y position data from a .txt file, which was
 * generated from the Kinect motion tracking, and generates dots.
 * Dot frames are rendered to an FBO and exported frame by frame.
 */

void ofApp::setup()
{
    ofBackground(0);
    
    // set up the proxy particle
    
    for(int i = 0; i < 14; i++)
    {
        dotParticle proxy;
        proxy.pos = {0, 0};
        proxy.vel = {0, 0};
        proxy.accel = {0, 0};
        proxy.size = 9;
        proxyFrame.push_back(proxy);
        proxyFrame2.push_back(proxy);
    }
    
    
    
    //refImage.load("/Users/dougrosman/openFrameworks/of_v20190323_osx_release/apps/mfa/dotsFromData/bin/reference/birthday_dots-0052.png");
    
    // pix2pixHD requires either 2048x1024 or 1024x512
    outputWidth = 1024;
    outputHeight = 512;
    
    //    gMin.x = 50.0;
    //    gMax.x = 685;
    //    gMin.y = 100.0;
    //    gMax.y = 620.0;
    
    ofEnableSmoothing();
    ofEnableAntiAliasing();
    
    
    // allocate fbo and savePixels for saving frames later
    fbo.allocate(outputWidth, outputHeight, GL_RGB);
    savePixels.allocate(outputWidth, outputHeight, GL_RGB);
    
    // clear the fbo
    fbo.begin();
    ofClear(0);
    fbo.end();
    
    
    
    font.load("Arial.ttf", 20);
    dataSet = "amalg_02";
    startIndex = 0;
    
    ofBuffer buffer = ofBufferFromFile("colors/" + dataSet + ".txt");
    
    ofSetCircleResolution(100);
    dotSize = 9;
    
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
                    dotData.push_back(df);
                }
            }
        }
    }
    
    
    
    std::cout << "# Dot Frames: " << dotData.size() << std::endl;
    endIndex = dotData.size() - 1;
    
    // Find the min/max of the entire sequence, and store in gMin and gMax
    for(int i = startIndex; i < endIndex; i++)
    {
        dotFrame currDf = dotData[i];
    
        if(currDf.x1 < gMin.x)
        {
            gMin.x = currDf.x1;
        }
        if(currDf.y1 < gMin.y)
        {
            gMin.y = currDf.y1;
        }
        if(currDf.x2 > gMax.x)
        {
            gMax.x = currDf.x2;
        }
        if(currDf.y2 > gMax.y)
        {
            gMax.y = currDf.y2;
        }
    }
    
    // add dotSize to upper and lower bounds to keep dots from being
    // cut off in output frames
    gMin.x -= dotSize;
    gMin.y -= dotSize;

    gMax.x += dotSize;
    gMax.y += dotSize;
}


void ofApp::update()
{
    for(int i = 0; i < 14; i++)
    {
        if(ofGetKeyPressed('m'))
        {
            for(int j = 0; j < 14; j++)
            {
                proxyFrame[j].melt();
            }
        }
        
        glm::vec2 proxyPos = dotData[dotIndex].fourteenDots[i] + proxyFrame[i].pos;
        
        if(proxyPos.x > ofGetWidth()-dotSize*2)
        {
            //proxyFrame[i].pos.x = ofGetWidth()-dotSize;
            proxyFrame[i].vel.x*=-.9;
        }
        
        if(proxyPos.x < dotSize*2)
        {
            //proxyFrame[i].pos.x = dotSize;
            proxyFrame[i].vel.x*=-.9;
        }
        
        if(proxyPos.y < dotSize*2)
        {
            //proxyFrame[i].pos.y = dotSize;
            proxyFrame[i].vel.y*=-.9;
        }
        
        if(proxyPos.y > ofGetHeight()-dotSize*2)
        {
            //proxyFrame[i].pos.y = ofGetHeight()-dotSize;
            proxyFrame[i].vel.y*=-.9;
        }
        
        proxyFrame[i].update();
    }
    
    
    
}

void ofApp::draw()
{
    dotFrame currDf = dotData[dotIndex];
    if(debug)
    {
        ofPushMatrix();
            ofTranslate(1000, 150);
            ofSetColor(0, 0, 255);
            font.drawString(ofToString(dotIndex), 0, 0);
        ofPopMatrix();
        
        
        
        ofPushMatrix();
            ofTranslate(800, 150);
            ofSetColor(0, 255, 0);
            font.drawString("minX = " + ofToString(currDf.x1) + " maxX = " + ofToString(currDf.x2), 0, 30);
            font.drawString("minY = " +  ofToString(currDf.y1) + " maxY = " + ofToString(currDf.y2), 0, 60);
            ofSetColor(255, 255, 0);
            font.drawString("gMinX = " + ofToString(gMin.x) + " gMaxX = " + ofToString(gMax.x), 0, 90);
            font.drawString("gMinY = " + ofToString(gMin.y) + " gMaxY = " + ofToString(gMax.y), 0, 120);
            ofSetColor(0, 0, 255);
            font.drawString("Global Size: " + ofToString(gMax.x - gMin.x) + ", " + ofToString(gMax.y - gMin.y), 0, 150);
        ofPopMatrix();
        
        // draw global rectangle
        ofSetColor(0, 0, 255, 200);
        ofDrawRectangle(gMin.x, gMin.y, gMax.x-gMin.x, gMax.y-gMin.y);
        
        // draw current frame rectangle
        ofSetColor(255, 0, 0, 200);
        ofDrawRectangle(currDf.x1, currDf.y1, currDf.x2-currDf.x1, currDf.y2-currDf.y1);
        
        // draw current dot frame
        for(int j = 0; j < dotData[0].fourteenDots.size(); j++)
        {
            ofColor dotColor = ofColor::fromHsb(ofMap(j, 0, dotData[0].fourteenDots.size(), 10, 245), 255, 255);
            ofSetColor(dotColor);
            ofDrawCircle(dotData[dotIndex].fourteenDots[j], dotSize);
            if(twoBods)
            {
                ofDrawCircle(dotData[dotIndex+1].fourteenDots[j], dotSize);
            }
        }
    }
    
    ///////////////////////////////// OUTPUT //////////////////////
    ///////////////////////////////// OUTPUT //////////////////////
    ///////////////////////////////// OUTPUT //////////////////////
    ///////////////////////////////// OUTPUT //////////////////////
    ///////////////////////////////// OUTPUT //////////////////////
    // The image to output
    else if(outputTest)
    {
//        ofSetColor(255);
//        refImage.draw(0, 0);
        
        ofScale(drawScale * (outputWidth/2048), drawScale * (outputWidth/2048));
        ofTranslate(drawX, drawY);
        
        for(int j = 0; j < dotData[0].fourteenDots.size(); j++)
        {
            ofColor dotColor = ofColor::fromHsb(ofMap(j, 0, dotData[0].fourteenDots.size(), 10, 245), 255, 255);
            ofSetColor(dotColor);
            ofFill();
            ofDrawCircle(dotData[dotIndex].fourteenDots[j] + proxyFrame[j].pos, dotSize);
            
            
            
            if(twoBods)
            {
                ofDrawCircle(dotData[dotIndex+1].fourteenDots[j], dotSize);
            }
        }
        
    }

    // if record
    else if(record)
    {
        for(int i = startIndex; i < endIndex; i++)
        {
            fbo.begin();
                ofPushMatrix();
                    ofScale(drawScale * (outputWidth/2048), drawScale * (outputWidth/2048));
                    ofTranslate(drawX, drawY);
            
                    ofClear(0);
                    for(int j = 0; j < dotData[0].fourteenDots.size(); j++)
                    {
                        ofColor dotColor = ofColor::fromHsb(ofMap(j, 0, dotData[0].fourteenDots.size(), 10, 245), 255, 255);
                        ofSetColor(dotColor);
                        ofDrawCircle(dotData[i].fourteenDots[j], dotSize);
                        if(twoBods)
                        {
                            ofDrawCircle(dotData[i+1].fourteenDots[j], dotSize);
                        }
                    }
                fbo.end();
                fbo.readToPixels(savePixels);
            ofPopMatrix();
            
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
    
        std::exit(1);
    }
    // if other modes are all false for some reason...
    else
    {
        debug = true;
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
    
    // switch modes (debug, outputTest, record)
    case 'b':
    case 'B':
        outputTest = false;
        record = false;
        debug = true;
        break;
            
    case 'o':
    case 'O':
        debug = false;
        record = false;
        outputTest = true;
        break;
            
    case 'r':
    case 'R':
        debug = false;
        outputTest = false;
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
        drawY+=1;
        std::cout << "------------------------" << std::endl;
        std::cout << "drawX =       " << drawX << std::endl;
        std::cout << "drawY =       " << drawY << std::endl;
        std::cout << "drawScale =   " << drawScale << std::endl;
        std::cout << "" << std::endl;
        break;
        
    case 'a':
    case 'A':
        drawX-=1;
        std::cout << "------------------------" << std::endl;
        std::cout << "drawX =       " << drawX << std::endl;
        std::cout << "drawY =       " << drawY << std::endl;
        std::cout << "drawScale =   " << drawScale << std::endl;
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
    }
}
