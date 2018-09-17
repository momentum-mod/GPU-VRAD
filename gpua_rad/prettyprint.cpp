#include "prettyprint.h"

namespace pp
{
	size_t color_printf(eConsoleColor color, const char * format, ...)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO info;
		WORD new_colors;
		char* out;
		va_list args;
		size_t ret;

		va_start(args, format);
		ret = _vscprintf(format, args);
		out = new char[ret + 1];
		ret = vsnprintf(out, ret + 1, format, args);
		va_end(args);

		if (GetConsoleScreenBufferInfo(hConsole, &info))
		{
			new_colors = info.wAttributes;
			new_colors &= ~eConsoleColor::ALL;
			new_colors |= color;

			SetConsoleTextAttribute(hConsole, new_colors);
			printf("%s", out);
			SetConsoleTextAttribute(hConsole, info.wAttributes);
		}
		else
		{
			printf("%s", out);
		}
		delete out;
		return ret;
	}

	void print_prefix(eConsoleColor color, const char* prefix)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO info;
		WORD new_colors;

		printf("[");
		if (GetConsoleScreenBufferInfo(hConsole, &info))
		{
			new_colors = info.wAttributes;
			new_colors &= ~eConsoleColor::ALL;
			new_colors |= color;

			SetConsoleTextAttribute(hConsole, new_colors);
			printf("%s", prefix);
			SetConsoleTextAttribute(hConsole, info.wAttributes);
		}
		else
		{
			printf("%s", prefix);
		}
		printf("] ");
	}

	size_t Success(const char * format, ...)
	{
		char* out;
		va_list args;
		size_t ret;

		va_start(args, format);
		ret = _vscprintf(format, args);
		out = new char[ret + 1];
		ret = vsnprintf(out, ret + 1, format, args);
		va_end(args);

		print_prefix(eConsoleColor::GREEN, "+");

		printf("%s\n", out);
		delete out;
		return ret;
	}

	size_t Info(const char * format, ...)
	{
		char* out;
		va_list args;
		size_t ret;

		va_start(args, format);
		ret = _vscprintf(format, args);
		out = new char[ret + 1];
		ret = vsnprintf(out, ret + 1, format, args);
		va_end(args);

		print_prefix(eConsoleColor::BLUE, "*");

		printf("%s\n", out);
		delete out;
		return ret;
	}

	size_t Warning(const char * format, ...)
	{
		char* out;
		va_list args;
		size_t ret;

		va_start(args, format);
		ret = _vscprintf(format, args);
		out = new char[ret + 1];
		ret = vsnprintf(out, ret + 1, format, args);
		va_end(args);

		print_prefix(eConsoleColor::YELLOW, "!");

		printf("%s\n", out);
		delete out;
		return ret;
	}

	size_t Error(const char * format, ...)
	{
		char* out;
		va_list args;
		size_t ret;

		va_start(args, format);
		ret = _vscprintf(format, args);
		out = new char[ret + 1];
		ret = vsnprintf(out, ret + 1, format, args);
		va_end(args);

		print_prefix(eConsoleColor::RED, "-");

		printf("%s\n", out);
		delete out;

		getchar();
		exit(1);

		return ret;
	}

	size_t Fatal(const char * format, ...)
	{
		char* out;
		va_list args;
		size_t ret;

		va_start(args, format);
		ret = _vscprintf(format, args);
		out = new char[ret + 1];
		ret = vsnprintf(out, ret + 1, format, args);
		va_end(args);

		print_prefix(eConsoleColor::RED, "FATAL");

		printf("%s\n", out);
		delete out;
		return ret;
	}

	size_t Hexdump(const unsigned char * base, size_t len)
	{
		size_t i, j, h;
		for (i = 0; i < (len / 0x10) + 1; i++)
		{
			for (j = 0; j < 0x10; j++)
			{
				h = (i * 0x10) + j;

				if (h >= len)
				{
					break;
				}

				if (h % 16 == 0)
				{
					if (h % 0x80 == 0)
					{
						if (i != 0)
						{
							printf("\n");
						}

						color_printf(eConsoleColor::DARKMAGENTA, "%- 6X", h);
						printf(" -> ");
					}
					else
					{
						printf("\n%- 6X -> ", (unsigned int)h);
					}
				}
				else
				{
#ifndef _WIN64
					if (h % 4 == 0)
					{
						printf(" ");
					}
#else
					if (h % 8 == 0)
					{
						printf(" ");
					}
#endif
				}

				if (base[h] >= 0x0 && base[h] <= 0x1F) // 0x0 - 0x1F
					color_printf(eConsoleColor::DARKRED, "%02X ", base[h]);
				else if (base[h] >= 0x20 && base[h] <= 0x7E) // 0x20 - 0x7E
					color_printf(eConsoleColor::WHITE, "%02X ", base[h]);
				else if (base[h] >= 0x7F && base[h] <= 0xFF) // 0x20 - 0x7E
					color_printf(eConsoleColor::GRAY, "%02X ", base[h]);
			}
			printf("  ");
			for (j = 0; j < 0x10; j++)
			{
				h = (i * 0x10) + j;

				if (h >= len)
				{
					break;
				}

				if (base[h] >= 0x0 && base[h] <= 0x1F) // 0x0 - 0x1F
					color_printf(eConsoleColor::DARKRED, ".");
				else if (base[h] >= 0x20 && base[h] <= 0x7E) // 0x20 - 0x7E
				{
					switch (base[h])
					{
					case 0x25: // Weird stuff with this char
						color_printf(eConsoleColor::WHITE, "%%%%");
						break;
					default:
						color_printf(eConsoleColor::WHITE, "%c", base[h]);
						break;
					}
				}
				else if (base[h] >= 0x7F && base[h] <= 0xFF) // 0x20 - 0x7E
					color_printf(eConsoleColor::GRAY, ".");
			}
		}
		printf("\n");
		return i;
	}

	bool GetColor(short &ret)
	{
		CONSOLE_SCREEN_BUFFER_INFO info;
		if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
			return false;
		ret = info.wAttributes;
		return true;
	}
}