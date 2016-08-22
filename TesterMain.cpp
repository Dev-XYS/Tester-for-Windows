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
#define COLOR_TLE 3

using namespace std;

int casec, maxtime;
char name[100], ord[100], temp[100], temps[100], tempd[100], runf[100];

PROCESS_INFORMATION pi;
bool working, istle;
unsigned long ret;

void copy(char *file, char *tar);
bool check(char *src, char *dest);
void setcolor(int color);
void runprogram();

int main()
{
	while (true)
	{
		// Input.
		printf("Please insert the problem name:\n");
		scanf("%s", name);
		printf("Please insert the testcase count:\n");
		scanf("%d", &casec);
		printf("Please insert the time limit (ms):\n");
		scanf("%d", &maxtime);
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
			istle = false;
			working = false;
			int starttime = clock();
			runprogram();
			int endtime = clock();
			
			if (istle == true)
			{
				setcolor(COLOR_TLE);
				printf("TLE ");
				setcolor(COLOR_NONE);
				printf("---- ms\n");
				goto END;
			}
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
			
			END:;
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
			fs.close();
			fd.close();
			return false;
		}
	}
	
	if (!fs.eof())
	{
		strcpy(temps, ""); 
		fs >> temps;
		if (strcmp(temps, "") != 0)
		{
			fs.close();
			fd.close();
			return false;
		}
	}
	if (!fd.eof())
	{
		strcpy(tempd, "");
		fd >> tempd;
		if (strcmp(tempd, "") != 0)
		{
			fs.close();
			fd.close();
			return false;
		}
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
	else if (color == COLOR_TLE)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	}
}

void runprogram()
{
	STARTUPINFOA si;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	working = CreateProcess(NULL, runf, NULL, NULL, false, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	if (working == false)
	{
		return;
	}
	if (WaitForSingleObject(pi.hProcess, maxtime) == WAIT_TIMEOUT)
	{
		TerminateProcess(pi.hProcess, 0);
		istle = true;
	}
	GetExitCodeProcess(pi.hProcess, &ret);
}
