#ifndef EXEC_H
#define EXEC_H
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include "memory.h"
#include "video.h"

using namespace std;

enum AnyVarType
{
	INT,
	FLOAT,
	STRING,
	NIL
};

class Var
{
public:

	size_t itExist = 1;
	AnyVarType varType = NIL;

	int val = 0;
	double dVal = 0;
	string str;

	Var()
		: varType{ NIL } {}

	explicit Var(int value)
		: varType{ INT }, val{ value } {}

	explicit Var(double value)
		: varType{ FLOAT }, dVal{ value } {}

	Var(const string& string)
		: varType{ STRING }, str{ string } {}

	bool operator==(int value)
	{
		return val == value;
	}
	bool operator==(double value)
	{
		return fabs(dVal - value) < 0.00001;
	}
	bool operator==(const string& value)
	{
		return str == value;
	}

	Var& operator=(int value)
	{
		if (itExist != 1)
			throw exception("Не найдена переменная");
		val = value;
		varType = INT;

		return *this;
	}

	Var& operator=(double value)
	{
		if (itExist != 1)
			throw exception("Не найдена переменная");
		dVal = value;
		varType = FLOAT;

		return *this;
	}

	Var& operator=(const string& value)
	{
		if (itExist != 1)
			throw exception("Не найдена переменная");
		str = move(value);
		varType = STRING;

		return *this;
	}

	Var& operator=(nullptr_t value)
	{
		if (itExist != 1)
			throw exception("Не найдена переменная");
		(void)value;
		varType = NIL;

		return *this;
	}

};

char nothing[1024];
unsigned long long pcRegister = 0;
int zeroFlag = 0;

vector<string> programData = // Home of the program
{
	"call setsprite 0 "
	"000000000000000000000000000000001010100000000000000000100000001"
	"000000000000000100000001000000000000000100000001000000000000000"
	"001010100000000000000010000010000000000000000000101010101000000"
	"000000000000010000010000000000000001000100000000000000000001000"
	"100000000000000000000000000000000000",
	"call setsprite 1 "
	"000000000000000000000000000000001010100000000000000000100000001"
	"000000000000000100000001000000000000000100000001000000000000000"
	"001010100000000000000000000010000010000000000000101010101000000"
	"000000010000010000000000000000000001000100000000000000000001000"
	"100000000000000000000000000000000000",
	"mark start",
	"call clear 0",

	"call displaysprite 0 0 0 0",
	"call displaysprite 1 12 0 0",
	"call display",
	"call delay 500",

	"call clear 0",

	"call displaysprite 1 0 0 0",
	"call displaysprite 0 12 0 0",
	"call display",
	"call delay 500",

	"jmp start",
	"halt",
}; 

map<string, unsigned long long> marks;
map<string, Var> vars;

