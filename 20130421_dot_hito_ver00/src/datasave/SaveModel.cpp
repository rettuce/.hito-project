//
//  SaveModel.cpp
//  Created by you tanaka @rettuce on 2013/04/27.
//

#include "SaveModel.h"

using namespace unframe;

const string SAVE_PATH_GIF = "output/gif/";
const string SAVE_PATH_PNG = "output/png/";
const string SAVE_PATH_OBJ = "output/obj/";
const string SAVE_PATH_STL = "output/stl/";
const string SAVE_PATH_HITO = "output/hito/";

static const string empty_string;   // staticな空文字列

SaveModel::SaveModel()
{
    gifWidth  = 500;
    gifHeight = 500;
    gifframes = -1;
    gifcolors = 256;

    gifEncoder.setup(gifWidth, gifHeight, .25, gifcolors );
    ofAddListener(ofxGifEncoder::OFX_GIF_SAVE_FINISHED, this, &SaveModel::gifSaved );

    strs.push_back(strptr( new string ));   // hito
    strs.push_back(strptr( new string ));   // obj

    ofRegisterKeyEvents(this);
}

SaveModel::~SaveModel(){}

// ポインタ渡す
void SaveModel::setup(ofVboMesh* model_, DataPost *dp, TimeCtr *timer_)
{
    model = model_;
    datapost = dp;
    timer = timer_;
}

void SaveModel::update()
{
    // vertexの数が300以下の時はキャプチャしない
    if( model->getNumVertices() < 300 )
    {
        timer->timereset();
        return;
    }
    else if(isCapRunning)
    {
        if(ofGetElapsedTimef()-lastUpdated > updateIntervalSec)
        {
            ofLog(OF_LOG_NOTICE, "cap -> "+ofToString(gifframes) +" / "+ofToString(gifmaxframes));

            lastUpdated = ofGetElapsedTimef();
            capture();
            if(gifframes>gifmaxframes) save();
        }
    }
}



// clear
void SaveModel::clear()
{
    gifframes = 0;
    gifEncoder.reset();
}

// start gif capture
void SaveModel::startcap()
{
    // vertexの数が300以下の時はキャプチャしない
    if( model->getNumVertices() < 300 )
    {
        ofLog(OF_LOG_NOTICE,"NOW VERTEX NUM -> " + ofToString(model->getNumVertices()) +" -> Timer Clear !" );
        timer->timereset();
        return;
    }else{

        ofLog(OF_LOG_NOTICE,"NOW VERTEX NUM -> " + ofToString(model->getNumVertices()) +" -> Capture Start !" );
    }

    clear();
    isCapRunning = true;
    lastUpdated = ofGetElapsedTimef();
    gifmaxframes = int(giffps * giftime);
}

// 1 capture
// gifEncoderに1frameキャプチャする。1フレーム目だけpngも書き出す
void SaveModel::capture()
{
    if(gifframes==0)
    {
        time = time.now();
        fname = time.getDateTime("%Y%m%d_%H%M%S");
        ptime = time.getDateTime("%Y-%m-%d %H:%M:%S");  // post data
        cout<<" fname is -> "<<fname<<endl;
    }

    screengrab.clear();
    screengrab.allocate(Global::H(), Global::H(), OF_IMAGE_COLOR );
    screengrab.grabScreen((Global::W()-Global::H())/2, 0, Global::H(), Global::H());

    if(gifframes==1)
    {
        // 1 frame目をpngに
        screengrab.saveImage( ofToDataPath( SAVE_PATH_PNG ).c_str()+fname+".png" );

    }else if(gifframes==2){
        // 2 frame目で png POST
        datapost->post("png", ofToDataPath( SAVE_PATH_PNG ).c_str()+fname+".png" );

    }else if(gifframes==3){
        // 3 frame目で .obj作成
        saveObj( fname );

    }else if(gifframes==4){
        // 4 frame目で.obj POST
        datapost->post("obj", ofToDataPath( SAVE_PATH_OBJ ).c_str()+fname+".obj" );

    }else if(gifframes==5){
        // 5 frame目で .stl作成
        saveStl( fname );

    }else if(gifframes==6){
        // 6 frame目で.stl POST
        datapost->post("stl", ofToDataPath( SAVE_PATH_STL ).c_str()+fname+".stl" );

    }else if(gifframes==7){
        // 7 frame目で.hito作成
        saveHito( fname );

    }else if(gifframes==8){
        // 8 frame目で.hito POST
        datapost->post("hito", ofToDataPath( SAVE_PATH_HITO ).c_str()+fname+".hito" );

    }else if(gifframes==9){
        // 9 frame目で DB分 POST
        datapost->upload_data( ofToString(ID), ptime, fname, *strs[0], *strs[1]);
    }

    screengrab.resize(gifWidth, gifHeight);
    gifEncoder.addFrame( screengrab.getPixels(), gifWidth, gifHeight, 24, 1.0/giffps );
    gifframes++;
}

