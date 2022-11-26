#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <map>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <cstdlib>

using namespace std;

#define BUF_SIZE 6
TCHAR szFileName[]=TEXT("WORDLE");

void trimite(LPCTSTR pChannel, string mesaj)
{
    LPTSTR szMsg = new TCHAR[mesaj.length() + 1];
    strcpy(szMsg, mesaj.c_str());
    CopyMemory((PVOID)pChannel, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));
}

string primeste(LPCTSTR pChannel)
{
    return string(pChannel);
}

enum _ConsoleTxtColor : const unsigned short
{
	_ConsoleTxtBlack = 0,
	_ConsoleTxtBlue = 1,
	_ConsoleTxtGreen = 2,
	_ConsoleTxtAqua = 3,
	_ConsoleTxtRed = 4,
	_ConsoleTxtPurple = 5,
	_ConsoleTxtYellow = 6,
	_ConsoleTxtLightGrey = 7,
	_ConsoleTxtGrey = 8,
	_ConsoleTxtLightBlue = 9,
	_ConsoleTxtLightGreen = 10,
	_ConsoleTxtTurquoise = 11,
	_ConsoleTxtPink = 12,
	_ConsoleTxtLightPurple = 13,
	_ConsoleTxtLightYellow = 14,
	_ConsoleTxtWhite = 15
};

enum _ConsoleBkgColor : const unsigned short
{
	_ConsoleBkgBlack = 0,
	_ConsoleBkgBlue = 16,
	_ConsoleBkgGreen = 32,
	_ConsoleBkgAqua = 48,
	_ConsoleBkgRed = 64,
	_ConsoleBkgPurple = 80,
	_ConsoleBkgYellow = 96,
	_ConsoleBkgLightGrey = 112,
	_ConsoleBkgGrey = 128,
	_ConsoleBkgLightBlue = 144,
	_ConsoleBkgLightGreen = 160,
	_ConsoleBkgTurquoise = 176,
	_ConsoleBkgPink = 192,
	_ConsoleBkgLightPurple = 208,
    _ConsoleBkgLightYellow = 224,
    _ConsoleBkgWhite = 240
};

int numara(char* cuvant, char litera)
{
    int aparitii=0;
    for (int i=0; i<6; i++)
    {
        if (cuvant[i]==litera)
            aparitii++;
    }
    return aparitii;
}

int cauta(vector<string> lista, string cuvant)
{
    for (int i=0; i<lista.size(); i++)
    {
        if (strcmp(lista[i].c_str(), cuvant.c_str())==0)
        {
            return i;
            cout <<i;
        }
    }
    return -1;
}

int main(int argc, char *argv[])
{
    ifstream fin("words.in");

    HANDLE hSTD = GetStdHandle(STD_OUTPUT_HANDLE);

    HANDLE hMapFile;
    LPCTSTR pChannel;

    char s[6],t[6];

    int max_incercari=6;
    bool fara_limita = argc > 1 && !strcmp(argv[1],"-l");
    bool server = argc > 1 && !strcmp(argv[1],"-s");

    SetConsoleTextAttribute(hSTD, _ConsoleTxtWhite | _ConsoleBkgBlack);
    cout <<"Introduceti cuvantul:" <<endl;
    SetConsoleTextAttribute(hSTD, _ConsoleTxtWhite | _ConsoleBkgBlack);

    if (server)
    {
        hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE,    // use paging file
            NULL,                    // default security
            PAGE_READWRITE,          // read/write access
            0,                       // maximum object size (high-order DWORD)
            BUF_SIZE,                // maximum object size (low-order DWORD)
            szFileName);             // name of mapping object

        if (hMapFile == NULL)
        {
          cout <<"Could not create file mapping object: " <<GetLastError() <<endl;
          return 1;
        }

        pChannel = (LPTSTR) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);

        if (pChannel == NULL)
        {
            cout <<"Could not map view of file: " <<GetLastError() <<endl;
            CloseHandle(hMapFile);
            return 1;
        }
    }

    vector<string> cuvinte;
    string cuvant, raspuns;
    while (fin >> cuvant)
        cuvinte.push_back(cuvant);

    srand((unsigned)time(NULL) * 100);
    int random = rand() % cuvinte.size();
    strcpy(t, cuvinte[random].c_str());

    while (strcmp(t,s)!=0 && (max_incercari>0 || fara_limita))
    {
        map<char,int> m;

        if (server)
        {
            string mesaj = primeste(pChannel);
            if (mesaj.length() == 0 || mesaj[0] == '>')
            {
                sleep(1);
                continue;
            }

            strcpy(s, mesaj.c_str()+1);
        }
        else
        {
            cin >>s;
        }

        max_incercari--;

        if (cauta(cuvinte, s) == -1)
        {
            SetConsoleTextAttribute(hSTD, _ConsoleTxtWhite | _ConsoleBkgBlack);
            cout <<"Cuvantul nu se afla in lista"<<endl;
            SetConsoleTextAttribute(hSTD, _ConsoleTxtWhite | _ConsoleBkgBlack);

            if (server)
            {
                trimite(pChannel, ">!!!!!");
            }
            continue;
        }

        for (int i=0;i<5;i++)
        {
            if (m.find(s[i]) == m.end())
                m[s[i]]=numara(t,s[i]);
            if (s[i]==t[i])
                m[s[i]]--;
        }

        raspuns = ">";

        for (int i=0; i<5; i++)
        {
            if (s[i]==t[i])
            {
                SetConsoleTextAttribute(hSTD, _ConsoleTxtGreen | _ConsoleBkgBlack);
                cout <<s[i];
                SetConsoleTextAttribute(hSTD, _ConsoleTxtWhite | _ConsoleBkgBlack);

                raspuns += s[i];
            }
            else if (m[s[i]]>0)
            {
                SetConsoleTextAttribute(hSTD, _ConsoleTxtYellow | _ConsoleBkgBlack);
                cout <<s[i];
                SetConsoleTextAttribute(hSTD, _ConsoleTxtWhite | _ConsoleBkgBlack);

                m[s[i]]--;
                raspuns += '?';
            }
            else
            {
                SetConsoleTextAttribute(hSTD, _ConsoleTxtRed | _ConsoleBkgBlack);
                cout <<s[i];
                SetConsoleTextAttribute(hSTD, _ConsoleTxtWhite | _ConsoleBkgBlack);

                raspuns += '_';
            }
        }
        cout <<endl;

        if (server)
            trimite(pChannel, raspuns);
    }

    SetConsoleTextAttribute(hSTD, _ConsoleTxtWhite | _ConsoleBkgBlack);

    if (strcmp(t,s) != 0)
    {
        cout <<"<GAME OVER>" <<endl;
        if (server)
            trimite(pChannel, ">!OVER");
    }
    else
    {
        cout <<"!>YOU WIN<!" <<endl;
        if (server)
            trimite(pChannel, ">!GOOD");
    }

    if (server) // curata memoria
    {
        UnmapViewOfFile(pChannel);
        CloseHandle(hMapFile);
    }

    CloseHandle(hSTD);

    return 0;
}
