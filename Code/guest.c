#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "gamedata.h"

// 전역 변수 선언
Gamedata* gamedata;
key_t key;

void signal_handler(int signo) {
    if (signo == SIGCONT) {
        system("clear");
        printf("\n\t당신은 왕국의 왕비입니다.\n");
        sleep(1);
        printf("\n\t왕과 함께 왕국을 운영해 나가야합니다.\n");
        sleep(1);
        printf("\n\t당신의 선택에 따라 왕국의 운명이 바뀔 것입니다.\n");
        sleep(1);
        printf("\n\t1년 동안 왕국의 번영을 위해 힘써주십시오.\n");
        sleep(1);
        printf("\n\t게임을 시작합니다.\n");
        sleep(3);
    } else if (signo == SIGTERM) {
        printf("\n\t게임을 종료합니다.\n");
        sleep(2);
        exit(1);
    } else if (signo == SIGUSR1) {
        printf("\n\t게임을 재시작합니다.\n");
        sleep(2);
    } else if (signo == SIGUSR2) {
        system("clear");
        printChanges(gamedata);
    }
}

void initialize_shared_memory() {
    // 공유 메모리 초기화
    key = ftok("HakGongChi", 1);
    int shmid = shmget(key, sizeof(Gamedata), 0);
    if (shmid == -1) {
        perror("failed shmget");
        exit(1);
    }
    gamedata = (Gamedata*)shmat(shmid, NULL, 0);
}

void initialize_pipe(int* fd) {
    // 파이프 초기화
    do {
        *fd = open("guestPipe", O_WRONLY);
        if (*fd == -1) {
            perror("파이프 열기 실패\n");
            sleep(1);
        }
    } while (*fd == -1);

    // 시그널을 위해 pid 서버에 보내기
    char input[100];
    int pid = getpid();
    sprintf(input, "%d", pid);
    int length = strlen(input) + 1;
    int n = write(*fd, input, length);
    if (n == -1) {
        perror("failed write guestPipe");
    }
}

int main() {
    int fd;

    initialize_shared_memory();
    initialize_pipe(&fd);

    int select;
    
    signal(SIGCONT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);

    while (1) {
        system("clear");
        if (gamedata->gameState == 1) {
            print_title();
            printf("\n\t선택을 입력해주세요 : ");
            scanf("%d", &select);
            int n = write(fd, &select, sizeof(int));
            if (n == -1) {
                perror("failed write hostPipe");
            }
            if (select == 1) {
                printf("\t호스트를 기다리는 중...\n");
                pause();
            } else if (select == 2) {
                printf("\t게임 종료 대기 중...\n");
                sleep(3);
            } else {
                printf("\t잘못된 입력입니다.\n");
                sleep(1);
            }
        } else if (gamedata->gameState == 2) {
            print_gameScreen(gamedata);
            print_selectText(gamedata->stageNum, 2);
            printf("\n\t선택을 입력해주세요 : ");
            scanf("%d", &select);
            int n = write(fd, &select, sizeof(int));
            if (n == -1) {
                perror("입력 실패");
            }

            if (select == 1 || select == 2) {
                printf("\t왕의 선택을 기다리는 중...\n");
                pause();
            } else {
                printf("\t잘못된 입력입니다.\n");
                sleep(1);
            }
        }
        else if (gamedata->gameState == 3){
            print_ending(gamedata->endNum);
            printf("\n\t선택을 입력해주세요 : ");
            scanf("%d", &select);
            int n = write(fd, &select, sizeof(int));
            if (n == -1) {
                perror("입력 실패");
            }
            
            if (select == 1 || select == 2) {
                printf("\t호스트의 선택을 기다리는 중...\n");
                pause();
            } else {
                printf("\t잘못된 입력입니다.\n");
                sleep(1);
            }
        }
    }

    close(fd);
    return 0;
}