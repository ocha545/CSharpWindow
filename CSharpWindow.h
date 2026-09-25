#pragma once
#include<iostream>
#include<windows.h>
#include<msclr/marshal.h>
using msclr::interop::marshal_context;

#using<System.dll>
#using<System.Core.dll>
#using<System.Drawing.dll>
//#using
using System::String;
using System::Action;
using System::IntPtr;
using System::Drawing::Color;
using System::Collections::Generic::List;
using System::Collections::Generic::HashSet;

#define CSW_RAWINPUT_DEVICE_INDEX_KEYBOARD (0)

namespace CSW
{
	public enum class Key : USHORT
	{
		Empty = 0xff,
		Backspace = VK_BACK,
		Tab = VK_TAB,
		Clear = VK_CLEAR,
		Enter = VK_RETURN,
		Shift = VK_SHIFT,
		Ctrl = VK_CONTROL,
		Alt,
		Pause,
		CapsLock,
		KANA,
		IME_HANGUL = KANA,
		IME_ON,
		IME_JUNJA,
		IME_Final,
		IME_HANJA,
		IME_KANJI = IME_HANJA,
		IME_OFF = VK_IME_OFF,
		Escape,
		IME_CONVERT,
		IME_NONCONVERT,
		IME_ACCEPT,
		IME_MODE_CHANGE,
		Space,
		PageUp,
		PageDown,
		End,
		Home,
		Left,
		Up,
		Right,
		Down,
		Select,
		Print,
		Execute,
		Snapshot,
		Insert,
		Delete,
		Help,
		NUM_0 = 0x30,
		NUM_1,
		NUM_2,
		NUM_3,
		NUM_4,
		NUM_5,
		NUM_6,
		NUM_7,
		NUM_8,
		NUM_9,
		A = 0x41,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		Win,
		RWin,
		Apps,
		Sleep = VK_SLEEP,
		NUMPAD_0,
		NUMPAD_1,
		NUMPAD_2,
		NUMPAD_3,
		NUMPAD_4,
		NUMPAD_5,
		NUMPAD_6,
		NUMPAD_7,
		NUMPAD_8,
		NUMPAD_9,
		Multiply,
		Add,
		Separator,
		Subtract,
		Decimal,
		Divide,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,
		NumLock = VK_NUMLOCK,
		Scroll,
		LShift = VK_LSHIFT,
		RShift,
		LCtrl,
		RCtrl,
		LAlt,
		RAlt,
		BROWSER_BACK,
		BROWSER_FORWARD,
		BROWSER_REFRESH,
		BROWSER_STOP,
		BROWSER_SEARCH,
		BROWSER_FAVORITES,
		BROWSER_HOME,
		VOLUMME_Mute,
		VOLUMME_Down,
		VOLUMME_Up,
		MEDIA_NextTrack,
		MEDIA_PrevTrack,
		MEDIA_STOP,
		MEDIA_PLAY_PAUSE,
		LAUNCH_MAIL,
		LAUNCH_MEDIA_SELECT,
		LAUNCH_APP1,
		LAUNCH_APP2,
		//ここから先は不要っぽいので書きません
	};

	private ref class GlobalKey
	{
	private:
		static HashSet<Key>^ downKeys;
		static HashSet<Key>^ prevKeys;

	public:
		static void Create()
		{
			downKeys = gcnew HashSet<Key>();
			prevKeys = gcnew HashSet<Key>();
		}

		static void AddKey(Key key)
		{
			downKeys->Add(key);
		}

		static void RemoveKey(Key key)
		{
			downKeys->Remove(key);
		}

		static bool Contains(Key key)
		{
			return downKeys->Contains(key);
		}

		static bool ContainsPrevious(Key key)
		{
			return prevKeys->Contains(key);
		}

		static void CopyPrevious()
		{
			prevKeys = gcnew HashSet<Key>(downKeys);
		}

		static void ClearAllKeys()
		{
			delete downKeys;
			delete prevKeys;
		}
	};

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	public ref class Window
	{
	private:
		HINSTANCE instance;
		HWND window;
		COLORREF backColor;

		List<Action^>^ updateFuncs;
		int globalKeysIndex;

	public:
		Window(String^ title, int width, int height);
		~Window();
		!Window();

		void AddUpdateFunc(Action^ function);

		void Show();

		bool Update();

		void UpdatePos(int x, int y);

		bool KeyDown(Key key);

		bool KeyUp(Key key);

		bool KeyPress(Key key);

		IntPtr GetHWND();

	public:
		property String^ IconPath;
		property String^ CursorPath;
		//property int Width;
		//property int Height;
		//使えるかあやしい
		//property Color BackColor{
		//	Color get() {
		//		//  AARRGGBB
		//		//0x00000000
		//		return Color::FromArgb(
		//			(backColor >> 24) & 0xff,
		//			(backColor >> 16) & 0xff,
		//			(backColor >>  8) & 0xff,
		//			(backColor >>  0) & 0xff
		//		);
		//	}
		//	void set(Color color) {
		//		backColor = (color.A << 24) | (color.R << 16) | (color.G << 8) | color.B;

		//		std::cout << "ARGB\n";
		//		std::cout << (int)color.A << "\n";
		//		std::cout << (int)color.R << "\n";
		//		std::cout << (int)color.G << "\n";
		//		std::cout << (int)color.B << "\n";
		//		std::cout << "REF:" << std::hex << backColor << "\n";
		//		SetClassLongPtrW(window, GCLP_HBRBACKGROUND, (LONG)backColor);
		//	}
		//}
	};
}