#include "ofApp.h"


void ofApp::setup()
{
    // constants to set at the beginning
    outputWidth = 1024;
    outputHeight = 512;
    drawScale = 2.21874;
    drawX = 116;
    drawY = -94;
    dotSize = 9 * (drawScale * outputWidth/2048);
    ofSetCircleResolution(80);
    dataSet = "amalg_02";
    
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
        for(int j = 0; j < 14; j)
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
    
    
}


void ofApp::update()
{

}


void ofApp::draw()
{

}


void ofApp::keyPressed(int key)
{

}

