#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <stdlib.h> 
#include <stdio.h>      
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h> 
#include <fcntl.h>
#include <list>
using std::list;


#include "basic.h"
#include "sudoku.h"


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

  while(true) {

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

void sudoku_Solve(int pos) {
  int board[N]; 
  printf("problem:");
  
  for(int i = 0;i < SIZE_SINGLE_LINE - 1; i++) {
    board[i] = data_buffer[pos][i] - '0';
    printf("%d", board[i]);
  }

  solve_sudoku_dancing_links(board);
  outputStatus[pos] = is_solved;

  printf("\nanswer :");
  for(int i = 0;i < SIZE_SINGLE_LINE - 1; i++) {
    data_buffer[pos][i] = board[i] + '0';//这里先复制,后面再看看
    printf("%d", board[i]);
  }
  printf("\n");
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

void sorted_output() {
  static int pos_now = 0;
  while(!flag_output_done) {
      int pos_be = before(pos_now);
      int pos_af = after(pos_now);

      while(outputStatus[pos_be] != sent || outputStatus[pos_now] != is_solved) {
        sleep(1);
        printf("Output waiting....\n");
        if(flag_output_run){//初次启动
          flag_output_run = 0;
          break;
        }
      } 
      //printf("solved :%s\n", data_buffer[pos_now]);
      outputStatus[pos_now] = sent;
      pos_now = pos_af;
      // 将缓存分成两部分(当前这么处理),
      if(pos_now == BUFFER_PROBLEMS/2 ){
        printf("将号码存入队列:%d -> %d\n", pos_now, BUFFER_PROBLEMS);
        Queue_FreeAry.push_back(FreeAry(pos_now, BUFFER_PROBLEMS));
      }else if(pos_now == 0){
        printf("将号码存入队列:%d -> %d\n", pos_now, BUFFER_PROBLEMS/2);
        Queue_FreeAry.push_back(FreeAry(pos_now, BUFFER_PROBLEMS/2));
      }
  }
}

// 用来保存可分配的数字号
list<FreeAry> Queue_FreeAry;
void takepos_fromqueue(int &pos_begin, int &pos_end){
  if(!Queue_FreeAry.empty()){
    pos_begin = Queue_FreeAry.front().begin;
    pos_end = Queue_FreeAry.front().end;
    Queue_FreeAry.pop_front();
  }
  if(DEBUG) {
    printf("从队列中取出号码:%d -> %d\n", pos_begin, pos_end);
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

    // 如果内层循环先结束,那么就去重新拿可用数组位置
    while(pos_now < pos_end) {

      // buffer作为指针,不分配内存只是赋值,永远只能覆盖
      buffer = (char*)malloc(SIZE_SINGLE_LINE);
      // 但是我为啥需要buffer呢?直接传到那个数组里面不就好了吗

      // 如果此处fgets,传入的参数是sizeof(buffer),只会传递指针的长度
      if(fgets((char*)data_buffer[pos_now], SIZE_SINGLE_LINE, file) != NULL){
        len_read = strlen((char*)buffer);
        int ch = fgetc(file);//接受掉换行符
        
        printf("data_buffer%d: %s \n",pos_now, data_buffer[pos_now]);
        // for(int i = 0; i <SIZE_SINGLE_LINE - 1; i++) {
        //   data_buffer[pos_now][i] = buffer[i] - '0';
        //   printf("%d", data_buffer[pos_now][i]);
        // } printf("\n");
        outputStatus[pos_now] = assigned;//该位置已经被分配
        // printf("buffer : %s \n",buffer);
        //printf("data_buffer%d: %s \n",pos_now, data_buffer[pos_now]);
      }else {
        flag_eof = 1;//说明此时该文件已经没有数据了
        if(DEBUG) {
            printf("将号码存入队列:%d -> %d\n", pos_now, pos_end);
        }
        //没用完的号码存回去,需要放到队列的前面
        Queue_FreeAry.push_front(FreeAry(pos_now,pos_end));
        break;
      }

      pos_now++;
    }


  }  
}