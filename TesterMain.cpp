/********************************************************
* TesterMain.cpp (c) 2016 Dev-XYS. All rights reserved. *
* Version : 2.4.0.19                                    *
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
#define RESULT_AC 0
#define RESULT_WA 1
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

// Represent a result of a single testcase.
struct case_result
{
	int res, time;
};

// Represent a result of a single competitor.
struct result
{
	string name;
	int score;
}comp_res[100];

// Command functions.
void cmd_conf();
void cmd_res();
void cmd_cpl();
void cmd_oc();

// Other functions.
void colorprint(string con, WORD color);

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

		if (cmd == "conf")
		{
			cmd_conf();
		}
		else if (cmd == "res")
		{
			cmd_res();
		}
		else if (cmd == "cpl")
		{
			cmd_cpl();
		}
		else if (cmd == "oc")
		{
			cmd_oc();
		}
		else
		{
			cout << "No such command." << endl;
		}
	}

	return 0;
}

void read_std_info(ifstream &coni, char *compilercmd, int &probc, int &pr_cnt, problem *probs)
{
	// Read in compiler command, problems and competitors.
	coni.getline(compilercmd, 256);
	coni >> probc;
	for (int i = 0; i < probc; i++)
	{
		coni >> probs[i].name >> probs[i].testcase >> probs[i].timelimit;
		pr_cnt += probs[i].testcase;
	}
}

void read_comp_info(ifstream &coni, int &compc, string *comps)
{
	coni >> compc;
	for (int i = 0; i < compc; i++)
	{
		coni >> comps[i];
	}
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

case_result check_case(string comp, problem prob, int testo)
{
	char ord[10];
	itoa(testo, ord, 10);

	// Copy .in and .out files.
	copy("data/" + prob.name + ord + ".in", prob.name + ".in");

	// Running.
	int starttime = clock();
	int cres = runprogram(prob.name + ".exe", prob.timelimit);
	int endtime = clock();

	if (cres == RESULT_TLE)
	{
		return case_result{ RESULT_TLE, -1 };
	}
	if (cres == RESULT_RE)
	{
		return case_result{ RESULT_RE, endtime - starttime };
	}
	if (cres == RESULT_SF)
	{
		return case_result{ RESULT_SF, -1 };
	}

	// Checking.
	if (check(prob.name + ".out", "data/" + prob.name + ord + ".ans") == true)
	{
		return case_result{ RESULT_AC, endtime - starttime };
	}
	else
	{
		return case_result{ RESULT_WA, endtime - starttime };
	}
}

void show_one_res(case_result cres)
{
	switch (cres.res)
	{
	case RESULT_AC:
		colorprint("AC  ", COLOR_AC);
		cout << right << setw(4) << cres.time << " ms\n";
		break;
	case RESULT_WA:
		colorprint("WA  ", COLOR_WA);
		cout << right << setw(4) << cres.time << " ms\n";
		break;
	case RESULT_RE:
		colorprint("RE  ", COLOR_RE);
		cout << right << setw(4) << cres.time << " ms\n";
		break;
	case RESULT_TLE:
		colorprint("TLE ", COLOR_TLE);
		cout << "---- ms\n";
		break;
	case RESULT_SF:
		colorprint("SF  ", COLOR_SF);
		cout << "---- ms\n";
		break;
	}
}

void cmd_conf()
{
	string conf, comps[50];
	int probc, pr_cnt = 0, ac_cnt;
	problem probs[10];
	char compiler[256];
	ifstream coni;

	cout << "Please insert the config file name:\n";
	cin >> conf;
	cout << endl;

	// Open the config file.
	coni.open(conf.c_str());

	// Read info.
	read_std_info(coni, compiler, probc, pr_cnt, probs);
	read_comp_info(coni, compc, comps);

	coni.close();

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
			colorprint("Compiling " + probs[i].name + "...\n", COLOR_PROBLEMNAME);
			compile(compiler, "src/" + comps[k] + '/' + probs[i].name + ".cpp", probs[i].name + ".exe");
		}
		cout << endl;

		// Running and checking.
		for (int i = 0; i < probc; i++)
		{
			int sub_ac_cnt = 0;

			// Show the problem's name;
			colorprint("Problem : " + probs[i].name + '\n', COLOR_PROBLEMNAME);

			for (int j = 1; j <= probs[i].testcase; j++)
			{
				// Running.
				cout << "Running for testcase #" << right << setw(3) << j << "... ";
				case_result cres = check_case(comps[k], probs[i], j);

				if (cres.res == RESULT_AC)
				{
					sub_ac_cnt++;
				}
				show_one_res(cres);

				// Deleting .out file.
				while (access((probs[i].name + ".out").c_str(), 0) == 0)
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

void cmd_res()
{
	sort(comp_res, comp_res + compc, _result_cmp);
	cout << "Sorted Result:" << endl;
	for (int i = 0; i < compc; i++)
	{
		cout << left << setw(25) << comp_res[i].name << right << setw(4) << comp_res[i].score << endl;
	}
	cout << endl;
}

void cmd_cpl()
{
	colorprint("\nAll competitors in directory /src:\n\n", COLOR_HEADING);
	system("dir /A:D /B src");
	cout << endl;
}

void cmd_oc()
{
	string conf, comps;
	int probc, pr_cnt = 0, ac_cnt = 0;
	problem probs[10];
	char compiler[256];
	ifstream coni;

	cout << "Please insert the config file name:\n";
	cin >> conf;
	cout << "Please insert the competitor's name:\n";
	cin >> comps;
	cout << endl;

	// Open the config file.
	coni.open(conf.c_str());

	// Read info.
	read_std_info(coni, compiler, probc, pr_cnt, probs);

	coni.close();

	// Disable Windows Error Reporting.
	switch_wer(1);

	// Inside the loop of 'k'.

	// Show the competitor's name;
	cout << ">>> " << comps << endl << endl;

	// Copying .exe files.
	for (int i = 0; i < probc; i++)
	{
		colorprint("Compiling " + probs[i].name + "...\n", COLOR_PROBLEMNAME);
		compile(compiler, "src/" + comps + '/' + probs[i].name + ".cpp", probs[i].name + ".exe");
	}
	cout << endl;

	// Running and checking.
	for (int i = 0; i < probc; i++)
	{
		int sub_ac_cnt = 0;

		// Show the problem's name;
		colorprint("Problem : " + probs[i].name + '\n', COLOR_PROBLEMNAME);

		for (int j = 1; j <= probs[i].testcase; j++)
		{
			// Running.
			cout << "Running for testcase #" << right << setw(3) << j << "... ";
			case_result cres = check_case(comps, probs[i], j);

			if (cres.res == RESULT_AC)
			{
				sub_ac_cnt++;
			}
			show_one_res(cres);

			// Deleting .out file.
			while (access((probs[i].name + ".out").c_str(), 0) == 0)
				remove((probs[i].name + ".out").c_str());
		}

		// Show score.
		ac_cnt += sub_ac_cnt;
		cout << "Sub-total : " << sub_ac_cnt << '/' << probs[i].testcase << endl << endl;
	}

	// Show total score.
	cout << "Total : " << ac_cnt << '/' << pr_cnt << endl << endl << endl;

	// Deleting .exe and .in files.
	for (int i = 0; i < probc; i++)
	{
		remove((probs[i].name + ".exe").c_str());
		remove((probs[i].name + ".in").c_str());
	}

	// Enable Windows Error Reporting.
	switch_wer(0);
}

void colorprint(string con, WORD color)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	cout << con;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), COLOR_NONE);
}
