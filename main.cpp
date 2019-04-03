// Code is very bad. Soon i will rewrite it.

//#include "pch.h" /* Fucking VS */
#define _CRT_SECURE_NO_WARNINGS
#include <initializer_list>
unsigned char start = 0;

template<typename A, size_t size>
class Array // baad idea
{
	A* arr = nullptr;
public:
	Array(A* arr)
	{
		this->arr = arr;
	}
	Array(std::initializer_list<A> list)
	{
		arr = new A[size];
		for (int i = 0; i < size; ++i)
			arr[i] = list.begin()[i];
	}
	Array(void)
	{
		arr = new A[size];
	}

	operator A*(void)
	{
		return arr;
	}

	A* begin(void)
	{
		return arr;
	}

	A* end(void)
	{
		return arr + (size - 1);
	}
};


#define DEBUG
#include <thread>
#include <fstream>
#include <SDL.h>
#include <iostream>
#include <atomic>
using namespace std;

#define trace(x) cout << #x << ": " << x << endl
#include "memory.h"
#include "exec.h"




void dump(void)
{
	cout << "Состояние консоли:\n";
	cout << "Памяти использовано: ";

	unsigned long long bytesInUse = 0;
	for (int i = 0; i < ramSize; ++i)
		if (ram[i] != 0)
			++bytesInUse;

	cout << bytesInUse << " байт\n";

	cout << "Размер программы: ";
	size_t programSize = 0;
	for (const string& c : program.data)
		programSize += c.size();
	cout << programSize << " байт(" << program.data.size() << " инструкций)\n";

	

}

int main(int args, char* argc[])
{
	uploadPallete(pallete);
	initializeVideo();



	try
	{
		for (register int i = 0; i < ramSize; ++i)
			ram[i] ^= ram[i];

		setlocale(LC_ALL, "rus");

		for (; program.data[pcRegister] != "halt"; ++pcRegister)
		{
			char cmd[32];
			sscanf(program.data[pcRegister].c_str(), "%s", cmd);
      for(register unsigned char i = 0; cmd[i] != '\0'; ++i)
        cmd[i] = tolower(cmd[i]);
			commands[cmd](program.data[pcRegister]);
			SDL_Event e;
			SDL_PollEvent(&e);
		}
		display("");

		dump();

		waitExit();

	}
	catch (exception& exc)
	{
		cerr << "Стандартная ошибка: " << exc.what() << endl;
		return -2;
	}
	catch (...)
	{
		cerr << "Неизвестная ошибка!" << endl;
		return -1;
	}
	return 0;
}
