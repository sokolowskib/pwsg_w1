#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <vector>


#define IDM_FILE_NEW    101
#define IDM_FILE_EXIT   102
#define IDM_HELP_ABOUT  201

#define IDC_MY_BUTTON   301
#define IDC_MY_EDIT     302
#define IDC_MY_LABEL    303

class AppTemplate
{
public:
    int Run(HINSTANCE hInst, int nCmdShow)
    {
        m_hInst = hInst;

        if (!RegisterMainClass())
            return 1;
        if (!CreateMainWindow(nCmdShow))
            return 1;

        CreateAppMenu();
        CreateControls();

        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        return (int)msg.wParam;
    }

private:
    HINSTANCE m_hInst = nullptr;
    HWND      m_hwnd = nullptr;

   
    HWND m_button = nullptr;
    HWND m_edit = nullptr;
    HWND m_label = nullptr;

 
    std::vector<HWND>    m_children;
    std::vector<HBRUSH>  m_brushes;

    HWND AddColorBox(int x, int y, int w, int h, COLORREF color)
    {
        HWND child = CreateWindowExW(
            0, L"STATIC", nullptr,
            WS_CHILD | WS_VISIBLE | SS_NOTIFY,
            x, y, w, h,
            m_hwnd, nullptr, m_hInst, nullptr
        );
        m_children.push_back(child);
        m_brushes.push_back(CreateSolidBrush(color));
        return child;
    }

 
    HWND AddButton(int x, int y, int w, int h, const wchar_t* text, int id)
    {
        return CreateWindowExW(
            0, L"BUTTON", text,
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            x, y, w, h,
            m_hwnd, (HMENU)(INT_PTR)id, m_hInst, nullptr
        );
    }

  
    HWND AddEdit(int x, int y, int w, int h, int id, const wchar_t* placeholder = L"")
    {
        HWND edit = CreateWindowExW(
            WS_EX_CLIENTEDGE, L"EDIT", placeholder,
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            x, y, w, h,
            m_hwnd, (HMENU)(INT_PTR)id, m_hInst, nullptr
        );
        return edit;
    }


    HWND AddLabel(int x, int y, int w, int h, const wchar_t* text, int id = 0)
    {
        return CreateWindowExW(
            0, L"STATIC", text,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            x, y, w, h,
            m_hwnd, (HMENU)(INT_PTR)id, m_hInst, nullptr
        );
    }

    
    HWND AddCheckbox(int x, int y, int w, int h, const wchar_t* text, int id)
    {
        return CreateWindowExW(
            0, L"BUTTON", text,
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            x, y, w, h,
            m_hwnd, (HMENU)(INT_PTR)id, m_hInst, nullptr
        );
    }

    
    HWND AddCombobox(int x, int y, int w, int dropH, int id)
    {
        return CreateWindowExW(
            0, L"COMBOBOX", nullptr,
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
            x, y, w, dropH,
            m_hwnd, (HMENU)(INT_PTR)id, m_hInst, nullptr
        );
    }

    bool RegisterMainClass()
    {
        WNDCLASSEXW wc = {};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = StaticWndProc;
        wc.hInstance = m_hInst;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
        wc.lpszClassName = L"AppTemplateClass";
        return RegisterClassExW(&wc) != 0;
    }

