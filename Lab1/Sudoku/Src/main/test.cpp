#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <stdlib.h>


char* problems[1024];
void aread() {
    problems[0] = (char*)malloc(1);
    strcpy(problems[0], "Hello World!");
}
int main(){
    aread();
    printf("%s\n",problems[0]);
    return 0;
}