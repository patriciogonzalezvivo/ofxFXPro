//
//  UIMatCap.cpp
//
//  Copyright (C) 2013 Patricio Gonzalez Vivo ( http://www.patriciogonzalezvivo.com )
//
//

#include "UIMatCap.h"
#include "ShaderFunctions.h"

UIMatCap::UIMatCap(){
    
    vertexShader = "#version 120\n\n\
uniform float normalScale;\n\
uniform float useRim;\n\
\n\
varying vec3 vPos;\n\
varying vec3 vePos;\n\
varying vec2 vTexCoord;\n\
varying vec2 vUv;\n\
varying vec3 vNormal;\n\
varying vec3 vEye;\n\
varying vec3 vU;\n\
varying vec2 vN;\n\
\n\
void main() {\n\
    vU = normalize( vec3( gl_ModelViewMatrix * vec4( gl_Vertex.xyz, 1.0 ) ) );\n\
\n\
    if( normalScale > 0. ) {\n\
        vec3 n = normalize( gl_NormalMatrix * gl_Normal );\n\
        vec3 r = reflect( vU, n );\n\
        float m = 2.0 * sqrt( r.x * r.x + r.y * r.y + ( r.z + 1.0 ) * ( r.z+1.0 ) );\n\
        vN = vec2(r.x / m + 0.5, r.y / m + 0.5);\n\
    } else {\n\
        vN = vec2(0.);\n\
    }\n\
\n\
    vTexCoord = gl_MultiTexCoord0.xy;\n\
    vUv = vTexCoord;\n\
\n\
    vPos = gl_Vertex.xyz;\n\
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4( gl_Vertex.xyz, 1.0 );\n\
\n\
    vNormal = normalize(gl_NormalMatrix * gl_Normal);\n\
\n\
    if( useRim > 0. ) {\n\
        vEye = ( gl_ModelViewMatrix * vec4( gl_Vertex.xyz, 1.0 ) ).xyz;\n\
    } else {\n\
        vEye = vec3( 0. );\n\
    }\n\
}";
    
    
    geometryShader = "#version 120\n\n\
#extension GL_EXT_geometry_shader4 : enable\n\
#extension GL_EXT_gpu_shader4 : enable\n\
\n\
uniform float normalScale;\n\
uniform float _useUV;\n\
\n\
varying in vec3 vPos[3];\n\
varying in vec2 vTexCoord[3];\n\
varying in vec2 vUv[3];\n\
varying in vec3 vNormal[3];\n\
varying in vec3 vEye[3];\n\
varying in vec3 vU[3];\n\
varying in vec2 vN[3];\n\
\n\
varying out vec2 Uv;\n\
varying out vec3 Tangent;\n\
varying out vec3 Binormal;\n\
varying out vec3 Normal;\n\
varying out vec3 Eye;\n\
varying out vec3 U;\n\
varying out vec2 N;\n\
\n\
varying out vec3 Pos;\n\
\n\
void main(void){\n\
\n\
    vec3 sdir = vec3(1.0);\n\
    vec3 tdir = vec3(1.0);\n\
    Tangent = vec3(0.);\n\
    Binormal = vec3(0.);\n\
\n\
    if( normalScale > 0. && _useUV == 1.) {\n\
        vec3 v1 = vPos[0].xyz;\n\
        vec3 v2 = vPos[1].xyz;\n\
        vec3 v3 = vPos[2].xyz;\n\
\n\
        vec2 w1 = vTexCoord[0];\n\
        vec2 w2 = vTexCoord[1];\n\
        vec2 w3 = vTexCoord[2];\n\
\n\
        float x1 = v2.x - v1.x;\n\
        float x2 = v3.x - v1.x;\n\
        float y1 = v2.y - v1.y;\n\
        float y2 = v3.y - v1.y;\n\
        float z1 = v2.z - v1.z;\n\
        float z2 = v3.z - v1.z;\n\
\n\
        float s1 = w2.x - w1.x;\n\
        float s2 = w3.x - w1.x;\n\
        float t1 = w2.y - w1.y;\n\
        float t2 = w3.y - w1.y;\n\
\n\
        float r = 1.0F / (s1 * t2 - s2 * t1);\n\
        sdir = vec3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);\n\
        tdir = vec3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);\n\
    }\n\
\n\
    for(int i = 0; i < 3; i++){\n\
        Uv = vUv[i];\n\
\n\
        Normal = vNormal[i];\n\
        if( normalScale > 0. && _useUV == 1.) {\n\
            vec3 t = normalize(sdir-vNormal[i]*dot(vNormal[i],sdir));\n\
            float w = (dot(cross(vNormal[i],sdir),tdir)<0.0)? -1.0:1.0;\n\
\n\
            Tangent = normalize( gl_NormalMatrix * t);\n\
            Binormal = normalize( cross( Normal, Tangent ) * w );\n\
        }\n\
\n\
        Eye = vEye[i];\n\
\n\
        U = vU[i];\n\
        N = vN[i];\n\
        gl_TexCoord[0].xy = vTexCoord[i];\n\
        gl_Position = gl_PositionIn[i];\n\
        Pos = vPos[i];\n\
        EmitVertex();\n\
    }\n\
\n\
    EndPrimitive();\n\
}";
    
    fragmentShader = "#version 120\n\
uniform sampler2D tMatCap;\n\
uniform sampler2D tNormal;\n\
\n\
uniform float _useUV;\n\
\n\
uniform float normalScale;\n\
uniform float normalRepeat;\n\
\n\
uniform float useRim;\n\
uniform float transparency;\n\
\n\
varying vec2 Uv;\n\
varying vec3 Tangent;\n\
varying vec3 Binormal;\n\
varying vec3 Normal;\n\
varying vec3 Eye;\n\
varying vec3 U;\n\
varying vec2 N;\n\
\n\
varying vec3 Pos;\n\
\n" +
    triplanarFunctions +
"\n\
vec2 barrelDistortion(vec2 coord, float amt) {\n\
    vec2 cc = coord - 0.5;\n\
    float dist = dot(cc, cc);\n\
    return coord + cc * dist * amt;\n\
}\n\
\n\
float sat( float t ){\n\
    return clamp( t, 0.0, 1.0 );\n\
}\n\
\n\
float linterp( float t ) {\n\
    return sat( 1.0 - abs( 2.0*t - 1.0 ) );\n\
}\n\
\n\
float remap( float t, float a, float b ) {\n\
    return sat( (t - a) / (b - a) );\n\
}\n\
\n\
vec3 spectrum_offset( float t ) {\n\
    vec3 ret;\n\
    float lo = step(t,0.5);\n\
    float hi = 1.0-lo;\n\
    float w = linterp( remap( t, 1.0/6.0, 5.0/6.0 ) );\n\
    ret = vec3(lo,1.0,hi) * vec3(1.0-w, w, 1.0-w);\n\
    \n\
    return pow( ret, vec3(1.0/2.2) );\n\
}\n\
\n\
uniform float chroma;\n\
const int num_iter = 12;\n\
const float reci_num_iter_f = 1.0 / float(num_iter);\n\
\n\
vec3 chromaAb( vec2 _pos, float _amount ){\n\
    vec3 sumcol = vec3(0.0);\n\
    vec3 sumw = vec3(0.0);\n\
    for ( int i=0; i<num_iter;++i ){\n\
        float t = float(i) * reci_num_iter_f;\n\
        vec3 w = spectrum_offset( t );\n\
        sumw += w;\n\
        sumcol += w * texture2D(tMatCap, barrelDistortion(_pos, _amount*t*chroma) ).rgb;\n\
    }\n\
    return sumcol.rgb / sumw;\n\
}\n\
\n\
void main() {\n\
    vec3 finalNormal = Normal;\n\
    vec2 calculatedNormal = N;\n\
\n\
    if( normalScale > 0. ) {\n\
        if(_useUV == 1.){\n\
            vec3 normalTex = texture2D( tNormal, Uv * normalRepeat*100.0).xyz;\n\
            normalTex = normalTex * 2.0 - 1.0;\n\
            normalTex.xy *= normalScale;\n\
            normalTex.y *= -1.;\n\
            normalTex = normalize( normalTex );\n\
\n\
            mat3 tsb = mat3( normalize( Tangent ), normalize( Binormal ), normalize( Normal ) );\n\
            finalNormal = tsb * normalTex;\n\
\n\
        } else {\n\
            finalNormal = getTriplanarNormal(tNormal,Pos.xyz,Normal,normalRepeat,normalScale);\n\
        }\n\
    }\n\
\n\
    vec3 r = reflect( U, normalize( finalNormal ) );\n\
    float m = 2.0 * sqrt( r.x * r.x + r.y * r.y + ( r.z + 1.0 ) * ( r.z+1.0 ) );\n\
    calculatedNormal = vec2( r.x / m + 0.5,  r.y / m + 0.5 );\n\
    calculatedNormal.y = 1.0-calculatedNormal.y;\n\
\n\
    float cosTheta = abs(dot( U, finalNormal));\n\
    float exp = pow(1.0-cosTheta, 4.0);\n\
    float fresnel = exp;\n\
\n\
    vec3 base = vec3(0.0);\n\
\n\
    if(chroma>0.0){\n\
        float f = abs(dot( U, finalNormal));\n\
        base = chromaAb(calculatedNormal,f);\n\
    } else {\n\
        base = texture2D( tMatCap, calculatedNormal ).rgb;\n\
    }\n\
\n\
    if( useRim > 0. ) {\n\
        float f = useRim * ( 1. - smoothstep( 0.0, 1., cosTheta ) );\n\
        base += vec3( f );\n\
    }\n\
\n\
    if(transparency>0.0){\n\
        float thickness = abs(pow(2.0,(-10.0-Pos.z)*0.11));\n\
            float sigma = 5.0;\n\
            float intensity = fresnel * exp(sigma * thickness);\n\
            vec4 glass = vec4(base,mix(gl_Color.a,intensity,transparency) );\n\
            gl_FragColor = vec4( base, mix(gl_Color.a,intensity,transparency) );\n\
        } else {\n\
            gl_FragColor = vec4( base, gl_Color.a );\n\
        }\n\
    }";
    
    setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
    setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
    setupShaderFromSource(GL_GEOMETRY_SHADER, geometryShader);

    geomInType = GL_TRIANGLES;
    geomOutType = GL_TRIANGLES;
    geomOutCount = 3;
    
    setGeometryInputType(geomInType);
    setGeometryOutputType(geomOutType);
    setGeometryOutputCount(geomOutCount);
    
    linkProgram();
    
    extractUniforms(fragmentShader);
}

