#ifndef ADVANCED_H
#define ADVANCED_H
#include <semaphore.h>
#include <list>


extern pthread_mutex_t Lock_FileNameQueue;
extern sem_t sem_FileName;
extern std::list<char*> Queue_FileName;//文件名队列,输入线程将文件名存放到里面
// 多线程
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


#endif