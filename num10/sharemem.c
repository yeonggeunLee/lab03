#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <errno.h>
#include <unistd.h>

#define SHMSIZE 4096
#define SHMNAME "/my_shm"
#define SEMNAME "/my_sem"

void p(sem_t *semd);
void v(sem_t *semd);

void main(int argc, char* argv[]) {
  FILE *fp, *copy_fp;
  char *path = argv[1]; 
  char buf[SHMSIZE]; 
  int status, shmd, count;
  void *shmaddr;
  pid_t pid;
  sem_t *semd;
  int i, val;

  if ((semd = sem_open(SEMNAME, O_CREAT, 0600, 1)) == SEM_FAILED) {
    perror ("sem_open 실패");
    exit (1);
  }

  sem_getvalue(semd, &val);
  memset(buf, 0, SHMSIZE);

  if((pid = fork()) == 0){

    if ((shmd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0666)) == -1) {
      perror ("shm_open 실패");
      exit (1);
    }

    if (ftruncate(shmd, SHMSIZE) != 0) {
      perror ("ftruncate 실패");
      exit (1);
    }

    if ((shmaddr = mmap(0, SHMSIZE, PROT_WRITE, MAP_SHARED, shmd, 0)) == MAP_FAILED) {
      perror ("mmap 실패");
      exit (1);
    }

    if((fp = fopen(path, "r")) == NULL){ // 파일 예외 처리
      perror("fopen");
      exit(1);
    }

    if (mlock(shmaddr, SHMSIZE) != 0) {
      perror ("mlock 실패");
      exit (1);
    }
    
    p(semd);
    printf("자식 프로세스 %d : 세마포어 사용 중\n", getpid());

    while(feof(fp) != 0){

      count = fread(buf, sizeof(char), SHMSIZE, fp);
      strcpy((char *)shmaddr, buf);      

    }
    v(semd);
    printf("자식 프로세스 %d : 세마포어 반환\n", getpid());
    
    if (munmap(shmaddr, SHMSIZE) == -1) {
      perror ("munmap 실패");
      exit (1);
    }

    fclose(fp);
    close(shmd);

    exit(0);
  } else if (pid > 0){
    pid = wait(&status);
    
    if ((shmd = shm_open(SHMNAME, O_RDWR, 0666)) == -1) {
      perror ("shm_open 실패");
      exit (1);
    }
  
    if ((shmaddr = mmap(0, SHMSIZE, PROT_READ, MAP_SHARED, shmd, 0)) == MAP_FAILED) {
      perror ("mmap 실패");
      exit (1);
    }
  
    if (mlock(shmaddr, SHMSIZE) != 0) {
      perror ("mlock 실패");
      exit (1);
    }

    char copy_path[256]; // 경로의 최대 길이를 적절히 가정
    strcpy(copy_path, path); // 원래 경로를 덮어쓰지 않도록 복사합니다.
    strcat(copy_path, "_copy");

    if((copy_fp = fopen(copy_path, "w")) == NULL){
      perror("fopen");
      exit(1);
    }

    p(semd);
    printf("부모 프로세스 %d : 세마포어 사용 중\n", getpid());

    fwrite(shmaddr, sizeof(char), SHMSIZE, copy_fp);
    fclose(copy_fp);
    
    if (munmap(shmaddr, SHMSIZE) == -1) {
      perror ("munmap 실패");
      exit (1);
    }

    printf("부모 프로세스 %d : 세마포어 반환\n", getpid());
    v(semd);

    close(shmd);
    
    if (shm_unlink(SHMNAME) == -1) {
      perror ("shm_unlink 실패");
      exit (1);
    }

    if (sem_close(semd) == -1) {
      perror ("sem_close 실패");
      exit (1);
    }

  } else {
    perror("fork 에러");
    exit(1);
  }
}

void p(sem_t *semd) {
  int ret;
  if ((ret = sem_trywait(semd)) != 0 && errno == EAGAIN)
    sem_wait(semd);
  else if (ret != 0) {
    perror ("sem_trywait 실패");
    exit (1);
  }
}

void v(sem_t *semd) {
  if (sem_post(semd) != 0) {
    perror ("sem_post 실패");
    exit (1);
  }
}

