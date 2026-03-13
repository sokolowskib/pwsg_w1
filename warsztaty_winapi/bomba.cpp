#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <vector>
#include <cmath>
#include <algorithm>

// ID menu
#define IDM_CLEAR_ALL   201
#define IDM_SET_BG      202
#define IDM_SET_RECT    203
#define IDM_EXIT        204
#define IDM_ABOUT       205

class RectApp
{
public:
    // ===== punkt wejscia =====
    int Run(HINSTANCE hInst, int nCmdShow)
    {
        m_hInst = hInst;
        m_bgColor = RGB(30, 50, 90);
        m_rectColor = RGB(170, 70, 80);
        m_bgBrush = CreateSolidBrush(m_bgColor);
        m_rectBrush = CreateSolidBrush(m_rectColor);

        if (!RegisterMainClass())
            return 1;

        if (!CreateMainWindow(nCmdShow))
            return 1;

        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        return (int)msg.wParam;
    }

private:
    // ===== dane =====
    HINSTANCE          m_hInst = nullptr;
    HWND               m_hwnd = nullptr;

    COLORREF           m_bgColor;
    COLORREF           m_rectColor;
    HBRUSH             m_bgBrush = nullptr;
    HBRUSH             m_rectBrush = nullptr;

    bool               m_drawing = false;
    POINT              m_startPt = {};
    HWND               m_activeRect = nullptr;
    std::vector<HWND>  m_rects;

    // ===== rejestracja klasy =====
    bool RegisterMainClass()
    {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = StaticWndProc;
        wc.hInstance = m_hInst;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = m_bgBrush;
        wc.lpszClassName = L"RectAppClass";
        return RegisterClassExW(&wc) != 0;
    }

    // ===== tworzenie okna =====
    bool CreateMainWindow(int nCmdShow)
    {
        DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
            | WS_MINIMIZEBOX | WS_CLIPCHILDREN;

        RECT rc = { 0, 0, 800, 600 };
        AdjustWindowRect(&rc, style, TRUE);

        m_hwnd = CreateWindowExW(
            0, L"RectAppClass", L"Not WM_PAINT",
            style,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rc.right - rc.left, rc.bottom - rc.top,
            nullptr, nullptr, m_hInst, this
        );

        if (!m_hwnd)
            return false;

        SetMenu(m_hwnd, CreateAppMenu());
        ShowWindow(m_hwnd, nCmdShow);
        UpdateWindow(m_hwnd);
        return true;
    }

