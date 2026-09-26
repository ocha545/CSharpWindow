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

bool CSW::Window::RegisterWindowClass()
{
	WNDCLASSEX windowClass{};
	windowClass.cbSize = sizeof(windowClass);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.hInstance = instance;
	windowClass.hbrBackground = (HBRUSH)DKGRAY_BRUSH;
	windowClass.lpfnWndProc = WndProc;
	windowClass.lpszClassName = CSW_REGISTER_CLASS_NAME;
	return ::RegisterClassExW(&windowClass) != FALSE;
}

bool CSW::Window::UnregisterWindowClass()
{
	return ::UnregisterClassW(CSW_REGISTER_CLASS_NAME, instance) != FALSE;
}

bool CSW::Window::CreateNativeWindow(String^ title, SIZE size, POINT pos)
{
	pin_ptr<const wchar_t> native_title = PtrToStringChars(title);
	window = CreateWindowExW(
		NULL,
		CSW_REGISTER_CLASS_NAME,
		native_title,
		WS_OVERLAPPEDWINDOW,
		pos.x, pos.y,
		size.cx, size.cy,
		NULL,
		NULL,
		instance,
		NULL
	);

	return window != NULL;
}

bool CalcWindowCenterPos(int width, int height, POINT* pos, SIZE* size)
{
	RECT windowRect{};
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = width;
	windowRect.bottom = height;
	bool isAdjust = AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
	if (!isAdjust)
	{
		return false;
	}

	size->cx = windowRect.right - windowRect.left;
	size->cy = windowRect.bottom - windowRect.top;
	pos->x = (::GetSystemMetrics(SM_CXSCREEN) - size->cx) >> 1;
	pos->y = (::GetSystemMetrics(SM_CYSCREEN) - size->cy) >> 1;
	return true;
}

CSW::Window::Window(String^ title, int width, int height)
	: window(NULL), instance(GetModuleHandleW(NULL)),
	updateFuncs(gcnew List<Action^>(0))
{
	GlobalKey::Initialize();


	if (SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
	{
		throw gcnew System::InvalidOperationException(
			"SetProcessDpiAwarenessContextで無効なパラメーターが指定されました Err:" + ::GetLastError()
		);
	}

	if (!RegisterWindowClass())
	{
		throw gcnew System::InvalidOperationException(
			"RegisterWindowClassでウィンドウクラスの登録に失敗しました Err:" + ::GetLastError()
		);
	}

	POINT windowPos{};
	SIZE windowSize{};
	if (!CalcWindowCenterPos(width, height, &windowPos, &windowSize))
	{
		throw gcnew System::InvalidOperationException(
			"CalcWindowCenterPosで座標の計算に失敗しました Err:" + ::GetLastError()
		);
	}


	if (!CreateNativeWindow(title, windowSize, windowPos))
	{
		throw gcnew System::InvalidOperationException(
			"CreateNativeWindowでウィンドウの作成に失敗しました Err:" + ::GetLastError()
		);
	}
}

CSW::Window::~Window()
{
	this->!Window();
}

CSW::Window::!Window()
{
	if (window)
	{
		DestroyWindow(window);
		window = nullptr;
	}

	UnregisterWindowClass();
}

void CSW::Window::AddUpdateFunc(Action^ function)
{
	updateFuncs->Add(function);
}

void CSW::Window::Show()
{
	if (window)
	{
		::ShowWindow(window, SW_SHOW);
	}
}

bool CSW::Window::Update()
{
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

	GlobalKey::CopyPrevious();

	for (int i = 0; i < updateFuncs->Count; i++)
	{
		updateFuncs[i]();
	}

	return true;
}

void CSW::Window::UpdatePos(int x, int y)
{
	throw gcnew System::NotImplementedException("UpdatePos関数はまだ実装されていません");
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

