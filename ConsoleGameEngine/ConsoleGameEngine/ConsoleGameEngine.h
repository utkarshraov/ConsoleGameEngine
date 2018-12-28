#pragma once
#pragma comment(lib, "winmm.lib")

#ifndef UNICODE
#error This project needs unicode, sorry! 
#endif

#include <windows.h>

#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>

enum COLOUR
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007,
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_DARK_BLUE = 0x0010,
	BG_DARK_GREEN = 0x0020,
	BG_DARK_CYAN = 0x0030,
	BG_DARK_RED = 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW = 0x0060,
	BG_GREY = 0x0070,
	BG_DARK_GREY = 0x0080,
	BG_BLUE = 0x0090,
	BG_GREEN = 0x00A0,
	BG_CYAN = 0x00B0,
	BG_RED = 0x00C0,
	BG_MAGENTA = 0x00D0,
	BG_YELLOW = 0x00E0,
	BG_WHITE = 0x00F0,
};

enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};

class Sprite
{
	//A class to create and store Sprites (as groups of pixels)
public:
	Sprite()
	{

	}

	Sprite(int w, int h)
	{
		Create(w, h);
	}

	Sprite(std::wstring sFile)
	{
		if (!Load(sFile))
			Create(8, 8);
	}

	int width = 0;
	int height = 0;

private:
	short *glyphs = nullptr;
	short *colours = nullptr;

	void Create(int w, int h)
	{
		width = w;
		height = h;
		glyphs = new short[w*h];
		colours = new short[w*h];
		for (int i = 0; i < w*h; i++)
		{
			glyphs[i] = L' ';
			colours[i] = FG_BLACK;
		}
	}

public:
	void SetGlyph(int x, int y, short c)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return;
		else
			glyphs[y * width + x] = c;
	}

	void SetColour(int x, int y, short c)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return;
		else
			colours[y * width + x] = c;
	}

	short GetGlyph(int x, int y)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return L' ';
		else
			return glyphs[y * width + x];
	}

	short GetColour(int x, int y)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return FG_BLACK;
		else
			return colours[y * width + x];
	}

	short SampleGlyph(float x, float y)
	{
		int sx = (int)(x * (float)width);
		int sy = (int)(y * (float)height - 1.0f);
		if (sx < 0 || sx >= width || sy < 0 || sy >= height)
			return L' ';
		else
			return glyphs[sy * width + sx];
	}

	short SampleColour(float x, float y)
	{
		int sx = (int)(x * (float)width);
		int sy = (int)(y * (float)height - 1.0f);
		if (sx < 0 || sx >= width || sy < 0 || sy >= height)
			return FG_BLACK;
		else
			return colours[sy * width + sx];
	}

	bool Save(std::wstring sFile)
	{
		FILE *f = nullptr;
		_wfopen_s(&f, sFile.c_str(), L"wb");
		if (f == nullptr)
			return false;

		fwrite(&width, sizeof(int), 1, f);
		fwrite(&height, sizeof(int), 1, f);
		fwrite(colours, sizeof(short), width * height, f);
		fwrite(glyphs, sizeof(short), width * height, f);

		fclose(f);

		return true;
	}

	bool Load(std::wstring sFile)
	{
		delete[] glyphs;
		delete[] colours;
		width = 0;
		height = 0;

		FILE *f = nullptr;
		_wfopen_s(&f, sFile.c_str(), L"rb");
		if (f == nullptr)
			return false;

		std::fread(&width, sizeof(int), 1, f);
		std::fread(&height, sizeof(int), 1, f);

		Create(width, height);

		std::fread(colours, sizeof(short), width * height, f);
		std::fread(glyphs, sizeof(short), width * height, f);

		std::fclose(f);
		return true;
	}
};
class ConsoleGameEngine
{
public:
	ConsoleGameEngine()
	{
		windowWidth = 80;
		windowHeight = 30;

		console = GetStdHandle(STD_OUTPUT_HANDLE);
		consoleIn = GetStdHandle(STD_INPUT_HANDLE);

		std::memset(newKeyState, 0, 256 * sizeof(short));
		std::memset(oldKeyState, 0, 256 * sizeof(short));
		std::memset(keys, 0, 256 * sizeof(KeyState));
		mousePosX = 0;
		mousePosY = 0;

		//m_bEnableSound = false;

		appName = L"Default";
	}

