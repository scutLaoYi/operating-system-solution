/*

   Copyright (C) 2013, scutLaoYi

   This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
website: http://www.gnu.org/licenses/old-licenses/gpl-2.0.html

   Contact scutLaoYi: HelloWorld636596@126.com
 */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;

/*
   Linux 命令：
   ipcs 显示和管理ipc相关数据
   ipcrm 释放相关数据空间
 */

/*
   Linux 系统函数：
   fork()返回俩值，子进程中返回0， 父进程中返回子进程id
   getpid()返回当前进程的id
   getppid()返回当前进程父进程id

   shmget 获取一个新的或者已存在的共享内存空间id
   shmat 连接到对应id的内存空间，映射到进程虚拟内存地址上，成功则返回该地址指针。
   shmdt 断开与对应内存空间的链接，取消映射。
   shmctl 可对共享内存空间进行管理，检测状态、删除等

   联合semun 用于处理信号量的相关数据
   semget 用于获取对应key的信号量集合id
   semctl 提供对信号量集合的控制
   semop 对特定信号量进行P/V操作
 */

#define SHM_NUM 9876
#define SEM_NUM 9876
#define SEM_AMOUNT 3
#define SEM_MUTEX 0
#define SEM_EMPTY 1
#define SEM_FULL 2
#define SEM_EMPTY_AMOUNT 10
#define SEM_FULL_AMOUNT 0
#define BUFFER_SIZE 10
#define MAX_SLEEP_LENGTH 5
const char RANDOM_BASE = 'A';
const int RAMDOM_SIZE = 26;

union semun
{
	int val;/*value for SETVAL*/
	struct semid_ds *buf;/*buffer for IPC_STAT&IPC_SET*/
	ushort *array;/*array for GETALL&SETALL*/
	struct seminfo *__buf;/*buffer for IPC_INFO*/
	void *__pad;
};

//该函数用于初始化信号量,sid为信号量集合id，index为索引值，value为待设定的数值
void initSem(int sid, int index, int value)
{
	semun semopts;
	semopts.val=value;
	semctl(sid,index,SETVAL,semopts);
	return;
}

//该函数用于修改特定信号量的值，sid为信号量集合id，index为信号量的索引值，value为待修改的数值，其中value>0表示增加，value<0表示降低
void modify(int sid, int index, int value)
{
	sembuf tbuf;
	tbuf.sem_num = index;
	tbuf.sem_op = value;
	tbuf.sem_flg = 0;
	semop(sid, &tbuf, 1);
}

void viewBuffer(void *ptr)
{
	int *counter = (int *)ptr;
	char *bufPtr = (char *)((long long)ptr + sizeof(int *) * 2);
	int cc(0);
	for(int i = counter[0]; i != counter[1]; i++, i%=BUFFER_SIZE )
	{
		printf("%c ", bufPtr[i]);
		++cc;
	}
	for(; cc < 15; ++cc)
		printf("  ");
	return;
}

//该函数用于从共享内存缓冲区中读取一个字符数据
char pullChar(int shmId)
{
	void *spacePtr = shmat(shmId, NULL, 0);
	viewBuffer(spacePtr); 

	if(spacePtr == (void *)-1)
	{
		printf("In function pullChar:attach to shared space failed!\n");
		return '?';
	}
	int *countPtr = (int *)spacePtr;
	char *bufPtr = (char *)((long long)spacePtr + sizeof(int *) * 2);
	char temp = bufPtr[countPtr[0]++];
	countPtr[0] %= BUFFER_SIZE;

	shmdt(spacePtr);
	return temp;
}

//该函数用于向共享缓冲区中写入一个字符数据
bool pushChar(int shmId, char cc)
{
	void *spacePtr = shmat(shmId, NULL, 0);
	viewBuffer(spacePtr);

	if(spacePtr == (void *)-1)
	{
		printf("In function pushChar:attach to shared space failed!\n");
		int number = errno;
		printf("errno:%d\n", number);
		printf("Message:%s\n", strerror(number));
		return false;
	}
	int *countPtr = (int *)spacePtr;
	char *bufPtr = (char *)((long long)spacePtr + sizeof(int *) * 2);
	bufPtr[countPtr[1]++] = cc;
	countPtr[1] %= BUFFER_SIZE;

	shmdt(spacePtr);
	return true;
}

