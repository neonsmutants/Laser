#include "testApp.h"

float diff = 0.0f;

//--------------------------------------------------------------
void testApp::setup()
{
    //OF setup
    ofSetVerticalSync(true);
	ofBackground(255, 114, 66);
    
    //VIDEO
    warpEnabled = true;
    treatImg = 1;
    doHSV = true;
    doBlur = false;
    dilateFirst = true;
    doInv = false;
    wichImg = 2;

    chan1Min =22.0;
	chan1Max = 255.0;

	chan2Min = 0;
	chan2Max = 106.0;

	chan3Min = 0;
	chan3Max = 255.0;

    dilateNtimes = 1;
    minBlobArea = 1;
    maxBlobArea = 1024;
    
    maxPlayers = 50;
    
    //warp related
    xVidPos  = 20;
    yVidPos = 20;
    
    
    //wichPoint = 0;  = inutilisé... à virer

    //camera related
    ps3eye.initGrabber(camW, camH);
    
    //OSC realted
    sender.setup( HOST, PORT );
    sendOsc= false;
    
    setGUI();
    
    ////////////////////////////////////////////////////

    chanName[0] = "HUE :";
    chanName[1] = "SAT :";
    chanName[2] = "VAL :";

    //image allocate
    colorImg.allocate(camW, camH);					// Live Cam as input source
    colorImgTrans.allocate(camW, camH);

    channels[0].allocate(camW, camH);					// Hue Image
    channels[1].allocate(camW, camH);					// Saturation Image
    channels[2].allocate(camW, camH);					// value Image

    ttImg.allocate(camW, camH);
    warpedImg.allocate(camW, camH);
    trackedTexture.allocate(camW, camH, OF_IMAGE_GRAYSCALE);

    colorTrackedPixels = new unsigned char [camW * camH];	// Tracked Image

    //keyStone
    srcPositions  = new ofPoint[4];
	srcPositions[0].set(XML.getValue("SRC:X1", xVidPos), XML.getValue("SRC:Y1", yVidPos), 0);
	srcPositions[1].set(XML.getValue("SRC:X2", camW + xVidPos), XML.getValue("SRC:Y2", yVidPos), 0);
	srcPositions[2].set(XML.getValue("SRC:X3", camW + xVidPos), XML.getValue("SRC:Y3", camH + yVidPos), 0);
	srcPositions[3].set(XML.getValue("SRC:X4", xVidPos), XML.getValue("SRC:Y4", camH + yVidPos), 0);

	dstPositions  = new ofPoint[4];
	dstPositions[0].set(xVidPos, yVidPos, 0);
	dstPositions[1].set(camW + xVidPos, yVidPos, 0);
	dstPositions[2].set(camW + xVidPos, camH + yVidPos, 0);
	dstPositions[3].set(xVidPos, camH + yVidPos, 0);
}

