#include "testApp.h"

#define TITLE ".hito project ver.001"

using namespace unframe;

//--------------------------------------------------------------
void testApp::setup()
{
    // window setting
    ofSetFrameRate(60);
    ofSetCircleResolution(60);
	ofEnableSmoothing();
    
    knct.setup();
    savemodel.setup( &knct.dmesh.mesh, &datapost, &timer );
    timer.setup( &savemodel, &knct.dmesh );
    
    // audio
    gain = 2;
    level = 0.000001;
    sound_init();
    
    guiset();
}

//--------------------------------------------------------------
void testApp::update()
{
    ofSetWindowTitle("fps "+ofToString(int(ofGetFrameRate())));
    
    sound_get();
    knct.seupdate(level*10.0);
    knct.update();
    savemodel.update();
    timer.update();
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofBackground(bgColor);

    if(!timer.isCopy)
    {
        glEnable(GL_DEPTH_TEST);
        glPushMatrix();
        cam.begin();
        cam.setDistance(camDistance);
        ofRotateX(180);
        
        knct.draw();
        
        cam.end();
        glPopMatrix();
        glDisable(GL_DEPTH_TEST);
    }
    
    timer.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key)
    {
        case 'g':
            gui->toggleVisible();
            gui2->toggleVisible();
            break;
            
        case 'f':
            ofToggleFullscreen();
            break;
            
        case 's':
            gui->saveSettings("GUI/guiSettings.xml");
            gui2->saveSettings("GUI/guiSettings2.xml");
            ofLog(OF_LOG_NOTICE, "=== Save GUI Settings.");
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    Global::W(w);
    Global::H(h);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
//--------------------------------------------------------------
void testApp::exit(){
    knct.exit();
    delete gui;
    delete gui2;
}






// debug
//--------------------------------------------------------------
void testApp::addFace(ofVec3f a, ofVec3f b, ofVec3f c, ofColor col )
{
	mesh.addVertex(a);
	mesh.addVertex(b);
	mesh.addVertex(c);
    mesh.addColor(col);
    mesh.addColor(col);
    mesh.addColor(col);
    faces.push_back(faces.size());
    faces.push_back(faces.size());
    faces.push_back(faces.size());
}

void testApp::addFace(ofVec3f a, ofVec3f b, ofVec3f c, ofVec3f d, ofColor col)
{
    addFace(a, b, c, ofColor(ofRandom(255),ofRandom(255),ofRandom(255)));
	addFace(a, c, d, ofColor(ofRandom(255),ofRandom(255),ofRandom(255)));
}







// GUI

void testApp::guiset()
{
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float barW  = 310;
    float barW3 = 101;
    float barH  = 12;
    float length = 320-xInit;
    
    gui = new ofxUICanvas(0,0,length+xInit*2.0,ofGetHeight());
    ofAddListener(gui->newGUIEvent, this, &testApp::guiEvent);
	gui->addWidgetDown(new ofxUILabel( string(TITLE), OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUIFPS(OFX_UI_FONT_SMALL));
    
    gui->addSpacer(length, 1);
    gui->addToggle("FullScreen", false, 16, 16);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addButton("Save", false, 16, 16);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->addSpacer(length, 1);
    gui->addSlider("R", 0.0, 255.0, bgColor.r, barW3, barH);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addSlider("G", 0.0, 255.0, bgColor.g, barW3, barH);
    gui->addSlider("B", 0.0, 255.0, bgColor.b, barW3, barH);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    /**  add other class widget. **/
    savemodel.setgui(gui);
    timer.setgui(gui);
    
    
    
    gui2 = new ofxUICanvas(length+xInit+4,0,length+xInit,ofGetHeight());
    ofAddListener(gui2->newGUIEvent, this, &testApp::guiEvent);    
    gui2->addSlider("Camera Distance", -1000, 1000, camDistance, barW, barH);
    /**  add other class widget. **/
    knct.setgui(gui2);
    
    
    
    gui->autoSizeToFitWidgets();
    gui2->autoSizeToFitWidgets();
    gui->loadSettings("GUI/guiSettings.xml");
    gui2->loadSettings("GUI/guiSettings2.xml");
    
    gui->setColorBack(ofColor(0,0,0,180));
    gui2->setColorBack(ofColor(0,0,0,180));
}

void testApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    
    if(name == "FullScreen"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        ofSetFullscreen(toggle->getValue());
        
    }else if(name == "Save"){
		ofxUIButton *btn = (ofxUIButton *) e.widget;
		if(btn->getValue()){
            gui->saveSettings("GUI/guiSettings.xml");
            gui2->saveSettings("GUI/guiSettings2.xml");
            ofLog(OF_LOG_NOTICE, "=== Save GUI Settings.");
        }
        
    }else if(name == "R"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
		bgColor.r = rslider->getScaledValue();
        
	}else if(name == "G"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
		bgColor.g = rslider->getScaledValue();
        
	}else if(name == "B"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
		bgColor.b = rslider->getScaledValue();
        
        
    }else if(name == "Camera Distance"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        camDistance = rslider->getScaledValue();
        
    }
    
    
}








// audio

void testApp::sound_init(){
    // 0 output channels,
    // 2 input channels
    // 44100 samples per second
    // BUFFER_SIZE samples per buffer
    // 4 num buffers (latency)
    
    ofSoundStreamSetup(0,2,this, 44100,BUFFER_SIZE, 4);
    left = new float[BUFFER_SIZE];
    right = new float[BUFFER_SIZE];
    
    for (int i = 0; i < NUM_WINDOWS; i++)
    {
        for (int j = 0; j < BUFFER_SIZE/2; j++)
        {
            freq[i][j] = 0;
        }
    }
}

void testApp::sound_get(){
    static int index=0;
    float avg_power = 0.0f;
    
    if(index < 80)
        index += 1;
    else
        index = 0;
    
    /* do the FFT */
    myfft.powerSpectrum(0,(int)BUFFER_SIZE/2, left,BUFFER_SIZE,&magnitude[0],&phase[0],&power[0],&avg_power);
    
    /* start from 1 because mag[0] = DC component */
    /* and discard the upper half of the buffer */
    level = 0;
    for(int j=1; j < BUFFER_SIZE/2; j++) {
        freq[index][j] = magnitude[j]*gain*3;
        level += magnitude[j];
        magnitude[j] *= gain*2;
    }
    level = level*gain;
    level *= 0.00001;
}


void testApp::audioReceived(float * input, int bufferSize, int nChannels){
    // samples are "interleaved"
    for (int i = 0; i < bufferSize; i++){
        left[i] = input[i*2];
        right[i] = input[i*2+1];
    }
    bufferCounter++;
}