string UIMatCap::getClassName(){
    if(fragFile.isFile()){
        return fragFile.getBaseName();
    } else {
        return "MatCap";
    }
}

void UIMatCap::setupUI(){
    
    int w = gui->getGlobalButtonDimension();
    
    
    {
        gui->addLabel("Materials");
        gui->setGlobalButtonDimension(34);
        vector<string> list;
        string path = "GUI/materials";
        ofDirectory dir(path);
        int count = 0;
        if(dir.exists()){
            dir.listDir();
            int total = dir.getFiles().size();
            for (int i = 0; i < total; i++) {
                list.push_back( dir.getName(i) );
                gui->addImageToggle("materials/"+dir.getName(i), "GUI/materials/"+dir.getName(i),false);
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
    {
        gui->addLabel("Normals");
        
        ofxUIToggle *toggle = gui->addToggle("'", &bUVNormals);
        gui->addWidgetUp(toggle, OFX_UI_ALIGN_RIGHT, true);
        
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

void UIMatCap::guiEvent(ofxUIEventArgs &e){
    string name = e.widget->getName();
    
    if (name.find('.') != string::npos){
        
        ofxUIImageToggle *toggle = (ofxUIImageToggle*)e.widget;

        if(toggle->getValue() == true){
            if(name.find("materials") == 0){
                loadMaterial("GUI/"+name);
                
                vector<ofxUIWidget*> w = gui->getWidgets();
                for (int i = 0; i < w.size(); i++) {
                    string n = w[i]->getName();
                    
                    if( w[i]->getKind() == 20 &&
                       n.find('.') != string::npos &&
                       n.find("materials") == 0 &&
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

void UIMatCap::loadMaterial(string _path){
    ofDisableArbTex();
    ofLoadImage(matCap, _path);
    ofEnableArbTex();
}

void UIMatCap::loadNormal(string _path){
    ofDisableArbTex();
    ofLoadImage(normal, _path);
    normal.setTextureWrap(GL_REPEAT, GL_REPEAT);
    ofEnableArbTex();
}

void UIMatCap::begin(){
    if (matCap.isAllocated()&&bEnable){
        UIShader::begin();
        setUniformTexture("tMatCap", matCap, 0);
        
        if(normal.isAllocated()){
            setUniformTexture("tNormal", normal, 1);
            setUniform1f("_useUV", bUVNormals);
        } else {
            setUniform1f("_useUV", 0);
        }
    }
}

void UIMatCap::end(){
    if (matCap.isAllocated()&&bEnable) {
        UIShader::end();
    }
}