    // ===== tworzenie menu =====
    HMENU CreateAppMenu()
    {
        HMENU hMenu = CreateMenu();

        HMENU hFile = CreatePopupMenu();
        AppendMenuW(hFile, MF_STRING, IDM_CLEAR_ALL, L"Wyczysc wszystko");
        AppendMenuW(hFile, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(hFile, MF_STRING, IDM_EXIT, L"Wyjdz");

        HMENU hColors = CreatePopupMenu();
        AppendMenuW(hColors, MF_STRING, IDM_SET_BG, L"Kolor tla...");
        AppendMenuW(hColors, MF_STRING, IDM_SET_RECT, L"Kolor prostokatow...");

        HMENU hHelp = CreatePopupMenu();
        AppendMenuW(hHelp, MF_STRING, IDM_ABOUT, L"O programie");

        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFile, L"Plik");
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hColors, L"Kolory");
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hHelp, L"Pomoc");

        return hMenu;
    }

    // ===== systemowy dialog wyboru koloru =====
    bool PickColor(COLORREF& color)
    {
        static COLORREF customColors[16] = {};

        CHOOSECOLORW cc = {};
        cc.lStructSize = sizeof(cc);
        cc.hwndOwner = m_hwnd;
        cc.rgbResult = color;
        cc.lpCustColors = customColors;
        cc.Flags = CC_FULLOPEN | CC_RGBINIT;

        if (ChooseColorW(&cc))
        {
            color = cc.rgbResult;
            return true;
        }
        return false;
    }

    // ===== zmiana koloru tla =====
    void ChangeBgColor()
    {
        COLORREF newColor = m_bgColor;
        if (!PickColor(newColor))
            return;

        m_bgColor = newColor;

        HBRUSH oldBrush = m_bgBrush;
        m_bgBrush = CreateSolidBrush(m_bgColor);
        SetClassLongPtrW(m_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)m_bgBrush);
        DeleteObject(oldBrush);

        InvalidateRect(m_hwnd, nullptr, TRUE);
    }

    // ===== zmiana koloru prostokatow =====
    void ChangeRectColor()
    {
        COLORREF newColor = m_rectColor;
        if (!PickColor(newColor))
            return;

        m_rectColor = newColor;

        HBRUSH oldBrush = m_rectBrush;
        m_rectBrush = CreateSolidBrush(m_rectColor);
        DeleteObject(oldBrush);

        for (HWND r : m_rects)
            InvalidateRect(r, nullptr, TRUE);

        if (m_activeRect)
            InvalidateRect(m_activeRect, nullptr, TRUE);
    }

    // ===== usuniecie wszystkich prostokatow =====
    void ClearAll()
    {
        if (m_drawing && m_activeRect)
        {
            DestroyWindow(m_activeRect);
            m_activeRect = nullptr;
            m_drawing = false;
            ReleaseCapture();
        }

        for (HWND r : m_rects)
            DestroyWindow(r);
        m_rects.clear();

        InvalidateRect(m_hwnd, nullptr, TRUE);
    }

    // ===== obsluga wiadomosci =====
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_LBUTTONDOWN:
        {
            m_startPt.x = (short)LOWORD(lParam);
            m_startPt.y = (short)HIWORD(lParam);

            m_activeRect = CreateWindowExW(
                0, L"STATIC", nullptr,
                WS_CHILD | WS_VISIBLE,
                m_startPt.x, m_startPt.y, 0, 0,
                m_hwnd, nullptr, m_hInst, nullptr
            );

            m_drawing = true;
            SetCapture(m_hwnd);
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if (!m_drawing || !m_activeRect)
                return 0;

            int cx = (short)LOWORD(lParam);
            int cy = (short)HIWORD(lParam);

            int x = (std::min)((LONG)cx, m_startPt.x);
            int y = (std::min)((LONG)cy, m_startPt.y);
            int w = std::abs(cx - (int)m_startPt.x);
            int h = std::abs(cy - (int)m_startPt.y);

            SetWindowPos(m_activeRect, nullptr, x, y, w, h,
                SWP_NOZORDER | SWP_NOACTIVATE);
            return 0;
        }

        case WM_LBUTTONUP:
        {
            if (!m_drawing)
                return 0;

            ReleaseCapture();
            m_drawing = false;

            if (m_activeRect)
            {
                m_rects.push_back(m_activeRect);
                m_activeRect = nullptr;
            }
            return 0;
        }

        case WM_KEYDOWN:
        {
            if (wParam == VK_BACK)
            {
                if (m_drawing && m_activeRect)
                {
                    DestroyWindow(m_activeRect);
                    m_activeRect = nullptr;
                    m_drawing = false;
                    ReleaseCapture();
                }
                else if (!m_rects.empty())
                {
                    DestroyWindow(m_rects.back());
                    m_rects.pop_back();
                }
                InvalidateRect(m_hwnd, nullptr, TRUE);
            }
            return 0;
        }

        case WM_CTLCOLORSTATIC:
        {
            HDC hdc = (HDC)wParam;
            SetBkColor(hdc, m_rectColor);
            return (LRESULT)m_rectBrush;
        }

        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            case IDM_CLEAR_ALL:
                ClearAll();
                break;
            case IDM_SET_BG:
                ChangeBgColor();
                break;
            case IDM_SET_RECT:
                ChangeRectColor();
                break;
            case IDM_ABOUT:
                MessageBoxW(m_hwnd,
                    L"Not WM_PAINT v2.0\n\n"
                    L"Rysowanie prostokatow w WinAPI.\n"
                    L"Lewy przycisk myszy - rysuj.\n"
                    L"Backspace - usun ostatni.\n"
                    L"Menu Kolory - zmien kolory.",
                    L"O programie", MB_OK | MB_ICONINFORMATION);
                break;
            case IDM_EXIT:
                PostMessageW(m_hwnd, WM_CLOSE, 0, 0);
                break;
            }
            return 0;
        }

        case WM_DESTROY:
            DeleteObject(m_rectBrush);
            PostQuitMessage(0);
            return 0;
        }

        return DefWindowProcW(m_hwnd, msg, wParam, lParam);
    }

    // ===== trampolina static -> instancja =====
    //
    // Problem: WndProc musi byc statyczna (wymaga tego system),
    // ale statyczna funkcja nie ma dostepu do pol klasy (brak this).
    //
    // Rozwiazanie:
    // 1. W CreateWindowExW podajemy 'this' jako ostatni parametr (lpParam)
    // 2. Pierwsza wiadomosc to WM_NCCREATE — w niej lParam wskazuje
    //    na CREATESTRUCT, a w nim lpCreateParams = nasz 'this'
    // 3. Zapisujemy go w GWLP_USERDATA okna (SetWindowLongPtrW)
    // 4. Przy kazdej kolejnej wiadomosci odczytujemy go (GetWindowLongPtrW)
    //    i delegujemy do HandleMessage na instancji

    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg,
        WPARAM wParam, LPARAM lParam)
    {
        RectApp* self = nullptr;

        if (msg == WM_NCCREATE)
        {
            auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            self = reinterpret_cast<RectApp*>(cs->lpCreateParams);
            self->m_hwnd = hwnd;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)self);
        }
        else
        {
            self = reinterpret_cast<RectApp*>(
                GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }

        if (self)
            return self->HandleMessage(msg, wParam, lParam);

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
};

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int nCmdShow)
{
    RectApp app;
    return app.Run(hInst, nCmdShow);
}