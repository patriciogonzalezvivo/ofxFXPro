//
//  ObjLoader.h
//
//  Created by Patricio Gonzalez Vivo on 9/23/13.
//
//

#pragma once

#include "ofx3DPro.h"
#include "ofxAssimpModelLoader.h"

#include "UIMapBackground.h"
#include "UI3DGrid.h"

#include "UIMatCap.h"
#include "UIBumpMap.h"
#include "UIHatching.h"

class ObjLoader : public ofx3DPro {
public:
    
    string getSystemName(){ return "ObjLoader";}
    
    void selfSetupGuis();
    
    void selfSetupSystemGui();
    void guiSystemEvent(ofxUIEventArgs &e);
    
    void selfSetupRenderGui();
    void guiRenderEvent(ofxUIEventArgs &e);
    void selfSetup();
    void selfUpdate();
    
    void selfDraw();
    
protected:
    
    ofVboMesh   mesh;
    ofTexture   meshTexture;
    ofPoint     meshCenter;
    
    UIMatCap    sphereMap;
    UIBumpMap   bumpMap;
    UIHatching    hatchShader;
    
    UI3DGrid    grid;
};