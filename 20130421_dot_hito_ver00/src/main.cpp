#include "Global.h"
#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

using namespace unframe;

//========================================================================
int main( )
{
    Global::W(1440.0);
    Global::H(900.0);
    ofAppGlutWindow window;
    window.setGlutDisplayString("rgba double depth alpha samples>=4");
	ofSetupOpenGL(&window, Global::W(), Global::H(), OF_WINDOW);
	ofRunApp( new testApp());
}
