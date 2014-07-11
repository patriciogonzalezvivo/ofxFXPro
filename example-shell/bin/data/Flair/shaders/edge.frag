#version 120

uniform sampler2DRect tex;
uniform sampler2DRect depthTexture;

uniform int _depth;
uniform float _focalDistance;
uniform float _focalRange;
uniform float _radius;

uniform int _debug;

uniform float pct;
     
float LinearizeDepth(float zoverw){
    float n = 1.0;
    float f = 20000.0;
    return (2.0 * n) / (f + n - zoverw * (f - n));
}

float FocalValue(vec2 pos){
    float depth = LinearizeDepth( texture2DRect(depthTexture, pos).r ) * 20000.;
    float value = abs(depth  - _focalDistance) / _focalRange;
    return min( value , 1.0);
}

float getIntensity(vec2 u){
    vec3 a = vec3(0.);
    if (_depth==1.){
        a = vec3(FocalValue(u));
    } else {
        a = texture2DRect(tex,u).xyz;
    }
    return (a.x+a.y+a.z)/3.0;
}

void main(void){
    vec2 uv = gl_TexCoord[0].st;

    vec2 p = vec2(_radius);
    
    float avg = 0.0;
    avg += getIntensity(uv+vec2(p.x,0.0));
    avg += getIntensity(uv+vec2(-p.x,0.0));
    avg += getIntensity(uv+vec2(0.0,p.y));
    avg += getIntensity(uv+vec2(0.0,-p.y));
    avg += getIntensity(uv+vec2(p.x,p.y));
    avg += getIntensity(uv+vec2(-p.x,-p.y));
    avg += getIntensity(uv+vec2(p.x,-p.y));
    avg += getIntensity(uv+vec2(-p.x,p.y));
    avg /= 8.0;
    
    float result = (1.0-getIntensity(uv)) + avg;
    result = (1.0 - result) * (pct*10.0);
    
    if(_debug==1.){
        float depth = FocalValue(uv);
        gl_FragColor.rgb = mix(vec3(0.),vec3(1.),depth);
        gl_FragColor.a = 1.;
    } else {
        gl_FragColor = vec4(vec3(1.0-result),result);
    }
}
