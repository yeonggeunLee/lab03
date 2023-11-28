#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

int is_hidden(const char *name) {
    return name[0] == '.';
}

int compare(const void *a, const void *b) {
    return strcmp((*(struct dirent **)a)->d_name, (*(struct dirent **)b)->d_name);
}

void list_files_recursive(char *path, int depth) {
    struct dirent **namelist;
    int n;

    if ((n = scandir(path, &namelist, NULL, alphasort)) == -1) {
        perror("scandir");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        if (namelist[i]->d_type == DT_DIR) {
            if (strcmp(namelist[i]->d_name, ".") != 0 && strcmp(namelist[i]->d_name, "..") != 0 &&
                !is_hidden(namelist[i]->d_name)) {
                for (int j = 0; j < depth; j++) {
                    printf("    "); // 하위 파일 출력 앞에 공간 생성
                }
                printf("%s:\n", namelist[i]->d_name);
                char next_path[1024];
                snprintf(next_path, sizeof(next_path), "%s/%s", path, namelist[i]->d_name);
                list_files_recursive(next_path, depth + 1);  // 서브 디렉터리 출력
            }
        } else if (!is_hidden(namelist[i]->d_name)) {
            for (int j = 0; j < depth; j++) {
                printf("    ");
            }
            printf("%s\n", namelist[i]->d_name);
        }

        free(namelist[i]);
    }

    free(namelist);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <dirname>\n", argv[0]);
        exit(1);
    }

    printf("%s:\n", argv[1]);
    list_files_recursive(argv[1], 1);

    return 0;
}

