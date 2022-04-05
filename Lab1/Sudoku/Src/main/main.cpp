#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <string>
#include <sys/time.h>
#include <signal.h>
#include <assert.h>
#include <stdint.h>


#include "sudoku.h"
#include "basic.h"
#include "advanced.h"


// 记得配套导图一起看
int main(int argc, char* argv[])
{
  init();
  now(TIME_SAVE);
  while(1) {}
  now(TIME_CALC);
  return 0;
} 