#include <bits/stdc++.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>


#include "sudoku.h"
#include "basic.h"
#include "advanced.h"

using std::list;

void now(TimeOption flag)
{
  // flag标记当前是记录时间点,还是输出时间点
  static timeval time_record;
  static long int time_first;
  static long int time_second;


  if(flag == TIME_SAVE) {
    gettimeofday(&time_record, NULL);
    time_first = time_record.tv_sec * 1000000 + time_record.tv_usec;
  } 
  else if(flag == TIME_CALC) {

    gettimeofday(&time_record, NULL);
    time_second = time_record.tv_sec * 1000000 + time_record.tv_usec;

    printf("ALL TIME : %ldms \n", (time_second - time_first) / 1000);
    time_first = time_second;
  } 

}

void handler_error(const char *message) {
    perror(message);
    exit(-1);
}

// 阻塞等待文件名输入
// C语言中返回字符串的方法?没那么简单
char* wait_Intput(FILE* fileIn) {
    char *file_name;
    file_name = (char *)malloc(SIZE_FILE_NAME);

    fgets(file_name ,SIZE_FILE_NAME, fileIn);
    file_name[strlen(file_name) - 1] = '\0';
    
    printf("Received File Name : %s \n", file_name);
    return file_name;
}

// 根据文件名读取数据,并且输出以验证
void read_File(const char *fileName, char **problem) {
  if(DEBUG) {
    printf("open file : %s \n",fileName);
  }

  FILE *file = fopen(fileName, "r");
  if(file == NULL) { 
    handler_error("open()");
  }

  int len_read = 0;
  int cnt_read = 0;
  char buffer[SIZE_SINGLE_LINE];
  // buffer作为指针,不分配内存只是赋值,永远只能覆盖
  //buffer = (char*)malloc(SIZE_SINGLE_LINE);
  // 但是我为啥需要buffer呢?直接传到那个数组里面不就好了吗

  while(true) {
     // 如果此处fgets,传入的参数是sizeof(buffer),只会传递指针的长度
    if(fgets((char*)buffer, sizeof(buffer), file) != NULL){
      // 此处输出将换行符一并接收
      len_read = strlen((char*)buffer);
      int ch = fgetc(file);//接受掉换行符
      printf("len_read: %d \n",len_read);
    }else{
      len_read = 0;
    }

    if(len_read > 0) {
      *problem = buffer;
      printf("Data Received %d :%s\n", cnt_read++, (char*)buffer);
    }else if(len_read == 0){
      printf("Data acceptance completed......\n");
      break;
    }

  }
}

// 这里有个问题没解决,缓存可以用int类型的,直接修改,不用复制
void sudoku_Solve(int pos) {
  int board[N]; 
  
  for(int i = 0;i < SIZE_SINGLE_LINE - 1; i++) {
    board[i] = data_buffer[pos][i] - '0';
  } 
  // 有点问题,输出的问题和答案结果一样
  // printf("problem :%s\n", data_buffer[pos]);

  solve_sudoku_dancing_links(board);
  outputStatus[pos] = is_solved;
  // printf("problem %d is solved :%s\n", pos, data_buffer[pos]);
 
  for(int i = 0;i < SIZE_SINGLE_LINE - 1; i++) {
    data_buffer[pos][i] = board[i] + '0';//这里先复制,后面再看看
  }
  // printf("answer %d:%s\n", pos, data_buffer[pos]);
}


int flag_output_done = 0;//判断是否结束输出
int flag_output_run = 0; //启动标志
char *data_buffer[BUFFER_PROBLEMS];//存放问题或者答案,一次最多解决1024个问题
output outputStatus[BUFFER_PROBLEMS];//存放每个位置的状态

int before(int now) {
  if(now == 0)
    return BUFFER_PROBLEMS - 1;
  return now -1;
}
int after(int now) {
  if(now == BUFFER_PROBLEMS - 1)
    return 0;
  return now + 1;
}

