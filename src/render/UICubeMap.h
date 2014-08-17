//
//  UICubeMap.h
//  example
//
//  Created by Patricio Gonzalez Vivo on 8/4/14.
//
//

#include "UIShader.h"

class UICubeMap : public UIShader {
public:
    UICubeMap();
    
    void    setupUI();
    
    void    loadMap( string _image );
    void    loadNormal(string _path);
    
    string  getClassName();
    
    void    begin();
    void    end();
    
    void    draw();
    
    ofTexture   normal;
    ofTexture   texture;
    
    float   cubeSize;
    bool    bDrawCube;
    
protected:
    void    guiEvent(ofxUIEventArgs &e);
    
    ofShader debugShader;
    GLuint  textureObjectID;
    int     size;
    
    bool    bAllocated;
};