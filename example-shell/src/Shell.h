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

#include "UIDoF.h"
#include "UIEdge.h"
//#include "UIAudioIn.h"

class Shell : public ofx3DPro {
public:

    string getSystemName(){return "Shell";}

    void selfSetupGuis();
    
    void selfSetupSystemGui();

    void selfSetup();
    void selfUpdate();
    void selfDraw();
    void selfPostDraw();

    void selfKeyPressed(ofKeyEventArgs & args);
    void selfKeyReleased(ofKeyEventArgs & args);

    void selfMouseDragged(ofMouseEventArgs& data);
    void selfMouseMoved(ofMouseEventArgs& data);
    void selfMousePressed(ofMouseEventArgs& data);
    void selfMouseReleased(ofMouseEventArgs& data);
    
protected:
    UIMatCap    matcap;
    UIBumpMap   bumpmap;
    UIHatchMap  hatching;
    
    UIShell     shell;
    //UIAudioIn   audioIn;
    
    UIDoF       dof;
    UIEdge      edge;
};
