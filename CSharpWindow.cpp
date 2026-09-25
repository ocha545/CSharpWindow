#include"CSharpWindow.h"

LRESULT CALLBACK CSW::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{

	case WM_CREATE:{
		//RawInputの設定
		//マウス入力も対応したいので配列にしておく
		RAWINPUTDEVICE devices[1]{};

		devices[CSW_RAWINPUT_DEVICE_INDEX_KEYBOARD].hwndTarget = hWnd;
		devices[CSW_RAWINPUT_DEVICE_INDEX_KEYBOARD].usUsagePage = 0x01;
		devices[CSW_RAWINPUT_DEVICE_INDEX_KEYBOARD].usUsage = 0x06;
		devices[CSW_RAWINPUT_DEVICE_INDEX_KEYBOARD].dwFlags = RIDEV_INPUTSINK;

		RegisterRawInputDevices(devices, 1, sizeof(devices[CSW_RAWINPUT_DEVICE_INDEX_KEYBOARD]));
	}	return DefWindowProcW(hWnd, msg, wp, lp);

	case WM_INPUT: {
		RAWINPUT rawInput{};
		UINT dwSize = sizeof(rawInput);
		::GetRawInputData((HRAWINPUT)lp, RID_INPUT, &rawInput, &dwSize, sizeof(RAWINPUTHEADER));
		if (rawInput.header.dwType == RIM_TYPEKEYBOARD)
		{
			if (rawInput.data.keyboard.VKey == 0xff)
			{
				return DefWindowProcW(hWnd, msg, wp, lp);
			}

			if ((rawInput.data.keyboard.Flags & RI_KEY_BREAK) == 0)
			{
				GlobalKey::AddKey((Key)rawInput.data.keyboard.VKey);
			}
			else
			{
				GlobalKey::RemoveKey((Key)rawInput.data.keyboard.VKey);
			}
		}
	}	return DefWindowProcW(hWnd, msg, wp, lp);

	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProcW(hWnd, msg, wp, lp);
	}
}

CSW::Window::Window(String^ title, int width, int height)
	: window(NULL), instance(GetModuleHandleW(NULL)),
	updateFuncs(gcnew List<Action^>(0))
{
	::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	GlobalKey::Create();

	WNDCLASSEX windowClass{};
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.hInstance = instance;
	windowClass.hbrBackground = (HBRUSH)DKGRAY_BRUSH;
	windowClass.lpfnWndProc = WndProc;
	windowClass.lpszClassName = L"CSharpWindow ^_^;";
	RegisterClassEx(&windowClass);

	RECT windowRect{};
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = width;
	windowRect.bottom = height;
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;

	int x = (::GetSystemMetrics(SM_CXSCREEN) - width) >> 1;
	int y = (::GetSystemMetrics(SM_CYSCREEN) - height) >> 1;

	marshal_context ctx{};
	pin_ptr<const wchar_t> native_title = ctx.marshal_as<const wchar_t*>(title);
	window = CreateWindowExW(
		NULL,
		windowClass.lpszClassName,
		native_title,
		WS_OVERLAPPEDWINDOW,
		x, y,
		windowWidth, windowHeight,
		NULL,
		NULL,
		instance,
		NULL
	);
}

CSW::Window::~Window()
{
	this->!Window();
}

CSW::Window::!Window()
{
}

void CSW::Window::AddUpdateFunc(Action^ function)
{
	updateFuncs->Add(function);
}

void CSW::Window::Show()
{
	::ShowWindow(window, SW_SHOW);
}

bool CSW::Window::Update()
{
	for (int i = 0; i < updateFuncs->Count; i++)
	{
		updateFuncs[i]();
	}

	GlobalKey::CopyPrevious();

	MSG msg{};
	while (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	}
	return true;
}

void CSW::Window::UpdatePos(int x, int y)
{

}

bool CSW::Window::KeyDown(Key key)
{
	return !GlobalKey::ContainsPrevious(key) && GlobalKey::Contains(key);
}

bool CSW::Window::KeyUp(Key key)
{
	return GlobalKey::ContainsPrevious(key) && !GlobalKey::Contains(key);
}

bool CSW::Window::KeyPress(Key key)
{
	return GlobalKey::Contains(key);
}

IntPtr CSW::Window::GetHWND()
{
	return IntPtr(window);
}

