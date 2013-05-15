//
//  KnctCtr.cpp
//  20130414_kinect2
//
//  Created by you tanaka @rettuce on 2013/04/27.
//
//

#include "KnctCtr.h"

using namespace ofxCv;
using namespace cv;
using namespace unframe;

KnctCtr::KnctCtr()
{
    POINT_SKIP = edgeSkip = 2;
    pointsize  = 5;
    angle = 0;
    ofRegisterKeyEvents(this);
}

KnctCtr::~KnctCtr(){}

void KnctCtr::exit(){
    for (int i=0; i<knct.size(); i++)
        knct[i]->close();
}

void KnctCtr::setup()
{
    for (int i=0; i<KINECT_LENG; i++)
    {
        knct.push_back(kinectPtr(new ofxKinect()));
        knct[i]->setRegistration(true);
        knct[i]->init();
        knct[i]->open();
        knct[i]->setCameraTiltAngle(angle);
    }

    dmesh.setup(knct[0],knct[1]);

    // debug
    mesh.push_back(ofVboMeshPtr( new ofVboMesh ));
    glPointSize(pointsize);
}

void KnctCtr::seupdate(float lv){ vol = lv; }

void KnctCtr::update()
{
    if(knct[0]->isConnected() && knct[1]->isConnected()) isRunning = true;
    if (!isRunning) return;

    if(isDebug){
        dev_update();
        return;
    }

    dmesh.reset();
    cols.clear();

    // Point追加 Edge
    getpos_edge();

    // Point追加 Kinect
    getpos_knct();

    // Delaunay計算
    if(dmesh.ps.size()>10){
        dmesh.setColors(cols);
        dmesh.seupdate(vol);
        dmesh.update();
    }
}

void KnctCtr::draw()
{
    if(!isRunning) return;

    if(isDebug){
        mesh[0]->draw();
    }else{
        dmesh.draw();
    }
    if(isMap) debugdraw();
    if(isEdgeDebug)
    {
        edge1.draw(-640, -300, 300);
        edge2.draw(   0, -300, 300);
    }
}



// get point - line edge

void KnctCtr::getpos_edge()
{
    convertColor( *knct[0], gray, CV_RGB2GRAY);
    Canny(gray, edge1, _CannyThreshold1, _CannyThreshold2, 3, true );
    edge1.update();
    edge_remap(&edge1, 0);

    convertColor( *knct[1], gray, CV_RGB2GRAY);
    Canny(gray, edge2, _CannyThreshold1, _CannyThreshold2, 3, true );
    edge2.update();
    edge_remap(&edge2, 1);
}

void KnctCtr::edge_remap(ofImage *edge, int ID)
{
    if(!isVisE) return;

    // Canny Edges Line → Points
    ofImage *tempImg = edge;

    int skip = edgeSkip;
    unsigned char * pixels2 = tempImg->getPixels();
    for (int i = int(tempImg->width*0.2); i < int(tempImg->width*0.8); i+=skip){
        for (int j = 0; j < tempImg->height; j+=skip){
            int brightness = pixels2[ j * tempImg->width + i];
            if(brightness>0)
            {
                float z = knct[ID]->getDistanceAt( i,j );
                if( ((ID==0)?dep1_N:dep2_N) < z && z < ((ID==0)?dep1_F:dep2_F))
                {
                    ofVec3f tempVec3 = rotateVec3( i,j,z, ID );
                    dmesh.ps.push_back( tempVec3 );
                    cols.push_back( knct[ID]->getColorAt( i,j ) );
                    setPos2d(i,j,ID);
                }
            }
        }
    }
}



// get point - 2 kinects

void KnctCtr::getpos_knct()
{
    for (int i=0; i<knct.size(); i++)
    {
        if(knct[i]->isConnected())
        {
            knct[i]->update();

            ofVec3f tempVec3;
            float z;
            ofPoint *p;
            for (int n=0; n<dmesh.ts1.size(); n++ )
            {
                p = (i==0)? &dmesh.ts1[n] : &dmesh.ts2[n];

                z = knct[i]->getDistanceAt( *p );
                if( ((i==0)?dep1_N:dep2_N) < z && z < ((i==0)?dep1_F:dep2_F))
                {
                    if(((i==0)?isVis1:isVis2) == true ){
                        tempVec3 = rotateVec3( p->x, p->y, z, i );
                        dmesh.ps.push_back( tempVec3 );
                        cols.push_back( knct[i]->getColorAt( *p ) );
                        setPos2d(p->x,p->y,i);
                    }
                }
            }
        }
    }
}


