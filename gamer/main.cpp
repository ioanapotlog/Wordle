#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <map>
#include <vector>
#include <time.h>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <algorithm>

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

void filtreaza(vector<string> &lista, const string cuvant, const string raspuns)
{
    lista.erase(remove_if(lista.begin(), lista.end(),
        [cuvant, raspuns](const string& elem) {
            string rest = elem;
            for (int j = raspuns.length() - 1; j >= 0; j--)
            {
                if (raspuns[j] != '?' && raspuns[j] != '_')
                {
                    if (raspuns[j] != elem[j])
                        return true;
                    rest.erase(j, 1);
                }
            }
            for (int j = 0; j < raspuns.length(); j++)
            {
                 if (raspuns[j] == '?')
                {
                    char c = cuvant[j];
                    if (cuvant[j] == elem[j])
                        return true;
                    int p = rest.find(cuvant[j]);
                    if (p == -1)
                        return true;
                    rest.erase(p, 1);
                }
            }
            for (int j = 0; j < raspuns.length(); j++)
            {
                if (raspuns[j] == '_')
                {
                    int p = rest.find(cuvant[j]);
                    if (p != -1)
                        return true;
                }
            }
            return false;
        }),
        lista.end());
}

int main()
{
    LPCTSTR pChannel;

    HANDLE hMapFile = OpenFileMapping(
       FILE_MAP_ALL_ACCESS,   // read/write access
       FALSE,                 // do not inherit the name
       szFileName);           // name of mapping object

    if (hMapFile == NULL)
    {
      cout <<"Could not open file mapping object: " <<GetLastError() <<endl;
      cout <<"Check game server is running!" <<endl;
      return 1;
    }

    pChannel = (LPCSTR) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);

    if (pChannel == NULL)
    {
        cout <<"Could not map view of file: " <<GetLastError() <<endl;
        CloseHandle(hMapFile);
        return 1;
    }

    srand((unsigned)time(NULL) * 100);

    ifstream fin("words.in");

    string cuvant, raspuns;
    vector<string> cuvinte;
    while (fin >> cuvant)
        cuvinte.push_back(cuvant);

    do
    {
        string mesaj = primeste(pChannel);

        if (mesaj.length() == 0 || mesaj[0] == '>')
        {
            if (mesaj.compare(">!OVER") == 0)
            {
                cout <<"GAME OVER" <<endl;
                break;
            }
            if (mesaj.compare(">!GOOD") == 0)
            {
                cout <<"GOOD GUESS" <<endl;
                break;
            }

            if (mesaj.length() > 0)
            {
                filtreaza(cuvinte, cuvant, mesaj.substr(1));
            }

            if (cuvinte.size() == 0)
            {
                cout <<"OUT OF WORDS" <<endl;
                break;
            }

//            cout <<cuvinte.size() <<":";
            int random = rand() % cuvinte.size();
            cuvant = cuvinte[random];
            cout <<cuvant <<endl;

            trimite(pChannel, "<" + cuvant);
        }
        else
        {
            sleep(1);
            continue;
        }

    } while (cuvant.compare(raspuns)!=0);

    UnmapViewOfFile(pChannel);
    CloseHandle(hMapFile);

    return 0;
}