//--------------------------------------------------------------
void testApp::update()
{
    ps3eye.update();

	if(ps3eye.isFrameNew())
	{
        colorImg.setFromPixels(ps3eye.getPixels(), ps3eye.getWidth(), ps3eye.getHeight());
        colorImgTrans = colorImg;
        colorImgTrans.convertRgbToHsv();
        colorImgTrans.convertToGrayscalePlanarImages(channels[0], channels[1], channels[2]);

        unsigned char * chan3Pixels = channels[2].getPixels();

		int nPixels = camW * camH;

        for (int i = 0; i < nPixels; i++)
		{
			if (chan3Pixels[i]>=chan3Min && chan3Pixels[i]<= chan3Max)
			{
				colorTrackedPixels[i] = 255;
			}
			else
			{
				colorTrackedPixels[i] = 0;
			}
        }

        trackedTexture.setFromPixels(colorTrackedPixels, ps3eye.getWidth(), ps3eye.getHeight(), OF_IMAGE_GRAYSCALE);
        ttImg.setFromPixels(trackedTexture.getPixels(), ps3eye.getWidth(), ps3eye.getHeight());
        warpedImg.warpIntoMe(ttImg, srcPositions, dstPositions);

        //for(int i = 0; i<dilateNtimes;i++) warpedImg.dilate_3x3();
        warpedImg.dilate_3x3();
        
        if(warpEnabled)
        {
            contourFinder.findContours(warpedImg, minBlobArea, camW*camH, maxPlayers, false);
        }
        else
        {
            contourFinder.findContours(ttImg, minBlobArea, maxBlobArea, maxPlayers, false);
        }

        if(contourFinder.nBlobs>0)
        {
            sendAllPointeurs();
        }
        else
        {
            sendNoLasers();
        }
    }
    
    
    if(keyIsDown['t'] && keyIsDown[OF_KEY_UP])
    {
        srcPositions[0].y -= 1;
    }
    else if(keyIsDown['t'] && keyIsDown[OF_KEY_DOWN])
    {
        srcPositions[0].y += 1;
    }
    else if(keyIsDown['t'] && keyIsDown[OF_KEY_LEFT])
    {
        srcPositions[0].x -= 1;
    }
    else if(keyIsDown['t'] && keyIsDown[OF_KEY_RIGHT])
    {
        srcPositions[0].x += 1;
    }
    
    else if(keyIsDown['y'] && keyIsDown[OF_KEY_UP])
    {
        srcPositions[1].y -= 1;
    }
    else if(keyIsDown['y'] && keyIsDown[OF_KEY_DOWN])
    {
        srcPositions[1].y += 1;
    }
    else if(keyIsDown['y'] && keyIsDown[OF_KEY_LEFT])
    {
        srcPositions[1].x -= 1;
    }
    else if(keyIsDown['y'] && keyIsDown[OF_KEY_RIGHT])
    {
        srcPositions[1].x += 1;
    }
    
    else if(keyIsDown['h'] && keyIsDown[OF_KEY_UP])
    {
        srcPositions[2].y -= 1;
    }
    else if(keyIsDown['h'] && keyIsDown[OF_KEY_DOWN])
    {
        srcPositions[2].y += 1;
    }
    else if(keyIsDown['h'] && keyIsDown[OF_KEY_LEFT])
    {
        srcPositions[2].x -= 1;
    }
    else if(keyIsDown['h'] && keyIsDown[OF_KEY_RIGHT])
    {
        srcPositions[2].x += 1;
    }
    
    else if(keyIsDown['g'] && keyIsDown[OF_KEY_UP])
    {
        srcPositions[3].y -= 1;
    }
    else if(keyIsDown['g'] && keyIsDown[OF_KEY_DOWN])
    {
        srcPositions[3].y += 1;
    }
    else if(keyIsDown['g'] && keyIsDown[OF_KEY_LEFT])
    {
        srcPositions[3].x -= 1;
    }
    else if(keyIsDown['g'] && keyIsDown[OF_KEY_RIGHT])
    {
        srcPositions[3].x += 1;
    }
}
//--------------------------------------------------------------
void testApp::draw()
{
	ofSetColor(255);
    
    //DRAW RIGHT SCREENS
    ttImg.draw(20, 20, 640, 480);
    contourFinder.draw(20, 20, 640, 480);
    
    //DRAW LEFT SCREENS
    warpedImg.draw(680, 20, 320, 240);
    channels[2].draw(680, 260, 320, 240);
    
    ofDrawBitmapString("Laser Tracker V0.1", 20, 15);
    ofDrawBitmapString("Warped Image", 690, 40);
        
    // Draw keyStone
    ofNoFill();
    ofBeginShape();
    for (int j = 0; j < 4; j++) ofVertex(srcPositions[j].x, srcPositions[j].y);
    ofEndShape(true);
    ofFill();
    ofSetColor(255);
    for (int j = 0; j < 4; j++) ofCircle(srcPositions[j].x, srcPositions[j].y, 3);
    
    //Draw GUI
    guiTrackingSettings->enable();
    ofDrawBitmapString("s = save settings / t, y, h, g = set keystone / v = reset keystone / k = enable warping", 20, 520, 0);
    ofDrawBitmapString("Send OSC to " + ofToString(HOST) + " on port " + ofToString(PORT), 20, 540, 0);
}