void KnctCtr::setPos2d(int x, int y, int knctId)
{
    pos2d pos;
    pos.x = x;
    pos.y = y;
    pos.KinectID = knctId;
    dmesh.is.push_back(pos);
}





















void KnctCtr::dev_update()
{
    for (int i=0; i<knct.size(); i++)
    {
        if(knct[i]->isConnected())
        {
            knct[i]->update();

            if(i==0) mesh[0]->clear();

            for (int h = 0; h < KINECT_HEIGHT; h+=POINT_SKIP)
            {
                for (int w = 0; w < KINECT_WIDTH; w+=POINT_SKIP)
                {
                    float z = knct[i]->getDistanceAt(w,h);
                    ofVec3f tempVec3;
                    ofColor tempCol;
                    if(i==0){
                        if(dep1_N < z && z < dep1_F)
                        {
                            z *= ZScl1;
                            tempVec3 = rotateDegY(ofVec3f(w-(KINECT_WIDTH/2)+D1.x, h-(KINECT_HEIGHT/2)+D1.y, z+D1.z), i);
                            tempCol  = knct[i]->getColorAt(w,h);
                            if(isVis1) mesh[0]->addVertex( tempVec3 );
                            if(isVis1) mesh[0]->addColor( tempCol );
                        }
                    }else{
                        if(dep2_N < z && z < dep2_F)
                        {
                            z *= ZScl2;
                            tempVec3 = rotateDegY(ofVec3f(w-(KINECT_WIDTH/2)+D2.x, h-(KINECT_HEIGHT/2)+D2.y, z+D2.z), i);
                            tempCol  = knct[i]->getColorAt(w,h);
                            if(isVis2) mesh[0]->addVertex( tempVec3 );
                            if(isVis2) mesh[0]->addColor( tempCol );
                        }
                    }
                }
                // mesh comp

            }

        }
    }
}



#define floor_size 1200
#define floor_skip 40

void KnctCtr::debugdraw()
{
    // 中心点
    ofSetHexColor(0xFF0000);
    ofLine(P1.x, floor_size/2, P1.z,
           P1.x, -floor_size/2, P1.z);
    ofSetHexColor(0x0000FF);
    ofLine(P2.x, floor_size/2, P2.z,
           P2.x, -floor_size/2, P2.z);

    // 基準線
    ofSetHexColor(0xBBBBBB);
    ofLine(0,-floor_size/2,0,
           0,floor_size/2,0);

    for (int i=0; i<=floor_skip; i++) {
        ofLine(
               -floor_size, floor_size/2, (-floor_size*2)/floor_skip*i+floor_size*2,
               floor_size,  floor_size/2, (-floor_size*2)/floor_skip*i+floor_size*2 );
        ofLine(
               (floor_size*2)/floor_skip*i-floor_size, floor_size/2, floor_size*2,
               (floor_size*2)/floor_skip*i-floor_size, floor_size/2, 0);
    }
}






void KnctCtr::keyPressed(ofKeyEventArgs &args)
{
    switch (args.key)
    {
        case '1':
            mesh[0]->setMode(OF_PRIMITIVE_POINTS);
            break;
        case '2':
            mesh[0]->setMode(OF_PRIMITIVE_LINES);
            break;
        case '3':
            mesh[0]->setMode(OF_PRIMITIVE_LINE_STRIP);
            break;
        case '4':
            mesh[0]->setMode(OF_PRIMITIVE_LINE_LOOP);
            break;
        case '5':
            mesh[0]->setMode(OF_PRIMITIVE_TRIANGLES);
            break;
        case '6':
            mesh[0]->setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
            break;
        case '7':
            mesh[0]->setMode(OF_PRIMITIVE_TRIANGLE_FAN);
            break;

        case OF_KEY_UP:
            break;

        case OF_KEY_DOWN:
            break;

        case OF_KEY_RIGHT:
            break;

        case OF_KEY_LEFT:
            break;

		case 'r':
            for (int i=0; i<knct.size(); i++){
                knct[i]->close();
                knct[i]->open();
            }

        case 's':
            break;
    }
}



