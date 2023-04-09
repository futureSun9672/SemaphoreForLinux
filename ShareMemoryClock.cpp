/* ShareMemoryClock.cpp
用锁机制实现共享内存的互斥使用
*/

#include<mysemaphore.h>  // 编译时加入
#include <sys/types.h>
#include <unistd.h>


typedef struct worker
{
        int age;
        char number[15];
        char name[20];
}worker;

int main()
{
	// 1.获得内存id
	int shmid = shmget( (key_t)0x8000, sizeof(worker), 0666|IPC_CREAT); 
	if(shmid == -1)
	{
			printf("(key_t)0x8000 failed\n");
			return -1;
	}
	worker*  p = NULL; //指向共享变量
	// 2.获得内存物理地址
	p = (worker*)shmat(shmid, 0, 0);
	
	// 3.读写操作,加锁
	CSEM sem;
	//初始信号灯
	if(sem.init(0x400)==false) { printf("sem.init failed.\n"); return -1; }
	printf("sem.init ok 等待资源中···\n");
	
	// 等待信号灯挂出，等待成功后将持有锁
	if(sem.wait()==false) { printf("sem.wait failde.\n"); return -1;}
	printf("sem.wait ok 获得资源，使用中···\n");
	
	// read 
	printf("写入前：%d, %s, %s\n",p->age,p->number,p->name);
	//printf("进程id：%d\n",getpid());
	// write
	sleep(30);
	p->age = 28;
	strcpy(p->number,"13984553895");
	strcpy(p->name,"张宁智");
	// read
	printf("写入后：%d, %s, %s\n",p->age,p->number,p->name);
	
	// 解锁
	if(sem.post() == false) { printf("sem.post failde.\n"); return -1;}
	printf("sem.post ok 使用资源结束，释放！\n");
	
	//sleep(30);
	// 4.从进程移除内存
	//shmdt(p); 
	//int age = p->age;
	// 5.删除共享内存
	//shmctl(shmid,IPC_RMID,0);
	return 0;
}

