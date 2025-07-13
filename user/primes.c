#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 筛选质数，每次都需要除以第一个传入该进程的质数，看是否能整除
void sieve(int pp[2]) {
    int p;
    read(pp[0], &p, sizeof(p));     // sizeof(p) = 4
    if(p == -1){
        exit(0);  // 如果读到-1，说明没有质数了，退出
    }
    printf("prime %d\n", p);  // 打印当前质数

    int pc[2];
    pipe(pc);  // 创建管道，用于传递筛选后的质数

    if(fork() == 0) {
        // 子进程，继续筛选
        close(pp[0]);  // 关闭父进程的读端
        close(pc[1]);  // 关闭子进程的写端
        sieve(pc);     // 递归调用筛选函数
    } else {
        // 父进程，筛选质数
        close(pc[0]);  // 关闭子进程的读端
        int x;
        while(read(pp[0], &x, sizeof(x)) > 0 && x != -1) {
            if(x % p != 0) {  // 如果x不是当前质数p的倍数
                write(pc[1], &x, sizeof(x));  // 写入筛选后的质数
            }
        }

        // 接收到-1后，表示没有更多的质数了
        x = -1;  // 发送-1表示结束
        write(pc[1], &x, sizeof(x));  // 写入-1给子进程
        wait(0);  // 等待子进程结束
        exit(0);  // 退出父进程
    }
}

int main(int argc, char *argv[]) {
    int p[2];
    pipe(p);  // 创建管道，用于传递质数

    if(fork() == 0) {
        // 子进程，筛选质数
        close(p[1]);  // 关闭父进程的写端
        sieve(p);     // 调用筛选函数
        exit(0);  // 正常退出子进程
    } else {
        // 父进程，生成质数
        close(p[0]);  // 关闭子进程的读端
        for(int i = 2; i <= 35; i++) {
            write(p[1], &i, sizeof(i));  // 写入质数候选
        }
        int end = -1;
        write(p[1], &end, sizeof(end));  // 写入-1表示结束
    }
    wait(0);  // 等待子进程结束
    exit(0);  // 正常退出
}