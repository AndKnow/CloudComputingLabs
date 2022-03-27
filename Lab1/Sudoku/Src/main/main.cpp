#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <string>
#include <sys/time.h>
#include <assert.h>
#include <stdint.h>
#include "sudoku.h"
#include "basic.h"

using std::string;


// 记得配套导图一起看
int main(int argc, char* argv[])
{
  now(TIME_SAVE);
  char *file_path,*path_prefix;
  Queue_FreeAry.push_back(FreeAry(0,BUFFER_PROBLEMS/2));
  Queue_FreeAry.push_back(FreeAry(BUFFER_PROBLEMS/2, BUFFER_PROBLEMS));
  for(int i = 0; i < BUFFER_PROBLEMS; i++) {
    data_buffer[i] = (int*)malloc(SIZE_SINGLE_LINE);
  }

  while (1) {
    file_path = wait_Intput(stdin);
    path_prefix = (char*)malloc(16);
    strcpy(path_prefix, "./test_answer/");//设置路径前缀

    if(DEBUG) {
      strcat(path_prefix, file_path);
      read_File(path_prefix);
    }else
      read_File(file_path);
    free(file_path);

    sudoku_Solve(0);//解决某个位置的问题
    sudoku_Solve(1);
    sudoku_Solve(2);
    sudoku_Solve(3);

    flag_output_run = 1;
    sorted_output();
    // printf("data_buffer :%s\n",data_buffer[0]);
    // printf("data_buffer :%s\n",data_buffer[1]);
      // sudoku_Solve(problems);
      // printf("solved :%s \n", problems);
  }

  now(TIME_CALC);
  return 0;
}