    bool CreateMainWindow(int nCmdShow)
    {
        DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
            | WS_MINIMIZEBOX | WS_CLIPCHILDREN;

        RECT rc = { 0, 0, 800, 600 };
        AdjustWindowRect(&rc, style, TRUE);

        m_hwnd = CreateWindowExW(
            0, L"AppTemplateClass", L"WinAPI Template",
            style,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rc.right - rc.left, rc.bottom - rc.top,
            nullptr, nullptr, m_hInst, this
        );

        if (!m_hwnd) return false;
        ShowWindow(m_hwnd, nCmdShow);
        UpdateWindow(m_hwnd);
        return true;
    }

  
    void CreateAppMenu()
    {
        HMENU hMenu = CreateMenu();

        HMENU hFile = CreatePopupMenu();
        AppendMenuW(hFile, MF_STRING, IDM_FILE_NEW, L"Nowy");
        AppendMenuW(hFile, MF_SEPARATOR, 0, nullptr);
        AppendMenuW(hFile, MF_STRING, IDM_FILE_EXIT, L"Wyjdz");

        HMENU hHelp = CreatePopupMenu();
        AppendMenuW(hHelp, MF_STRING, IDM_HELP_ABOUT, L"O programie");

        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFile, L"Plik");
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hHelp, L"Pomoc");

        SetMenu(m_hwnd, hMenu);
    }


    void CreateControls()
    {
        

        m_label = AddLabel(20, 20, 200, 20, L"Wpisz cos:");
        m_edit = AddEdit(20, 45, 200, 25, IDC_MY_EDIT);
        m_button = AddButton(230, 44, 100, 27, L"OK", IDC_MY_BUTTON);

      

        AddColorBox(20, 100, 80, 80, RGB(255, 80, 80));
        AddColorBox(110, 100, 80, 80, RGB(80, 200, 80));
        AddColorBox(200, 100, 80, 80, RGB(80, 80, 255));

        

        HWND combo = AddCombobox(20, 210, 200, 200, 401);
        SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)L"Opcja 1");
        SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)L"Opcja 2");
        SendMessageW(combo, CB_ADDSTRING, 0, (LPARAM)L"Opcja 3");
        SendMessageW(combo, CB_SETCURSEL, 0, 0);  // zaznacz pierwsza

        

        AddCheckbox(20, 250, 200, 25, L"Zaznacz mnie", 501);
    }

    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
           
        case WM_COMMAND:
        {
            int id = LOWORD(wParam);
            int code = HIWORD(wParam);

            // menu
            if (id == IDM_FILE_EXIT)
                PostMessageW(m_hwnd, WM_CLOSE, 0, 0);

            if (id == IDM_FILE_NEW)
                MessageBoxW(m_hwnd, L"Nowy!", L"Info", MB_OK);

            if (id == IDM_HELP_ABOUT)
                MessageBoxW(m_hwnd, L"WinAPI Template\nv1.0", L"O programie", MB_OK);

           
            if (id == IDC_MY_BUTTON && code == BN_CLICKED)
            {
                wchar_t buf[256] = {};
                GetWindowTextW(m_edit, buf, 256);
                MessageBoxW(m_hwnd, buf, L"Wpisales:", MB_OK);
            }

            if (code == STN_CLICKED)
            {
                HWND clicked = (HWND)lParam;
                // sprawdz czy to jeden z  boxow
                for (size_t i = 0; i < m_children.size(); i++)
                {
                    if (m_children[i] == clicked)
                    {
                        wchar_t buf[64];
                        wsprintfW(buf, L"Kliknieto box nr %d", (int)i);
                        MessageBoxW(m_hwnd, buf, L"Box", MB_OK);
                        break;
                    }
                }
            }

            return 0;
        }

       
        case WM_CTLCOLORSTATIC:
        {
            HWND ctrl = (HWND)lParam;
            HDC hdc = (HDC)wParam;

            for (size_t i = 0; i < m_children.size(); i++)
            {
                if (m_children[i] == ctrl)
                {
                    SetBkMode(hdc, TRANSPARENT);
                    return (LRESULT)m_brushes[i];
                }
            }

            SetBkMode(hdc, TRANSPARENT);
            return (LRESULT)GetStockObject(NULL_BRUSH);
        }

   
        case WM_DESTROY:
        {
            for (HBRUSH b : m_brushes)
                DeleteObject(b);
            PostQuitMessage(0);
            return 0;
        }
        }

        return DefWindowProcW(m_hwnd, msg, wParam, lParam);
    }

    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg,
        WPARAM wParam, LPARAM lParam)
    {
        AppTemplate* self = nullptr;

        if (msg == WM_NCCREATE)
        {
            auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            self = reinterpret_cast<AppTemplate*>(cs->lpCreateParams);
            self->m_hwnd = hwnd;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)self);
        }
        else
        {
            self = reinterpret_cast<AppTemplate*>(
                GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        }

        if (self)
            return self->HandleMessage(msg, wParam, lParam);

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
};

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int nCmdShow)
{
    AppTemplate app;
    return app.Run(hInst, nCmdShow);
}