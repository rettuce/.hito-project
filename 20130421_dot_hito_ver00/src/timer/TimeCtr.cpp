//
//  TimeCtr.cpp
//  20130421_dot_hito_ver00
//
//  Created by you tanaka @rettuce on 2013/04/27.
//
//

#include "TimeCtr.h"

using namespace unframe;

#define PORT 7012

const float ONE_SYCLE = 60.0f;
const float TIMING_CAP = 30.0f;
const float TIMING_UPLOAD = 45.0f;
const float TIMING_COPY = 50.0f;

const float BAR = 100.0f;
const float BARH = 3.0;

const float LEFT = 30.0;
const float TOP  = 40.0;
const float TOP2 = 20.0;

TimeCtr::TimeCtr()
{
    copy.loadImage("copy.png");
}

// ポインタ渡す
void TimeCtr::setup(SaveModel *save_, DelaunayMesh *dmesh_)
{
    receiver.setup( PORT );
    save = save_;
    dmesh = dmesh_;
}

void TimeCtr::update()
{
    oscupdate();

    sec = (ofGetElapsedTimef()-lastUpdated);

    if(!isStop){
        sec = pausetime;
    }

    // 60s Timer
    if( !isCap && sec > TIMING_CAP )
    {
        // Capture start
        isCap = true;
        save->startcap();
    }
    else if( !isUp && sec > TIMING_UPLOAD )
    {
        // Upload start
        isUp = true;

    }
    else if( !isCopy && sec > TIMING_COPY )
    {
        // copy start
        isCopy = true;

    }
    else if( sec > ONE_SYCLE )
    {
        timereset();
    }
}

void TimeCtr::draw()
{
    if(isCopy){
        msg = "capture...";
    }else if(isUp){
        msg = "upload...";
    }else if(isCap){
        msg = "capture...";
    }else{
        msg = "trial...";
    }

    if(!isCopy)
    {
        float r = LEFT;

        ofSetColor(255, 255, 255);
        ofRect( r, TOP2, BAR*( sec / TIMING_COPY ), BARH );
        ofDrawBitmapString( msg , r , TOP );

        ofSetColor(255, 255, 255, 100);
        ofRect( r, TOP2, BAR*( TIMING_CAP / TIMING_COPY ), BARH );

        ofSetColor(255, 0, 0, 100);
        ofRect( r + BAR*( TIMING_CAP / TIMING_COPY ), TOP2,
               BAR*( (TIMING_UPLOAD-TIMING_CAP) / TIMING_COPY ), BARH );

        ofSetColor(255, 255, 255, 100);
        ofRect( r + BAR*( TIMING_UPLOAD / TIMING_COPY ), TOP2,
               BAR*( (TIMING_COPY-TIMING_UPLOAD) / TIMING_COPY ), BARH );

        ofSetColor(255, 255, 255);

    }else{
        copy.draw((Global::W()/2)-(copy.width/2), (Global::H()/2)-(copy.height/2),
                  copy.width, copy.height );
    }
}

void TimeCtr::timereset()
{
    isCap = isUp = isCopy = false;
    lastUpdated = ofGetElapsedTimef();
}


// GUI

void TimeCtr::setgui(ofxUICanvas *gui)
{
    this->gui = gui;
    ofAddListener(gui->newGUIEvent, this, &TimeCtr::guiEvent);

    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float barW  = 310;
    float barW3 = 101;
    float barH  = 12;
    float length = 320-xInit;

    gui->addSpacer(length, 1);
	gui->addWidgetDown(new ofxUILabel("Capture Timer", OFX_UI_FONT_MEDIUM));
    gui->addToggle("CaptureRunning", isStop, 16, 16);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addButton("reset", false, 16, 16);
    gui->autoSizeToFitWidgets();
}

void TimeCtr::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    if(name == "CaptureRunning"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        isStop = toggle->getValue();
        if(!isStop) pausetime = sec;
        else timereset();

    }else if(name == "reset"){
		ofxUIButton *btn = (ofxUIButton *) e.widget;
		if(btn->getValue()){
            timereset();
            pausetime = 0;
        }

    }
}







void TimeCtr::oscupdate()
{
    //現在順番待ちのOSCメッセージがあるか確認
    while( receiver.hasWaitingMessages() )
    {
        //次のメッセージを取得
        ofxOscMessage m;
        receiver.getNextMessage( &m );

        if ( m.getAddress() == "/oF/reset" )
        {
            if(m.getArgAsFloat(0)==1){
                timereset();
                pausetime = 0;
            }

        }else if ( m.getAddress() == "/oF/param0" ){
            dmesh->keisu0 = m.getArgAsFloat(0)*10.0;


        }else if ( m.getAddress() == "/oF/param1" ){
            dmesh->keisu1 = m.getArgAsFloat(0)*10.0;

        }else if ( m.getAddress() == "/oF/param2" ){
            dmesh->keisu2 = m.getArgAsFloat(0)*10.0;

        }else if ( m.getAddress() == "/oF/param3" ){
            dmesh->keisu3 = m.getArgAsFloat(0);

        }

        //OSCメッセージをそのままコンソールに出力
        dumpOSC(m);

    }
}




//OSCメッセージをコンソールに出力する関数
void TimeCtr::dumpOSC(ofxOscMessage m) {
    string msg_string;
    msg_string = m.getAddress();
    for (int i=0; i<m.getNumArgs(); i++ ) {
        msg_string += " ";
        if(m.getArgType(i) == OFXOSC_TYPE_INT32)
            msg_string += ofToString( m.getArgAsInt32(i));
        else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT)
            msg_string += ofToString( m.getArgAsFloat(i));
        else if(m.getArgType(i) == OFXOSC_TYPE_STRING)
            msg_string += m.getArgAsString(i);
    }
    ofLog( OF_LOG_NOTICE, "OSC - "+msg_string );
}