// model & capture save
// gifEncoderから撮り貯めたgifアニメ生成する。.hito,.obj,.stl も書き出す。
void SaveModel::save()
{
    // captur終了 gifファイル生成
    isCapRunning = false;
    gifEncoder.save( ofToDataPath( SAVE_PATH_GIF ).c_str() + fname + ".gif" );
}

// saved
void SaveModel::gifSaved(string &fileName)
{
    ofLog(OF_LOG_NOTICE, "gif saved as " + fileName );
    datapost->post("gif", fileName);

    // ID 保存
    ID++;     // post data
    maxidgui->setValue(float(ID));
    gui->saveSettings("GUI/guiSettings.xml");
    ofLog(OF_LOG_VERBOSE, "=== ID SAVE. "+ofToString(ID));
}

// object save
void SaveModel::saveObj(string filename)
{
    *strs[1] = empty_string;

	char _filename[128];
	sprintf(_filename, "%s", filename.c_str());

	char buf[256];
	sprintf(buf, "%s%s.obj", ofToDataPath( SAVE_PATH_OBJ ).c_str(), _filename);

    cout << "[Start .obj exporting...]" << buf << endl;

    FILE *file;
    vector <int> vert_lut;
    vector <bool> vert_flag;
    int point_num = 0;

    //    file = fopen(buf,"w");
    file = fopen(buf,"wb");
    if (file == NULL) {
        cout << "NULL!!" << endl;
        return;
    }
    fprintf(file, "#--- Rendered by rettuce .hito project @unframe v001 ---\n\n\n");
    fprintf(file, "g model_%s\n\n", _filename );
    *strs[1] = *strs[1] + "#--- Rendered by rettuce .hito project @unframe v001 ---\n\n\n";
    *strs[1] = *strs[1] + "g model_"+ _filename +"\n\n";

    point_num = 0;
    vert_lut.clear();

    int totalVert = model->getNumVertices();
    cout << "Total vertices::" << totalVert << endl;
    ofVec3f* vs = model->getVerticesPointer();
    for (int i = 0;i < totalVert;i++){
        bool bWritten = false;

        vert_flag.push_back(vs[i].z == 0);
        vert_lut.push_back(point_num+1);
        for (int o = 0;o < i;o++){
            if (vs[i] == vs[o]){
                vert_lut[vert_lut.size()-1] = vert_lut[o];
                bWritten = true;
            }
            if (bWritten) break;
        }
        fprintf(file, "v %f %f %f\n",
                vs[i].x,
                -vs[i].y,
                -vs[i].z);

        *strs[1] = *strs[1]+"v "+ofToString(vs[i].x)+" "+ofToString(-vs[i].y)+" "+ofToString(-vs[i].z)+"\n";
        point_num++;
    }
    cout << "End vertices" << endl;


    fprintf(file, "\n");
    int totalInd = model->getNumIndices();
    ofIndexType* vi = model->getIndexPointer();
    for (int i = 0;i < totalInd;i+=3)
    {
        if ((vert_flag[vi[i]] == false)&&
            (vert_flag[vi[i]] == false)&&
            (vert_flag[vi[i]] == false)&&
            (abs(vs[vi[i  ]].z - vs[vi[i+1]].z) < 500)&&
            (abs(vs[vi[i+1]].z - vs[vi[i+2]].z) < 500)&&
            (abs(vs[vi[i+2]].z - vs[vi[i  ]].z) < 500)){
            fprintf(file, "f %d/%d %d/%d %d/%d\n",
                    vert_lut[vi[i]],
                    vert_lut[vi[i]],
                    vert_lut[vi[i+1]],
                    vert_lut[vi[i+1]],
                    vert_lut[vi[i+2]],
                    vert_lut[vi[i+2]]);

            *strs[1] = *strs[1]+"f "+ofToString(vert_lut[vi[i]])+"/"+ofToString(vert_lut[vi[i]])+" "+ofToString(vert_lut[vi[i+1]])+"/"+ofToString(vert_lut[vi[i+1]])+" "+ofToString(vert_lut[vi[i+2]])+"/"+ofToString(vert_lut[vi[i+2]])+"\n";
        }
    }
    fclose(file);
}



