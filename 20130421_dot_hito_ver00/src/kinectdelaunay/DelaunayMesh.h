//
//  DelaunayMesh.h
//  DelaunayKinect
//
//  Created by you tanaka @rettuce on 2013/04/27.
//
//

#ifndef __DelaunayKinect__DelaunayMesh__
#define __DelaunayKinect__DelaunayMesh__

#include <iostream>
#include "ofxDelaunay.h"
#include "../Global.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

namespace unframe
{
    typedef ofPtr<ofxKinect> kinectPtr;

    class DelaunayMesh : public ofxDelaunay
    {
    public:

        DelaunayMesh();
        virtual ~DelaunayMesh(){};

        void setColors(vector<ofColor> color);
        void setup( kinectPtr &knct1_, kinectPtr &knct2_ );
        void update();
        void draw();
        void reset();
        void dlnyclear();

        // kinect
        kinectPtr knct1,knct2;

        // triangleMesh     // 計算用 既存オブジェクト　ofVboMesh
        int MaxPointNum;    // ランダムポイント数 上限
        void mapReset(int num); // ランダムポイント再配置

        vector<pos2d> is;               // 最後のface計算用
        vector<ofPoint> ps,ts1,ts2;     // 計算前ポイント, ランダムポイント1,2用
        vector<ofColor> cols;           // Kinectから取得したカラー情報
        vector<ofIndexType> inds;       // = dlny.triangleMesh.getIndices();
        vector<ofVec3f> verts;          // = dlny.triangleMesh.getVertices();
        int size;

        ofVboMesh mesh;             // 実際に描かれるオブジェクト
        vector<ofIndexType> faces;  // 表面Index

        void addFace(ofVboMesh* mesh, ofVec3f a, ofVec3f b, ofVec3f c, ofColor col );


        // 三角形の一辺が大きすぎるかどうかで不必要なトライアングル削除
        float leng_buffer_min, leng_buffer_max;

        // volume level
        float vol;
        float keisu0,keisu1,keisu2,keisu3;
        void seupdate(float lv);

    protected:
    private:
    };

}

#endif /* defined(__DelaunayKinect__DelaunayMesh__) */
