//
//  DataPost.h
//  20130421_dot_hito_ver00
//
//  Created by you tanaka @rettuce on 2013/04/27.
//
//

#ifndef ___0130421_dot_hito_ver00__DataPost__
#define ___0130421_dot_hito_ver00__DataPost__

#include <iostream>

#include "ofMain.h"
#include "ofxUI.h"
#include "../Global.h"

#include "ofxHttpUtils.h"

namespace unframe
{
    class DataPost
    {
    public:
        DataPost();
        ~DataPost();

        void setgui(ofxUICanvas *gui);

        void setup();
        void update();
        void draw();

        void post(string key, string path);
        void upload_file( string path, string url );
        void upload_data(string id, string date, string name, string hito, string obj );

    private:

        void newResponse(ofxHttpResponse & response);

        ofxHttpForm form;
        ofxHttpUtils httpUtils;
        int counter;
        string responseStr, requestStr;

        // GUI
        ofxUICanvas *gui;
        void guiEvent(ofxUIEventArgs &e);

    };
}

#endif /* defined(___0130421_dot_hito_ver00__DataPost__) */
