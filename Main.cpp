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