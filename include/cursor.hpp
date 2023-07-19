#ifndef ACONN__CURSOR__
#define ACONN__CURSOR__

#include <rlutil.h>

#ifdef _WIN32  // Initialize only on Windows
class __Cursor{
public:
	__Cursor(){ // use UTF8 code page; enable ANSI escape sequence
		SetConsoleOutputCP(CP_UTF8);
		HANDLE sout=GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dwMode;
		GetConsoleMode(sout,&dwMode);
		dwMode|=0x0004;// ENABLE_VIRTUAL_TERMINAL_PROCESSING
		dwMode|=0x0008;// DISABLE_NEWLINE_AUTO_RETURN
		SetConsoleMode(sout,dwMode);
	}
}cursor;
#endif

#endif // ACONN__CURSOR__