void line(const string& str)
{
	char x1[32];
	char y1[32];
	char x2[32];
	char y2[32];
	char cl[32];

	int ix1 = 0;
	int iy1 = 0;
	int ix2 = 0;
	int iy2 = 0;
	int clr = 0;

	sscanf(str.c_str(), "%s %s %s %s %s %s %s", nothing, nothing, x1, y1, x2, y2, cl); //-V576


	if (x1[0] == '$')
		ix1 = vars[x1].val;
	else
		ix1 = atoi(x1);

	if (y1[0] == '$')
		iy1 = vars[y1].val;
	else
		iy1 = atoi(y1);

	if (x2[0] == '$')
		ix2 = vars[x2].val;
	else
		ix2 = atoi(x2);

	if (y2[0] == '$')
		iy2 = vars[y2].val;
	else
		iy2 = atoi(y2);

	if (cl[0] == '$')
		clr = vars[cl].val;
	else
		clr = atoi(cl);

	const int deltaX = abs(ix2 - ix1);
	const int deltaY = abs(iy2 - iy1);
	const int signX = ix1 < ix2 ? 1 : -1;
	const int signY = iy1 < iy2 ? 1 : -1;

	int error = deltaX - deltaY;


	ram[xyToAddress(ix2, iy2)] = clr;

	while (ix1 != ix2 || iy1 != iy2)
	{

		ram[xyToAddress(ix1, iy1)] = clr;

		if (error * 2 > -deltaY)
		{
			error -= deltaY;
			ix1 += signX;
		}
		if (error * 2 < deltaX)
		{
			error += deltaX;
			iy1 += signY;
		}
	}
}
void circ(const string& str)
{
	char cx[32];
		char cy[32];
		char cr[32];
		char cc[32];
		int x, y, r, c;

		sscanf(str.c_str(), "%s %s  %s %s %s %s", nothing, nothing, cx, cy, cr, cc); //-V576

		if (cx[0] == '$')
			x = vars[cx].val;
		else
			x = atoi(cx);

		if (cy[0] == '$')
			y = vars[cy].val;
		else
			y = atoi(cy);

		if (cr[0] == '$')
			r = vars[cr].val;
		else
			r = atoi(cr);

		if (cc[0] == '$')
			c = vars[cc].val;
		else
			c = atoi(cc);


		int xq = 0;
		int yq = r;
		int delta = 1 - 2 * r;
		int error = 0;

		validateCoordinate(x, 1);
		validateCoordinate(y, 0);

		while (yq >= 0)
		{
			validateCoordinate(x, x+xq, 1);
			validateCoordinate(y, y+yq, 0);
			validateCoordinate(y, y - yq, 0);

			ram[xyToAddress(x + xq, y + yq)] = c;
			ram[xyToAddress(x + xq, y - yq)] = c;
			ram[xyToAddress(x - xq, y + yq)] = c;
			ram[xyToAddress(x - xq, y - yq)] = c;

			char bufer[50];
			sprintf(bufer, "%s %s %d %d %d %d %d", "call", "line", x + xq, y + yq, x + xq, y - yq, c);
			line(bufer);
			sprintf(bufer, "%s %s %d %d %d %d %d", "call", "line", x - xq, y + yq, x - xq, y - yq, c);
			line(bufer);

			error = 2 * (delta + yq) - 1;
			if ((delta < 0) && (error <= 0))
			{
				delta += 2 * ++xq + 1;
				continue;
			}
			if ((delta > 0) && (error > 0))
			{
				delta -= 2 * --yq + 1;
				continue;
			}

			delta += 2 * (++xq - yq--);
		}
}
void circc(const string& str)
{
	char cx[32];
	char cy[32];
	char cr[32];
	char cc[32];
	int x, y, r, c;

	sscanf(str.c_str(), "%s %s  %s %s %s %s", nothing, nothing, cx, cy, cr, cc); //-V576

	if (cx[0] == '$')
		x = vars[cx].val;
	else
		x = atoi(cx);

	if (cy[0] == '$')
		y = vars[cy].val;
	else
		y = atoi(cy);

	if (cr[0] == '$')
		r = vars[cr].val;
	else
		r = atoi(cr);

	if (cc[0] == '$')
		c = vars[cc].val;
	else
		c = atoi(cc);


	int xq = 0;
	int yq = r;
	int delta = 1 - 2 * r;
	int error = 0;

	while (yq >= 0)
	{
		ram[xyToAddress(x + xq, y + yq)] = c;
		ram[xyToAddress(x + xq, y - yq)] = c;
		ram[xyToAddress(x - xq, y + yq)] = c;
		ram[xyToAddress(x - xq, y - yq)] = c;
		error = 2 * (delta + yq) - 1;
		if ((delta < 0) && (error <= 0))
		{
			delta += 2 * ++xq + 1;
			continue;
		}
		if ((delta > 0) && (error > 0))
		{
			delta -= 2 * --yq + 1;
			continue;
		}

		delta += 2 * (++xq - yq--);
	}
}

void clearScreen(const string& str)
{
	char colour[32];
	int color;

	sscanf(str.c_str(), "%s %s %s", nothing, nothing, colour); //-V576
	if (colour[0] == '$')
		color = vars[colour].val;
	else
		color = atoi(colour);
	for (register int i = 0; i < videoRamSize; ++i)
		ram[i] = color;
}

void getMousePos(const string& str)
{
	char val1[32];
	char val2[32];
	sscanf(str.c_str(), "%s %s %s %s", nothing, nothing, val1, val2); //-V576

	int x, y;
	SDL_GetMouseState(&x, &y);
	vars[val1] = x / 3;
	vars[val2] = y / 3;
}
void wGetMousePos(const string& str)
{
	bool isDone = false;
	SDL_Event event;
	while (!isDone)
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_MOUSEMOTION:
				getMousePos(str);
				isDone = true;
				break;
			}
		}
}

void rect(const string& str)
{
	char cx[32];
	char cy[32];
	char cw[32];
	char ch[32];
	char cc[32];
	int x, y, w, h, c;

	(void)sscanf(str.c_str(), "%s %s  %s %s %s %s %s", nothing, nothing, cx, cy, cw, ch, cc); //-V576

	if (cx[0] == '$')
		x = vars[cx].val;
	else
		x = atoi(cx);

	if (cy[0] == '$')
		y = vars[cy].val;
	else
		y = atoi(cy);

	if (cw[0] == '$')
		w = vars[cw].val;
	else
		w = atoi(cw);

	if (ch[0] == '$')
		h = vars[ch].val;
	else
		h = atoi(ch);

	if (cc[0] == '$')
		c = vars[cc].val;
	else
		c = atoi(cc);

	for (int xx = 0; xx < h; ++xx)
		for (int i = 0; i < h; ++i)
			ram[xyToAddress(x+xx,y+i)] = c;
}

