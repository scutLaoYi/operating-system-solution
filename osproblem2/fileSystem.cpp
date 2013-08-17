#include "fileSystem.h"
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cstdio>
using namespace std;

void FileSystem::saveChange()
{
	printf("Info:Now saving file info into record.\n");
	if(currentFile != NULLFILE)
	{
		closeFile();
	}
	currentFile = getRecordFileName(listName);
	if(!openFile('w'))
	{

		printf("Error:Opening record file failed!");
		printf("File info saving failed!\n");
		return;
	}
	outFilePtr.write((const char *)&fileAmount, sizeof(int));
	for(vector<File *>::iterator iter = fileList->begin(); iter != fileList->end(); ++iter)
	{
		if((*iter)->isDeleted())
			continue;
		(*iter)->recordIntoStream(outFilePtr);
	}
	closeFile();
	return ;
}

char *FileSystem::getRecordFileName(const char *fileName)
{
	int length = strlen(dirName) + strlen(fileName);
	char *recordFileName = new char[length + 1];
	strcpy(recordFileName, dirName);
	strcat(recordFileName, fileName);
	return recordFileName;
}

FileSystem::FileSystem(char *currentUser)
{
	fileList = new vector<File *>();
	this->currentUser = currentUser;

	currentFile = getRecordFileName(listName);
	if(!openFile('r'))
	{
		printf("Error:FileList open failed!");
		printf("Cannot recover file info!\n");
		return;
	}
	inFilePtr.read((char *)&fileAmount, sizeof(int));
	for(int counter = 0; counter < fileAmount; ++counter)
	{
		File *tempFile = File::objectByInfoFromStream(inFilePtr);
		fileList->push_back(tempFile);		
		string strFileName(tempFile->fileNameInSystem);
		indexMap[strFileName] = counter;
	}	
	closeFile();
	printf("Info:File information recovering complete!Get %d files.\n", fileAmount);
}

FileSystem::~FileSystem()
{
	saveChange();
	for(vector<File *>::iterator iter = fileList->begin(); iter != fileList->end(); ++iter)
	{
		delete *iter;
	}
	delete fileList;
	delete currentUser;
	return ;
}

void FileSystem::listFile()
{
	printf("Info:Listing all files in system.\n");
	printf("There is %d files in the file system.\n", fileAmount);
	for(vector<File *>::iterator iter = fileList->begin(); iter != fileList->end(); ++iter)
	{
		if((*iter)->isDeleted())
			continue;
		(*iter)->description();
	}
	return ;
}

void FileSystem::createFile(string fileName)
{
	if(currentFile != NULLFILE)
		closeFile();

	char *newFileNameInSystem = new char[fileName.size()+1];
	strcpy(newFileNameInSystem, fileName.c_str());
	newFileNameInSystem[fileName.size()] = '\0';
	char *tUserName = new char[strlen(currentUser) + 1];
	strcpy(tUserName, currentUser);

	File *newFile = new File(newFileNameInSystem, tUserName);

	fileList->push_back(newFile);
	indexMap[fileName] = fileList->size()-1;
	++this->fileAmount;
	return;
}

bool FileSystem::deleteFile(string fileName)
{
	if(indexMap.find(fileName) == indexMap.end())
		return false;
	int index = indexMap[fileName];
	File* nowFile = fileList->at(index);
	if(!nowFile->checkUser(this->currentUser))
		return false;
	if( nowFile->removeFile())
	{
		--this->fileAmount;
		indexMap.erase(indexMap.find(fileName));
		return true;
	}
	return false;
}

bool FileSystem::changeCurrentFile(string fileName)
{
	if(indexMap.find(fileName) == indexMap.end())
		return false;
	if(currentFile != NULLFILE)
		closeFile();
	int index = indexMap[fileName];
	File *file = fileList->at(index);
	if(!file->checkUser(this->currentUser))
		return false;
	this->currentFile = new char[strlen(file->physicalAddress) + 1];
	strcpy(this->currentFile, file->physicalAddress);
	currentFileObject = file;
	return true; 
}
bool FileSystem::openFile(char flag)
{
	if(currentFile == 0)
		return false;
	if(flag == 'r')
	{
		if(inFilePtr.is_open())
			return false;
		inFilePtr.open(currentFile, ios::in|ios::binary);
		return true;
	}
	else if(flag == 'w')
	{
		if(outFilePtr.is_open())
		{
			return false;
		}
		else
		{
			outFilePtr.open(currentFile, ios::out|ios::binary);
			return true;
		}
	}
	return false;
}

void FileSystem::closeFile()
{
	if(inFilePtr.is_open())
		inFilePtr.close();
	if(outFilePtr.is_open())
		outFilePtr.close();
	if(currentFile != NULLFILE)
	{
		delete currentFile;
		currentFile = NULLFILE;
	}
	currentFileObject = 0;
	return;
}

void FileSystem::reflashFileLength(long long length)
{
	if(currentFileObject == 0)
		return;
	currentFileObject->reflashSize(length);
	return;
}

bool FileSystem::readFile()
{
	if(inFilePtr.is_open() == false)
	{
		return false;
	}
	inFilePtr.seekg(0, inFilePtr.beg);
	while(inFilePtr.good())
	{
		char c = inFilePtr.get();
		if(inFilePtr.good())
		{
			printf("%c", c);
		}
	}
	return true;
}

bool FileSystem::writeFile(char data)
{
	if(!outFilePtr.is_open())
	{
		return false;
	}
	outFilePtr.write(&data, sizeof(char));
	return true;
}
