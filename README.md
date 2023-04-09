## Linux信号量操作
### 目录

 1. 信号量作用
 2. 函数
 3. 示例程序
 
 ### 1. 信号量作用
保护共享资源，使得资源在一个时刻只有一个进程（线程）所拥有。信号量的值为正(在程序中是sem.val)的时候，说明它空闲。所测试的线程可以锁定而使用它。若为 0，说明它被占用，测试的线程要进入睡眠队列中，等待被唤醒。
###  2. 函数
#### (1)int semid = semget( key, 1, 0640 )
key :  信号量的键值，一般用十六进制
num = 1: 信号量的数量
0646 ： 使用权限
返回值 semid 信号量的id
若信号量已经存在，返回当前信号量的id
若不存在，错因保存在errno中，errno=2，semid返回-1，不主动创建，只有当权限含有IPC_CREAT则主动创建。
#### (2) int semctl(int semid , int num, int cmd, ···）
semid 信号量的id
num：信号量集的下标，表示一个信号量时，填0
cmd: 命令种类，常用两个
ICP_RMID 销毁信号量，不用第四个参数
SETVAL: 初始化，第四个参数是自定义共同体，如下：

```cpp
union semun   // 用于信号量默认的共同体
		{
		  int val;
		  struct semid_ds *buf;
		  unsigned short int *array;
		  struct seminfo *__buf;
		};
```
销毁

```cpp
semctl(semid,0,IPC_RMID)
```
初始化

```cpp
union semun sem_union;
			sem_union.val = 1;
			if( (semctl(semid,0,SETVAL,sem_union) ) <0) { perror("init semctl()"); return 0;}
```
#### （3）int semop( int semid, struct sembuf *sops, unsigned nsops)
功能：
1)等待信号量的值变为1，如果等待成功，立即把信号量的值置为0，这个过程也称之为等待锁；
2)把信号量的值置为1，这个过程也称之为释放锁。
#### 机制：
若shortsem_op== -1, 当信号量小于1时继续等待，等信号量大于1时立刻减1
若shortsem_op== 1，对信号量加1

第三个参数  nsops是操作信号量的个数，即sops结构变量的个数，设置它的为1（只对一个信号量的操作）
第二个参数sops 是一个结构体，如下：

```cpp
struct sembuf
{
	short sem_num;  /信号量集的个数，单个信号量设置为0。
	shortsem_op;  /信号量在本次操作中需要改变的数据：-1-等待操作；1-发送操作。
	shortsem_flg;  /把此标志设置为SEM UNDO,操作系统将跟踪这个信号量。
	// 如果当前进程退出时没有释放信号量，操作系统将释放信号量，避免资源被死锁。
};
```
示例
（1 等待信号量变为1，此时资源空闲，立即将信号量变为0，占用

```cpp
struct sembuf sem_b;
sem_b.sem_num = 0;
sem_b.sem_op = -1;
sem_b.sem_flg = SEM_UNDO;
semop(semid, &sem_b, 1);
```
(2 释放资源，把信号量置1
```cpp
struct sembuf sem_b;
sem_b.sem_num = 0;
sem_b.sem_op = 1;
sem_b.sem_flg = SEM_UNDO;
semop(semid, &sem_b, 1);
```
###  3.示例程序
（1 mysemaphore.cpp 创建信号量，并使用
（2  mysemaphore.h  ShareMemoryClock.cpp  创建共享内存并加信号量锁
编译： g++ -o ShareMemoryClock ShareMemoryClock.cpp mysemaphore.h