void getButton(const string& str)
{
	char where[32];
	sscanf(str.c_str(), "%s %s %s", nothing, nothing, where);

	SDL_Event e;
	if (SDL_PollEvent(&e))
		if (e.key.type == SDL_KEYDOWN || e.key.type == SDL_KEYUP)
			vars[where] = e.key.keysym.scancode;
}
void wGetButton(const string& str)
{
	char where[32];
	sscanf(str.c_str(), "%s %s %s", nothing, nothing, where);

	SDL_Event e;
	while (true)
	{
		if (SDL_PollEvent(&e))
			if (e.key.keysym.scancode != SDL_SCANCODE_UNKNOWN)
			{
				vars[where] = e.key.keysym.scancode;
				break;
			}
	}

}

void setSprite(const string& str)
{
	char spriteData[300];
	char spriteNumber[32];

	string spriteDataStr;
	size_t spriteNumberSizeT = 0;

	sscanf(str.c_str(), "%s %s %s %s", nothing, nothing, spriteNumber, spriteData);
	
	if (spriteData[0] == '$')
		spriteDataStr = vars[spriteData].str;
	else
		spriteDataStr = spriteData;

	if (spriteNumber[0] == '$')
		spriteNumberSizeT = (size_t)vars[spriteNumber].val;
	else
		spriteNumberSizeT = atol(spriteNumber);

	sprites[spriteNumberSizeT] = stringToSprite(spriteDataStr);
}
void displaySprite(const string& str)
{
	char spriteNumber[32];
	char x_s[32];
	char y_s[32];
	char chromaKey_s[32];
	size_t x = 0, y = 0, chromaKey = 0,
		spriteNumberSizeT = 0;

	sscanf(str.c_str(), "%s %s %s %s %s %s", nothing, nothing, spriteNumber, x_s, y_s, chromaKey_s);

	if (spriteNumber[0] == '$')
		spriteNumberSizeT = (size_t)vars[spriteNumber].val;
	else
		spriteNumberSizeT = atol(spriteNumber);

	if (x_s[0] == '$')
		x = (size_t)vars[x_s].val;
	else
		x = atol(x_s);
	if (y_s[0] == '$')
		y = (size_t)vars[y_s].val;
	else
		y = atol(y_s);

	if (chromaKey_s[0] == '$')
		chromaKey = (size_t)vars[chromaKey_s].val;
	else
		chromaKey = atol(chromaKey_s);

	displaySprite(true, spriteNumberSizeT, x, y, chromaKey);
}

void delay(const string& str)
{
	char delayMillis[32];
	int delay;

	sscanf(str.c_str(), "%s %s %s", nothing, nothing, delayMillis); //-V576
	if (delayMillis[0] == '$')
		delay = vars[delayMillis].val;
	else
		delay = atoi(delayMillis);
	for (register int i = 0; i < videoRamSize; ++i)
		ram[i] = delay;
	SDL_Delay(delay);
}

map<string, void(*)(const string&)> functions =
{
	{{"line"}, line},
	{{"circc"}, circc},
	{{"circ"}, circ},
	{{"clear"}, clearScreen},
	{{"getmousepos"}, getMousePos},
	{{"display"}, display},
	{{"wgetmousepos"}, wGetMousePos},
	{{"rect"}, rect},
	{{"getbutton"}, getButton},
	{{"wgetbutton"}, wGetButton},
	{{"setsprite"}, setSprite},
	{{"displaysprite"}, displaySprite},
	{{"delay"}, delay}
};





void jmp(const string& str)
{
	char newpcRegister[32];

	sscanf(str.c_str(), "%s %s", nothing, &newpcRegister); //-V576

	if (isalpha(newpcRegister[0]))
	{
		auto temp = marks[newpcRegister];
		pcRegister = temp;
	}
	else
	{
		stringstream converter;
		converter << hex << newpcRegister;
		converter >> pcRegister;
	}
}

void jms(const string& str)
{
	char newpcRegister[32];

	sscanf(str.c_str(), "%s %s", nothing, &newpcRegister); //-V576

	if (isalpha(newpcRegister[0]))
	{
		auto temp = marks[newpcRegister];
		pcRegister += temp;
	}
	else
	{
		stringstream converter;
		unsigned long long i = 0;
		converter << hex << newpcRegister;
		converter >> i;
		pcRegister += i;
	}
}

