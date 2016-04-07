// Semaphore.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <SDL.h>
#include <iostream>
#include <list>
#include <string>
using namespace std;

int nr = 0;/*number of active readers*/
SDL_sem* rw = NULL; /*lock for readers/writer exclusion*/
SDL_sem* nrLock = NULL; /*lock for readers/writer exclusion*/

int Writer(void *data);
int Reader(void *data);


int _tmain(int argc, _TCHAR* argv[])
{
	rw = SDL_CreateSemaphore(1);
	nrLock = SDL_CreateSemaphore(1);
	list<SDL_Thread*> readerThreads;
	for (int i = 0; i < 10; i++)
	{
		readerThreads.push_back(SDL_CreateThread(Reader, "reader", (void*)&i));
	}
	
	list<SDL_Thread*> writerThreads;
	for (int i = 0; i < 5; i++)
	{
		writerThreads.push_back(SDL_CreateThread(Writer, "writer", (void*)&i));
	}
	system("pause");
	return 0;
}

int Writer(void *data)
{
	int message = *((int*)data);
	while (true)
	{
		SDL_SemWait(rw);
		/*Write to the file*/
		cout << "Writing\n";

		/*Do writning stuff here */
		std::ofstream out("file.txt");
		out << message;
		out.close();

		SDL_SemPost(rw);
		//Wait randomly
		SDL_Delay(16 + rand() % 640);
	}
	
	return 0;
}

int Reader(void *data)
{
	while (true)
	{
		SDL_SemWait(nrLock);
		nr = nr++;
		if (nr == 1)/*if first get lock*/
		{
			SDL_SemWait(rw);
		}
		SDL_SemPost(nrLock);

		string line;
		ifstream myfile("file.txt");

		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				cout << "Reading thread " << line << '\n';
			}
			myfile.close();
		}

		else cout << "Unable to open file";

		SDL_SemWait(nrLock);
		nr = nr--;
		if (nr == 0)/*if last , release lock*/
		{
			SDL_SemPost(rw);
		}
		SDL_SemPost(nrLock);

		//Wait randomly
		SDL_Delay(16 + rand() % 64);
	}
	return 0;
}

