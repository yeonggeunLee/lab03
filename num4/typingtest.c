#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define PASSWORDSIZE 12

int main(void) {
    int fd;
    int nread, cnt = 0, errcnt = 0;
    char ch, text[] = "The magic thing is that you can change it.";
    struct termios init_attr, new_attr;

    fd = open(ttyname(fileno(stdin)), O_RDWR);
    tcgetattr(fd, &init_attr);
    new_attr = init_attr;
    new_attr.c_lflag &= ~ICANON;
    new_attr.c_lflag &= ~ECHO; /* ~(ECHO | ECHOE | ECHOK | ECHONL); */
    new_attr.c_cc[VMIN] = 1;
    new_attr.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &new_attr) != 0) {
        fprintf(stderr, "터미널 속성을 설정할 수 없음.\n");
        return 1;
    }

    printf("다음 문장을 그대로 입력하세요.\n%s\n", text);

    time_t start_time = time(NULL);

    while ((nread = read(fd, &ch, 1)) > 0 && ch != '\n') {
        if (ch == text[cnt++]) {
            write(fd, &ch, 1);
        } else {
            write(fd, "*", 1);
            errcnt++;
        }
    }

    time_t end_time = time(NULL);
    double elapsed_time = difftime(end_time, start_time);

    printf("\n타이핑 오류의 횟수는 %d\n", errcnt);
    printf("총 경과 시간: %.0f 초\n", elapsed_time);
    
    // 평균 분당 타자수 계산
    if (elapsed_time > 0) {
        double characters_per_minute = (cnt / elapsed_time) * 60;
        printf("평균 분당 타자수: %.2f\n", characters_per_minute);
    }

    tcsetattr(fd, TCSANOW, &init_attr);
    close(fd);

    return 0;
}

