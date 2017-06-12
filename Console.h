#ifndef CONSOLE_HEADER
#define CONSOLE_HEADER

#include <Windows.h>
#include <sstream>

using namespace std;

class Console
{
public:
	Console()
	{
		this->SetHandle(GetStdHandle(STD_OUTPUT_HANDLE));
	}
	Console(HANDLE hStdOut)
	{
		this->SetHandle(hStdOut);
	}
	void SetHandle(HANDLE hStdOut)
	{
		this->hStdOut = hStdOut;
		this->SetConsoleScreenBufferInfo();
	}
	void PrintToConsole(string data, int xLocation = 0, int yLocation = 0, int width = 0xFFFFFFFF, int height = 0xFFFFFFFF, bool moveCursor = false)
	{
		int LineLength, Returns;

		if (width != 0xFFFFFFFF)
			LineLength = width;
		else
			LineLength = GetLongestLineLength(data);

		if (height != 0xFFFFFFFF)
			Returns = height;
		else
			Returns = CountReturns(data) + 1;

		COORD BufferSize, BufferLocation;
		SMALL_RECT WriteRegion;

		this->SetConsoleScreenBufferInfo();

		CHAR_INFO * Buffer = new CHAR_INFO[LineLength * Returns];

		string formattedString = ReplaceReturns(data, LineLength);

		for (int i = 0; i < LineLength * Returns; i++)
		{
			if (i <= (int)formattedString.size())
				Buffer[i].Char.AsciiChar = formattedString[i];
			else
				Buffer[i].Char.AsciiChar = ' ';
			Buffer[i].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
		}

		BufferSize.X = LineLength;
		BufferSize.Y = Returns;

		BufferLocation.X = 0;
		BufferLocation.Y = 0;

		if (xLocation != 0xFFFFFFFF)
		{
			WriteRegion.Left = xLocation;
			WriteRegion.Right = LineLength + xLocation - 1;
		}
		else
		{
			int x, y;
			this->GetCursor(&x, &y);

			WriteRegion.Left = x;
			WriteRegion.Right = LineLength + x - 1;
		}

		if (yLocation != 0xFFFFFFFF)
		{
			WriteRegion.Top = yLocation;
			WriteRegion.Bottom = Returns + yLocation - 1;
		}
		else
		{
			int x, y;
			this->GetCursor(&x, &y);

			WriteRegion.Top = y;
			WriteRegion.Bottom = Returns + y - 1;
		}

		WriteConsoleOutputA(this->hStdOut, Buffer, BufferSize, BufferLocation, &WriteRegion);

		if(moveCursor)
			this->MoveCursor(WriteRegion.Right + 1, WriteRegion.Bottom);

		delete[] Buffer;
	}
	void MoveCursor(int x, int y)
	{
		COORD coordinate = { (short)x, (short)y };

		SetConsoleCursorPosition(this->hStdOut, coordinate);
	}
	void GetCursor(int * x, int * y)
	{
		CONSOLE_SCREEN_BUFFER_INFOEX ConsoleScreenBufferInfoEx;

		ZeroMemory(&ConsoleScreenBufferInfoEx, sizeof(CONSOLE_SCREEN_BUFFER_INFOEX));

		ConsoleScreenBufferInfoEx.cbSize = sizeof(ConsoleScreenBufferInfoEx);

		GetConsoleScreenBufferInfoEx(this->hStdOut, &ConsoleScreenBufferInfoEx);

		*x = ConsoleScreenBufferInfoEx.dwCursorPosition.X;
		*y = ConsoleScreenBufferInfoEx.dwCursorPosition.Y;
	}
	void Clear(int top, int left, int bottom, int right)
	{
		COORD BufferSize, BufferLocation;
		SMALL_RECT WriteRegion;

		CHAR_INFO * Buffer = new CHAR_INFO[(right - left) * (bottom - top)];

		for (int i = 0; i < ((right - left) * (bottom - top)); i++)
		{
			Buffer[i].Char.AsciiChar = ' ';
			Buffer[i].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
		}

		BufferSize.X = right - left;
		BufferSize.Y = bottom - top;

		BufferLocation.X = 0;
		BufferLocation.Y = 0;

		WriteRegion.Top = top;
		WriteRegion.Left = left;
		WriteRegion.Bottom = bottom;
		WriteRegion.Right = right;

		WriteConsoleOutput(this->hStdOut, Buffer, BufferSize, BufferLocation, &WriteRegion);

		delete[] Buffer;
	}
	void Clear()
	{
		COORD coordScreen = { 0, 0 };
		DWORD numberOfCharactersWritten, consoleSize;
		CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;

		GetConsoleScreenBufferInfo(this->hStdOut, &consoleScreenBufferInfo);

		consoleSize = consoleScreenBufferInfo.dwSize.X * consoleScreenBufferInfo.dwSize.Y;

		FillConsoleOutputCharacter(this->hStdOut, ' ', consoleSize, coordScreen, &numberOfCharactersWritten);

		GetConsoleScreenBufferInfo(this->hStdOut, &consoleScreenBufferInfo);

		FillConsoleOutputAttribute(this->hStdOut, consoleScreenBufferInfo.wAttributes, consoleSize, coordScreen, &numberOfCharactersWritten);

		SetConsoleCursorPosition(this->hStdOut, coordScreen);
	}
	int GetConsoleWindowWidth()
	{
		CONSOLE_SCREEN_BUFFER_INFOEX ConsoleScreenBufferInfoEx;

		ZeroMemory(&ConsoleScreenBufferInfoEx, sizeof(CONSOLE_SCREEN_BUFFER_INFOEX));

		ConsoleScreenBufferInfoEx.cbSize = sizeof(ConsoleScreenBufferInfoEx);

		GetConsoleScreenBufferInfoEx(this->hStdOut, &ConsoleScreenBufferInfoEx);

		return ConsoleScreenBufferInfoEx.srWindow.Right + 1;
	}
	int GetConsoleWindowHeight()
	{
		CONSOLE_SCREEN_BUFFER_INFOEX ConsoleScreenBufferInfoEx;

		ZeroMemory(&ConsoleScreenBufferInfoEx, sizeof(CONSOLE_SCREEN_BUFFER_INFOEX));

		ConsoleScreenBufferInfoEx.cbSize = sizeof(ConsoleScreenBufferInfoEx);

		GetConsoleScreenBufferInfoEx(this->hStdOut, &ConsoleScreenBufferInfoEx);

		return ConsoleScreenBufferInfoEx.srWindow.Bottom + 1;
	}
	
private:
	HANDLE hStdOut;

