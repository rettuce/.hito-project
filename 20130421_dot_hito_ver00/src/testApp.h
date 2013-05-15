#pragma once

#include "Global.h"
#include "ofMain.h"
#include "ofxUI.h"

#include "./kinectdelaunay/KnctCtr.h"
#include "./datasave/SaveModel.h"
#include "./datapost/DataPost.h"
#include "./timer/TimeCtr.h"
#include "./sound/sound_input.h"

#define BUFFER_SIZE 1024
#define NUM_WINDOWS 80

namespace unframe
{

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();
        void guiset();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
	
    
    // debug
    ofVboMesh mesh;
    ofColor c0,c1,c2;
    ofVec3f v0,v1,v2,v3, v4,v5,v6,v7;
    vector<ofIndexType> faces;
    void addFace(ofVec3f a, ofVec3f b, ofVec3f c, ofColor col);
    void addFace(ofVec3f a, ofVec3f b, ofVec3f c, ofVec3f d, ofColor col);
    // debug
    
    ofLight light, reflections;
    ofEasyCam cam;
    float camDistance;
    
    unframe::TimeCtr timer;
    unframe::KnctCtr knct;
    unframe::SaveModel savemodel;
    unframe::DataPost datapost;
    
    
    // audio
    void sound_init();
    void sound_get();
    void audioReceived  (float * input, int bufferSize, int nChannels);
    float * left;
    float * right;
    float gain;
    float level;
    float timeline_array[257][2];
    int  bufferCounter;
    fft  myfft;
    float phase[BUFFER_SIZE];
    float power[BUFFER_SIZE];
    float freq[NUM_WINDOWS][BUFFER_SIZE/2];
    float freq_phase[NUM_WINDOWS][BUFFER_SIZE/2];
    float magnitude[BUFFER_SIZE];
    
    
    // GUI
    ofxUICanvas *gui, *gui2;
    void guiEvent(ofxUIEventArgs &e);
    ofColor bgColor;
};
}