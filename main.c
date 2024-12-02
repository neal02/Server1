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

int main(){
    // 공유 메모리 관련 코드
    Gamedata* gamedata;
    key_t key = ftok("HakGongChi", 1);
    int shmid = shmget(key, sizeof(Gamedata), IPC_CREAT|0666);
    
    if(shmid == -1){
        perror("failed shmget");
        exit(1);
    }
    gamedata = (Gamedata*)shmat(shmid, NULL, 0);
    game_init(gamedata);
    
    // 파이프 관련 코드
    unlink("hostPipe"); //일단 파이프 해제
    unlink("guestPipe");
    
    int fd1, fd2;
    
    mkfifo("hostPipe", 0660); //파이프 생성
    mkfifo("guestPipe", 0660);
    
    char hostInput[20], guestInput[20];
    int hostPID, guestPID;
    
    fd1 = open("hostPipe", O_RDONLY);
    fd2 = open("guestPipe", O_RDONLY);
    
    if(fd1 == -1 || fd2 == -1){
        perror("파이프 열기 실패\n");
        exit(1);
    } else {
        printf("파이프 열기 성공\n");
    }
    
    int n = read(fd1, hostInput, sizeof(hostInput));
    int m = read(fd2, guestInput, sizeof(guestInput));
    hostPID = atoi(hostInput);
    guestPID = atoi(guestInput);
    printf("호스트 PID : %d\n", hostPID);
    printf("게스트 PID : %d\n", guestPID);
    
    int hostSelect;
    int guestSelect;
    
    pid_t pid1 = fork();
    if(pid1 == 0){ //호스트 입력을 처리하는 프로세스
        while(1){
            if(gamedata->gameState == 4){ //게임 종료 상황일 시 프로세스 종료
                exit(1);
            }
            int n = read(fd1, &hostSelect, sizeof(int));
            if (n > 0) {
                if(gamedata->gameState == 1 && hostSelect == 1){ //게임 대기 상태에서 준비 완료 시
                    gamedata->p1ready = 1;
                    printf("호스트 플레이어 게임 준비\n");
                }
                else if(gamedata->gameState == 1 && hostSelect == 2){
                    printf("호스트가 게임을 종료했습니다.\n");
                    gamedata->gameState = 4;
                    printf("호스트 입력 처리 프로세스를 종료합니다.\n");
                    exit(1);
                }
                if(gamedata->gameState == 2){
                    if(hostSelect == 1 || hostSelect == 2){
                        gamedata->p1select = hostSelect;
                        printf("호스트의 입력이 확인되었습니다.\n");
                    }
                }
                if(gamedata->gameState == 3){
                    if(hostSelect == 1 || hostSelect == 2){
                        gamedata->p1select = hostSelect;
                        printf("호스트의 입력이 확인되었습니다.\n");
                    }
                }
            } 
        }
    }
    
    pid_t pid2 = fork();
    if(pid2 == 0){ //게스트 입력을 처리하는 프로세스
        while(1){
            if(gamedata->gameState == 4){ //게임 종료 상황일 시 프로세스 종료
                exit(1);
            }
            int m = read(fd2, &guestSelect, sizeof(int));
            if (m > 0) {
                if(gamedata->gameState == 1 && guestSelect == 1){ //게임 대기 상태에서 준비 완료 시
                    gamedata->p2ready = 1;
                    printf("게스트 플레이어 게임 준비\n");
                }
                else if(gamedata->gameState == 1 && guestSelect == 2){
                    printf("게스트가 게임을 종료했습니다.\n");
                    gamedata->gameState = 4;
                    printf("게스트 입력 처리 프로세스를 종료합니다.\n");
                    exit(1);
                }
                if(gamedata->gameState == 2){
                    if(guestSelect == 1 || guestSelect == 2){
                        gamedata->p2select = guestSelect;
                        printf("게스트의 입력이 확인되었습니다.\n");
                    }
                }
                if(gamedata->gameState == 3){
                    if(guestSelect == 1 || guestSelect == 2){
                        gamedata->p2select = guestSelect;
                        printf("게스트의 입력이 확인되었습니다.\n");
                    }
                }
            }
        }
    }
    
    while(1){
        if(gamedata->gameState == 1){
            if(gamedata->p1ready == 1 && gamedata->p2ready == 1){ //둘다 게임 시작 선택 시 게임 시작
                printf("양 플레이어가 모두 준비완료 했습니다.\n");
                printf("게임을 시작합니다.\n");
                gamedata->gameState = 2; 
                if(kill(hostPID, SIGCONT) == -1){
                    printf("시그널 보내기 실패\n");
                }
                else{
                    printf("시그널 보내기 성공\n");
                }
                if(kill(guestPID, SIGCONT) == -1){
                    printf("시그널 보내기 실패\n");
                }
                else{
                    printf("시그널 보내기 성공\n");
                }
            }
        }
        else if(gamedata->gameState == 2){
            while(1){
                if(gamedata->p1select != 0 && gamedata->p2select != 0){
                    break;
                }
                usleep(1000);
            }
            if(gamedata->p1select != 0 && gamedata->p2select != 0){
                printf("플레이어들의 정상 입력을 확인했습니다.\n");
                gamedata_update(gamedata); //선택에 따른 게임 내용 업데이트
                printf("게임 내용이 업데이트 되었습니다.\n");
                
                kill(hostPID, SIGUSR2);
                kill(guestPID, SIGUSR2);
                printf("시그널 보내기 성공\n");
                
                sleep(2);
                //플레이어들에게 왕국 능력치 변화를 출력해주고 다시 변화 전 능력치 업데이트
                gamedata->beforeKingdom.public = gamedata->kingdom.public;
                gamedata->beforeKingdom.gold = gamedata->kingdom.gold;
                gamedata->beforeKingdom.power = gamedata->kingdom.power;
                gamedata->beforeKingdom.faith = gamedata->kingdom.faith;
            }
        }
        else if(gamedata->gameState == 3){
            if(gamedata->p1select == 2 || gamedata->p2select == 2){ //둘 중 한명이라도 게임 종료 시
                gamedata->gameState = 4;
            }
            else if(gamedata->p1select == 1 && gamedata->p2select == 1){ //둘 모두 재시작시
                game_init(gamedata); //게임 초기화
                kill(hostPID, SIGUSR1);
                kill(guestPID, SIGUSR1);
            }
        }
        else if(gamedata->gameState == 4){
            printf("게임 상태 : %d\n", gamedata->gameState);
            kill(hostPID, SIGTERM);
            kill(guestPID, SIGTERM);
            
            kill(pid1, SIGCONT);
            kill(pid2, SIGCONT);
            break;
        }  
    }
    
    wait(NULL); //자식 프로세스 종료 기다리기
    
    shmdt(gamedata);
    shmctl(shmid, IPC_RMID, NULL); // 공유 메모리 정리
    
    close(fd1);
    close(fd2); //파이프 닫기
    return 0;
}