#ifndef ADVANCED_H
#define ADVANCED_H
#include <semaphore.h>
#include <list>


extern pthread_mutex_t Lock_FileNameQueue;
extern sem_t sem_FileName;
extern std::list<char*> Queue_FileName;//文件名队列,输入线程将文件名存放到里面
// 多线程
 //输入线程
    // 设置异步
void set_async();
    // 设置信号
void set_signal();
    // 异步事件处理输入
void async_input(int);
    // 专门处理任务产生的线程
extern int flag_AppendDone;
extern pthread_t thread_appendTask;
void* thread_AppendTask(void*);
    // 在队列里申请文件名
char* takeFileName_FromQueue();
 //工作线程
const int nums_workerThreads = 4;//工作线程数量
extern int flag_workerDone;
extern pthread_t threads_worker[nums_workerThreads];
//一个线程一次取的任务量,避免多次使用互斥量
const int SIZE_BATCH = 15;
void* thread_Worker(void*);
//管理任务队列
extern pthread_mutex_t Lock_TaskQueue;
extern sem_t sem_Task;
extern std::list<FreeAry> Queue_Task; 
void takepos_fromTask(int &pos_begin, int &pos_end);
void putPos_toTask(int a, int b,int where);

 //输出线程,不需要互斥量
extern pthread_t thread_output;
extern int total;
#endif