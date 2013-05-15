//
//  DataPost.cpp
//  20130421_dot_hito_ver00
//
//  Created by you tanaka @rettuce on 2013/04/27.
//
//

#include "DataPost.h"

using namespace unframe;

const string HOST = "http://hito.rettuce.com/";

const string UPLOAD_GIF = ".php";
const string UPLOAD_PNG = ".php";
const string UPLOAD_OBJ = ".php";
const string UPLOAD_STL = ".php";
const string UPLOAD_HITO = ".php";
const string WRITE_DB = ".php";

DataPost::DataPost()
{
	ofAddListener(httpUtils.newResponseEvent,this,&DataPost::newResponse);
	httpUtils.start();
}
DataPost::~DataPost(){}

void DataPost::setup()
{
}

void DataPost::update()
{
}

void DataPost::draw()
{
}

void DataPost::post(string key, string path )
{
    if(key=="png"){
        upload_file(path, UPLOAD_PNG);

    }else if (key=="gif"){
        upload_file(path, UPLOAD_GIF);

    }else if (key=="hito"){
        upload_file(path, UPLOAD_HITO);

    }else if (key=="obj"){
        upload_file(path, UPLOAD_OBJ);

    }else if (key=="stl"){
        upload_file(path, UPLOAD_STL);

    }
}


void DataPost::upload_file(string path, string url )
{
    form.action = HOST + url;
    form.method = OFX_HTTP_POST;
    form.addFile("file", path );
    httpUtils.addForm(form);
    requestStr = "file sent to server : " + url;
    ofLog( OF_LOG_NOTICE, requestStr );
}

void DataPost::upload_data(string id, string date, string name, string hito, string obj )
{
    form.action = HOST + WRITE_DB;
    form.method = OFX_HTTP_POST;
    form.addFormField("ID", id );
    form.addFormField("DATE", date );   // 2013-04-19 07:52:56
    form.addFormField("NAME", name );
    form.addFormField("HITO", hito );
    form.addFormField("OBJ",  obj );
    httpUtils.addForm(form);
    requestStr = "data post to server : " + HOST + WRITE_DB;
    ofLog( OF_LOG_NOTICE, requestStr );
}


//--------------------------------------------------------------
void DataPost::newResponse(ofxHttpResponse & response)
{
    // URLアクセス成功
    if(response.status==200){
        responseStr = ofToString(response.status) + ": " + (string)response.responseBody;
        ofLog( OF_LOG_NOTICE, responseStr );
    }
}









