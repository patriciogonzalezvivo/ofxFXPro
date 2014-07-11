#version 120


uniform sampler2D tNormal;
uniform sampler2D tColor;

uniform float normalRepeat;
uniform float normalScale;
uniform float rim;
uniform float transparency;

uniform float _useColorTexture;

varying vec4 vPos;
varying vec2 vTexCoord;
varying vec3 vNormal;
varying vec3 vEye;

int lightsNumber = 8;

void DirectionalLight(in int i, in vec3 normal,inout vec4 ambient, inout vec4 diffuse, inout vec4 specular){
    float nDotVP;
    float nDotHV;
    float pf;

    nDotVP = max(0.0, dot(normal, normalize(vec3(gl_LightSource[i].position))));
    nDotHV = max(0.0, dot(normal, vec3(gl_LightSource[i].halfVector)));

    if (nDotVP == 0.0)
        pf = 0.0;
    else
        pf = pow(nDotHV, gl_FrontMaterial.shininess);

    ambient  += gl_LightSource[i].ambient;
    diffuse  += gl_LightSource[i].diffuse * nDotVP;
    specular += gl_LightSource[i].specular * pf;
}

void PointLight(in int i, in vec3 eye, in vec3 ecPosition3, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular){
    float nDotVP;
    float nDotHV;
    float pf;
    float attenuation;
    float d;
    vec3  VP;
    vec3  halfVector;

    VP = vec3(gl_LightSource[i].position) - ecPosition3;

    d = length(VP);

    VP = normalize(VP);

    attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
                         gl_LightSource[i].linearAttenuation * d +
                         gl_LightSource[i].quadraticAttenuation * d * d);

    halfVector = normalize(VP + eye);

    nDotVP = max(0.0, dot(normal, VP));
    nDotHV = max(0.0, dot(normal, halfVector));

    if (nDotVP == 0.0)
        pf = 0.0;
    else
        pf = pow(nDotHV, gl_FrontMaterial.shininess);

    ambient += gl_LightSource[i].ambient * attenuation;
    diffuse += gl_LightSource[i].diffuse * nDotVP * attenuation;
    specular += gl_LightSource[i].specular * pf * attenuation;
}

void SpotLight(in int i, in vec3 eye, vec3 ecPosition3, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular){
    float nDotVP, nDotHV, pf, spotDot, spotAttenuation, attenuation, d;
    vec3 VP, halfVector;

    VP = vec3(gl_LightSource[i].position) - ecPosition3;

    d = length(VP);

    VP = normalize(VP);

    attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
                         gl_LightSource[i].linearAttenuation * d +
                         gl_LightSource[i].quadraticAttenuation * d * d);

    spotDot = dot(-VP, normalize(gl_LightSource[i].spotDirection));

    if (spotDot < gl_LightSource[i].spotCosCutoff)
        spotAttenuation = 0.0;
    else
        spotAttenuation = pow(spotDot, gl_LightSource[i].spotExponent);

    attenuation *= spotAttenuation;

    halfVector = normalize(VP + eye);

    nDotVP = max(0.0, dot(normal, VP));
    nDotHV = max(0.0, dot(normal, halfVector));

    if (nDotVP == 0.0)
        pf = 0.0;
    else
        pf = pow(nDotHV, gl_FrontMaterial.shininess);

    ambient  += gl_LightSource[i].ambient * attenuation;
    diffuse  += gl_LightSource[i].diffuse * nDotVP * attenuation;
    specular += gl_LightSource[i].specular * pf * attenuation;
}

vec4 calc_lighting_color(in vec3 _ecPosition, in vec3 _normal) {
    vec3 eye = vec3(0.0, 0.0, 1.0);

    vec4 amb  = vec4(0.0);
    vec4 diff = vec4(0.0);
    vec4 spec = vec4(0.0);

    for (int i = 0; i < lightsNumber; i++){
        if (gl_LightSource[i].position.w == 0.0)
            DirectionalLight(i, normalize(_normal), amb, diff, spec);
        else if (gl_LightSource[i].spotCutoff == 180.0)
            PointLight(i, eye, _ecPosition, normalize(_normal), amb, diff, spec);
        else
            SpotLight(i, eye, _ecPosition, normalize(_normal), amb, diff, spec);
    }

    vec4 diffuseColor = gl_FrontMaterial.diffuse;

    if(_useColorTexture == 1.){
        diffuseColor = texture2D(tColor, vTexCoord);
    }

    return  gl_FrontLightModelProduct.sceneColor +
            amb * gl_FrontMaterial.ambient +
            diff * diffuseColor +
            spec * gl_FrontMaterial.specular;
}

vec3 getTriPlanarBlend(vec3 _wNorm){
    vec3 blending = abs( _wNorm );
    blending = normalize(max(blending, 0.00001));
    float b = (blending.x + blending.y + blending.z);
    blending /= vec3(b, b, b);
    return blending;
}

void main() {
    vec3 N = normalize(vNormal);

    if(normalScale>0.){
        vec3 blending = getTriPlanarBlend(vNormal);
        vec3 xaxis = texture2D( tNormal, vPos.yz * normalRepeat).rgb;
        vec3 yaxis = texture2D( tNormal, vPos.xz * normalRepeat).rgb;
        vec3 zaxis = texture2D( tNormal, vPos.xy * normalRepeat).rgb;
        vec3 normalTex = xaxis * blending.x + xaxis * blending.y + zaxis * blending.z;
        normalTex = normalTex * 2.0 - 1.0;
        normalTex.xy *= normalScale*2.0;
        normalTex = normalize( normalTex );
        
        vec3 T = vec3(0.,1.,0.);
        vec3 BT = normalize( cross( vNormal, T ) * 1.0 );
        mat3 tsb = mat3( normalize( T ), normalize( BT ), normalize( vNormal ) );
        N = tsb * normalTex;
    }

    vec3 color = calc_lighting_color(vEye,N).rgb;;

    if( rim > 0. ) {
        float cosTheta = abs( dot( normalize(vEye), N) );
        float f = rim * ( 1. - smoothstep( 0.0, 1., cosTheta ) );

        color += gl_FrontMaterial.ambient.rgb *f;
    }

    if(transparency>0.0){
        float cosTheta = abs( dot( normalize(vEye), N) );
        float fresnel = pow(1.0 - cosTheta, 4.0);

        gl_FragColor = vec4(color,mix(gl_Color.a,fresnel,transparency) );
    } else {
        gl_FragColor = vec4( color, gl_Color.a );
    }
}