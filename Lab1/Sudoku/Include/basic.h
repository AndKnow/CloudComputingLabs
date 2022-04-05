#ifndef BASIC_H
#define BASIC_H
#include <list>
#include <semaphore.h>

const int DEBUG = 0;

// 通用工具和处理
    // 初始化
void init();
    // 保存当前时间,或者计算时间差
enum TimeOption{TIME_SAVE = 0, TIME_CALC};
void now(TimeOption flag); 

    // 输出错误原因,并且退出
void handler_error(const char *message);

// 基础要求
    // 接受输入文件名,读取内容后输出
const int SIZE_FILE_NAME = 64;
char* wait_Intput(FILE* in);
    // 根据文件名读取数据,并且输出验证
const int SIZE_SINGLE_LINE = 82;//每行数据大小
const int SIZE_LINE_NUM = 10;//一次读取多少行
void read_File(const char *fileName, char **problem);
    // 封装解决数独函数,未解决
void sudoku_Solve(int);
    // 利用数组解决输出顺序问题
const int BUFFER_PROBLEMS = 1028;//缓存区能放4096条任务或者答案
enum output{assigned = 0, is_solved, sent };
extern int flag_output_done ;//判断是否结束输出
extern int flag_output_run ; //启动标志
extern char *data_buffer[BUFFER_PROBLEMS];//存放问题或者答案,一次最多解决1024个问题
extern output outputStatus[BUFFER_PROBLEMS];//存放每个位置的状态
void* sorted_output(void*);
    // 动态分配任务编码
struct FreeAry {
  // 代表缓冲数组里面,begin 到 end 是可以分配的的位置
  int begin; 
  int end;
  FreeAry(int a = 0, int b = 0):begin(a), end(b) {}
};

// .h文件中使用的标准库类型,需要指明,以及头文件
extern pthread_mutex_t Lock_FreeAry;
extern sem_t sem_FreeAry;
extern std::list<FreeAry> Queue_FreeAry;//可用编码队列
    //从可用编码队列获取数据后进行赋值
void read_File(const char *fileName); 
    //从可用编码队列获取号码
void takepos_fromqueue(int &a, int &b);//
enum putWhere{begin = 0, end};
void putPos_toQueue(int a, int b, int where);//
    //输出调试使用
void handler_SIGINT(int);

#endif