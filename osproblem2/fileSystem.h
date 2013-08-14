#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#define NULLFILE 0
#include <map>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include "file.h"
using namespace std;
const char listName[] = "fileList.dat"; 

class FileSystem
{
	//hash from file name in system to index in file list
	map<string, int> indexMap;
	//file list stored in vector,each element is a pointer to a File object
	vector<File*> *fileList;
	//amount of alive Files
	int fileAmount;
	//the user logged in the system
	char *currentUser;
	//the file which is operated now
	char *currentFile;
	//file pointer
	ifstream inFilePtr;
	//too
	ofstream outFilePtr;
	//record all file information into the data file
	void saveChange();
	//get the path of the file with file name
	//actually it just add a dir name before the file name ^_^
	char *getRecordFileName(const char *fileName);
	
	public:
	//constructor, with a parameter(user name)
	FileSystem(char *currentUser);
	~FileSystem();

	//function to list all file in system
	void listFile();
	//function to create a new file in system
	void createFile(string fileName);
	//function to remove a file from system
	bool deleteFile(string fileName);
	bool changeCurrentFile(string currentFile);
	//open the current file with two file pointers
	bool openFile(char flag);
	//redo
	void closeFile();
	//read the data stored in the current file(must open it first)
	bool readFile();
	//write some data into the current file(must be opened too)
	bool writeFile(char data);
};

#endif