// Y軸での回転行列
ofVec3f KnctCtr::rotateDegY(ofVec3f v, int i)
{
    if(i==0){
        return ofVec3f(
                       cos(rad1)*(v.x-P1.x) - sin(rad1)*(v.z+P1.z) + P1.x,
                       v.y,
                       sin(rad1)*(v.x-P1.x) + cos(rad1)*(v.z+P1.z) - P1.z);

    }else{
        return ofVec3f(
                       cos(rad2)*(v.x-P2.x) - sin(rad2)*(v.z+P2.z) + P2.x,
                       v.y,
                       sin(rad2)*(v.x-P2.x) + cos(rad2)*(v.z+P2.z) - P2.z);
    }
}
ofVec3f KnctCtr::rotateVec3(float x, float y, float z, int ID)
{
    x = x -(KINECT_WIDTH/2)  + ((ID==0)? D1.x : D2.x);
    y = y -(KINECT_HEIGHT/2) + ((ID==0)? D1.y : D2.y);
    z = ((ID==0)? z * ZScl1 : z * ZScl2 ) + ((ID==0)? D1.z : D2.z);
    return rotateDegY(ofVec3f(x,y,z),ID);
}




// GUI

void KnctCtr::setgui(ofxUICanvas *gui)
{
    this->gui = gui;
    ofAddListener(gui->newGUIEvent, this, &KnctCtr::guiEvent);

    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float barW  = 310;
    float barW3 = 101;
    float barH  = 12;
    float length = 320-xInit;


    gui->addSpacer(length, 1);
	gui->addWidgetDown(new ofxUILabel("CameraSetting", OFX_UI_FONT_MEDIUM));
    gui->addSlider("Kinect angle", -30, 30, 0.0, barW, barH);


    gui->addSpacer(length, 1);
	gui->addWidgetDown(new ofxUILabel("Kinect 1", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUIRangeSlider(length-xInit,barH,
                                            10.0, 3000.0, dep1_N, dep1_F,"Depth1 Near&Far"));
    gui->addSlider("Position1 X", -300.0, 300.0, P1.x, barW, barH);
    gui->addSlider("Position1 Z", 0.0, -1500.0, P1.z, barW, barH);
    gui->addSlider("Rotation1", -90.0, 90.0, R1, barW, barH);
    gui->addSlider("Distance1 X", -500, 500, D1.x, barW, barH);
    gui->addSlider("Distance1 Y", -500, 500, D1.x, barW, barH);
    gui->addSlider("Distance1 Z", -1000, 1000, D1.z, barW, barH);
    gui->addSlider("Z Scale1", 0.0, 1.0, ZScl1, barW, barH);


    gui->addSpacer(length, 1);
	gui->addWidgetDown(new ofxUILabel("Kinect 2", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUIRangeSlider(length-xInit,barH,
                                            10.0, 3000.0, dep2_N, dep2_F,"Depth2 Near&Far"));
    gui->addSlider("Position2 X", -300.0, 300.0, P2.x, barW, barH);
    gui->addSlider("Position2 Z", 0.0, -1500.0, P2.z, barW, barH);
    gui->addSlider("Rotation2", -90.0, 90.0, R2, barW, barH);
    gui->addSlider("Distance2 X", -500, 500, D2.x, barW, barH);
    gui->addSlider("Distance2 Y", -500, 500, D2.y, barW, barH);
    gui->addSlider("Distance2 Z", -1000, 1000, D2.z, barW, barH);
    gui->addSlider("Z Scale2", 0.0, 1.0, ZScl2, barW, barH);

    gui->addSpacer(length, 1);
	gui->addWidgetDown(new ofxUILabel("Canny Edge", OFX_UI_FONT_MEDIUM));
    gui->addWidgetDown(new ofxUIRangeSlider(length-xInit, barH,
                                            0.0, 200.0, _CannyThreshold1, _CannyThreshold2,
                                            "Canny Hysteresis Threshold"));
    gui->addSlider("Edge Point Skip", 1, 20, edgeSkip, barW, barH);

    gui->addSpacer(length, 1);
	gui->addWidgetDown(new ofxUILabel("Debug", OFX_UI_FONT_MEDIUM));
    gui->addToggle("Visible1", isVis1, 16, 16);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addToggle("Visible2", isVis2, 16, 16);
    gui->addToggle("VisibleE", isVisE, 16, 16);

    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addToggle("DevMode", isDebug, 16, 16);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addToggle("DevMap", isMap, 16, 16);
    gui->addToggle("DevEdge", isEdgeDebug, 16, 16);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addSlider("Point Size", 0, 50, pointsize, barW, barH);
    gui->addSlider("Point Length", 0, 10000, pointleng, barW, barH);
    gui->addWidgetDown(new ofxUIRangeSlider(length-xInit, barH,
                                            1.0, 1000.0, dmesh.leng_buffer_min, dmesh.leng_buffer_max,
                                            "Poly leng buffer"));

    gui->autoSizeToFitWidgets();
}

void KnctCtr::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();

    if(name == "Depth1 Near&Far"){
        ofxUIRangeSlider *rslider = (ofxUIRangeSlider *) e.widget;
		dep1_N = rslider->getScaledValueLow();
		dep1_F = rslider->getScaledValueHigh();

    }else if(name == "Distance1 X"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        D1.x = rslider->getScaledValue();

    }else if(name == "Distance1 Y"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        D1.y = rslider->getScaledValue();

    }else if(name == "Distance1 Z"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        D1.z = rslider->getScaledValue();

    }else if(name == "Position1 X"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
		P1.x = rslider->getScaledValue();

    }else if(name == "Position1 Z"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
		P1.z = rslider->getScaledValue();

    }else if(name == "Rotation1"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
		R1 = rslider->getScaledValue();
        rad1 = R1 * PI/180;

    }else if(name == "Z Scale1"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
		ZScl1 = rslider->getScaledValue();




    }else if(name == "Depth2 Near&Far"){
        ofxUIRangeSlider *rslider = (ofxUIRangeSlider *) e.widget;
		dep2_N = rslider->getScaledValueLow();
		dep2_F = rslider->getScaledValueHigh();

    }else if(name == "Distance2 X"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        D2.x = rslider->getScaledValue();

    }else if(name == "Distance2 Y"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        D2.y = rslider->getScaledValue();

    }else if(name == "Distance2 Z"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        D2.z = rslider->getScaledValue();

    }else if(name == "Position2 X"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
		P2.x = rslider->getScaledValue();

    }else if(name == "Position2 Z"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
		P2.z = rslider->getScaledValue();

    }else if(name == "Rotation2"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
		R2 = rslider->getScaledValue();
        rad2 = R2 * PI/180;

    }else if(name == "Z Scale2"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
		ZScl2 = rslider->getScaledValue();




    }else if(name == "Kinect angle"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        for (int i=0; i<knct.size(); i++)
            knct[i]->setCameraTiltAngle(rslider->getScaledValue());



    }else if(name == "DevEdge"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        isEdgeDebug = toggle->getValue();

    }else if(name == "DevMode"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        isDebug = toggle->getValue();

    }else if(name == "DevMap"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        isMap = toggle->getValue();

    }else if(name == "Visible1"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        isVis1 = toggle->getValue();

    }else if(name == "Visible2"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        isVis2 = toggle->getValue();

    }else if(name == "VisibleE"){
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        isVisE = toggle->getValue();




    }else if(name == "Point Length"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        pointleng = int(rslider->getScaledValue());
        dmesh.mapReset(pointleng);

    }else if(name == "Point Size"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        pointsize = int(rslider->getScaledValue());
        glPointSize(pointsize);

    }else if(name == "Poly leng buffer"){
        ofxUIRangeSlider *rslider = (ofxUIRangeSlider *) e.widget;
		dmesh.leng_buffer_min= rslider->getScaledValueLow();
		dmesh.leng_buffer_max = rslider->getScaledValueHigh();



    }else if(name == "Canny Hysteresis Threshold"){
        ofxUIRangeSlider *rslider = (ofxUIRangeSlider *) e.widget;
        _CannyThreshold1 = rslider->getScaledValueLow();
        _CannyThreshold2 = rslider->getScaledValueHigh();

    }else if(name == "Edge Point Skip"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        edgeSkip = int(rslider->getScaledValue());

    }
}





