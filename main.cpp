#include"CSharpWindow.h"
#include<iostream>
using namespace CSW;

int main()
{
	Window^ window = gcnew Window("Title", 960, 720);
	window->Show();

	while (window->Update())
	{
		if (window->KeyDown(Key::A))
		{
			std::cout << "asd\n";
		}
	}
}