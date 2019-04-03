#ifndef VIDEO_H
#define VIDEO_H
#include "memory.h"
#include <SDL.h>

const size_t videoRamSize = 98304;
const size_t videoRamSizeH = 384;
const size_t videoRamSizeY = 256;

void validateCoordinate(int& cord, bool a = true)
{
	if ((size_t)cord >= (a ? videoRamSizeH : videoRamSizeY))
		cord = (a ? videoRamSizeH-1 : videoRamSizeY-1)-1;
	else if (cord < 0)
		cord = 1;
}
void validateCoordinate(int& cord, size_t cordValue,  bool a)
{
	if (cordValue >= (a ? videoRamSizeH : videoRamSizeY))
		cord = (a ? videoRamSizeH : videoRamSizeY)-1;
}


inline unsigned long long xyToAddress(int x, int y)
{
	return ((unsigned long long)videoRamSizeH * y) + x;
}

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

void initializeVideo(void)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Fantasy Console", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, videoRamSizeH * 3, videoRamSizeY * 3, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void printPoint(int x, int y)
{
	int dX, dY;
	SDL_GetWindowSize(window, &dX, &dY);
	int outX = dX / (int)videoRamSizeH;
	int outY = dY / (int)videoRamSizeY;

	SDL_Rect rect = { x*outX, y*outY, outX, outY };
	SDL_RenderFillRect(renderer, &rect);
}

SDL_Color pallete[] =
{
	{71, 45, 60},
	{94, 54, 67},
	{122, 68, 74},
	{160, 91, 83},
	{191, 121, 88},
	{238, 161, 96},
	{244, 204, 161},
	{182, 213, 60},
	{113, 170, 52},
	{57, 123, 68},
	{60, 89, 86},
	{48, 44, 46},
	{90, 83, 83},
	{125, 112, 113},
	{160, 147, 142},
	{207, 198, 184},
	{223, 246, 245},
	{138, 235, 241},
	{40, 204, 223},
	{57, 120, 168},
	{57, 71, 120},
	{57, 49, 75},
	{86, 64, 100},
	{142, 71, 140},
	{205, 96, 147},
	{255, 174, 182},
	{244, 180, 27},
	{244, 126, 27},
	{230, 72, 46},
	{169, 59, 59},
	{130, 112, 148},
	{79, 84, 107},
};

const size_t palleteRamSize = 96;

void uploadPallete(SDL_Color pallete[32])
{
	size_t startAddr = videoRamSize + 1;
	for (int i = 0, o = 0; i < 32 * 3; i += 3, ++o)
	{
		ram[startAddr + i] = pallete[o].r;
		ram[startAddr + i + 1] = pallete[o].g;
		ram[startAddr + i + 2] = pallete[o].b;
	}
}
void display(const string& str)
{
	(void)str;
		for (int x = 0; x < videoRamSizeH; ++x)
			for (int y = 0; y < videoRamSizeY; ++y)
			{
				SDL_SetRenderDrawColor(renderer, pallete[ram[xyToAddress(x, y)]].r, pallete[ram[xyToAddress(x, y)]].g, pallete[ram[xyToAddress(x, y)]].b, 0);
				printPoint(x, y);
			}
	SDL_RenderPresent(renderer);
}

typedef byte Sprite;

#define SPRITE [12][12] 
#define SPRITE_BY_PTR ** 

Array<Array<Array<Sprite, 12>, 12>, 32> sprites;

void displaySprite(bool a, size_t sprite, int x, int y, int chromaKey)
{
	(void)a;
	for (int xq = 0; xq < 12; ++xq)
		for (int yq = 0; yq < 12; ++yq)
			if (sprites[sprite][xq][yq] != chromaKey)
			ram[xyToAddress(x + yq, y + xq)] = sprites[sprite][xq][yq];

}

void displaySprite(Sprite SPRITE_BY_PTR sprite, int x, int y, int chromaKey)
{
	for (int xq = 0; xq < 12; ++xq)
		for (int yq = 0; yq < 12; ++yq)
			if (sprite[xq] > &start)
				if(sprite[xq][yq] != chromaKey)
				ram[xyToAddress(x + yq, y + xq)] = sprite[xq][yq];

}


void displaySprite(Array<Array<Sprite, 12>, 12> sprite, int x, int y, int chromaKey)
{
	for (int xq = 0; xq < 12; ++xq)
		for (int yq = 0; yq < 12; ++yq)
			if (sprite[xq] > &start)
				if (sprite[xq][yq] != chromaKey)
					ram[xyToAddress(x + yq, y + xq)] = sprite[xq][yq];

}

/////////////////////////////////////////////////////

int ctoi(char c)
{
	char chars[2] = { c, 0 };
	int toReturn = 0;
	sscanf(chars, "%X", &toReturn);
	return toReturn;
}

string spriteToString(Array<Array<Sprite, 12>, 12> sprite)
{
	string str;
	stringstream bufer;

	for (int i = 0; i < 12; ++i)
		for (int ii = 0; ii < 12; ++ii)
		{
			char newS[20];
			int right = sprite[i][ii] & 15;
			int left = sprite[i][ii] > 4;

			sprintf(newS, "%X%X", (int)left, (int)right);
			bufer << newS;
		}
	bufer >> hex >> str;
	return str;
}
Array<Array<Sprite, 12>, 12> stringToSprite(const string& str)
{
	stringstream bufer;
	bufer << str;

	Array<Array<Sprite, 12>, 12> toReturn;
	for (int i = 0; i < 12; ++i)
		for (int ii = 0; ii < 12; ++ii)
		{
			string nstr;
			nstr.push_back(bufer.get());
			nstr.push_back(bufer.get());
			sscanf(nstr.c_str(), "%X", (int*)&(toReturn[i][ii]));
		}
	return toReturn;
}


void waitExit(void)
{
	bool isExit = false;
	SDL_Event event;
	while(!isExit)
		while (SDL_PollEvent(&event))
			switch (event.type)
			{
			case SDL_QUIT:
				isExit = true;
				break;

			default:
				break;
			}
}
#endif
