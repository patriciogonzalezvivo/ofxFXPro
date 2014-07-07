//
//  Shell.h
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
#include "UIAudioIn.h"

class Shell : public ofx3DPro {
public:

    string getSystemName(){return "Shell";}

    void selfSetupGuis();

    void selfSetup();
    void selfUpdate();
    void selfDraw();

    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);

    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
protected:
    UI3DGrid    grid;
    
    UIMatCap    matcap;
    UIBumpMap   bumpmap;
    UIHatchMap  hatching;
    
    UIShell     shell;
    UIAudioIn   audioIn;
};
