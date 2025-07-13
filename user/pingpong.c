#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int ptoc[2], ctop[2];
    if (pipe(ptoc) < 0 || pipe(ctop) < 0){
        fprintf(2, "pingpong: pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if(pid < 0){
        fprintf(2, "pingpong: fork failed\n");
        exit(1);
    }
    if(pid != 0){
        write(ptoc[1], "ping", 4);
        close(ptoc[1]);

        // 从子进程读取字符
        char buf[5];
        read(ctop[0], buf, 4);
        buf[4] = '\0';
        printf("%d: received pong\n", getpid());
        wait(0);
    }else{
        // 从父进程读取字符
        char buf[5];
        read(ptoc[0], buf, 4);
        buf[4] = '\0';
        printf("%d: received ping\n", getpid());

        // 写入父进程
        write(ctop[1], "pong", 4);
        close(ctop[1]);
    }

    close(ptoc[0]);
    close(ctop[0]);
    exit(0);
}