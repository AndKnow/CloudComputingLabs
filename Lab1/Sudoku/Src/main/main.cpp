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
  char* file_path;
  Queue_FreeAry.push_back(FreeAry(0,BUFFER_PROBLEMS/2));
  Queue_FreeAry.push_back(FreeAry(BUFFER_PROBLEMS/2, BUFFER_PROBLEMS));

  while (1) {
    file_path = wait_Intput(stdin);
    read_File("./test_answer/test1000");
    free(file_path);
    // printf("data_buffer :%s\n",data_buffer[0]);
    // printf("data_buffer :%s\n",data_buffer[1]);
      // sudoku_Solve(problems);
      // printf("solved :%s \n", problems);
  }

  now(TIME_CALC);
  return 0;
}
