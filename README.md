# ConsoleOutput
C++ class for abstracted usage of the Win32 console functions.

### Disclaimer
This is not production-worthy code! View this simply as a proof-of-concept. Preconditions are implicit. No error checking exists.

More in-depth documentation is forthcoming! For now, a simple example program is provided.

### Example
```C++
#include "Console.h"

int main()
{
	Console console;

	int height = console.GetConsoleWindowHeight();
	int width = console.GetConsoleWindowWidth();

	console.PrintToConsole("hello\nthere.\nthis is \na huge\n test.", 0, 0, 6, 6, true);

	console.MoveCursor(5, 0);

	int x, y;
	console.GetCursor(&x, &y);

	console.PrintToConsole("hello\n", -1, -1, 2, 3);

	console.Clear();

	return 0;
}
```
Here is a simple example to demonstrate usage of various functions of the `Console` class.
