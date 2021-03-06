/*
   Copyright (C) 2013, scutLaoYi

   This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
website: http://www.gnu.org/licenses/old-licenses/gpl-2.0.html

   Contact scutLaoYi: HelloWorld636596@126.com
 */  
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "fileSystem.h"
using namespace std;

const int USERNAMEBUFFERSIZE = 100;
const int COMMANDLENGTH = 100;
FileSystem *sharedFileSystem;

//function to create a dir for data storage.
//It will also create a data file holds all files information.
void init()
{
	sharedFileSystem = 0;
	char *cmd = new char[COMMANDLENGTH];
	strcpy(cmd, "mkdir ");
	strcat(cmd, dirName);
	system(cmd);
	delete cmd;
	cmd = 0;

	char *fileListPath = new char[strlen(dirName) + strlen(listName) + 1];
	strcpy(fileListPath, dirName);
	strcat(fileListPath, listName);
	ifstream test(fileListPath);
	if(test)
	{
		test.close();
		return;
	}
	ofstream create;
	create.open(fileListPath, ios::out|ios::binary);
	int tempInt(0);
	create.write((const char*)&tempInt, sizeof(int));
	create.close();
	delete fileListPath;
	return;
}

void printHelp()
{
	printf("----help----\n");
	printf("All possible commands:\n");
	printf("login\tlogout\tdir\tcreate\tdelete\topen\tclose\tread\twrite\texit\n");
	printf("----help----\n");
	return;
}

void printFailed()
{
	printf("Operation failed!\n");
	return;
}


void login()
{
	if(sharedFileSystem != 0)
	{
		printf("You have login right now!\n");
		return;
	}
	char *buffer = new char[USERNAMEBUFFERSIZE];
	printf("Please input your user name:\n");
	scanf("%s", buffer);
	printf("Login with username:%s\n", buffer);
	sharedFileSystem = new FileSystem(buffer);
	return;
}

void logout()
{
	if(sharedFileSystem == 0)
	{
		printf("You have logout now!\n");
		return;
	}
	delete sharedFileSystem;
	sharedFileSystem = 0;
	return;
}

bool checkLogin()
{
	if(sharedFileSystem == 0)
	{
		printf("You haven't login right now.\n");
		return false;
	}
	return true;
}

void dir()
{
	if(!checkLogin())
	{
		return;
	}
	sharedFileSystem->listFile();
	return;
}

void create()
{
	if(!checkLogin())
	{
		return;
	}
	printf("Input a new file name:");
	string newFileName;
	cin>>newFileName;
	if(sharedFileSystem->createFile(newFileName))
	{
		printf("operation success!\n");
	}
	else
	{
		printf("operation failed!\n");
		printf("Please check if the current file has existed!\n");
	}
	return;
}
void deleteFile()
{
	if(!checkLogin())
	{
		return;
	}
	printf("Input the file name you wish to delete:");
	string fileName;
	cin>>fileName;
	if(sharedFileSystem->deleteFile(fileName))
	{
		printf("delete it successfully.\n");
	}
	else
	{
		printf("deleting failed!\n");
	}
	return;
}
void open()
{
	if(!checkLogin())
	{
		return;
	}
	printf("Input the file name you wish to open:");
	string fileName;
	cin>>fileName;
	if(sharedFileSystem->changeCurrentFile(fileName))
	{
		printf("File opening success!\n");
	}
	else
	{
		printf("File opening failed!\n");
	}
	return;
}
void close()
{
	if(!checkLogin())
	{
		return;
	}
	sharedFileSystem->closeFile();
	return;
}
void read()
{
	if(!checkLogin())
	{
		return;
	}
	if(!sharedFileSystem->openFile('r'))
	{
		printf("file opening failed!\n");
		return;
	}
	if(!sharedFileSystem->readFile())
	{
		printf("file reading failed!\n");
		return;
	}
	return;
}
void write()
{
	if(!checkLogin())
	{
		return;
	}
	if(!sharedFileSystem->openFile('w'))
	{
		printf("file opening failed!");
		return;
	}
	printf("Input the data, end with a single Z in one line:\n");
	char temp[COMMANDLENGTH];
	long long fileSize = 0;
	while(true)
	{
		cin.getline(temp, COMMANDLENGTH);
		if(strlen(temp) == 1 && temp[0]=='Z')
			break;
		int end = strlen(temp);
		fileSize += end;
		for(int i = 0 ; i < end; ++i)
		{
			sharedFileSystem->writeFile(temp[i]);
		}
		sharedFileSystem->writeFile('\n');
	}
	sharedFileSystem->reflashFileLength(fileSize);
	return;
}
int main(int argc,char *argv[])
{
	init();
	char *cmd = new char[COMMANDLENGTH];
	printf("\n>>>");
	while(scanf("%s", cmd) != EOF)
	{
		if(strcmp(cmd, "login")==0)
		{
			login();
		}
		else if(strcmp(cmd, "logout")==0)
		{
			logout();
		}
		else if(strcmp(cmd, "dir")==0)
		{
			dir();
		}
		else if(strcmp(cmd, "create")==0)
		{
			create();
		}
		else if(strcmp(cmd, "delete")==0)
		{
			deleteFile();
		}
		else if(strcmp(cmd, "open")==0)
		{
			open();
		}
		else if(strcmp(cmd, "close") == 0)
		{
			close();
		}
		else if(strcmp(cmd, "read")==0)
		{
			read();
		}
		else if(strcmp(cmd, "write")==0)
		{
			write();
		}
		else if(strcmp(cmd, "help")==0)
		{
			printHelp();
		}
		else if(strcmp(cmd, "exit")==0)
		{
			printf("log out and exit the system.\n");
			if(sharedFileSystem)
			{
				delete sharedFileSystem;
			}
			return 0;
		}
		else
		{
			printf("Unrecognizable command!\n");
		}
		printf("\n>>>");
	}
return 0;
}
