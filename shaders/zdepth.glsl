@module zdepth
@ctype float float

@vs vs
layout(binding=0) uniform vs_uniforms {
    float z_value;    // Z-value for depth ordering
    vec3 _padding;    // Padding to match the C-side struct (16-byte alignment)
};
layout(location=0) in vec4 coord;
layout(location=1) in vec4 color;
layout(location=0) out vec2 texUV;
layout(location=1) out vec4 iColor;

void main() {
    // Use Z-value for depth ordering (mapped to [-1,1] range for GL depth buffer)
    gl_Position = vec4(coord.xy, 1.0 - z_value, 1.0);
    gl_PointSize = 1.0;
    texUV = coord.zw;
    iColor = color;
}
@end

@fs fs
layout(binding=0) uniform texture2D iTexChannel0;
layout(binding=0) uniform sampler iSmpChannel0;
layout(location=0) in vec2 texUV;
layout(location=1) in vec4 iColor;
layout(location=0) out vec4 fragColor;

void main() {
    fragColor = texture(sampler2D(iTexChannel0, iSmpChannel0), texUV) * iColor;
}
@end

@program program vs fs 