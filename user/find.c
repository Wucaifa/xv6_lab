#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 递归查找，查找path路径下的目标文件
void find(char *path, char *name) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    
    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    
    switch (st.type) {
        case T_FILE:
        if (strcmp(path + strlen(path) - strlen(name), name) == 0) {
            printf("%s\n", path);
        }
        break;
    
        case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);  // 指向路径末尾
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0)
                continue;
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = '\0'; // 确保字符串以null结尾
            if(stat(buf, &st) < 0) {
                fprintf(2, "find: cannot stat %s\n", buf);
                continue;
            }
            if (st.type == T_DIR) {
                // 如果是目录，递归查找
                find(buf, name);
            } else if (st.type == T_FILE) {
                // 如果是文件，检查是否匹配
                if (strcmp(buf + strlen(buf) - strlen(name), name) == 0) {
                    printf("%s\n", buf);
                }
            }
        }
        break;
    
        default:
        break;
    }
    
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <path> <name>\n");
        exit(1);        // 异常退出
    }
    
    char *path = argv[1];
    char *name = argv[2];
    
    // 开始查找
    find(path, name);
    
    exit(0);
}