	void EnableSound()
	{
		//m_bEnableSound = true;
	}

	int ConstructConsole(int width, int height, int fontw, int fonth)
	{
		if (console == INVALID_HANDLE_VALUE)
			return Error(L"Bad Handle");

		windowWidth = width;
		windowHeight = height;

		// Change console visual size to a minimum so ScreenBuffer can shrink
		// below the actual visual size
		rectWindow = { 0, 0, 1, 1 };
		SetConsoleWindowInfo(console, TRUE, &rectWindow);

		// Set the size of the screen buffer
		COORD coord = { (short)windowWidth, (short)windowHeight };
		if (!SetConsoleScreenBufferSize(console, coord))
			Error(L"SetConsoleScreenBufferSize");

		// Assign screen buffer to the console
		if (!SetConsoleActiveScreenBuffer(console))
			return Error(L"SetConsoleActiveScreenBuffer");

		// Set the font size now that the screen buffer has been assigned to the console
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = fontw;
		cfi.dwFontSize.Y = fonth;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;

		wcscpy_s(cfi.FaceName, L"Consolas");
		if (!SetCurrentConsoleFontEx(console, false, &cfi))
			return Error(L"SetCurrentConsoleFontEx");

		// Get screen buffer info and check the maximum allowed window size. Return
		// error if exceeded, so user knows their dimensions/fontsize are too large
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(console, &csbi))
			return Error(L"GetConsoleScreenBufferInfo");
		if (windowHeight > csbi.dwMaximumWindowSize.Y)
			return Error(L"Screen Height / Font Height Too Big");
		if (windowWidth > csbi.dwMaximumWindowSize.X)
			return Error(L"Screen Width / Font Width Too Big");

		// Set Physical Console Window Size
		rectWindow = { 0, 0, (short)windowWidth - 1, (short)windowHeight - 1 };
		if (!SetConsoleWindowInfo(console, TRUE, &rectWindow))
			return Error(L"SetConsoleWindowInfo");

		// Set flags to allow mouse input		
		if (!SetConsoleMode(consoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
			return Error(L"SetConsoleMode");

		// Allocate memory for screen buffer
		m_bufScreen = new CHAR_INFO[windowWidth*windowHeight];
		memset(m_bufScreen, 0, sizeof(CHAR_INFO) * windowWidth * windowHeight);

		SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandler, TRUE);
		return 1;
	}
	//I found and modified a bunch of drawing algorithms online and implemented them here

	virtual void Draw(int x, int y, short c = 0x2588, short col = 0x000F)
	{
		if (x >= 0 && x < windowWidth && y >= 0 && y < windowHeight)
		{
			m_bufScreen[y * windowWidth + x].Char.UnicodeChar = c;
			m_bufScreen[y * windowWidth + x].Attributes = col;
		}
	}

	void Fill(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F)
	{
		Clip(x1, y1);
		Clip(x2, y2);
		for (int x = x1; x < x2; x++)
			for (int y = y1; y < y2; y++)
				Draw(x, y, c, col);
	}

	void DrawString(int x, int y, std::wstring c, short col = 0x000F)
	{
		for (size_t i = 0; i < c.size(); i++)
		{
			m_bufScreen[y * windowWidth + x + i].Char.UnicodeChar = c[i];
			m_bufScreen[y * windowWidth + x + i].Attributes = col;
		}
	}

	void DrawStringAlpha(int x, int y, std::wstring c, short col = 0x000F)
	{
		for (size_t i = 0; i < c.size(); i++)
		{
			if (c[i] != L' ')
			{
				m_bufScreen[y * windowWidth + x + i].Char.UnicodeChar = c[i];
				m_bufScreen[y * windowWidth + x + i].Attributes = col;
			}
		}
	}

	void Clip(int &x, int &y)
	{
		if (x < 0) x = 0;
		if (x >= windowWidth) x = windowWidth;
		if (y < 0) y = 0;
		if (y >= windowHeight) y = windowHeight;
	}

	void DrawLine(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F)
	{
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1; dy = y2 - y1;
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1; y = y1; xe = x2;
			}
			else
			{
				x = x2; y = y2; xe = x1;
			}

			Draw(x, y, c, col);

			for (i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
					px = px + 2 * dy1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				Draw(x, y, c, col);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1; y = y1; ye = y2;
			}
			else
			{
				x = x2; y = y2; ye = y1;
			}