	void SetConsoleScreenBufferInfo()
	{
		CONSOLE_SCREEN_BUFFER_INFOEX ConsoleScreenBufferInfoEx;

		ZeroMemory(&ConsoleScreenBufferInfoEx, sizeof(CONSOLE_SCREEN_BUFFER_INFOEX));

		ConsoleScreenBufferInfoEx.cbSize = sizeof(ConsoleScreenBufferInfoEx);

		GetConsoleScreenBufferInfoEx(this->hStdOut, &ConsoleScreenBufferInfoEx);

		ConsoleScreenBufferInfoEx.bFullscreenSupported = false;
		ConsoleScreenBufferInfoEx.dwCursorPosition.X = 0;
		ConsoleScreenBufferInfoEx.dwCursorPosition.Y = 0;
		ConsoleScreenBufferInfoEx.srWindow.Right++;
		ConsoleScreenBufferInfoEx.srWindow.Bottom++;
		ConsoleScreenBufferInfoEx.dwSize.X = ConsoleScreenBufferInfoEx.srWindow.Right;
		ConsoleScreenBufferInfoEx.dwSize.Y = ConsoleScreenBufferInfoEx.srWindow.Bottom;

		SetConsoleScreenBufferInfoEx(this->hStdOut, &ConsoleScreenBufferInfoEx);
	}
	int CountReturns(string input)
	{
		int count = 0;
		for (int i = 0; i < (int)input.size(); i++)
			if (input[i] == '\n')
				count++;
		return count;
	}
	int GetLongestLineLength(string input)
	{
		int count = 0, length = 0;
		for (int i = 0; i < (int)input.size(); i++)
			if (input[i] == '\n')
			{
				if (count > length)
					length = count;
				count = 0;
			}
			else if (input[i] == '\t')
			{
				int tabLocation = count / 8;
				if (count % 8 != 0)
					tabLocation++;

				count += ((tabLocation * 8) - count);
			}
			else if (i == input.size() - 1)
			{
				count++;
				if (count > length)
					length = count;
				count = 0;
			}
			else
				count++;
		return length;
	}
	string ReplaceReturns(string input, int lineLength)
	{
		int count = 0;
		for (int i = 0; i < (int)input.size(); i++)
			if (input[i] == '\n')
			{
				input.erase(i, 1);

				stringstream ss;

				int j;
				for (j = 0; j < (lineLength - count); j++)
					ss << " ";

				input.insert(i, ss.str());

				i += (j - 1);

				count = 0;
			}
			else if (input[i] == '\t')
			{
				input.erase(i, 1);

				int tabLocation = count / 8;
				if (count % 8 != 0)
					tabLocation++;

				stringstream ss;

				int j;
				for (j = 0; j < ((tabLocation * 8) - count); j++)
					ss << " ";

				input.insert(i, ss.str());

				i += (j - 1);

				count += j;
			}
			else
				count++;
		return input;
	}
};

#endif