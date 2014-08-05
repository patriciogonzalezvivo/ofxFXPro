#version 120

uniform samplerCube Cubemap;
uniform sampler2D tNormal;
uniform sampler2D tColor;

uniform float normalRepeat;
uniform float normalScale;
uniform float rim;

uniform float _useColorTexture;

uniform float ReflectPct;
uniform float FresnelPower;
uniform float ChromaAb;
uniform float transparency;

varying vec4 vPos;
varying vec3 vNormal;
varying vec3 vEye;
varying vec2 vTexCoord;

const float Eta = 1./1.5;

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

vec3 getTriPlanarBlend(vec3 _Norm){
    vec3 blending = abs( _Norm );
    blending = normalize(max(blending, 0.00001));
    float b = (blending.x + blending.y + blending.z);
    blending /= vec3(b, b, b);
    return blending;
}

vec3 getTriplanarNormal(sampler2D _NormalMap, vec3 _vertexPosistion, vec3 _vertexNormal, float _repeat, float _scale){
    vec3 blending = getTriPlanarBlend(_vertexNormal);
    vec3 xaxis = texture2D( _NormalMap, _vertexPosistion.yz * _repeat).rgb;
    vec3 yaxis = texture2D( _NormalMap, _vertexPosistion.xz * _repeat).rgb;
    vec3 zaxis = texture2D( _NormalMap, _vertexPosistion.xy * _repeat).rgb;
    vec3 normalTex = xaxis * blending.x + xaxis * blending.y + zaxis * blending.z;
    normalTex = normalTex * 2.0 - 1.0;
    normalTex.xy *= _scale*2.0;
    normalTex = normalize( normalTex );
        
    vec3 T = vec3(1.,0.,0.);
    vec3 BT = normalize( cross( _vertexNormal, T ) * 1.0 );
    mat3 tsb = mat3( normalize( T ), normalize( BT ), normalize( _vertexNormal ) );
    return tsb * normalTex;
}

void main (void){
    vec3 N = normalize(vNormal);

    if(normalScale>0.){
        N = getTriplanarNormal(tNormal,vPos.xyz,vNormal,normalRepeat,normalScale);
    }

    vec3 color = vec3(0.);

    {
        vec4 eyeSpaceVertexPos = gl_ModelViewMatrix * vPos;
    	color = calc_lighting_color(vec3(eyeSpaceVertexPos) / eyeSpaceVertexPos.w,N).rgb;
   	}

    {
    	float F  = ((1.0-Eta) * (1.0-Eta)) / ((1.0+Eta) * (1.0+Eta));
    	float Ratio = F + (1.0 - F) * pow((1.0 - dot(-vEye, N)), (FresnelPower*5.0) );

    	vec3 RefractR = refract(vEye, N, Eta-ChromaAb*0.2);
    	RefractR = vec3(gl_TextureMatrix[0] * vec4(RefractR, 1.0));
    	vec3 RefractG = refract(vEye, N, Eta+ChromaAb*0.1);
    	RefractG = vec3(gl_TextureMatrix[0] * vec4(RefractG, 1.0));
    	vec3 RefractB = refract(vEye, N, Eta+ChromaAb*0.2);
    	RefractB = vec3(gl_TextureMatrix[0] * vec4(RefractB, 1.0));

    	vec3 Reflect  = reflect(vEye, N);
    	Reflect  = vec3(gl_TextureMatrix[0] * vec4(Reflect, 1.0));

    	vec3 refractColor, reflectColor;
    	refractColor.r = vec3(textureCube(Cubemap, RefractR)).r;
    	refractColor.g = vec3(textureCube(Cubemap, RefractG)).g;
    	refractColor.b = vec3(textureCube(Cubemap, RefractB)).b;
    	reflectColor   = vec3(textureCube(Cubemap, Reflect));
   		color = mix(color,mix(refractColor, reflectColor, Ratio),ReflectPct);
    }

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
        gl_FragColor = vec4(color,gl_Color.a );
    }
}