/********************************************************
* TesterMain.cpp (c) 2016 Dev-XYS. All rights reserved. *
* Version : 2.3.3.16                                    *
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
#include <algorithm>

#pragma warning (disable : 4996)

// Result definitions.
#define RESULT_NONE -1
#define RESULT_RE 2
#define RESULT_TLE 3
#define RESULT_SF 4

// Console color definitions.
#define COLOR_NONE FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define COLOR_HEADING FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define COLOR_PROBLEMNAME FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define COLOR_AC FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define COLOR_WA FOREGROUND_RED | FOREGROUND_INTENSITY
#define COLOR_RE FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define COLOR_TLE FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY
#define COLOR_SF FOREGROUND_BLUE | FOREGROUND_INTENSITY

using namespace std;

// Represent a problem.
struct problem
{
	string name;
	int testcase, timelimit;
};

// Represent a result.
struct result
{
	string name;
	int score;
}comp_res[100];

// Command functions.
void cmd_config();
void cmd_resprt();
void cmd_getcpl();

// Other functions.
void setcolor(WORD color);

// Variables.
string cmd;
int compc;

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
		else if (cmd == "resprt")
		{
			cmd_resprt();
		}
		else if (cmd == "getcpl")
		{
			cmd_getcpl();
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

	fin.open(src.c_str(), ios::binary);
	fout.open(dest.c_str(), ios::binary);

	fout << fin.rdbuf();

	fin.close();
	fout.close();
}

void compile(const char *compiler, string src, string dest)
{
	system(((string)compiler + " " + src + " -o" + dest).c_str());
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

	fs.open(src.c_str());
	fd.open(dest.c_str());

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
	int probc, pr_cnt = 0, ac_cnt;
	problem probs[10];
	char compiler[256], ord[10];
	ifstream coni;

	cout << "Please insert the config file name:\n";
	cin >> conf;
	cout << endl;

	// Open the config file.
	coni.open(conf.c_str());

	// Read in problems and competitors.
	coni.getline(compiler, 256);
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
			setcolor(COLOR_PROBLEMNAME);
			cout << "Compiling " << probs[i].name << "..." << endl;
			setcolor(COLOR_NONE);
			compile(compiler, "src/" + comps[k] + '/' + probs[i].name + ".cpp", probs[i].name + ".exe");
		}
		cout << endl;

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

		// Record scores.
		comp_res[k].name = comps[k];
		comp_res[k].score = ac_cnt;
	}

	// Deleting .in files.
	for (int i = 0; i < probc; i++)
	{
		remove((probs[i].name + ".in").c_str());
	}

	// Enable Windows Error Reporting.
	switch_wer(0);
}

inline bool _result_cmp(const result &x, const result &y)
{
	return x.score > y.score;
}

void cmd_resprt()
{
	sort(comp_res, comp_res + compc, _result_cmp);
	cout << "Sorted Result:" << endl;
	for (int i = 0; i < compc; i++)
	{
		cout << left << setw(20) << comp_res[i].name << right << setw(4) << comp_res[i].score << endl;
	}
	cout << endl;
}

void cmd_getcpl()
{
	setcolor(COLOR_HEADING);
	cout << "\nAll competitors in directory /src:\n\n";
	setcolor(COLOR_NONE);
	system("dir /A:D /B src");
	cout << endl;
}

void setcolor(WORD color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
