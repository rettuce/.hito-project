//
//  KnctCtr.h
//  20130414_kinect2
//
//  Created by you tanaka @rettuce on 2013/04/27.
//
//

#ifndef ___0130414_kinect2__KnctCtr__
#define ___0130414_kinect2__KnctCtr__

#include <iostream>

#include "../Global.h"

#include "ofxCv.h"
#include "ofxUI.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "DelaunayMesh.h"
#include "ofMain.h"

namespace unframe
{
    typedef ofPtr<ofxKinect> kinectPtr;
    typedef vector<kinectPtr> kinectPtrVec;

    typedef ofPtr<ofVboMesh> ofVboMeshPtr;
    typedef vector<ofVboMeshPtr> ofVboMeshPtrVec;

    class KnctCtr
    {
    public:
        KnctCtr();
        ~KnctCtr();

        bool isRunning;     // Kinect.isConnected?

        void setup();
        void update();
        void draw();

        void seupdate(float lv);

        void getpos_edge();
        void getpos_knct();

        void dev_update();
        void dev_draw();

        void exit();
        void setgui(ofxUICanvas *gui);

        void keyPressed(ofKeyEventArgs &args);
        void keyReleased(ofKeyEventArgs &args){};

        unframe::DelaunayMesh dmesh;

    protected:
    private:

        // volume level
        float vol;

        ofPoint P1,P2;     // 回転行列用任意の点 P ここを中心として回転する
        ofPoint D1,D2;     // モデル位置調整点 D モデルをカメラに対してX,Z軸で操作
        float R1,R2, rad1,rad2;   // 回転角度
        float dep1_N,dep2_N, dep1_F,dep2_F; // 取得データから有効深度の調整
        float ZScl1, ZScl2;         // Z 深度のスケール率
        int POINT_SKIP, angle;      // Kinectの取得Skip数とKinectのカメラアングル
        ofVec3f rotateDegY(ofVec3f v, int i);  // Y軸の回転行列により新しい点を返す
        ofVec3f rotateVec3(float x, float y, float z, int ID);  // rotateDegY使用関数


        kinectPtrVec knct;
        ofVboMeshPtrVec mesh;       // debug用

        vector<ofColor> cols;      // 最後のポリゴンface 記録用
        vector<ofPoint> inds;      // 最後のポリゴンface Index記録用

        void setPos2d(int x, int y, int index);    // 2D画面位置保存用

        // edge抽出
        ofPixels gray;
        ofImage edge1,edge2;           // edge抽出用
        float _CannyThreshold1, _CannyThreshold2 ;
        void edge_remap(ofImage *edge, int ID);

        void debugdraw();
        bool isDebug, isMap, isVis1, isVis2, isVisE, isEdgeDebug;
        int pointsize, pointleng, edgeSkip;

        // GUI
        ofxUICanvas *gui;
        void guiEvent(ofxUIEventArgs &e);
    };

}

#endif /* defined(___0130414_kinect2__KnctCtr__) */
