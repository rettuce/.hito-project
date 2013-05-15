//
//  SaveModel.h
//  Created by you tanaka @rettuce on 2013/04/27.
//

#ifndef ___20130401_SaveModel__
#define ___20130401_SaveModel__

#include <iostream>

#include "ofxGifEncoder.h"
#include "ofxDateTime.h"
#include "ofxUI.h"
#include "ofxSTLExporter.h"

#include "../Global.h"
#include "ofMain.h"

#include "../datapost/DataPost.h"
#include "../timer/TimeCtr.h"

namespace unframe
{
    class TimeCtr;  // 相互参照用の前方宣言

    typedef ofPtr<string> strptr;
    typedef vector<strptr> strptrVec;

    class SaveModel
    {
    public:
        SaveModel();
        ~SaveModel();

        void setgui(ofxUICanvas *gui);
        void setup(ofVboMesh * model_, DataPost * datapost, TimeCtr *timer_);
        void update();
        void draw();

        void resize();
        void clear();
        void capture();
        void startcap();
        void save();
        void gifSaved(string &fileName);

        void fpsset(int _f);
        void colorset(int _c);

        void keyPressed(ofKeyEventArgs &args);
        void keyReleased(ofKeyEventArgs &args){};

        void saveObj(string filename);
        void saveHito(string filename);
        void saveStl(string filename);

    protected:
    private:

        strptrVec strs; // post用ファイル 一時保存

        TimeCtr *timer;
        DataPost *datapost;
        ofVboMesh* model;
        string fname, ptime;
        bool isCapRunning;

        // capture & gif
        ofImage screengrab;
        float updateIntervalSec, lastUpdated;

        ofxDateTime time;
        int capcount;
        int gifWidth, gifHeight;
        int gifframes, gifmaxframes, gifcolors;
        float giffps, giftime, captime;
        ofxGifEncoder gifEncoder;
        ofxSTLExporter stlExporter;

        // GUI
        int ID; // save毎にカウントアップ
        ofxUICanvas *gui;
        ofxUISlider *maxidgui;
        void guiEvent(ofxUIEventArgs &e);
    };
}

#endif /* defined(___20130401_SaveGif__) */
