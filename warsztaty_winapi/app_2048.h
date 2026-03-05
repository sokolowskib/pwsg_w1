#pragma once
#include <windows.h>
#include <string>


class app_2048 {
private:
	bool register_class();

	static std::wstring const s_class_name;

	static LRESULT CALLBACK window_proc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

	LRESULT window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);


	HWND create_window(DWORD style, HWND parent = nullptr);

	HINSTANCE m_instance;


	HWND m_main, m_popup;

public:
	app_2048(HINSTANCE instance);

	int run(int show_command);
};
