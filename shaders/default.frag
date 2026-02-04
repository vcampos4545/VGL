#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform vec3 color;
uniform vec3 lightDir;
uniform vec3 viewPos;
uniform bool useLighting;

out vec4 FragColor;

void main() {
  if (!useLighting) {
    FragColor = vec4(color, 1.0);
    return;
  }

  vec3 norm = normalize(Normal);
  vec3 light = normalize(lightDir);

  float ambient = 0.15;
  float diffuse = max(dot(norm, light), 0.0);

  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 halfDir = normalize(light + viewDir);
  float specular = pow(max(dot(norm, halfDir), 0.0), 32.0) * 0.3;

  vec3 result = color * (ambient + diffuse) + vec3(specular);
  FragColor = vec4(result, 1.0);
}
