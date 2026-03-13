#include "app_2048.h"
#include <stdexcept>



std::wstring const app_2048::s_class_name{ L"2048 Window" };


bool app_2048::register_class() {
	WNDCLASSEXW desc{};


	if (GetClassInfoExW(m_instance, s_class_name.c_str(), &desc) != 0) return true;

	desc = {
		.cbSize = sizeof(WNDCLASSEXW),

		.lpfnWndProc = window_proc_static,

		.hInstance = m_instance,

		.hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
		.hbrBackground = CreateSolidBrush(RGB(255,255,254)),

		.lpszClassName = s_class_name.c_str()

	};
	return RegisterClassExW(&desc) != 0;
}


HWND app_2048::create_window(DWORD style, HWND parent) {
	return CreateWindowExW(0, //wysyla wszystkie 
		s_class_name.c_str(), //nazwa klasy
		L"2048", //tytul okna

		style,
		CW_USEDEFAULT, 0, //pozycja x,y sam wybierze
		CW_USEDEFAULT, 0, //szerokosc i wysokosc, sam wybierze

		parent, //paremt
		nullptr, //menu
		m_instance, //handle na instancje naszej aplikacji
		this //wskaznik na obiekt , czyli nas


	);
}


LRESULT app_2048::window_proc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam) { //statyczna procedura okna , generalnie przekierowuje ona do obiektu 
	app_2048* app = nullptr;
	if (message == WM_NCCREATE) { //jesli message jest o stworzeniu okna
		auto p = reinterpret_cast<LPCREATESTRUCTW>(lparam); //rzutujemy liczbe na wskaznik na structa

		app = static_cast<app_2048*>(p->lpCreateParams); //p->lpCreateParams to jest this z CreateWindowExW, rzutujemy na app


		SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
	}
	else {
		app = reinterpret_cast<app_2048*>(GetWindowLongPtrW(window, GWLP_USERDATA));
	}
	if (app != nullptr) {
			return app->window_proc(window, message, wparam, lparam);
	}
	return DefWindowProcW(window, message, wparam, lparam);

}

LRESULT app_2048::window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) { //niestatyczna procedura, nwm jeszcze po co
	switch (message) {
	case WM_CLOSE:
		DestroyWindow(window);
		return 0;
	case WM_DESTROY:
		if (window == m_main)
			PostQuitMessage(EXIT_SUCCESS);
		return 0;
	}
	return DefWindowProcW(window, message, wparam, lparam);
}

app_2048::app_2048(HINSTANCE instance) : m_instance{ instance }, m_main{}, m_popup{} {
	register_class();
	DWORD main_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;

	DWORD popup_style = WS_OVERLAPPED | WS_CAPTION;


	m_main = create_window(main_style);
	m_popup = create_window(popup_style, m_main);
}


int app_2048::run(int show_command) {
	ShowWindow(m_main, show_command);
	ShowWindow(m_popup, SW_SHOWNA); //show no activate
	MSG msg{};

	BOOL result = true;

	while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0) {
		if (result == -1) return EXIT_FAILURE;

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return EXIT_SUCCESS;

}



