// #include <stdlib.h>
// #include <cstdio>
// #include <cstring>
// #include <sys/types.h>
// #include <list>
// #include <signal.h>
#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>

#include "basic.h"
#include "advanced.h"

using std::list;

void set_async() {
    fcntl(0, F_SETOWN, getpid());

    int flags = fcntl(0, F_GETFL);
    flags |= O_ASYNC;    
    fcntl(0, F_SETFL, flags);

    signal(SIGIO, async_input);
}

void async_input(int sig) {
    if(sig == SIGIO) {
        char *file_name = wait_Intput(stdin);
        char *path_prefix = (char*)malloc(16);
        strcpy(path_prefix, "./test_answer/");//设置路径前缀

        if(DEBUG) {
            strcat(path_prefix, file_name);
            strcpy(file_name, path_prefix);
        }

        printf("文件名队列新加入 :%s\n", file_name);

        pthread_mutex_lock(&Lock_FileNameQueue);
        Queue_FileName.push_back(file_name);
        pthread_mutex_unlock(&Lock_FileNameQueue);
        sem_post(&sem_FileName);

    }

}

//文件名队列的互斥量
list<char*> Queue_FileName;
pthread_mutex_t Lock_FileNameQueue;
sem_t sem_FileName;

char* takeFileName_FromQueue() {
    char *filename;

    sem_wait(&sem_FileName);
    pthread_mutex_lock(&Lock_FileNameQueue);
    filename = Queue_FileName.front();
    Queue_FileName.pop_back();
    pthread_mutex_unlock(&Lock_FileNameQueue);

    if(DEBUG) {
        printf("从文件名队列中拿取文件名 :%s\n", filename);
    }
    return filename;
}


int flag_AppendDone = 0;
pthread_t thread_appendTask;
void* thread_AppendTask(void* para) {
    char *fileName;
    while(!flag_AppendDone) {
        fileName = takeFileName_FromQueue();//封装了信号量等待和互斥量
        read_File(fileName);
    }

    return 0;
}