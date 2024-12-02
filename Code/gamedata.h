#ifndef GAMEDATA_H

typedef struct kingdom{
    int public; //민심
    int gold; //재정
    int power; //국력
    int faith; //신앙
}Kingdom;

typedef struct gamedata{
    int p1ready;
    int p2ready; //플레이어 두 명이 준비가 되면 게임 시작
    
    //플레이어의 선택을 담는 변수
    int p1select;
    int p2select;
    
    //게임 내 관련 정보
    int stageNum; 
    volatile int gameState; //1: 타이틀 2: 게임 플레이 3: 엔딩 출력 4: 게임 종료
    int endNum; //플레이어 능력치에 따른 엔딩 넘버
    Kingdom kingdom;
    Kingdom beforeKingdom;
} Gamedata;

Kingdom kingdom_init(Kingdom kingdom);

void game_init(Gamedata* gamedata);

void print_title();

void print_UI(Kingdom kingdom);

void print_art(int stageNum);

void print_gameScreen(Gamedata* gamedata);

void print_selectText(int stageNum, int playerNum);

void print_ending(int endNum);

void gamedata_update(Gamedata* gamedata);

void printChanges(Gamedata* gamedata);
#endif