Texture2D src : register(t0);
cbuffer params : register(b0) {
    float2 res;
    float2 range;
    float mode;
    float col_space;
};

static const float eps = 1.0e-4;

inline float4 unpremul_col(float4 col) {
    return saturate(float4(col.rgb * rcp(max(col.a, eps)), col.a));
}

inline float4 premul_col(float4 col) {
    return float4(col.rgb * col.a, col.a);
}

float4 to_linear(float4 col) {
    switch (int(col_space)) {
        case 1:
            float3 t = step(col.rgb, 0.04045);
            float3 low = col.rgb * rcp(12.92);
            float3 high = pow(abs((col.rgb + 0.055) * rcp(1.055)), 2.4);
            return saturate(float4(lerp(high, low, t), col.a));
        default:
            return col;
    }
}

float rgb2metric(float3 rgb) {
    switch (int(mode)) {
        case 0:
            return dot(rgb, float3(0.299, 0.587, 0.114));
        case 2:
            return dot(rgb, float3(0.2627, 0.6780, 0.0593));
        case 3:
            return dot(rgb, 1.0) * rcp(3.0);
        case 4:
            return min(rgb.r, min(rgb.g, rgb.b));
        case 5:
            return max(rgb.r, max(rgb.g, rgb.b));
        default:
            return dot(rgb, float3(0.2126, 0.7152, 0.0722));
    }
}

float4 meta(float4 pos : SV_Position) : SV_Target {
    float4 tex = src.Load(int3(pos.xy, 0));
    float4 col = to_linear(unpremul_col(tex));
    float v = rgb2metric(col.rgb);
    float in_range = step(range.x, v) * step(v, range.y) * step(eps, col.a);
    return premul_col(float4(v.xxx, in_range));
}