int total = 0;
// 如果当前输出完,而下一个位置是未分配,情况会怎么样
void* sorted_output(void*) {
  static int pos_now = 0;
  static int pos_be;
  static int pos_af;
  while(!flag_output_done) {
      // sleep(0.1);
      // printf("1\n");
      pos_be = before(pos_now);
      pos_af = after(pos_now);

      while(outputStatus[pos_be] != sent || outputStatus[pos_now] != is_solved) {
        // sleep(1);
        // printf("2\n");
        if(flag_output_run && outputStatus[pos_now] == is_solved){
          //标志位启动,无需等待,但是要等问题解决完
          flag_output_run = 0;
          break;  
        } if(DEBUG)
          printf("pow_now :%d,output waiting......\n",pos_now);
      }  

      //printf("solved and output %d :%s,total :%d\n ", pos_now, data_buffer[pos_now], total++);
      outputStatus[pos_now] = sent;
       
      if(DEBUG) { 
        printf("%d,%d,%d:%d, %d, %d\n", pos_be, pos_now, pos_af,
          outputStatus[pos_be], outputStatus[pos_now], outputStatus[pos_af]);
      }   
      pos_now = pos_af; 
   
      if(!(total++ % 100000)){
        printf("total :%d\n", total);
        now(TIME_CALC);
      }
      // 将缓存分成两部分(当前这么处理),
      // 这里存在一个问题,输出之后,把下标放到可用队列里面
      // 还没有到达下一轮之前,位置已经被分配出去,里面的状态sent可能被覆盖
      if(pos_now == BUFFER_PROBLEMS/2 ){
        putPos_toQueue(0, BUFFER_PROBLEMS/2, end);
        flag_output_run = 1;

      }else if(pos_now == 0){
        putPos_toQueue(BUFFER_PROBLEMS/2, BUFFER_PROBLEMS, end);
        flag_output_run = 1;
      }
  
  }
  return 0;
}

// 用来保存可分配的数字号 
list<FreeAry> Queue_FreeAry;
pthread_mutex_t Lock_FreeAry;
sem_t sem_FreeAry;

// 封装线程同步
void takepos_fromqueue(int &pos_begin, int &pos_end){
    sem_wait(&sem_FreeAry);
    pthread_mutex_lock(&Lock_FreeAry);

    if(!Queue_FreeAry.empty()) {
      // for(auto it = Queue_FreeAry.begin(); it != Queue_FreeAry.end(); it ++) {
      //   printf("当前可用号码队列,%d -> %d\n", it->begin, it->end);
      // }
      pos_begin = Queue_FreeAry.front().begin;
      pos_end = Queue_FreeAry.front().end;
      Queue_FreeAry.pop_front();
    }

    pthread_mutex_unlock(&Lock_FreeAry);
 
  if(DEBUG) {
    printf("从缓存队列中取出号码:%d -> %d\n", pos_begin, pos_end);
  }
}

void putPos_toQueue(int a, int b,int where) {
    pthread_mutex_lock(&Lock_FreeAry);

    if(where == begin)
      Queue_FreeAry.push_front(FreeAry(a, b));
    else if(where == end)
      Queue_FreeAry.push_back(FreeAry(a, b));

    pthread_mutex_unlock(&Lock_FreeAry);
    sem_post(&sem_FreeAry);

  if(DEBUG) {
    printf("将号码存入缓存队列 :%d -> %d\n", a, b);
  }
}

void takepos_fromTask(int &pos_begin, int &pos_end){
    sem_wait(&sem_Task);
    pthread_mutex_lock(&Lock_TaskQueue);

    if(!Queue_Task.empty()) {
      pos_begin = Queue_Task.front().begin;
      pos_end = Queue_Task.front().end;
      Queue_Task.pop_front();
    }

    pthread_mutex_unlock(&Lock_TaskQueue);

  if(DEBUG) {
    printf("从任务队列中取出号码:%d -> %d\n", pos_begin, pos_end);
  }
}

