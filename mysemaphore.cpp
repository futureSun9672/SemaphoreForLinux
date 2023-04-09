// mysemaphore.cpp

#include <cstdio> // 含perror()
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>  // 含errno
#include <unistd.h>

class CSEM
{
	private:
		union semun   // 用于信号量默认的共同体
		{
		  int val;
		  struct semid_ds *buf;
		  unsigned short int *array;
		  struct seminfo *__buf;
		};
		int semid;
	public:
		bool init(key_t key );
		bool wait();
		bool post();
		bool destroy();
};

int main()
{
	CSEM sem;
	//初始信号灯
	if(sem.init(0x400)==false) { printf("sem.init failed.\n"); return -1; }
	printf("sem.init ok\n");
	
	// 等待信号灯挂出，等待成功后将持有锁
	if(sem.wait()==false) { printf("sem.wait failde.\n"); return -1;}
	printf("sem.wait ok\n");
	sleep(40); //等待过程中mysemaphore将持有锁；
	
	// 挂出信号灯，释放锁
	if(sem.post() == false) { printf("sem.post failde.\n"); return -1;}
	printf("sem.post ok\n");
	
	//销毁信号灯
	// if(sem.destroy() == false) { printf("sem.destroy failde.\n"); return -1;}
	// printf("sem.destroy ok\n");
	
}

bool CSEM::init(key_t key)
{
	//获取信号灯
	if( (semid = semget(key, 1, 0640)) == -1 )  //此时信号量未创建
	{
		// 如果信号量不存在，创建
		if(errno==2)   // errno==2，错误类型：没有该文件或文件夹
		{
			if( (semid = semget(key, 1, 0640|IPC_CREAT)) == -1 )
			{
				perror("init 1 semget()");
				return false;		
			}
			// 创建成功后初始化
			union semun sem_union;
			sem_union.val = 1;
			if( (semctl(semid,0,SETVAL,sem_union) ) <0) { perror("init semctl()"); return 0;}
			
		}
		else
		{
			perror("init 2 semget()"); return false;
		}
	}
	return true;
}

bool CSEM:: destroy()
{
	if( semctl(semid,0,IPC_RMID) == -1 ) { perror("wait semop()"); return false;}
	return true;
}

bool CSEM:: wait()
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1;
	sem_b.sem_flg = SEM_UNDO;
	if( semop(semid, &sem_b, 1) == -1) {perror("wait semop()"); return false;}
	return true;
}

bool CSEM::post()
{
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1;
	sem_b.sem_flg = SEM_UNDO;
	if( semop(semid, &sem_b, 1) == -1){perror("post semop()"); return false;}
	return true;
}

