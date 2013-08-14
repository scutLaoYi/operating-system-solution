#include <cstdio>
#include <fstream>
#include <ctime>
#include "file.h"
using std::ofstream;
using std::ios;

File::File()
{
	this->deleted = false;
	return;
}

char *File::getStringInLength(ifstream &inFilePtr, int length)
{
	char *tempStr = new char[length + 1];
	inFilePtr.read(tempStr, sizeof(char) * length);
	tempStr[length] = '\0';
	return tempStr;
}
char *File::getFileNameByTime()
{
	char *buffer = new char[20];
	time_t rawtime;
	struct tm *info;
	time(&rawtime);
	info = localtime(&rawtime);
	strftime(buffer, 20, "%Y%m%d%H%M%S", info);
	buffer[14] = '\0';
	char *pAddress = new char[40];
	strcpy(pAddress, dirName);
	strcat(pAddress, buffer);
	delete buffer;
	return pAddress;
}


File::File(char *fileNameInSystem, char *userName)
{
	this->fileNameInSystem = fileNameInSystem;
	this->physicalAddress = getFileNameByTime();
	this->fileLength = 0;
	this->userName = userName;
	this->deleted = false;
	ofstream create;
	create.open(this->physicalAddress, ios::out);
	create.close();
	return;
}

File::~File()
{
	delete fileNameInSystem;
	delete physicalAddress;
	delete userName;
	return ;
}

bool File::removeFile()
{
	if(remove(physicalAddress) == 0)
	{
		deleted = true;
		return true;
	}
	return false;
}
bool File::isDeleted()
{
	return deleted;
}

void File::description()
{
	printf("File info:\n");
	printf("File Name:%s\n", this->fileNameInSystem);
	printf("Physical Address:%s\n", this->physicalAddress);
	printf("File length:%lld\n", this->fileLength);
	printf("User:%s\n", this->userName);
}

void File::recordIntoStream(ofstream &out)
{
	int length;
	length = strlen(fileNameInSystem);
	out.write((const char*)&length, sizeof(int));
	out.write(fileNameInSystem, sizeof(char)*length);

	length = strlen(physicalAddress);
	out.write((const char*)&length, sizeof(int));
	out.write(physicalAddress, sizeof(char)*length);

	out.write((const char*)&fileLength, sizeof(long long));
	
	length = strlen(userName);
	out.write((const char*)&length, sizeof(int));
	out.write(userName, sizeof(char)*length);
	return;
}
File *File::objectByInfoFromStream(ifstream &inFilePtr)
{
	int length;
	File *tempFile = new File();
	inFilePtr.read((char *)&length, sizeof(int));
	tempFile->fileNameInSystem = getStringInLength(inFilePtr, length);

	inFilePtr.read((char *)&length, sizeof(int));
	tempFile->physicalAddress = getStringInLength(inFilePtr, length);

	inFilePtr.read((char *)&tempFile->fileLength, sizeof(long long));
	
	inFilePtr.read((char *)&length, sizeof(int));
	tempFile->userName = getStringInLength(inFilePtr, length);
	return tempFile;
}