void putPos_toTask(int a, int b,int where) {
    pthread_mutex_lock(&Lock_TaskQueue);

    if(where == begin)
      Queue_Task.push_front(FreeAry(a, b));
    else if(where == end)
      Queue_Task.push_back(FreeAry(a, b));

    pthread_mutex_unlock(&Lock_TaskQueue);
    sem_post(&sem_Task);

  if(DEBUG) {
    printf("将号码存入任务队列 :%d -> %d\n", a, b);
  }
}
                                                                           
void read_File(const char *fileName){
  if(DEBUG) {
    printf("open file : %s \n",fileName);
  }

  FILE *file = fopen(fileName, "r");
  if(file == NULL) { 
    handler_error("open()");
  }

  int len_read, pos_begin, pos_end, pos_now;
  len_read = pos_begin = pos_end = pos_end = 0;
  char *buffer;

  int flag_eof = 0;
  // 只有读完当前的文件之后,才关闭这个输入
  while(true && !flag_eof) {
    takepos_fromqueue(pos_now, pos_end);
    int task_begin = pos_now;
    int task_cnt = 0;
    // 如果内层循环先结束,那么就去重新拿可用数组位置
    while(pos_now < pos_end) {


      if(fgets((char*)data_buffer[pos_now], SIZE_SINGLE_LINE, file) != NULL){
        // len_read = strlen((char*)buffer);
        int ch = fgetc(file);//接受掉换行符
        task_cnt++;
        outputStatus[pos_now] = assigned;//该位置已经被分配
        if(DEBUG) {
          printf("data_buffer%d: %s,\n", pos_now, data_buffer[pos_now]);
          printf("now  status %d :%d\n", pos_now,outputStatus[pos_now]);
          printf("next status %d :%d\n", after(pos_now),outputStatus[after(pos_now)]);
        }
        if(task_cnt == SIZE_BATCH) {
          putPos_toTask(task_begin,task_begin + task_cnt, end);
          task_begin = task_begin + task_cnt;
          task_cnt = 0;
        }

      }else {
        flag_eof = 1;//说明此时该文件已经没有数据了
        //没用完的号码存回去,需要放到队列的前面
        putPos_toQueue(pos_now, pos_end, begin);
        putPos_toTask(task_begin,task_begin + task_cnt, end);
        break;
      }

      pos_now++;
    }
    //这里代表取的号码被使用结束了,而文件没有读取结束
    putPos_toTask(task_begin,task_begin + task_cnt, end);


  }  
}



void init() {
  
  set_async(); //设置异步输入,接收文件名

  // 分配可用的空闲数据缓存区
  Queue_FreeAry.push_back(FreeAry(0,BUFFER_PROBLEMS/2));
  Queue_FreeAry.push_back(FreeAry(BUFFER_PROBLEMS/2, BUFFER_PROBLEMS));
  Lock_FreeAry = PTHREAD_MUTEX_INITIALIZER;
  sem_init(&sem_FreeAry, 0, 2);//一开始有两个资源

  // 分配缓存区资源
  for(int i = 0; i < BUFFER_PROBLEMS; i++) {
    data_buffer[i] = (char*)malloc(SIZE_SINGLE_LINE);
  }

  // 线程及同步初始化
    //输入处理线程
  Lock_FileNameQueue = PTHREAD_MUTEX_INITIALIZER;
  sem_init(&sem_FileName, 0, 0);
  pthread_create(&thread_appendTask, NULL, thread_AppendTask, nullptr);
  pthread_detach(thread_appendTask);
    //处理工作线程
  Lock_TaskQueue = PTHREAD_MUTEX_INITIALIZER;
  for(int i = 0; i < nums_workerThreads; i++) {
    pthread_create(&threads_worker[i], NULL, thread_Worker, nullptr);
    pthread_detach(threads_worker[i]);
  }
  sem_init(&sem_Task, 0, 0);
    //处理输出线程
  flag_output_run = 1;//不等待启动标志
  pthread_create(&thread_output, NULL, sorted_output, nullptr);
  pthread_detach(thread_output);
  

}