			Draw(x, y, c, col);

			for (i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				Draw(x, y, c, col);
			}
		}
	}

	void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F)
	{
		DrawLine(x1, y1, x2, y2, c, col);
		DrawLine(x2, y2, x3, y3, c, col);
		DrawLine(x3, y3, x1, y1, c, col);
	}

	// Found on the internet and slightly modified
	void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F)
	{
		auto SWAP = [](int &x, int &y) { int t = x; x = y; y = t; };
		auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) Draw(i, ny, c, col); };

		int t1x, t2x, y, minx, maxx, t1xp, t2xp;
		bool changed1 = false;
		bool changed2 = false;
		int signx1, signx2, dx1, dy1, dx2, dy2;
		int e1, e2;
		// Sort vertices
		if (y1 > y2) { SWAP(y1, y2); SWAP(x1, x2); }
		if (y1 > y3) { SWAP(y1, y3); SWAP(x1, x3); }
		if (y2 > y3) { SWAP(y2, y3); SWAP(x2, x3); }

		t1x = t2x = x1; y = y1;   // Starting points
		dx1 = (int)(x2 - x1); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(y2 - y1);

		dx2 = (int)(x3 - x1); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
		else signx2 = 1;
		dy2 = (int)(y3 - y1);

		if (dy1 > dx1) {   // swap values
			SWAP(dx1, dy1);
			changed1 = true;
		}
		if (dy2 > dx2) {   // swap values
			SWAP(dy2, dx2);
			changed2 = true;
		}

		e2 = (int)(dx2 >> 1);
		// Flat top, just process the second half
		if (y1 == y2) goto next;
		e1 = (int)(dx1 >> 1);

		for (int i = 0; i < dx1;) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				i++;
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) t1xp = signx1;//t1x += signx1;
					else          goto next1;
				}
				if (changed1) break;
				else t1x += signx1;
			}
			// Move line
		next1:
			// process second line until y value is about to change
			while (1) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;//t2x += signx2;
					else          goto next2;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next2:
			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);    // Draw line from min to max points found on the y
										 // Now increase y
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y == y2) break;

		}
	next:
		// Second half
		dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(y3 - y2);
		t1x = x2;

		if (dy1 > dx1) {   // swap values
			SWAP(dy1, dx1);
			changed1 = true;
		}
		else changed1 = false;

		e1 = (int)(dx1 >> 1);

		for (int i = 0; i <= dx1; i++) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) { t1xp = signx1; break; }//t1x += signx1;
					else          goto next3;
				}
				if (changed1) break;
				else   	   	  t1x += signx1;
				if (i < dx1) i++;
			}
		next3:
			// process second line until y value is about to change
			while (t2x != x3) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;
					else          goto next4;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next4:

			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y > y3) return;
		}
	}

	void DrawCircle(int xc, int yc, int r, short c = 0x2588, short col = 0x000F)
	{
		int x = 0;
		int y = r;
		int p = 3 - 2 * r;
		if (!r) return;

		while (y >= x) // only formulate 1/8 of circle
		{
			Draw(xc - x, yc - y, c, col);//upper left left
			Draw(xc - y, yc - x, c, col);//upper upper left
			Draw(xc + y, yc - x, c, col);//upper upper right
			Draw(xc + x, yc - y, c, col);//upper right right
			Draw(xc - x, yc + y, c, col);//lower left left
			Draw(xc - y, yc + x, c, col);//lower lower left
			Draw(xc + y, yc + x, c, col);//lower lower right
			Draw(xc + x, yc + y, c, col);//lower right right
			if (p < 0) p += 4 * x++ + 6;
			else p += 4 * (x++ - y--) + 10;
		}
	}

	void FillCircle(int xc, int yc, int r, short c = 0x2588, short col = 0x000F)
	{
		// Taken from wikipedia
		int x = 0;
		int y = r;
		int p = 3 - 2 * r;
		if (!r) return;

		auto drawline = [&](int sx, int ex, int ny)
		{
			for (int i = sx; i <= ex; i++)
				Draw(i, ny, c, col);
		};

		while (y >= x)
		{
			// Modified to draw scan-lines instead of edges
			drawline(xc - x, xc + x, yc - y);
			drawline(xc - y, xc + y, yc - x);
			drawline(xc - x, xc + x, yc + y);
			drawline(xc - y, xc + y, yc + x);
			if (p < 0) p += 4 * x++ + 6;
			else p += 4 * (x++ - y--) + 10;
		}
	};

	void DrawSprite(int x, int y, Sprite *sprite)
	{
		if (sprite == nullptr)
			return;

		for (int i = 0; i < sprite->width; i++)
		{
			for (int j = 0; j < sprite->height; j++)
			{
				if (sprite->GetGlyph(i, j) != L' ')
					Draw(x + i, y + j, sprite->GetGlyph(i, j), sprite->GetColour(i, j));
			}
		}
	}

	void DrawPartialSprite(int x, int y, Sprite *sprite, int ox, int oy, int w, int h)
	{
		if (sprite == nullptr)
			return;

		for (int i = 0; i < w; i++)
		{
			for (int j = 0; j < h; j++)
			{
				if (sprite->GetGlyph(i + ox, j + oy) != L' ')
					Draw(x + i, y + j, sprite->GetGlyph(i + ox, j + oy), sprite->GetColour(i + ox, j + oy));
			}
		}
	}

	void DrawWireFrameModel(const std::vector<std::pair<float, float>> &vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short col = FG_WHITE, short c = PIXEL_SOLID)
	{
		// pair.first = x coordinate
		// pair.second = y coordinate

		// Create translated model vector of coordinate pairs
		std::vector<std::pair<float, float>> vecTransformedCoordinates;
		int verts = vecModelCoordinates.size();
		vecTransformedCoordinates.resize(verts);

		// Rotate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
			vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
		}

		// Scale
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
		}

		// Translate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
		}

		// Draw Closed Polygon
		for (int i = 0; i < verts + 1; i++)
		{
			int j = (i + 1);
			DrawLine((int)vecTransformedCoordinates[i % verts].first, (int)vecTransformedCoordinates[i % verts].second,
				(int)vecTransformedCoordinates[j % verts].first, (int)vecTransformedCoordinates[j % verts].second, c, col);
		}
	}

	~ConsoleGameEngine()
	{
		SetConsoleActiveScreenBuffer(m_hOriginalConsole);
		delete[] m_bufScreen;
	}

