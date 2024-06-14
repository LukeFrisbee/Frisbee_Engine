#include "app.h"

// std
#include <cstdlib>
#include <iostream>
#include <windows.h>
#include <cstdio>

int main()
{
	fengine::App app;

	try {
		app.run();
	} catch(const std::exception &e) {
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{

// AttachConsole if debug mode
#ifdef _DEBUG
	if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
		AllocConsole();
		// attach console to std::iostream
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
	}
#endif

	main();

	return EXIT_SUCCESS;
}