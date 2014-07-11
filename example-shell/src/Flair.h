//
//  Flair.h
//
//  Created by Patricio Gonzalez Vivo on 9/6/13.
//
//

#pragma once

#include "ofxPro.h"

#include "UI3DGrid.h"
#include "UIMapBackground.h"

#include "UIMatCap.h"
#include "UIBumpMap.h"
#include "UIHatchMap.h"

#include "UIShell.h"

//#include "UIAudioIn.h"

#include "ofxBlur.h"

class Flair : public ofx3DPro {
public:

    string getSystemName(){return "Flair";}

    void selfSetupGuis();
    
    void selfSetupSystemGui();

    void selfSetup();
    void selfUpdate();
    void selfDraw();
    void selfPostDraw();
    
    void selfWindowResized(ofResizeEventArgs & args);
    
protected:
    UIMatCap    matcap;
    UIBumpMap   bumpmap;
    UIHatchMap  hatching;
    
    UIShell     shell;
    
    //  Light Flair
    //
    UIShader scattering;
    ofFbo lightRays;
    
    UIShader thresholdShader;
    ofFbo thresholdFbo;
    
    UIShader flareShader;
    UIShader modulateTextureShader;
    PingPong lensFlareFbo;
    
    ofImage sunImage;
    ofImage dirtImage;
};
