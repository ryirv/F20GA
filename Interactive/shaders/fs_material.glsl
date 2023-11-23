#version 410 core
out vec4 color;

in VS_OUT
{
    vec3 vertex;
    vec3 normals;
    vec2 tc;
} fs_in;

uniform sampler2D tex;
uniform mat4 model_matrix;


uniform vec3 light_direction;

//material
uniform vec3 view_direction;
uniform vec3 in_ambient;
uniform vec3 in_diffuse;
uniform vec3 in_specular;
uniform float in_shininess;
uniform vec3 in_lightColor;

uniform float in_flashIntesity;


void main(void)
{
  vec3 normal = fs_in.normals;

  // based on https://learnopengl.com/Lighting/Materials
  // materials

  // ambient
  vec3 ambient = in_lightColor * in_ambient;
  	
  // diffuse 
  vec3 norm = normalize(normal);
  float diff = max(dot(norm, light_direction), 0.0);
  vec3 diffuse = in_lightColor * (diff * in_diffuse);
    
  // specular
  vec3 reflectDir = reflect(-light_direction, norm);  
  float spec = pow(max(dot(view_direction, reflectDir), 0.0), in_shininess);
  vec3 specular = in_lightColor * (spec * in_specular);  
        
  vec3 result = ambient + diffuse + specular;

  vec3 flashIntesity = vec3(in_flashIntesity, in_flashIntesity, in_flashIntesity);
  if(in_flashIntesity > 0){

      color = texture(tex, fs_in.tc) * 1.0 * vec4((result*flashIntesity), 1.0);

  }else{
      color = texture(tex, fs_in.tc) * 1.0 * vec4(result, 1.0);
  }

    
}