//
//  ObjLoader.cpp
//
//  Created by Patricio Gonzalez Vivo on 9/23/13.
//
//
#include "ObjLoader.h"

void ObjLoader::selfSetup(){
    ofSetVerticalSync(true);
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    ofShowCursor();
    ofSetFullscreen(false);
    
    ofxAssimpModelLoader meshLoader;
    meshLoader.enableTextures();
    meshLoader.enableMaterials();
    meshLoader.loadModel("pgv-mesh.obj", true);
    
    mesh = meshLoader.getMesh(0);
    meshCenter = mesh.getCentroid();
    
    ofDisableArbTex();
    ofLoadImage(meshTexture,"pgv-color.png");
    ofEnableArbTex();
    
    bumpMap.texture = meshTexture;
    cube.texture = meshTexture;
    
    cube.load(getDataPath()+"shaders/cubeReflection");
}

void ObjLoader::selfSetupGuis(){
    backgroundSet(new UIMapBackground());
    lightAdd("SPOT", OF_LIGHT_SPOT);
    guiAdd(grid);
    guiAdd(bumpMap);
    guiAdd(sphereMap);
    guiAdd(hatchShader);
    guiAdd(cube);
}

void ObjLoader::selfSetupSystemGui(){
    
}

void ObjLoader::guiSystemEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if(name.find("terrain_") == 0){
    }
}

void ObjLoader::selfSetupRenderGui(){
    
}

void ObjLoader::guiRenderEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
}

void ObjLoader::selfUpdate(){

}

void ObjLoader::selfDraw(){
    
    ofPushMatrix();
    ofTranslate(0, -50, 0);
    grid.draw();
    ofPopMatrix();
    
    materials["MATERIAL 1"]->begin();
    
    float scale = 3;
    
    cube.begin();
    bumpMap.begin();
    sphereMap.begin();
    hatchShader.begin();
    
    ofPushMatrix();
    ofScale(scale,scale,scale);
    ofTranslate(-meshCenter);
    mesh.draw();
    ofPopMatrix();

    hatchShader.end();
    sphereMap.end();
    bumpMap.end();
    cube.end();
    
    materials["MATERIAL 1"]->end();
}