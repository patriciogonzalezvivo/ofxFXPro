//
//  UIBumpMap.cpp
//
//  Created by Patricio Gonzalez Vivo on 7/3/14.
//
//

#include "UIBumpMap.h"
#include "ShaderFunctions.h"

UIBumpMap::UIBumpMap(){
    
    vertexShader = "#version 120\n\
\n\
varying vec4 vPos;\n\
\n\
varying vec2 vTexCoord;\n\
varying vec3 vNormal;\n\
varying vec3 vEye;\n\
\n\
void main() {\n\
\n\
    vPos =  vec4( gl_Vertex.xyz, 1.0 );\n\
    vTexCoord = gl_MultiTexCoord0.xy;\n\
\n\
    vNormal = normalize(gl_NormalMatrix * gl_Normal);\n\
\n\
    vec4 eyeSpaceVertexPos = gl_ModelViewMatrix * gl_Vertex;\n\
    vEye = (vec3(eyeSpaceVertexPos)) / eyeSpaceVertexPos.w;\n\
\n\
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4( gl_Vertex.xyz, 1.0 );\n\
    gl_FrontColor = gl_Color;\n\
}";
    
    fragmentShader = "#version 120\n\n\
\n\
uniform sampler2D tNormal;\n\
uniform sampler2D tColor;\n\
\n\
uniform float normalRepeat;\n\
uniform float normalScale;\n\
uniform float rim;\n\
uniform float transparency;\n\
\n\
uniform float _useColorTexture;\n\
\n\
varying vec4 vPos;\n\
varying vec2 vTexCoord;\n\
varying vec3 vNormal;\n\
varying vec3 vEye;\n" +
    lightsFunctions +
    triplanarFunctions +
"\n\
void main() {\n\
    vec3 N = normalize(vNormal);\n\
\n\
    if(normalScale>0.){\n\
        N = getTriplanarNormal(tNormal,vPos.xyz,vNormal,normalRepeat,normalScale);\n\
    }\n\
\n\
    vec3 color = gl_FrontLightModelProduct.sceneColor.rgb;\n\
    if(_useColorTexture>0.){\n\
        color += calc_lighting(vEye,N,gl_FrontMaterial.ambient,texture2D(tColor, vTexCoord),gl_FrontMaterial.specular).rgb;\n\
    } else {\n\
        color += calc_lighting_color(vEye,N).rgb;\n\
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
\n\
        gl_FragColor = vec4(color,mix(gl_Color.a,fresnel,transparency) );\n\
    } else {\n\
        gl_FragColor = vec4( color, gl_Color.a );\n\
    }\n\
}";

    setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    extractUniforms(fragmentShader);
}

string UIBumpMap::getClassName(){
    if(fragFile.isFile()){
        return fragFile.getBaseName();
    } else {
        return "BumpMap";
    }
}

void UIBumpMap::setupUI(){
    
    int w = gui->getGlobalButtonDimension();
    
    {
        gui->addLabel("Normals");
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
    UIShader::setupUI();
}

void UIBumpMap::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if (name.find('.') != string::npos){
        
        ofxUIImageToggle *toggle = (ofxUIImageToggle*)e.widget;
        
        if(toggle->getValue() == true){
            if (name.find("normals") == 0){
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

void UIBumpMap::loadNormal(string _path){
    ofDisableArbTex();
    ofLoadImage(normal, _path);
    normal.setTextureWrap(GL_REPEAT, GL_REPEAT);
    ofEnableArbTex();
}

void UIBumpMap::begin(){
    if (bEnable) {
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

void UIBumpMap::end(){
    if (bEnable) {
        UIShader::end();
    }
}