#include "ofApp.h"

/**
 * This sketch takes in X+Y position data from a .txt file, which was
 * generated from the Kinect motion tracking, and generates white dots.
 * White dots are rendered to an FBO and exported frame by frame.
 */

/*
 * 1. Load the file
 * 2. Parse the strings into a std::vector<glm::vec2>
 * 3. Push that vector back into a std::vector
 * 4.
 *
 */

void ofApp::setup()
{
    ofBackground(0);
    exportSize = 2048;
    
    fbo.allocate(2048, 1024, GL_RGB);
    savePixels.allocate(2048, 1024, GL_RGB);
    fbo.begin();
    ofClear(255,255,255, 0);
    fbo.end();
    
    ofSetCircleResolution(60);
    
    font.load("Arial.ttf", 20);
    // datasets: maroon, uw
    dataSet = "amalg_02";
    startIndex = 0;
    
    ofBuffer buffer = ofBufferFromFile("colors/" + dataSet + ".txt");
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
                std::cout << "SCREEEEAAM" << std::endl;
                df.fourteenDots.clear();
                df.x1 = 10000;
                df.y1 = 10000;
                df.x2 = 0;
                df.y2 = 0;
            }
            
            else
            {
                // store the x+y values in a glm::vec2
                glm::vec2 tempXY = {ofToFloat(parsedLine[0]), ofToFloat(parsedLine[1])};
                
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
                
                if (df.fourteenDots.size() == 14)
                {
                    dotData.push_back(df);
                }
            }
        }
    }
    
    // Find the min/max of the entire sequence
    
    std::cout << dotData.size() << std::endl;
    endIndex = dotData.size() - 1;
    
    //endIndex = dotData.size() - 50;
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
    
    gMin.x = 50.0;
    gMax.x = 685;
    gMin.y = 100.0;
    gMax.y = 620.0;
    
//    if(gMax.y-gMin.y > gMax.x-gMin.x)
//    {
//        float sizeDiff = ((gMax.y - gMin.y) - (gMax.x-gMin.x))/2;
//        gMin.x -= sizeDiff;
//        gMax.x += sizeDiff;
//    }
//    else
//    {
//        float sizeDiff = ((gMax.x - gMin.x) - (gMax.y - gMin.y))/2;
//        gMin.y -= sizeDiff;
//        gMax.y += sizeDiff;
//    }
    
    gMin.x -= dotSize;
    gMin.y -= dotSize;

    gMax.x += dotSize;
    gMax.y += dotSize;
    
    
}


void ofApp::update()
{
    if(print)
    {
        std::cout << "startIndex: " << startIndex << std::endl;
        std::cout << "endIndex: " << endIndex << std::endl;
        std::cout << "gMin.x: " << gMin.x << std::endl;
        std::cout << "gMax.x: " << gMax.x << std::endl;
        std::cout << "gMin.y: " << gMin.y << std::endl;
        std::cout << "gMax.y: " << gMax.y << std::endl;
        std::cout << "Global Size: " << gMax.x - gMin.x << ", " << gMax.y - gMin.y;
        
        print = false;
    }
}


void ofApp::draw()
{
    
    ofPushMatrix();
    ofTranslate(1000, 150);
    ofSetColor(0, 0, 255);
    font.drawString(ofToString(dotIndex), 0, 0);
    
    ofPopMatrix();
    
    dotFrame currDf = dotData[dotIndex];
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
    
    
    ofDrawRectangle(gMin.x, gMin.y, gMax.x-gMin.x, gMax.y-gMin.y);
    
    
    ofSetColor(255, 0, 0);
    //ofDrawRectangle(currDf.x1, currDf.y1, currDf.x2-currDf.x1, currDf.y2-currDf.y1);
    
    if(debug)
    {
        for(int j = 0; j < dotData[0].fourteenDots.size(); j++)
        {
            //ofSetColor(255);
            ofColor dotColor = ofColor::fromHsb(ofMap(j, 0, dotData[0].fourteenDots.size(), 10, 245), 255, 255);
            ofSetColor(dotColor);
            ofDrawCircle(dotData[dotIndex].fourteenDots[j], dotSize);
            if(twoBods)
            {
                ofDrawCircle(dotData[dotIndex+1].fourteenDots[j], dotSize);
            }
        }
    }

    else
    {
        
        for(int i = startIndex; i < endIndex; i++)
        {
            fbo.begin();
                ofPushMatrix();
            
                if(HD)
                {
                    //ofScale((gMax.x - gMin.x)*(1024/(gMax.y-gMin.y)), 1024/(gMax.y - gMin.y));
                    ofScale(1.832, 1.832);
                    ofTranslate(-gMin.x, -gMin.y);
                    
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
                }
                else
                {
                    ofTranslate(-gMin.x, -gMin.y);
                    ofScale(256.0/(gMax.x - gMin.x)*(exportSize/256.0), 256.0/(gMax.y-gMin.y)*(exportSize/256.0));
                    
                    
                    ofClear(0);
                    for(int j = 0; j < dotData[0].fourteenDots.size(); j++)
                    {
                        ofColor dotColor = ofColor::fromHsb(ofMap(j, 0, dotData[0].fourteenDots.size(), 10, 245), 255, 255);
                        ofSetColor(dotColor);
                        ofDrawCircle(dotData[i].fourteenDots[j], dotSize);
                    }
                }
            

            fbo.end();
        
            fbo.readToPixels(savePixels);
            ofPopMatrix();

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
            //ofSaveImage(savePixels, "../exports/colors/" + dataSet + "/1024-colorDots/" + dataSet + "dots_" + saveName, OF_IMAGE_QUALITY_BEST);
            ofSaveImage(savePixels, "/Users/dougrosman/openFrameworks/of_v20190120_osx_release/apps/possible_bodies/whiteDotsFromData/bin/exports/colors/" + dataSet + "/dots_" + saveName, OF_IMAGE_QUALITY_BEST);
            std::cout << saveName << std::endl;
//            std::cout << "gMin.x: " << gMin.x << std::endl;
//            std::cout << "gMin.y: " << gMin.y << std::endl;
//            std::cout << "gMax.x: " << gMax.x << std::endl;
//            std::cout << "gMax.y: " << gMax.y << std::endl;
        }
    
        std::exit(1);
    }
}

void ofApp::keyPressed(int key)
{
    if(key == '.' && dotIndex < dotData.size())
    {
        dotIndex+=incRate;
    }
    else if(key == ',' && dotIndex > 0)
    {
        dotIndex-=incRate;
    }
    else if(key == '[' && incRate > 0)
    {
        incRate--;
    }
    else if(key == ']' && incRate < 20)
    {
        incRate++;
    }
    
    
}