// Hito save
void SaveModel::saveHito(string filename)
{
    *strs[0] = empty_string;

	char _filename[128];
	sprintf(_filename, "%s", filename.c_str());

	char buf[256];
	sprintf(buf, "%s%s.hito", ofToDataPath( SAVE_PATH_HITO ).c_str(), _filename);

    cout << "[Start .hito exporting...]" << buf << endl;

    FILE *file;
    int i;
    file = fopen(buf,"wb");
    if (file == NULL) {
        cout << "NULL!!" << endl;
        return;
    }

    fprintf(file, "#--- Rendered by rettuce .hito project @unframe v001 ---\n\n\n");
    fprintf(file, "g model_%s\n\n", _filename );
    *strs[0] = *strs[0] + "#--- Rendered by rettuce .hito project @unframe v001 ---\n\n\n";
    *strs[0] = *strs[0] + "g model_"+ _filename +"\n\n";

    int totalVert = model->getNumVertices();
    ofVec3f* vs = model->getVerticesPointer();
    for (i = 0;i < totalVert;i++){
        fprintf(file, "v %f %f %f\n",
                vs[i].x,
                -vs[i].y,
                -vs[i].z);
        *strs[0] = *strs[0]+"v "+ofToString(vs[i].x)+" "+ofToString(-vs[i].y)+" "+ofToString(-vs[i].z)+"\n";
    }

    fprintf(file, "\n");
    *strs[0] = *strs[0] + "\n";
    int totalInd = model->getNumIndices();
    ofIndexType* vi = model->getIndexPointer();
    for (i = 0;i < totalInd;i+=3){
        fprintf(file, "f %d %d %d\n",
                vi[i+0],
                vi[i+1],
                vi[i+2]);
        *strs[0] = *strs[0]+"f "+ofToString(vi[i+0])+" "+ofToString(vi[i+1])+" "+ofToString(vi[i+2])+"\n";
    }

    fprintf(file, "\n");
    *strs[0] = *strs[0] + "\n";
    int totalCol = model->getNumColors();
    ofFloatColor* ci = model->getColorsPointer();
    for (i = 0;i < totalCol; i+=3){
        fprintf(file, "c %f %f %f\n",
                ci[i].r,
                ci[i].g,
                ci[i].b);
        *strs[0] = *strs[0]+"c "+ofToString(ci[i].r)+" "+ofToString(ci[i].g)+" "+ofToString(ci[i].b)+"\n";
    }
    cout << "End colors" << endl;

    fclose(file);
}


// object save
void SaveModel::saveStl(string filename)
{
	char _filename[128];
	sprintf(_filename, "%s", filename.c_str());
    char buf[256];
	sprintf(buf, "%s%s.stl", ofToDataPath( SAVE_PATH_STL ).c_str(), _filename);

    cout << "[Start .hito exporting...]" << buf << endl;

    int totalVert = model->getNumVertices();
    ofVec3f* vs = model->getVerticesPointer();
    stlExporter.beginModel( _filename );
    for (int i = 0;i < totalVert;i++){
        stlExporter.addTriangle( vs[i+0], vs[i+1], vs[i+2], ofPoint(0.0f, 0.0f, 1.0f) );
    }
    stlExporter.useASCIIFormat(false); //export as binary
    stlExporter.saveModel(buf);

    cout << "End STL" << endl;
}






void SaveModel::keyPressed(ofKeyEventArgs &args)
{
    switch (args.key)
    {
        case 'c':
            startcap();
            break;
    }
}




// GUI

void SaveModel::setgui(ofxUICanvas *gui)
{
    this->gui = gui;
    ofAddListener(gui->newGUIEvent, this, &SaveModel::guiEvent);

    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float barW  = 310;
    float barW3 = 101;
    float barH  = 12;
    float length = 320-xInit;

    gui->addSpacer(length, 1);
	gui->addWidgetDown(new ofxUILabel("SaveImg", OFX_UI_FONT_MEDIUM));

    gui->addButton("SaveFileGif", false, 16, 16);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addButton("ClearCapture", false, 16, 16);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    gui->addSlider("gif width", 1, 500, gifWidth, barW*0.5, barH);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addSlider("gif height", 1, 500, gifHeight, barW*0.5, barH);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addSlider("gif fps", 1, 30, giffps, barW*0.5, barH);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addSlider("gif time", 0.1, 5.0, giftime, barW*0.5, barH);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    gui->addSlider("capture timing", 0.03, 3.0, captime, barW*0.5, barH);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addSlider("gif colors", 2, 256, gifcolors, barW*0.5, barH);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    maxidgui = gui->addSlider("ID", 0, 5000, ID, barW, barH);

    gui->autoSizeToFitWidgets();
}

void SaveModel::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();

    if(name == "gif width"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        gifWidth = int(rslider->getScaledValue());
        resize();

    }else if(name == "gif height"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        gifHeight = int(rslider->getScaledValue());
        resize();

    }else if(name == "SaveFileGif"){
        ofxUIButton *btn = (ofxUIButton *) e.widget;
		if(btn->getValue()){
            startcap();
        }

    }else if(name == "ClearCapture"){
        ofxUIButton *btn = (ofxUIButton *) e.widget;
		if(btn->getValue()) clear();


    }else if(name == "gif fps"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        giffps = int(rslider->getScaledValue());
        clear();

    }else if(name == "gif time"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        giftime = rslider->getScaledValue();

    }else if(name == "capture timing"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        captime = rslider->getScaledValue();
        updateIntervalSec = captime;

    }else if(name == "gif colors"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
        gifcolors = int(rslider->getScaledValue());
        gifEncoder.setNumColors(gifcolors);
        clear();

    }else if(name == "ID"){
		ofxUISlider *rslider = (ofxUISlider *) e.widget;
		ID = int(rslider->getScaledValue());

    }
}


void SaveModel::resize()
{
    clear();
    gifEncoder.setup(gifWidth, gifHeight);
}


