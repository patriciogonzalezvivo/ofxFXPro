//
//  UICubeMap.cpp
//
//  Created by Patricio Gonzalez Vivo on 8/4/14 using http://www.3dcpptutorials.sk/index.php?id=24
//
//

#include "UICubeMap.h"
#include "ShaderFunctions.h"

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
    debugShader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    debugShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    debugShader.linkProgram();
    
    vertexShader = "#version 120\n\n\
    \n\
varying vec4 vPos;\n\
varying vec3 vNormal;\n\
varying vec3 vEye;\n\
varying vec2 vTexCoord;\n\
\n\
void main(){\n\
    vec3 CameraPosition = vec4(gl_ModelViewMatrixInverse * normalize(vec4(1.))).xyz;\n\
    \n\
    vec4 ecPosition  = gl_Vertex;\n\
    vec3 ecPosition3 = ecPosition.xyz / ecPosition.w;\n\
    \n\
    vEye = normalize(ecPosition3-CameraPosition);\n\
    vNormal = gl_Normal;\n\
    \n\
    vPos =  vec4( gl_Vertex.xyz, 1.0 );\n\
    vTexCoord = gl_MultiTexCoord0.xy;\n\
    \n\
    gl_Position    = ftransform();\n\
    gl_FrontColor = gl_Color;\n\
}";
    
    fragmentShader = "#version 120\n\
\n\
uniform samplerCube Cubemap;\n\
uniform sampler2D tNormal;\n\
uniform sampler2D tColor;\n\
\n\
uniform float normalRepeat;\n\
uniform float normalScale;\n\
uniform float rim;\n\
\n\
uniform float _useColorTexture;\n\
\n\
uniform float ReflectPct;\n\
uniform float FresnelPower;\n\
uniform float ChromaAb;\n\
uniform float transparency;\n\
\n\
varying vec4 vPos;\n\
varying vec3 vNormal;\n\
varying vec3 vEye;\n\
varying vec2 vTexCoord;\n\
\n\
const float Eta = 1./1.5;\n" +
    lightsFunctions +
    triplanarFunctions +
    + "\n\
\n\
void main (void){\n\
    vec3 N = normalize(vNormal);\n\
\n\
    if(normalScale>0.){\n\
        N = getTriplanarNormal(tNormal,vPos.xyz,vNormal,normalRepeat,normalScale);\n\
    }\n\
    \n\
    vec3 color = gl_FrontLightModelProduct.sceneColor.rgb;\n\
    vec4 eyeSpaceVertexPos = gl_ModelViewMatrix * vPos;\n\
    \n\
    if(_useColorTexture>0.){\n\
        color += calc_lighting(vec3(eyeSpaceVertexPos) / eyeSpaceVertexPos.w,N,gl_FrontMaterial.ambient,texture2D(tColor, vTexCoord),gl_FrontMaterial.specular).rgb;\n\
    } else {\n\
        color += calc_lighting_color(vec3(eyeSpaceVertexPos) / eyeSpaceVertexPos.w,N).rgb;\n\
    }\n\
\n\
    {\n\
        float F  = ((1.0-Eta) * (1.0-Eta)) / ((1.0+Eta) * (1.0+Eta));\n\
        float Ratio = F + (1.0 - F) * pow((1.0 - dot(-vEye, N)), (FresnelPower*5.0) );\n\
        \n\
        vec3 RefractR = refract(vEye, N, Eta-ChromaAb*0.2);\n\
        RefractR = vec3(gl_TextureMatrix[0] * vec4(RefractR, 1.0));\n\
        vec3 RefractG = refract(vEye, N, Eta+ChromaAb*0.1);\n\
        RefractG = vec3(gl_TextureMatrix[0] * vec4(RefractG, 1.0));\n\
        vec3 RefractB = refract(vEye, N, Eta+ChromaAb*0.2);\n\
        RefractB = vec3(gl_TextureMatrix[0] * vec4(RefractB, 1.0));\n\
        \n\
        vec3 Reflect  = reflect(vEye, N);\n\
        Reflect  = vec3(gl_TextureMatrix[0] * vec4(Reflect, 1.0));\n\
        \n\
        vec3 refractColor, reflectColor;\n\
        refractColor.r = vec3(textureCube(Cubemap, RefractR)).r;\n\
        refractColor.g = vec3(textureCube(Cubemap, RefractG)).g;\n\
        refractColor.b = vec3(textureCube(Cubemap, RefractB)).b;\n\
        reflectColor   = vec3(textureCube(Cubemap, Reflect));\n\
        color = mix(color,mix(refractColor, reflectColor, Ratio),ReflectPct);\n\
    }\n\
    \n\
    if( rim > 0. ) {\n\
        float cosTheta = abs( dot( normalize(vEye), N) );\n\
        float f = rim * ( 1. - smoothstep( 0.0, 1., cosTheta ) );\n\
        \n\
        color += gl_FrontMaterial.ambient.rgb *f;\n\
    }\n\
    \n\
    if(transparency>0.0){\n\
        float cosTheta = abs( dot( normalize(vEye), N) );\n\
        float fresnel = pow(1.0 - cosTheta, 4.0);\n\
        gl_FragColor = vec4(color,mix(gl_Color.a,fresnel,transparency) );\n\
    } else {\n\
        gl_FragColor = vec4(color,gl_Color.a );\n\
    }\n\
}";
    
    setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    extractUniforms(fragmentShader);
    
    bAllocated = false;
    bDrawCube = false;
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

    {
        gui->setGlobalButtonDimension(34);
        vector<string> list;
        string path = "GUI/normals";
        ofDirectory dir(path);
        int count = 0;
        if(dir.exists()){
            dir.listDir();
            int total = dir.getFiles().size();
            for (int i = 0; i < total; i++) {
                list.push_back( dir.getName(i) );
                gui->addImageToggle("normals/"+dir.getName(i), "GUI/normals/"+dir.getName(i), false);
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
    
    gui->addSlider("CubeSize", 100, 10000, &cubeSize);
    gui->addToggle("DEBUG", &bDrawCube);
    UIShader::setupUI();
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
                
            } else if (name.find("normals") == 0){
                loadNormal("GUI/"+name);
                
                vector<ofxUIWidget*> w = gui->getWidgets();
                for (int i = 0; i < w.size(); i++) {
                    string n = w[i]->getName();
                    
                    if( w[i]->getKind() == 20 &&
                       n.find('.') != string::npos &&
                       n.find("normals") == 0 &&
                       n != name){
                        
                        ofxUIImageToggle* t = (ofxUIImageToggle*)w[i];
                        t->setDrawFill(true);
                        t->setValue(false);
                    }
                }
            }
        }
    }
    
    UIShader::guiEvent(e);
}

