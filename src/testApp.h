#pragma once

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxUI.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"
#include "ofxMacamPs3Eye.h"

#define HOST "127.0.0.1"
#define PORT 12345

#define camW 640
#define camH 480

#define screenW 1024.0f
#define screenH 768.0f

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void setGUI();
    
        Boolean keyIsDown[360];

        ofxXmlSettings          movieGuiSettings;
        ofxXmlSettings          XML;//corners.xml
    
        ofxMacamPs3Eye ps3eye;

        ofxCvColorImage		    colorImg;
        ofxCvColorImage		    colorImgTrans;

        ofxCvGrayscaleImage		chan1Img;
        ofxCvGrayscaleImage		chan2Img;
        ofxCvGrayscaleImage		chan3Img;


        ofxCvGrayscaleImage		channels[3];

        string                  chanName[3];

        ofxCvGrayscaleImage     ttImg, warpedImg;

        ///warp stuff
        ofRectangle             warpGui;
        ofPoint *				srcPositions;
        ofPoint *				dstPositions;
        int                     activePoint;
        int                     wichPoint, xVidPos, yVidPos;
        /////
        bool                    doHSV, doBlur, dilateFirst, doInv, warpEnabled, sendOsc, clicked;

        unsigned char *         colorTrackedPixels;

        ofImage                 trackedTexture;

        int wichImg, chan1Min, chan1Max, chan2Min, chan2Max, chan3Min, chan3Max, dilateNtimes;
        float chan1, chan1Width, chan2, chan2Width, chan3, chan3Width, minDispl, ghost, minBlobArea, maxBlobArea;
        int treatImg, thresh, maxPlayers;
    
        ofxUICanvas *guiTrackingSettings;

        void guiEvent(ofxUIEventArgs &e);

        ofxCvContourFinder contourFinder;

        ofxOscSender sender;

        void sendPosP0();

        void loadMovieGuiSettings(string wich);

        ofxCvColorImage fillColorImg;
        ofxCvGrayscaleImage fillGrayImg;

        /////
    
		void sendAllPointeurs();
		void sendNoLasers();
};

