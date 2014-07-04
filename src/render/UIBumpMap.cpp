//
//  UIBumpMap.cpp
//
//  Created by Patricio Gonzalez Vivo on 7/3/14.
//
//

#include "UIBumpMap.h"

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
varying vec3 vEye;\n\
\n\
int lightsNumber = 8;\n\
\n\
void DirectionalLight(in int i, in vec3 normal,inout vec4 ambient, inout vec4 diffuse, inout vec4 specular){\n\
    float nDotVP;\n\
    float nDotHV;\n\
    float pf;\n\
\n\
    nDotVP = max(0.0, dot(normal, normalize(vec3(gl_LightSource[i].position))));\n\
    nDotHV = max(0.0, dot(normal, vec3(gl_LightSource[i].halfVector)));\n\
\n\
    if (nDotVP == 0.0)\n\
        pf = 0.0;\n\
    else\n\
        pf = pow(nDotHV, gl_FrontMaterial.shininess);\n\
\n\
    ambient  += gl_LightSource[i].ambient;\n\
    diffuse  += gl_LightSource[i].diffuse * nDotVP;\n\
    specular += gl_LightSource[i].specular * pf;\n\
}\n\
\n\
void PointLight(in int i, in vec3 eye, in vec3 ecPosition3, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular){\n\
    float nDotVP;\n\
    float nDotHV;\n\
    float pf;\n\
    float attenuation;\n\
    float d;\n\
    vec3  VP;\n\
    vec3  halfVector;\n\
\n\
    VP = vec3(gl_LightSource[i].position) - ecPosition3;\n\
\n\
    d = length(VP);\n\
\n\
    VP = normalize(VP);\n\
\n\
    attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +\n\
                         gl_LightSource[i].linearAttenuation * d +\n\
                         gl_LightSource[i].quadraticAttenuation * d * d);\n\
\n\
    halfVector = normalize(VP + eye);\n\
\n\
    nDotVP = max(0.0, dot(normal, VP));\n\
    nDotHV = max(0.0, dot(normal, halfVector));\n\
\n\
    if (nDotVP == 0.0)\n\
        pf = 0.0;\n\
    else\n\
        pf = pow(nDotHV, gl_FrontMaterial.shininess);\n\
\n\
    ambient += gl_LightSource[i].ambient * attenuation;\n\
    diffuse += gl_LightSource[i].diffuse * nDotVP * attenuation;\n\
    specular += gl_LightSource[i].specular * pf * attenuation;\n\
}\n\
\n\
void SpotLight(in int i, in vec3 eye, vec3 ecPosition3, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular){\n\
    float nDotVP, nDotHV, pf, spotDot, spotAttenuation, attenuation, d;\n\
    vec3 VP, halfVector;\n\
\n\
    VP = vec3(gl_LightSource[i].position) - ecPosition3;\n\
\n\
    d = length(VP);\n\
\n\
    VP = normalize(VP);\n\
\n\
    attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +\n\
                         gl_LightSource[i].linearAttenuation * d +\n\
                         gl_LightSource[i].quadraticAttenuation * d * d);\n\
\n\
    spotDot = dot(-VP, normalize(gl_LightSource[i].spotDirection));\n\
\n\
    if (spotDot < gl_LightSource[i].spotCosCutoff)\n\
        spotAttenuation = 0.0;\n\
    else\n\
        spotAttenuation = pow(spotDot, gl_LightSource[i].spotExponent);\n\
\n\
    attenuation *= spotAttenuation;\n\
\n\
    halfVector = normalize(VP + eye);\n\
\n\
    nDotVP = max(0.0, dot(normal, VP));\n\
    nDotHV = max(0.0, dot(normal, halfVector));\n\
\n\
    if (nDotVP == 0.0)\n\
        pf = 0.0;\n\
    else\n\
        pf = pow(nDotHV, gl_FrontMaterial.shininess);\n\
\n\
    ambient  += gl_LightSource[i].ambient * attenuation;\n\
    diffuse  += gl_LightSource[i].diffuse * nDotVP * attenuation;\n\
    specular += gl_LightSource[i].specular * pf * attenuation;\n\
}\n\
\n\
vec4 calc_lighting_color(in vec3 _ecPosition, in vec3 _normal) {\n\
    vec3 eye = vec3(0.0, 0.0, 1.0);\n\
\n\
    vec4 amb  = vec4(0.0);\n\
    vec4 diff = vec4(0.0);\n\
    vec4 spec = vec4(0.0);\n\
\n\
    for (int i = 0; i < lightsNumber; i++){\n\
        if (gl_LightSource[i].position.w == 0.0)\n\
            DirectionalLight(i, normalize(_normal), amb, diff, spec);\n\
        else if (gl_LightSource[i].spotCutoff == 180.0)\n\
            PointLight(i, eye, _ecPosition, normalize(_normal), amb, diff, spec);\n\
        else\n\
            SpotLight(i, eye, _ecPosition, normalize(_normal), amb, diff, spec);\n\
    }\n\
\n\
    vec4 diffuseColor = gl_FrontMaterial.diffuse;\n\
\n\
    if(_useColorTexture == 1.){\n\
        diffuseColor = texture2D(tColor, vTexCoord);\n\
    }\n\
\n\
    return  gl_FrontLightModelProduct.sceneColor +\n\
            amb * gl_FrontMaterial.ambient +\n\
            diff * diffuseColor +\n\
            spec * gl_FrontMaterial.specular;\n\
}\n\
\n\
vec3 getTriPlanarBlend(vec3 _wNorm){\n\
    vec3 blending = abs( _wNorm );\n\
    blending = normalize(max(blending, 0.00001));\n\
    float b = (blending.x + blending.y + blending.z);\n\
    blending /= vec3(b, b, b);\n\
    return blending;\n\
}\n\
\n\
void main() {\n\
    vec3 N = normalize(vNormal);\n\
\n\
    if(normalScale>0.){\n\
        vec3 blending = getTriPlanarBlend(vNormal);\n\
        vec3 xaxis = texture2D( tNormal, vPos.yz * normalRepeat).rgb;\n\
        vec3 yaxis = texture2D( tNormal, vPos.xz * normalRepeat).rgb;\n\
        vec3 zaxis = texture2D( tNormal, vPos.xy * normalRepeat).rgb;\n\
        vec3 normalTex = xaxis * blending.x + xaxis * blending.y + zaxis * blending.z;\n\
        normalTex = normalTex * 2.0 - 1.0;\n\
        normalTex.xy *= normalScale*2.0;\n\
        normalTex = normalize( normalTex );\n\
        \n\
        vec3 T = vec3(0.,1.,0.);\n\
        vec3 BT = normalize( cross( vNormal, T ) * 1.0 );\n\
        mat3 tsb = mat3( normalize( T ), normalize( BT ), normalize( vNormal ) );\n\
        N = tsb * normalTex;\n\
    }\n\
\n\
    vec3 color = calc_lighting_color(vEye,N).rgb;;\n\
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
    
    linkProgram();
    
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
            setUniformTexture("tNormal", normal, 0);
        } else {
            setUniform1f("normalScale", 0);
        }
        if(texture.isAllocated()){
            setUniformTexture("tColor", texture, 1);
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