void testApp::keyPressed(int key)
{
    switch(key)
	{
        case('a'):
            treatImg = 1;
        break;
        case('z'):
            treatImg = 2;
        break;
        case('e'):
            treatImg = 3;
        break;
        case('r'):
            treatImg = 4;
        break;
        case('b'):
            doBlur = !doBlur;
        break;
        case('d'):
            dilateFirst = !dilateFirst;
        break;
        case('i'):
            doInv = !doInv;
        break;

        case('t'):
            if(contourFinder.blobs.size() > 0)
            {
                srcPositions[0].x = contourFinder.blobs[0].centroid.x + xVidPos;
                srcPositions[0].y = contourFinder.blobs[0].centroid.y + yVidPos;
            }
        break;

        case('y'):
            if(contourFinder.blobs.size() > 0)
            {
                srcPositions[1].x = contourFinder.blobs[0].centroid.x + xVidPos;
                srcPositions[1].y = contourFinder.blobs[0].centroid.y + yVidPos;
            }
        break;

        case('h'):
            if(contourFinder.blobs.size() > 0)
            {
                srcPositions[2].x = contourFinder.blobs[0].centroid.x + xVidPos;
                srcPositions[2].y = contourFinder.blobs[0].centroid.y + yVidPos;
            }
        break;

        case('g'):
            if(contourFinder.blobs.size() > 0)
            {
                srcPositions[3].x = contourFinder.blobs[0].centroid.x + xVidPos;
                srcPositions[3].y = contourFinder.blobs[0].centroid.y + yVidPos;
            }
        break;

        case('s'):
            guiTrackingSettings->saveSettings("settings/settings.xml");
            XML.setValue("SRC:X1", srcPositions[0].x);
            XML.setValue("SRC:Y1", srcPositions[0].y);
            XML.setValue("SRC:X2", srcPositions[1].x);
            XML.setValue("SRC:Y2", srcPositions[1].y);
            XML.setValue("SRC:X3", srcPositions[2].x);
            XML.setValue("SRC:Y3", srcPositions[2].y);
            XML.setValue("SRC:X4", srcPositions[3].x);
            XML.setValue("SRC:Y4", srcPositions[3].y);

            if (XML.saveFile("settings/corners.xml")) cout<<"keystone saved"<<endl;
        break;
            
        case('l'):
			guiTrackingSettings->loadSettings("settings/settings.xml");
            
			if(XML.loadFile("settings/corners.xml"))
			{
                cout<<"keystone loaded"<<endl;
				srcPositions[0].set(XML.getValue("SRC:X1", xVidPos), XML.getValue("SRC:Y1", yVidPos), 0);
				srcPositions[1].set(XML.getValue("SRC:X2", camW + xVidPos), XML.getValue("SRC:Y2", yVidPos), 0);
				srcPositions[2].set(XML.getValue("SRC:X3", camW + xVidPos), XML.getValue("SRC:Y3", camH + yVidPos), 0);
				srcPositions[3].set(XML.getValue("SRC:X4", xVidPos), XML.getValue("SRC:Y4", camH + yVidPos), 0);
			}
			else
			{
				cout<<"Unable to load"<<endl;
			}
        break;

        case('v'):
            srcPositions[0].set(xVidPos, yVidPos, 0);
            srcPositions[1].set(ps3eye.getWidth() + xVidPos, yVidPos, 0);
            srcPositions[2].set(ps3eye.getWidth() + xVidPos, ps3eye.getHeight() + yVidPos, 0);
            srcPositions[3].set(xVidPos, ps3eye.getHeight() + yVidPos, 0);
        break;

        case('k'):
            warpEnabled = !warpEnabled;
        break;
            
        case ('='):
            ps3eye.videoSettings();
        break;
    }
    
    
    keyIsDown[key] = true;
}

void testApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.widget->getName();

    if(name == "CHAN1")
    {
        ofxUIRangeSlider *vslider = (ofxUIRangeSlider *) e.widget;
        chan1Min = vslider->getScaledValueLow();
        chan1Max = vslider->getScaledValueHigh();

    }
    else if(name == "CHAN2")
    {
        ofxUIRangeSlider *vslider = (ofxUIRangeSlider *) e.widget;
        chan2Min = vslider->getScaledValueLow();
        chan2Max = vslider->getScaledValueHigh();

    }
    else if(name == "CHAN3")
    {
        ofxUIRangeSlider *vslider = (ofxUIRangeSlider *) e.widget;
        chan3Min = vslider->getScaledValueLow();
        chan3Max = vslider->getScaledValueHigh();
    }
    else if(name == "GHOST")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        ghost = (int)slider->getScaledValue();
    }
    else if(name == "DILT")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        dilateNtimes = slider->getScaledValue();
    }
    else if(name == "AREA")
    {
        ofxUIRangeSlider *slider = (ofxUIRangeSlider *) e.widget;
        minBlobArea = int(slider->getScaledValueLow());
        maxBlobArea = int(slider->getScaledValueHigh());
    }
    else if(name == "MAX_BLOBS")
    {
        ofxUISlider *slider = (ofxUISlider *) e.widget;
        maxPlayers = slider->getValue();
    }
}

//////////////////////////////////////////

void testApp::sendAllPointeurs()
{
    ofxOscMessage m;
    m.setAddress("/newFrame");
    sender.sendMessage(m);
    
    for (int i=0; i < contourFinder.nBlobs;i++)
	{
        ofxOscMessage m;
        m.setAddress("/laser");
        m.addIntArg(i);
        m.addIntArg((contourFinder.blobs[i].centroid.x));
        m.addIntArg((contourFinder.blobs[i].centroid.y));
        m.addIntArg(contourFinder.blobs[i].area);
        
        m.addIntArg(contourFinder.blobs[i].boundingRect.width);
        m.addIntArg(contourFinder.blobs[i].boundingRect.height);
        m.addIntArg(contourFinder.blobs[i].boundingRect.getCenter().x);
        m.addIntArg(contourFinder.blobs[i].boundingRect.getCenter().y);
        sender.sendMessage(m);
    }
}

void testApp::sendNoLasers()
{
    ofxOscMessage m;
    m.setAddress("/noLasers");
    sender.sendMessage(m);
}

void testApp::setGUI()
{
    guiTrackingSettings = new ofxUICanvas(20, ofGetHeight() - 90, ofGetWidth() - 40, 70);

	int width = 255;
	int height = 20;
	
    guiTrackingSettings->addWidgetRight(new ofxUIRangeSlider("CHAN3", chan3Min, chan3Max, 0.0, 255.0, width, height));
    guiTrackingSettings->addWidgetRight(new ofxUIRangeSlider("AREA", minBlobArea, maxBlobArea, minBlobArea, maxBlobArea, width, height));
    //guiTrackingSettings->addWidgetRight(new ofxUISlider("MAX_BLOBS", 1, 100, 10, width, height));
    
    //guiTrackingSettings->addWidgetRight(new ofxUISlider("GHOST", 0, 10, 1, width, height));
    //guiTrackingSettings->addWidgetRight(new ofxUISlider(width,height, 0, 100, 2, "DISPL"));
    //guiTrackingSettings->addWidgetDown(new ofxUISlider(width,height, 0, 50, dilateNtimes, "DILT"));

	guiTrackingSettings->addSpacer(255, 2);
    guiTrackingSettings->addWidgetDown(new ofxUIFPS(OFX_UI_FONT_MEDIUM));
	//
	ofAddListener(guiTrackingSettings->newGUIEvent,this,&testApp::guiEvent);
	//
	guiTrackingSettings->loadSettings("settings/settings.xml");

	//init gui + keystone
    if(XML.loadFile("settings/corners.xml")) cout<<"keystone loaded"<<endl;
	else cout<<"unable to load keystone"<<endl;
}

void testApp::exit()
{
    guiTrackingSettings->saveSettings("GUI/guiSettings.xml");  
	delete guiTrackingSettings;
}

void testApp::windowResized(int, int){}
void testApp::keyReleased(int key){keyIsDown[key] = false;}
void testApp::mouseMoved(int, int){}
void testApp::mouseDragged(int, int, int){}
void testApp::mousePressed(int, int, int){}
void testApp::mouseReleased(int, int, int){}
void testApp::dragEvent(ofDragInfo){}
void testApp::gotMessage(ofMessage){}
