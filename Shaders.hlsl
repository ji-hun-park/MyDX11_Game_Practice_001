// --------------------------------------------------------
// Constant Buffer (cbuffer)
// CPU에서 매 프레임 갱신해서 보내줄 데이터(행렬)입니다.
// register(b0): 0번 슬롯(Slot)을 사용하겠다는 뜻
// --------------------------------------------------------
cbuffer ConstantBuffer : register(b0)
{
    matrix World; // 물체의 위치/회전/크기
    matrix View; // 카메라의 위치/각도
    matrix Projection; // 카메라의 렌즈(원근감)
}

// 1. 입력 데이터 구조체 (C++에서 보낼 데이터와 모양이 같아야 함)
struct VS_INPUT
{
    float4 Pos : POSITION; // 정점 위치 (x, y, z, 1.0)
    float4 Color : COLOR; // 정점 색상 (r, g, b, a)
};

// 2. 버텍스 셰이더와 픽셀 셰이더 사이의 통신 구조체
struct PS_INPUT
{
    float4 Pos : SV_POSITION; // 중요: 화면상 좌표 (System Value)
    float4 Color : COLOR; // 색상
};

// --------------------------------------------------------
// Vertex Shader (VS)
// 역할: 3D 공간의 점을 화면 좌표로 변환
// --------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    
    // 지금은 행렬 연산 없이 입력받은 좌표를 그대로 넘깁니다.
    // (좌표계: 화면 중앙이 0,0 / 우측이 +1, 좌측이 -1)
    output.Pos = input.Pos;
    
    // 색상도 그대로 넘깁니다.
    output.Color = input.Color;
    
    return output;
}

// --------------------------------------------------------
// Pixel Shader (PS)
// 역할: 픽셀 하나의 최종 색상을 결정 (Rasterizer가 보간해준 데이터 사용)
// --------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
    // 정점 셰이더에서 넘어온 색상을 그대로 화면에 찍습니다.
    return input.Color;
}