void mark(const string& str)
{
	char newMark[32];
	sscanf(str.c_str(), "%s %s", nothing, newMark); //-V576
	marks[newMark] = pcRegister;
}

void mov(const string& str)
{
	try
	{
		string name = str.substr(str.find(' ') + 2, str.find(' ', str.find(' ') + 1) - str.find(' ') - 2);
		string value = str.substr(str.find(' ', str.find(' ')+1)+1, str.size()- str.find(' ', str.find(' ')));



		if (value[0] == '"')
			vars[name] = value.substr(1, value.size() - 2);
		else if (value.find('.') != string::npos)
		{
			stringstream converter;
			converter << value;
			converter >> vars[name].dVal;
			zeroFlag = (int)vars[name].dVal;
		}
		else
			vars[name] = atoi(value.c_str());
		zeroFlag = atoi(value.c_str()); //-V519
		
	}
	catch (exception& except)
	{
		cerr << "Ошибка ассемблирования команды mov! Ошибка: " << except.what() << endl;
		exit(1);
	}

};

void call(const string& str)
{
	char func[32];
	sscanf(str.c_str(), "%s %s", nothing, func); //-V576


	auto funct = (functions[func]);
	if (funct)
		funct(str);
}

void addi(const string& str)
{
	char in1[32];
	char in2[32];
	char out[32];
	int in, in0;

	sscanf(str.c_str(), "%s  %s %s %s", nothing, in1, in2, out); //-V576

	if (in1[0] == '$')
		in = vars[in1].val;
	else
		in = atoi(in1);

	if (in2[0] == '$')
		in0 = vars[in2].val;
	else
		in0 = atoi(in2);

	vars[out] = in + in0;
	zeroFlag = in + in0;
}
void subi(const string& str)
{
	char in1[32];
	char in2[32];
	char out[32];
	int in, in0;

	sscanf(str.c_str(), "%s  %s %s %s", nothing, in1, in2, out); //-V576

	if (in1[0] == '$')
		in = vars[in1].val;
	else
		in = atoi(in1);

	if (in2[0] == '$')
		in0 = vars[in2].val;
	else
		in0 = atoi(in2);

	vars[out] = in - in0;
	zeroFlag = in - in0;
}

void addf(const string& str)
{
	char in1[32];
	char in2[32];
	char out[32];
	double in, in0;

	sscanf(str.c_str(), "%s  %s %s %s", nothing, in1, in2, out); //-V576

	if (in1[0] == '$')
		in = vars[in1].dVal;
	else
		in = atof(in1);

	if (in2[0] == '$')
		in0 = vars[in2].dVal;
	else
		in0 = atof(in2);

	vars[out] = in + in0;
	zeroFlag = (int)(in + in0);
}
void subf(const string& str)
{
	char in1[32];
	char in2[32];
	char out[32];
	double in, in0;

	sscanf(str.c_str(), "%s  %s %s %s", nothing, in1, in2, out); //-V576

	if (in1[0] == '$')
		in = vars[in1].dVal;
	else
		in = atof(in1);

	if (in2[0] == '$')
		in0 = vars[in2].dVal;
	else
		in0 = atof(in2);

	vars[out] = in - in0;
	zeroFlag = (int)(in - in0);
}

void zero(const string& str)
{
	char cout[32];
	sscanf(str.c_str(), "%s  %s", nothing, cout);

	if (!zeroFlag)
		jmp("jmp " + string(cout));
}
void notZero(const string& str)
{
	char cout[32];
	sscanf(str.c_str(), "%s  %s", nothing, cout);

	if (zeroFlag)
		jmp("jmp " + string(cout));
}
void zeros(const string& str)
{
	char cout[32];
	sscanf(str.c_str(), "%s  %s", nothing, cout);

	if (!zeroFlag)
		jms("jms " + string(cout));
}
void notZeros(const string& str)
{
	char cout[32];
	sscanf(str.c_str(), "%s  %s", nothing, cout);

	if (zeroFlag)
		jms("jms " + string(cout));
}

void neg(const string& str)
{
	char in[32];
	char out[32];
	int ini = 0;
	double inif = 0.0;

	sscanf(str.c_str(), "%s %s %s", nothing, in, out);

	if (in[0] == '$')
		if(vars[in].varType == INT)
			ini = vars[in].val;
		else
			inif = vars[in].dVal;

	if (vars[out].varType == INT)
		vars[out].val = -ini;
	else
		vars[out].dVal = -inif;

	zeroFlag = (-ini) | ((int)-inif);
}
void notF(const string& str)
{
	char in[32];
	char out[32];
	int ini;

	sscanf(str.c_str(), "%s %s %s", nothing, in, out);

	if (in[0] == '$')
		ini = vars[in].val;
	else
		ini = atoi(in);
	if (vars[out].varType == INT)
		vars[out].val = ~ini;

	zeroFlag = ~ini;
}

