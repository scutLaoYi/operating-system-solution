#ifndef FILE_H
#define FILE_H
#include <cstring>
#include <fstream>
using std::fstream;
using std::ifstream;
using std::ofstream;
const char dirName[] = "fileSystemDir/";
class File
{
	//the size of file
	long long fileLength;
	//the creator of this file
	char *userName;
	//flag whether the exact file has been removed
	bool deleted;

	//constructor with no parameter, used inside the class
	File();
	//return the cstring from ifstream with size length
	static char *getStringInLength(ifstream &inFilePtr, int length);
	//return the current time in cstring
	char *getFileNameByTime();

	public:
	//the file name showed in the system
	char *fileNameInSystem;
	//the file name in physical
	char *physicalAddress;
	//constructor with parameters, used when new file has been created
	File(char *fileNameInSystem,char *userName);
	//...
	~File();
	//function to remove the file in physical
	bool removeFile();
	//judge whether the file has been removed
	bool isDeleted();
	//print the description of this file in command line
	void description();
	//record the file information into ofstream
	void recordIntoStream(ofstream &out);
	//class function which will create and return a new File object with information read from ifstream
	static File *objectByInfoFromStream(ifstream &in);
};

#endif
