//
//  UICubeMap.h
//  example
//
//  Created by Patricio Gonzalez Vivo on 8/4/14.
//
//

#include "UIBumpMap.h"

class UICubeMap : public UIBumpMap {
public:
    UICubeMap();
    
    void    setupUI();
    
    void    loadMap( string _image );
    
    string  getClassName();
    
    void    begin();
    void    end();
    
    void    draw();
    
    float   cubeSize;
    bool    bDrawCube;
    
protected:
    void    guiEvent(ofxUIEventArgs &e);
    
    ofShader debugShader;
    GLuint  textureObjectID;
    int     size;
    
    bool    bAllocated;
};