void cmp(const string& str)
{
	char var[32];
	char what[32];
	char go[32];
	sscanf(str.c_str(), "%s %s %s %s", nothing, var, what, go);
	string convertedWhat = what;
	if (convertedWhat.find('.') != string::npos)
		if (vars[var] == (double)atof(convertedWhat.c_str()))
			jmp("jmp " + (string)go);
	if (what[0] == '$')
	{
		bool isDo = false;
		switch (vars[what].varType)
		{
		case INT:
			isDo = vars[what] == vars[var].val;
			break;

		case FLOAT:
			isDo = vars[what] == vars[var].dVal;
			break;

		case STRING:
			isDo = vars[what] == vars[var].str;
			break;

		default:
			break;
		}
		if(isDo)
			jmp("jmp " + (string)go);
	}

	if(atoi(convertedWhat.c_str()) == vars[var].val)
		jmp("jmp " + (string)go);
}
void cmps(const string& str)
{
	char var[32];
	char what[32];
	char go[32];
	sscanf(str.c_str(), "%s %s %s %s", nothing, var, what, go);
	string convertedWhat = what;
	if (convertedWhat.find('.') != string::npos)
		if (atof(convertedWhat.c_str()) == vars[var].dVal)
			jmp("jms " + (string)go);
	if (what[0] == '$')
	{
		bool isDo = false;
		switch (vars[what].varType)
		{
		case INT:
			isDo = vars[what] == vars[var].val;
			break;

		case FLOAT:
			isDo = vars[what] == vars[var].dVal;
			break;

		case STRING:
			isDo = vars[what] == vars[var].str;
			break;

		default:
			break;
		}
		if (isDo)
			jmp("jms " + (string)go);
	}

	if (atoi(convertedWhat.c_str()) == vars[var].val)
		jmp("jms " + (string)go);
}
void notcmp(const string& str)
{
	char var[32];
	char what[32];
	char go[32];
	sscanf(str.c_str(), "%s %s %s %s", nothing, var, what, go);
	string convertedWhat = what;
	if (convertedWhat.find('.') != string::npos)
		if (atof(convertedWhat.c_str()) != vars[var].dVal)
			jmp("jmp " + (string)go);
	if (what[0] == '$')
	{
		bool isDo = false;
		switch (vars[what].varType)
		{
		case INT:
			isDo = vars[what] == vars[var].val;
			break;

		case FLOAT:
			isDo = vars[what] == vars[var].dVal;
			break;

		case STRING:
			isDo = vars[what] == vars[var].str;
			break;

		default:
			break;
		}
		if (!isDo)
			jmp("jmp " + (string)go);
	}

	if (atoi(convertedWhat.c_str()) != vars[var].val)
		jmp("jmp " + (string)go);
}
void notcmps(const string& str)
{
	char var[32];
	char what[32];
	char go[32];
	sscanf(str.c_str(), "%s %s %s %s", nothing, var, what, go);
	string convertedWhat = what;
	if (convertedWhat.find('.') != string::npos)
		if (atof(convertedWhat.c_str()) != vars[var].dVal)
			jmp("jms " + (string)go);
	if (what[0] == '$')
	{
		bool isDo = false;
		switch (vars[what].varType)
		{
		case INT:
			isDo = vars[what] == vars[var].val;
			break;

		case FLOAT:
			isDo = vars[what] == vars[var].dVal;
			break;

		case STRING:
			isDo = vars[what] == vars[var].str;
			break;

		default:
			break;
		}
		if (!isDo)
			jmp("jms " + (string)go);
	}

	if (atoi(convertedWhat.c_str()) != vars[var].val)
		jmp("jms " + (string)go);
}


map<string, void(*)(const string&)> commands =
{
	{"jmp", jmp},
	{"jms", jms},

	{"mark", mark},
	{"mov", mov},
	{"call", call},

	{"addi", addi},
	{"addf", addf},

	{"zero", zero},
	{"notzero", notZero},
	{"zeros", zeros},
	{"notzeros", notZeros},

	{"neg", neg},
	{"not", notF},

	{"subi", subi},
	{"subf", subf},

	{"cmp", cmp},
	{"cmps", cmps},
	{"notcmp", notcmp},
	{"notcmps", notcmps},
};
#endif