public:
	void StartThread()
	{
		// Start the thread
		atomActive = true;
		std::thread t = std::thread(&ConsoleGameEngine::GameThread, this);

		// Wait for thread to be exited
		t.join();
	}

	int ScreenWidth()
	{
		return windowWidth;
	}

	int ScreenHeight()
	{
		return windowHeight;
	}

private:
	void GameThread()
	{
		// Create user resources as part of this thread
		if (!Start())
			atomActive = false;

		//// Check if sound system should be enabled
		//if (m_bEnableSound)
		//{
		//	if (!CreateAudio())
		//	{
		//		atomActive = false; // Failed to create audio system			
		//		m_bEnableSound = false;
		//	}
		//}

		auto tp1 = std::chrono::system_clock::now();
		auto tp2 = std::chrono::system_clock::now();

		while (atomActive)
		{
			// Run as fast as possible
			while (atomActive)
			{
				// Handle Timing
				tp2 = std::chrono::system_clock::now();
				std::chrono::duration<float> elapsedTime = tp2 - tp1;
				tp1 = tp2;
				float fElapsedTime = elapsedTime.count();

				// Handle Keyboard Input
				for (int i = 0; i < 256; i++)
				{
					newKeyState[i] = GetAsyncKeyState(i);

					keys[i].onPressed = false;
					keys[i].onReleased = false;

					if (newKeyState[i] != oldKeyState[i])
					{
						if (newKeyState[i] & 0x8000)
						{
							keys[i].onPressed = !keys[i].onHeld;
							keys[i].onHeld = true;
						}
						else
						{
							keys[i].onReleased = true;
							keys[i].onHeld = false;
						}
					}

					oldKeyState[i] = newKeyState[i];
				}

				// Handle Mouse Input - Check for window events
				INPUT_RECORD inBuf[32];
				DWORD events = 0;
				GetNumberOfConsoleInputEvents(consoleIn, &events);
				if (events > 0)
					ReadConsoleInput(consoleIn, inBuf, events, &events);

				// Handle events - we only care about mouse clicks and movement

				for (DWORD i = 0; i < events; i++)
				{
					switch (inBuf[i].EventType)
					{
					case FOCUS_EVENT:
					{
						m_bConsoleInFocus = inBuf[i].Event.FocusEvent.bSetFocus;
					}
					break;

					case MOUSE_EVENT:
					{
						switch (inBuf[i].Event.MouseEvent.dwEventFlags)
						{
						case MOUSE_MOVED:
						{
							mousePosX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
							mousePosY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
						}
						break;

						case 0:
						{
							for (int m = 0; m < 5; m++)
								mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;

						}
						break;

						default:
							break;
						}
					}
					break;

					default:
						break;

					}
				}

				for (int m = 0; m < 5; m++)
				{
					mouse[m].onPressed = false;
					mouse[m].onReleased = false;

					if (mouseNewState[m] != mouseOldState[m])
					{
						if (mouseNewState[m])
						{
							mouse[m].onPressed = true;
							mouse[m].onHeld = true;
						}
						else
						{
							mouse[m].onReleased = true;
							mouse[m].onHeld = false;
						}
					}

					mouseOldState[m] = mouseNewState[m];
				}


				// Handle Frame Update
				if (!Update(fElapsedTime))
					atomActive = false;

				// Update Title & Present Screen Buffer
				wchar_t s[256];
				swprintf_s(s, 256, L"%s - FPS: %3.2f", appName.c_str(), 1.0f / fElapsedTime);
				SetConsoleTitle(s);
				WriteConsoleOutput(console, m_bufScreen, { (short)windowWidth, (short)windowHeight }, { 0,0 }, &rectWindow);
			}

			//if (m_bEnableSound)
			//{
			//	// Close and Clean up audio system
			//}

			// Allow the user to free resources if they have overrided the destroy function
			if (OnUserDestroy())
			{
				// User has permitted destroy, so exit and clean up
				delete[] m_bufScreen;
				SetConsoleActiveScreenBuffer(m_hOriginalConsole);
				gameFinished.notify_one();
			}
			else
			{
				// User denied destroy for some reason, so continue running
				atomActive = true;
			}
		}
	}