void printTime()
{
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, "%T:\t",timeinfo);

	printf("%s", buffer);
	return;
}
//大作业一的主函数，包含了程序运行的基本流程，调用上述函数完成功能
int problemOne()
{
	//Creating a shared memory as buffer, and three semaphores
	int shmId = shmget(SHM_NUM, sizeof(char) * SEM_FULL_AMOUNT + sizeof(char *) * 2, IPC_CREAT | 0666);
	if(shmId < 0)
	{
		printf("Allocating shared space failed!\n");
		return -1;
	}
	printf("Allocating shared space success.\n");

	//initialization
	void *spacePtr = shmat(shmId, NULL, 0);
	if(spacePtr == (void *)-1)
	{
		printf("Attach pointer to the shared space failed!\n");
		shmctl(shmId, IPC_RMID, NULL);
		return -1;
	}
	int *countPtr = (int *)spacePtr;
	countPtr[0] = countPtr[1] = 0;
	shmdt(spacePtr);


	printf("Allocating semaphore with size 3...");
	int semId = semget(SEM_NUM, SEM_AMOUNT, IPC_CREAT | 0666);
	if(semId == -1)
	{
		printf("failed!\n");
		shmctl(shmId, IPC_RMID, NULL);
		return -1;
	}
	else
	{
		printf("OK.\n");
		initSem(semId, SEM_MUTEX, 1);
		initSem(semId, SEM_EMPTY, SEM_EMPTY_AMOUNT);
		initSem(semId, SEM_FULL, SEM_FULL_AMOUNT);
	}

	//Creating two producer and two consumer
	int pid[2];
	for(int i = 0; i < 2; ++i)
	{
		pid[i] = fork();
	}
	if(pid[1] == 0)
	{
		printf("Child process created!\n");
	}
	else
	{
		printf("Parent process created!\n");
	}
	sleep(2);

	/*Using two parents as producer, and two childs as consumer.*/
	if(pid[1] == 0)
	{
		int parentId = getppid();
		for(int t = 0; t < 10; ++t)
		{
			sleep((rand() + parentId) % MAX_SLEEP_LENGTH + 1);
			modify(semId, SEM_FULL, -1);
			//enter the critical region
			modify(semId, SEM_MUTEX, -1);
			printTime();
			char get = pullChar(shmId);
			if(get == '?')
			{
				printf("something wrong!\n");
			}
			modify(semId, SEM_MUTEX, 1);
			modify(semId, SEM_EMPTY, 1);
			printf("\tFrom consumer:\tget %c\n", get);
		}
	}
	else
	{
		int selfId = getpid();
		for(int t = 0; t < 10; ++t)
		{
			sleep((rand() + selfId) % MAX_SLEEP_LENGTH);
			char product = RANDOM_BASE + (rand()+selfId) % RAMDOM_SIZE;
			modify(semId, SEM_EMPTY, -1);
			modify(semId, SEM_MUTEX, -1);

			printTime();
			if(!pushChar(shmId, product))
			{
				printf("push char failed!Check your code again!\n");
				break;
			}

			modify(semId, SEM_MUTEX, 1);
			modify(semId, SEM_FULL, 1);
			printf("\tFrom producer:\tpush %c\n", product);
		}
	}

	//waiting for two consumer.
	if(pid[1] != 0)
	{
		waitpid(pid[1], NULL, 0);
	}

	//waiting for child process.
	if(pid[0] != 0 && pid[1] != 0)
	{
		waitpid(pid[0], NULL, 0);
		//sleep(10);
		//empty the trash.
		shmctl(shmId, IPC_RMID, NULL);
		semctl(semId, 0, IPC_RMID);
	}

	return 0;
}


int main(int argc, char *argv[])
{
	srand(time(0));
	problemOne();
	return 0;
}
