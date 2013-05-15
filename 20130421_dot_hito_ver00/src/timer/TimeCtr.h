//
//  TimeCtr.h
//  20130421_dot_hito_ver00
//
//  Created by you tanaka @rettuce on 2013/04/27.
//
//

#ifndef ___0130421_dot_hito_ver00__TimeCtr__
#define ___0130421_dot_hito_ver00__TimeCtr__

#include "../Global.h"
#include "ofMain.h"
#include "ofxUI.h"
#include "ofxOsc.h"
#include "../datasave/SaveModel.h"
#include "../kinectdelaunay/DelaunayMesh.h"

#include <iostream>

namespace unframe
{
    class SaveModel;  // 相互参照用の前方宣言
    class TimeCtr
    {
    public:
        TimeCtr();
        virtual ~TimeCtr(){};
        void setup(SaveModel *save, DelaunayMesh *dmesh_);
        void update();
        void draw();
        void setgui(ofxUICanvas *gui);

        bool isCap, isUp, isCopy, isStop;
        void timereset();

    protected:
    private:

        ofxOscReceiver  receiver;
        void oscupdate();
        void dumpOSC(ofxOscMessage m);

        DelaunayMesh *dmesh;

        SaveModel *save;
        float lastUpdated, sec, pausetime;
        string msg;
        ofImage copy;

        // GUI
        ofxUICanvas *gui;
        void guiEvent(ofxUIEventArgs &e);

        int getMaxCount;

    };

}
#endif /* defined(___0130421_dot_hito_ver00__TimeCtr__) */
