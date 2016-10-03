/********************************************************
* TesterMain.cpp (c) 2016 Dev-XYS. All rights reserved. *
* Version : 2.2.0.10                                    *
********************************************************/

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <conio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <windows.h>
#include <string>
#include <io.h>

#pragma warning (disable : 4996)

// Result definitions.
#define RESULT_NONE -1
#define RESULT_RE 2
#define RESULT_TLE 3
#define RESULT_SF 4

// Console color definitions.
#define COLOR_NONE -1
#define COLOR_AC 0
#define COLOR_WA 1
#define COLOR_RE 2
#define COLOR_TLE 3
#define COLOR_SF 4

#define COLOR_PROBLEMNAME 16

using namespace std;

// Represent a problem.
struct problem
{
	string name;
	int testcase, timelimit;
};

// Command functions.
void cmd_config();

// Other functions.
void setcolor(int color);

// Variables.
string cmd;

int main()
{
	// Print copyright.
	cout << "TesterMain (c) 2016 Dev-XYS. All rights reserved.\n";
	cout << "Repository URL : github.com/Dev-XYS/Tester\n\n";

	while (true)
	{
		// Wait for command.
		cout << "test->";
		cin >> cmd;

		if (cmd == "config")
		{
			cmd_config();
		}
		else
		{
			cout << "No such command." << endl;
		}
	}

	return 0;
}

void switch_wer(DWORD val)
{
	HKEY key;
	RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\", NULL, KEY_ALL_ACCESS, &key);
	RegSetValueEx(key, "Disabled", NULL, REG_DWORD, (LPBYTE)&val, sizeof(val));
	RegSetValueEx(key, "DontShowUI", NULL, REG_DWORD, (LPBYTE)&val, sizeof(val));
	RegCloseKey(key);
}

void copy(string src, string dest)
{
	ofstream fout;
	ifstream fin;

	fin.open(src, ios::binary);
	fout.open(dest, ios::binary);

	fout << fin.rdbuf();

	fin.close();
	fout.close();
}

void compile(const char *compiler, string src, string dest)
{
	system("echo off");
	system(((string)compiler + ">nul 2>nul " + src + " -o" + dest).c_str());
}

int runprogram(string exef, int timelim)
{
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	BOOL working = CreateProcess(NULL, (char *)exef.c_str(), NULL, NULL, false, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	if (working == false)
	{
		return RESULT_SF;
	}
	DWORD waitres = WaitForSingleObject(pi.hProcess, timelim);
	if (waitres == WAIT_TIMEOUT)
	{
		TerminateProcess(pi.hProcess, 0);
		return RESULT_TLE;
	}
	unsigned long ret = 0;
	GetExitCodeProcess(pi.hProcess, &ret);
	return ret == 0 ? RESULT_NONE : RESULT_RE;
}

bool check(string src, string dest)
{
	ifstream fs, fd;
	char temps[1024], tempd[1024];

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

void cmd_config()
{
	string conf, comps[50];
	int probc, compc, pr_cnt = 0, ac_cnt;
	problem probs[10];
	char compiler[128], ord[10];
	ifstream coni;

	cout << "Please insert the config file name:\n";
	cin >> conf;
	cout << endl;

	// Open the config file.
	coni.open(conf);

	// Read in problems and competitors.
	coni.getline(compiler, 128);
	coni >> probc;
	for (int i = 0; i < probc; i++)
	{
		coni >> probs[i].name >> probs[i].testcase >> probs[i].timelimit;
		pr_cnt += probs[i].testcase;
	}
	coni >> compc;
	for (int i = 0; i < compc; i++)
	{
		coni >> comps[i];
	}

	// Disable Windows Error Reporting.
	switch_wer(1);

	// Copying and checking.
	for (int k = 0; k < compc; k++)
	{
		ac_cnt = 0;

		// Show the competitor's name;
		cout << ">>> " << comps[k] << endl << endl;

		// Copying .exe files.
		for (int i = 0; i < probc; i++)
		{
			compile(compiler, "src/" + comps[k] + '/' + probs[i].name + ".cpp", probs[i].name + ".exe");
		}

		// Running and checking.
		for (int i = 0; i < probc; i++)
		{
			int sub_ac_cnt = 0;

			// Show the problem's name;
			setcolor(COLOR_PROBLEMNAME);
			cout << "Problem : " << probs[i].name << endl;
			setcolor(COLOR_NONE);

			for (int j = 1; j <= probs[i].testcase; j++)
			{
				// Copy .in and .out files.
				itoa(j, ord, 10);

				copy("data/" + probs[i].name + ord + ".in", probs[i].name + ".in");

				// Running.
				cout << "Running for testcase #" << right << setw(3) << j << "... ";
				int starttime = clock();
				int result = runprogram(probs[i].name + ".exe", probs[i].timelimit);
				int endtime = clock();

				if (result == RESULT_TLE)
				{
					setcolor(COLOR_TLE);
					cout << "TLE ";
					setcolor(COLOR_NONE);
					cout << "---- ms\n";
					goto END;
				}
				if (result == RESULT_RE)
				{
					setcolor(COLOR_RE);
					cout << "RE  ";
					setcolor(COLOR_NONE);
					goto SHOWTIME;
				}
				if (result == RESULT_SF)
				{
					setcolor(COLOR_SF);
					cout << "SF  ";
					setcolor(COLOR_NONE);
					cout << "---- ms\n";
					goto END;
				}

				// Checking.
				if (check(probs[i].name + ".out", "data/" + probs[i].name + ord + ".ans") == true)
				{
					setcolor(COLOR_AC);
					cout << "AC  ";
					setcolor(COLOR_NONE);
					sub_ac_cnt++;
				}
				else
				{
					setcolor(COLOR_WA);
					cout << "WA  ";
					setcolor(COLOR_NONE);
				}

			SHOWTIME:
				cout << setw(4) << endtime - starttime << " ms\n";

			END:;

				// Deleting .out file.
				remove((probs[i].name + ".out").c_str());
			}

			// Show score.
			ac_cnt += sub_ac_cnt;
			cout << "Sub-total : " << sub_ac_cnt << '/' << probs[i].testcase << endl << endl;
		}

		// Show total score.
		cout << "Total : " << ac_cnt << '/' << pr_cnt << endl << endl << endl;

		// Deleting .exe files.
		for (int i = 0; i < probc; i++)
		{
			remove((probs[i].name + ".exe").c_str());
		}
	}

	// Deleting .in files.
	for (int i = 0; i < probc; i++)
	{
		remove((probs[i].name + ".in").c_str());
	}

	// Enable Windows Error Reporting.
	switch_wer(0);
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
	else if (color == COLOR_SF)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	}
	else if (color == COLOR_PROBLEMNAME)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	}
}
