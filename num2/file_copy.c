#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#define MAX_READ 10

int main(int argc, char *argv[]) {
    int src_fd; /* source file descriptor */
    int dst_fd; /* destination file descriptor */
    char buf[MAX_READ];
    ssize_t rcnt; /* read count */
    ssize_t tot_cnt = 0; /* total write count */
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; /* == 0644 */

    if (argc < 3) {
        fprintf(stderr, "Usage: file_copy src_file dest_file\n");
        exit(1);
    }

    if ((src_fd = open(argv[1], O_RDONLY)) == -1 ){
        perror("src open"); /* errno에 대응하는 메시지 출력됨 */
        exit(1);
    }

    if ( (dst_fd = creat(argv[2], mode)) == -1 ){
        perror("dst open"); /* errno에 대응하는 메시지 출력됨 */
        exit(1);
    }

    while ( (rcnt = read(src_fd, buf, MAX_READ)) > 0) {
        tot_cnt += write(dst_fd, buf, rcnt);
    }

    if (rcnt < 0) {
        perror("read");
        exit(1);
    }
    
    printf("total write count = %d\n", tot_cnt);
    close(src_fd);
    close(dst_fd);
} 