public:
	// Start and update! 
	virtual bool Start() = 0;
	virtual bool Update(float fElapsedTime) = 0;

	// Optional for clean up 
	virtual bool OnUserDestroy() { return true; }


protected:


	struct KeyState
	{
		bool onPressed;
		bool onReleased;
		bool onHeld;
	} keys[256], mouse[5];

	int mousePosX;
	int mousePosY;

public:
	KeyState GetKey(int nKeyID) { return keys[nKeyID]; }
	int GetMouseX() { return mousePosX; }
	int GetMouseY() { return mousePosY; }
	KeyState GetMouse(int nMouseButtonID) { return mouse[nMouseButtonID]; }
	bool IsFocused() { return m_bConsoleInFocus; }


protected:
	int Error(const wchar_t *msg)
	{
		wchar_t buf[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
		SetConsoleActiveScreenBuffer(m_hOriginalConsole);
		wprintf(L"ERROR: %s\n\t%s\n", msg, buf);
		return 0;
	}

	static BOOL CloseHandler(DWORD evt)
	{
		// Note this gets called in a seperate OS thread, so it must
		// only exit when the game has finished cleaning up, or else
		// the process will be killed before OnUserDestroy() has finished
		if (evt == CTRL_CLOSE_EVENT)
		{
			atomActive = false;

			// Wait for thread to be exited
			std::unique_lock<std::mutex> ul(muxGame);
			gameFinished.wait(ul);
		}
		return true;
	}

protected:
	int windowWidth;
	int windowHeight;
	CHAR_INFO *m_bufScreen;
	std::wstring appName;
	HANDLE m_hOriginalConsole;
	CONSOLE_SCREEN_BUFFER_INFO m_OriginalConsoleInfo;
	HANDLE console;
	HANDLE consoleIn;
	SMALL_RECT rectWindow;
	short oldKeyState[256] = { 0 };
	short newKeyState[256] = { 0 };
	bool mouseOldState[5] = { 0 };
	bool mouseNewState[5] = { 0 };
	bool m_bConsoleInFocus = true;

	// These need to be static because of the OnDestroy call the OS may make. The OS
	// spawns a special thread just for that
	static std::atomic<bool> atomActive;
	static std::condition_variable gameFinished;
	static std::mutex muxGame;
};
// Define our static variables
std::atomic<bool> ConsoleGameEngine::atomActive(false);
std::condition_variable ConsoleGameEngine::gameFinished;
std::mutex ConsoleGameEngine::muxGame;