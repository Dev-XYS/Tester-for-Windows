#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <conio.h>
#include <fstream>
#include <ctime>
#include <windows.h>

#define COLOR_NONE -1
#define COLOR_AC 0
#define COLOR_WA 1
#define COLOR_RE 2

using namespace std;

int casec;
char name[100], ord[100], temp[100], temps[100], tempd[100], runf[100];

void copy(char *file, char *tar);
bool check(char *src, char *dest);
void setcolor(int color);

int main()
{
	while (true)
	{
		// Input.
		printf("Please insert the problem name:\n");
		scanf("%s", name);
		printf("Please insert the testcase count:\n");
		scanf("%d", &casec);
		printf("Press any key to start testing...\n");
		getch();
		
		// Copy .exe name.
		strcpy(runf, name);
		strcat(runf, ".exe");
		
		// Running and checking.
		for (int i = 1; i <= casec; i++)
		{
			// Copy .in and .out.
			itoa(i, ord, 10);
			
			strcpy(temps, name);
			strcpy(tempd, name);
			strcat(temps, ord);
			strcat(temps, ".in");
			strcat(tempd, ".in");
			copy(temps, tempd);
			
			// Running.
			printf("Running for testcase #%2d... ", i);
			int starttime = clock();
			int ret = system(runf);
			int endtime = clock();
			
			if (ret != 0)
			{
				setcolor(COLOR_RE);
				printf("RE  ");
				setcolor(COLOR_NONE);
				goto SHOWTIME;
			}
			
			// Checking.
			strcpy(temps, name);
			strcat(temps, ord);
			strcat(temps, ".ans");
			strcpy(tempd, name);
			strcat(tempd, ".out");
			
			if (check(temps, tempd) == true)
			{
				setcolor(COLOR_AC);
				printf("AC  ");
				setcolor(COLOR_NONE);
			}
			else
			{
				setcolor(COLOR_WA);
				printf("WA  ");
				setcolor(COLOR_NONE);
			}
			
			SHOWTIME:
			printf("%4d ms\n", endtime - starttime);
		}
		
		printf("\n");
	}
	
	return 0;
}

void copy(char *file, char *tar)
{
	ofstream fout;
	ifstream fin;
	
	fout.open(tar, ios::binary);
	fin.open(file, ios::binary);
	
	fout << fin.rdbuf();
	
	fout.close();
	fin.close();
}

bool check(char *src, char *dest)
{
	ifstream fs, fd;
	
	fs.open(src);
	fd.open(dest);
	
	while (!fs.eof() && !fd.eof())
	{
		fs >> temps;
		fd >> tempd;
		if (strcmp(temps, tempd) != 0)
		{
			return false;
		}
	}
	
	if (!fs.eof() || !fd.eof())
	{
		return false;
	}
	
	fs.close();
	fd.close();
	
	return true;
}

void setcolor(int color)
{
	if (color == COLOR_NONE)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
	else if (color == COLOR_AC)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	}
	else if (color == COLOR_WA)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
	}
	else if (color == COLOR_RE)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	}
}
