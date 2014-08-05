//
//  UICubeMap.cpp
//
//  Created by Patricio Gonzalez Vivo on 8/4/14 using http://www.3dcpptutorials.sk/index.php?id=24
//
//

#include "UICubeMap.h"

UICubeMap::UICubeMap(){
    vertexShader = "#version 120\n\n\
\n\
void main(void){\n\
    gl_TexCoord[0].stp = normalize(gl_Vertex.xyz);\n\
    \n\
    gl_Position    = ftransform();\n\
}";
    
    fragmentShader = "#version 120\n\
\n\
uniform samplerCube EnvMap;\n\
\n\
void main (void){\n\
    gl_FragColor = textureCube(EnvMap, gl_TexCoord[0].stp);\n\
}";
    
    setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    linkProgram();
    
    debugShader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    debugShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    debugShader.linkProgram();
    
    extractUniforms(fragmentShader);
    
    bAllocated = false;
    bDebug = false;
}

void UICubeMap::setupUI(){
    
    int w = gui->getGlobalButtonDimension();

    {
        gui->setGlobalButtonDimension(34);
        vector<string> list;
        string path = "GUI/cubeMap";
        ofDirectory dir(path);
        int count = 0;
        if(dir.exists()){
            dir.listDir();
            int total = dir.getFiles().size();
            for (int i = 0; i < total; i++) {
                list.push_back( dir.getName(i) );
                gui->addImageToggle("cubeMap/"+dir.getName(i), "GUI/cubeMap/"+dir.getName(i),false);
                gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
                count++;
                if(count == 5){
                    count = 0;
                    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
                }
            }
        }
        gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
        gui->addSpacer();
    }

    gui->setGlobalButtonDimension(w);
    
    gui->addToggle("DEBUG", &bDebug);
    UIBumpMap::setupUI();
}

void UICubeMap::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if (name.find('.') != string::npos){
        
        ofxUIImageToggle *toggle = (ofxUIImageToggle*)e.widget;
        
        if(toggle->getValue() == true){
            if(name.find("cubeMap") == 0){
                loadMap("GUI/"+name);
                
                vector<ofxUIWidget*> w = gui->getWidgets();
                for (int i = 0; i < w.size(); i++) {
                    string n = w[i]->getName();
                    
                    if( w[i]->getKind() == 20 &&
                       n.find('.') != string::npos &&
                       n.find("cubeMap") == 0 &&
                       n != name){
                        
                        ofxUIImageToggle* t = (ofxUIImageToggle*)w[i];
                        t->setValue(false);
                    }
                }
                
            }
        }
    }
    
    UIBumpMap::guiEvent(e);
}

void UICubeMap::loadMap( string _image ){
    ofPixels pixels;
    if (ofLoadImage(pixels, _image)){
        size = pixels.getWidth()/4.;
        
        if(size*3==pixels.getHeight()){
            cout<<"Seems like a cubeMap!"<<endl;
        } else {
            cout<<"DON'T seems like a valid cubeMap"<<endl;
        }
        
        ofPixels pos_x,pos_y,pos_z,neg_x,neg_y,neg_z;
        pixels.cropTo(pos_x, size*2, size, size, size); // RIGHT
        pixels.cropTo(pos_y, size, 0, size, size);      // TOP
        pixels.cropTo(neg_z, size*3, size, size, size); // BACK
        pixels.cropTo(neg_x, 0, size, size, size);      // LEFT
        pixels.cropTo(neg_y, size, size*2, size, size); // BOTTOM
        pixels.cropTo(pos_z, size, size, size, size);   // FRONT
        
        glGenTextures(1, &textureObjectID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureObjectID);
        
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
#ifndef TARGET_OPENGLES
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // GL_TEXTURE_WRAP_R is not in the ES2 header, hmm..
#endif
        
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
        unsigned char * data_px, * data_nx, * data_py, * data_ny, * data_pz, * data_nz;
        data_px = pos_x.getPixels();
        data_py = pos_y.getPixels();
        data_pz = pos_z.getPixels();
        data_nx = neg_x.getPixels();
        data_ny = neg_y.getPixels();
        data_nz = neg_z.getPixels();
        
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data_px); // positive x
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data_py); // positive y
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data_pz); // positive z
        
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data_nx); // negative x
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data_ny); // negative y
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data_nz); // negative z
        
        bAllocated = true;
    }
}

string UICubeMap::getClassName(){
    if(fragFile.isFile()){
        return fragFile.getBaseName();
    } else {
        return "CubeMap";
    }
}

void UICubeMap::begin(){
    if(bEnable&&bAllocated){
        if(bDebug){
            draw();
        }
        
        glEnable( GL_TEXTURE_CUBE_MAP );
        glBindTexture( GL_TEXTURE_CUBE_MAP, textureObjectID );
//        UIBumpMap::begin();
        UIShader::begin();
        if(normal.isAllocated()){
            setUniformTexture("tNormal", normal, 1);
        } else {
            setUniform1f("normalScale", 0);
        }
        if(texture.isAllocated()){
            setUniformTexture("tColor", texture, 2);
            setUniform1f("_useColorTexture", 1);
        } else {
            setUniform1f("_useColorTexture", 0);
        }
    }
}

void UICubeMap::end(){
    if(bEnable&&bAllocated){
        UIBumpMap::end();
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0 );
        glDisable( GL_TEXTURE_CUBE_MAP );
    }
    
}

void UICubeMap::draw(){
    if(bEnable&&bAllocated){
        glEnable( GL_TEXTURE_CUBE_MAP );
        glBindTexture( GL_TEXTURE_CUBE_MAP, textureObjectID );
        debugShader.begin();
        ofDrawBox(5000);
        debugShader.end();
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0 );
        glDisable( GL_TEXTURE_CUBE_MAP );
    }
}