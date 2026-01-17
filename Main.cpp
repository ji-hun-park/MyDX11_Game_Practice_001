#include <windows.h>
#include <wrl/client.h> // ComPtr용

// DirectX 11 관련 헤더
#include <d3d11.h>
#include <d3dcompiler.h> // 셰이더 컴파일용
#include <DirectXMath.h> // 수학 라이브러리 (행렬, 벡터)

// 라이브러리 링크 (속성 페이지에서 추가하는 대신 코드로 처리)
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX; // DirectXMath 네임스페이스
using Microsoft::WRL::ComPtr; // ComPtr 스마트 포인터

// 전역 변수 (나중에는 클래스로 관리해야 함)
HINSTANCE g_hInst = nullptr;
HWND g_hWnd = nullptr;

// DirectX 핵심 객체들
ComPtr<ID3D11Device> g_pd3dDevice;                  // 자원 생성 공장
ComPtr<ID3D11DeviceContext> g_pImmediateContext;    // 그리기 명령 작업자
ComPtr<IDXGISwapChain> g_pSwapChain;                // 더블 버퍼링 관리자
ComPtr<ID3D11RenderTargetView> g_pRenderTargetView; // 렌더링 타겟 (도화지)

// 윈도우 프로시저 (이벤트 처리기)
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Direct3D 초기화 함수
HRESULT InitD3D(HWND hWnd) {
    HRESULT hr = S_OK;

    // 1. 스왑 체인 설정 구조체 작성
    DXGI_SWAP_CHAIN_DESC sd = { 0 };
    sd.BufferCount = 1;                                     // 백버퍼 개수 (1개면 충분)
    sd.BufferDesc.Width = 800;                              // 해상도 너비
    sd.BufferDesc.Height = 600;                             // 해상도 높이
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;      // 색상 포맷 (RGBA 32비트)
    sd.BufferDesc.RefreshRate.Numerator = 60;               // 주사율 60Hz
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;       // "이 버퍼에 그림 그릴거임"
    sd.OutputWindow = hWnd;                                 // 그림 그릴 윈도우 핸들
    sd.SampleDesc.Count = 1;                                // 멀티샘플링(MSAA) 끄기
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;                                     // 창 모드

    // 2. 장치(Device), 컨텍스트(Context), 스왑체인(SwapChain) 동시 생성
    // D3D_FEATURE_LEVEL_11_0 을 지원하는 그래픽 카드를 요청합니다.
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    hr = D3D11CreateDeviceAndSwapChain(
        nullptr,                    // 기본 어댑터(그래픽카드) 사용
        D3D_DRIVER_TYPE_HARDWARE,   // 하드웨어 가속 사용 (필수!)
        nullptr,
        0,                          // 플래그 (디버그 시 D3D11_CREATE_DEVICE_DEBUG 사용 가능)
        featureLevels, numFeatureLevels,
        D3D11_SDK_VERSION,
        &sd,
        g_pSwapChain.GetAddressOf(),
        g_pd3dDevice.GetAddressOf(),
        nullptr,
        g_pImmediateContext.GetAddressOf()
    );

    if (FAILED(hr)) return hr;

    // 3. 렌더 타겟 뷰(RTV) 생성
    // 스왑 체인에서 백 버퍼(그림 그릴 종이)를 가져옵니다.
    ComPtr<ID3D11Texture2D> pBackBuffer;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)pBackBuffer.GetAddressOf());
    if (FAILED(hr)) return hr;

    // 가져온 백 버퍼를 타겟으로 뷰를 만듭니다.
    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, g_pRenderTargetView.GetAddressOf());
    if (FAILED(hr)) return hr;

    // 4. 렌더 타겟 설정 (Output Merger 단계)
    // "이제부터 이 도화지(RTV)에 그릴 거야"라고 작업자(Context)에게 지시
    g_pImmediateContext->OMSetRenderTargets(1, g_pRenderTargetView.GetAddressOf(), nullptr);

    // 5. 뷰포트(Viewport) 설정 (Rasterizer 단계)
    // 도화지의 어느 영역에 그릴지 설정 (전체 화면)
    D3D11_VIEWPORT vp;
    vp.Width = 800.0f;
    vp.Height = 600.0f;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports(1, &vp);

    return S_OK;
}

// 렌더링 함수
void Render() {
    // 1. 화면 지우기 (Clear)
    // RGBA (파란색 계열: Cornflower Blue)
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView.Get(), ClearColor);

    // 2. ... 여기서 삼각형 등을 그립니다 ...

    // 3. 보여주기 (Swap Buffer)
    // 백 버퍼와 프론트 버퍼 교체
    g_pSwapChain->Present(0, 0); // 첫 번째 인자: 1이면 VSync 켜기, 0이면 끄기
}

// 메인 진입점
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    g_hInst = hInstance;

    // 1. 윈도우 클래스 등록
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.lpszClassName = L"DX11GameClass";

    RegisterClassExW(&wcex);

    // 2. 윈도우 생성
    g_hWnd = CreateWindowW(L"DX11GameClass", L"My First DX11 Game", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);

    if (!g_hWnd) return FALSE;

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    // 3. 메인 루프 (유니티의 엔진 루프와 같음)
    MSG msg = { 0 };
    while (msg.message != WM_QUIT) {
        // 윈도우 메시지가 있으면 처리 (입력 등)
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            // 메시지가 없을 때 게임 로직 실행 (Update & Render)
            // 여기에 나중에 Game->Run() 같은 코드가 들어갑니다.
        }
    }

    return (int)msg.wParam;
}