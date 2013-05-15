//
//  DelaunayMesh.cpp
//  DelaunayKinect
//
//  Created by you tanaka @rettuce on 2013/04/27.
//
//

#include "DelaunayMesh.h"

using namespace unframe;

//constructor
DelaunayMesh::DelaunayMesh()
{
    MaxPointNum = 1000;
    keisu0 = keisu1 = keisu2 = keisu3 = 1.0;
}

void DelaunayMesh::reset()
{
    dlnyclear();
    mesh.clear();
    faces.clear();
}
void DelaunayMesh::dlnyclear()
{
    ofxDelaunay::reset();
    inds.clear();
    verts.clear();
    ps.clear();
    is.clear();
}

void DelaunayMesh::mapReset(int num)
{
    MaxPointNum = num;
    ts1.clear();
    ts2.clear();
    for (int i=0; i< MaxPointNum; i++ ){
        ts1.push_back(ofPoint(int(ofRandom(KINECT_WIDTH*0.2,KINECT_WIDTH*0.8)), int(ofRandom(KINECT_HEIGHT)) ));
        ts2.push_back(ofPoint(int(ofRandom(KINECT_WIDTH*0.2,KINECT_WIDTH*0.8)), int(ofRandom(KINECT_HEIGHT)) ));
    }
}

void DelaunayMesh::setup( kinectPtr &knct1_, kinectPtr &knct2_ )
{
    knct1 = knct1_;
    knct2 = knct2_;
}

void DelaunayMesh::setColors(vector<ofColor> color)
{
    cols = color;
}

void DelaunayMesh::seupdate(float lv){ vol = lv; }

void DelaunayMesh::update()
{
    inds.clear();
    verts.clear();

    addPoints(ps);

    size = triangulate();
    inds  = triangleMesh.getIndices();
    verts = triangleMesh.getVertices();

    vector< ofVec3f >::iterator it;

    for (int i=0; i<inds.size()-3; i+=3)
    {
        int i0 = inds[i+0];
        int i1 = inds[i+1];
        int i2 = inds[i+2];

        float dist1 = ofDist(verts[i0].x, verts[i0].y, verts[i1].x, verts[i1].y);
        float dist2 = ofDist(verts[i1].x, verts[i1].y, verts[i2].x, verts[i2].y);
        float dist3 = ofDist(verts[i2].x, verts[i2].y, verts[i0].x, verts[i0].y);

        if(( dist1+dist2+dist3 > leng_buffer_min ) && ( dist1+dist2+dist3 < leng_buffer_max ))
        {
            ofColor setcol;
            ofVec2f p1,p2,p3;

            it = find( ps.begin(), ps.end(), verts[i0] );
            pos2d *pos1 = &is[int(it - ps.begin())];
            it = find( ps.begin(), ps.end(), verts[i1] );
            pos2d *pos2 = &is[int(it - ps.begin())];
            it = find( ps.begin(), ps.end(), verts[i2] );
            pos2d *pos3 = &is[int(it - ps.begin())];

            if((pos1->KinectID==pos2->KinectID)&&(pos3->KinectID==pos2->KinectID))
            {
                p1.set((pos1->x+pos2->x)*.5, (pos1->y+pos2->y)*.5);
                p2.set((pos2->x+pos3->x)*.5, (pos2->y+pos3->y)*.5);
                p3.set((pos3->x+pos1->x)*.5, (pos3->y+pos1->y)*.5);

                for (int q=0; q<3; q++)
                {
                    p1.set((p1.x+p2.x)*.5, (p1.y+p2.y)*.5);
                    p2.set((p2.x+p3.x)*.5, (p2.y+p3.y)*.5);
                    p3.set((p1.x+p3.x)*.5, (p1.y+p3.y)*.5);
                }

            }else{

                pos1 = pos2;
                p1.set(pos1->x,pos1->y);
            }

            if(pos1->KinectID==0){
                setcol = knct1->getColorAt(p1);
            }else{
                setcol = knct2->getColorAt(p1);
            }

            addFace(
                    &mesh,
                    verts[i0] + ofRandom(-vol*keisu0, vol*keisu0)*keisu3,
                    verts[i1] + ofRandom(-vol*keisu1, vol*keisu1)*keisu3,
                    verts[i2] + ofRandom(-vol*keisu2, vol*keisu2)*keisu3,
                    setcol
            );
        }
    }
    mesh.addIndices(faces);
}

void DelaunayMesh::draw()
{
    mesh.draw();
}


//--------------------------------------------------------------
void DelaunayMesh::addFace(ofVboMesh* mesh, ofVec3f a, ofVec3f b, ofVec3f c, ofColor col )
{
	mesh->addVertex(a);
	mesh->addVertex(b);
	mesh->addVertex(c);
    mesh->addColor(col);
    mesh->addColor(col);
    mesh->addColor(col);
    faces.push_back(faces.size());
    faces.push_back(faces.size());
    faces.push_back(faces.size());
}







