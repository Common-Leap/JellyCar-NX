#version 330 core
in vec2 localCoord;

uniform vec4 fillColor;
uniform vec4 fillColor2;
uniform vec4 borderColor;
uniform float rectWidth;
uniform float rectHeight;
uniform float radius;
uniform float borderWidth;
uniform float useGradient;

out vec4 color;

float roundedBoxSDF(vec2 p, vec2 halfSize, float r)
{
    vec2 q = abs(p) - halfSize + vec2(r);
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
}

void main()
{
    vec2 rectSize = vec2(rectWidth, rectHeight);
    vec2 halfSize = rectSize * 0.5;
    vec2 p = (localCoord - 0.5) * rectSize;

    float dist = roundedBoxSDF(p, halfSize, radius);

    float fillAlpha = 1.0 - smoothstep(-1.0, 1.0, dist);
    float borderAlpha = 1.0 - smoothstep(-1.0, 1.0, abs(dist) - borderWidth);

    vec4 baseFill = mix(fillColor, fillColor2, localCoord.y * useGradient);
    vec4 col = vec4(baseFill.rgb, baseFill.a * fillAlpha);

    col = mix(col, borderColor, borderAlpha * borderColor.a);

    if (col.a < 0.004)
        discard;

    color = col;
}