void UICubeMap::loadMap( string _image ){
    ofPixels pixels;
    if (ofLoadImage(pixels, _image)){
        size = pixels.getWidth()/4.;
        
        if(size*3==pixels.getHeight()){
            cout<<"Seems like a cubeMap!"<<endl;
        } else {
            cout << _image << " DON'T seems like a valid cubeMap"<<endl;
            return;
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
        
        GLuint pixelType = GL_RGB;
        if(pixels.getNumChannels()==4){
            pixelType = GL_RGBA;
        }
        
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, pixelType, size, size, 0, pixelType, GL_UNSIGNED_BYTE, data_px); // positive x
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, pixelType, size, size, 0, pixelType, GL_UNSIGNED_BYTE, data_py); // positive y
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, pixelType, size, size, 0, pixelType, GL_UNSIGNED_BYTE, data_pz); // positive z
        
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, pixelType, size, size, 0, pixelType, GL_UNSIGNED_BYTE, data_nx); // negative x
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, pixelType, size, size, 0, pixelType, GL_UNSIGNED_BYTE, data_ny); // negative y
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, pixelType, size, size, 0, pixelType, GL_UNSIGNED_BYTE, data_nz); // negative z
        
        bAllocated = true;
    }
}

void UICubeMap::loadNormal(string _path){
    ofDisableArbTex();
    ofLoadImage(normal, _path);
    normal.setTextureWrap(GL_REPEAT, GL_REPEAT);
    ofEnableArbTex();
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
        if(bDrawCube){
            draw();
        }
        
        glEnable( GL_TEXTURE_CUBE_MAP );
        glBindTexture( GL_TEXTURE_CUBE_MAP, textureObjectID );
        
        UIShader::begin();
        if(normal.isAllocated()){
            setUniformTexture("tNormal", normal, 1);
        } else {
            setUniform1f("normalScale", 0);
        }
        if(texture.isAllocated()){
            setUniformTexture("tColor", texture, 2);
            setUniform1f("_useColorTexture", 1.);
        } else {
            setUniformTexture("tColor", normal, 2);
            setUniform1f("_useColorTexture", 0.);
        }
    }
}

void UICubeMap::end(){
    if(bEnable&&bAllocated){
        UIShader::end();
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0 );
        glDisable( GL_TEXTURE_CUBE_MAP );
    }
    
}

void UICubeMap::draw(){
    if(bEnable&&bAllocated){
        glEnable( GL_TEXTURE_CUBE_MAP );
        glBindTexture( GL_TEXTURE_CUBE_MAP, textureObjectID );
        debugShader.begin();
        ofDrawBox(cubeSize);
        debugShader.end();
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0 );
        glDisable( GL_TEXTURE_CUBE_MAP );
    }
}