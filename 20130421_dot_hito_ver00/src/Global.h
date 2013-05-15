//
//  Global.h
//  All Parameter file.
//
//  Created by you tanaka @rettuce on 2013/04/27.
//

#ifndef _20130401_TestGlobal_h
#define _20130401_TestGlobal_h

#include "ofMain.h"

namespace unframe
{

#define KINECT_LENG 2
#define KINECT_WIDTH 640
#define KINECT_HEIGHT 480

    struct pos2d{
        int x, y;
        int KinectID;   // 0 or 1
    };

    static float STW;
    static float STH;
    static int FPS = 60;

    class Global{
        public :
        static void W(float w_){ STW = w_; return W(); }
        static float W(){ return STW; }
        static void H(float h_){ STH = h_; return H(); }
        static float H(){ return STH; }
    };
}

#endif
