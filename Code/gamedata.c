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

Kingdom kingdom_init(Kingdom kingdom){
    kingdom.public = 40;
    kingdom.gold = 40;
    kingdom.power = 40;
    kingdom.faith = 40;
    return kingdom;
}

void game_init(Gamedata* gamedata){
    gamedata->p1ready = 0;
    gamedata->p2ready = 0; //플레이어 준비 X 상태
    
    gamedata->p1select = 0;
    gamedata->p2select = 0; //플레이어의 선택지에 따른 결과값 계산
    
    gamedata->stageNum = 1;
    gamedata->gameState = 1;
    gamedata->endNum = 0;
    gamedata->kingdom = kingdom_init(gamedata->kingdom);
    gamedata->beforeKingdom = kingdom_init(gamedata->beforeKingdom);
}

void print_title() {
    printf("\n\n\n\t███████╗ ██████╗ ██████╗     ██╗  ██╗██╗███╗   ██╗ ██████╗ ██████╗  ██████╗ ███╗   ███╗\n");
    printf("\t██╔════╝██╔═══██╗██╔══██╗    ██║ ██╔╝██║████╗  ██║██╔════╝ ██╔══██╗██╔═══██╗████╗ ████║\n");
    printf("\t█████╗  ██║   ██║██████╔╝    █████╔╝ ██║██╔██╗ ██║██║  ███╗██║  ██║██║   ██║██╔████╔██║\n");
    printf("\t██╔══╝  ██║   ██║██╔══██╗    ██╔═██╗ ██║██║╚██╗██║██║   ██║██║  ██║██║   ██║██║╚██╔╝██║\n");
    printf("\t██║     ╚██████╔╝██║  ██║    ██║  ██╗██║██║ ╚████║╚██████╔╝██████╔╝╚██████╔╝██║ ╚═╝ ██║\n");
    printf("\t╚═╝      ╚═════╝ ╚═╝  ╚═╝    ╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝ ╚═════╝ ╚═════╝  ╚═════╝ ╚═╝     ╚═╝\n\n\n");
    printf("\t1. 게임 시작\n");
    printf("\t2. 게임 종료\n");
}

void print_UI(Kingdom kingdom){
    int publicCount = kingdom.public / 5;
    int goldCount = kingdom.gold / 5;
    int powerCount = kingdom.power / 5;
    int faithCount = kingdom.faith / 5;
    printf("\t\t    민심 : ");
    for(int i = 0; i<20; i++){
        if(publicCount > 0){
            printf("■");
            publicCount--;
        }
        else{
            printf("□");
        }
    }
    printf("\n\t\t    재정 : ");
    for(int i = 0; i<20; i++){
        if(goldCount > 0){
            printf("■");
            goldCount--;
        }
        else{
            printf("□");
        }
    }
    printf("\n\t\t    국력 : ");
    for(int i = 0; i<20; i++){
        if(powerCount > 0){
            printf("■");
            powerCount--;
        }
        else{
            printf("□");
        }
    }
    printf("\n\t\t    신앙 : ");
    for(int i = 0; i<20; i++){
        if(faithCount > 0){
            printf("■");
            faithCount--;
        }
        else{
            printf("□");
        }
    }
    printf("\n\n");
}

void print_art(int stageNum){
    switch(stageNum){
    case 1:
        printf("\t\t⡹⡪⡳⡹⡪⣳⣙⢎⢗⢝⢎⢗⢝⢎⢗⢝⣎⠃⠹⣪⢳⣙⢎⢗⢝⣎⡳⡹⡪⣳⡙⣎⢗⢭⡛\n");
        printf("\t\t⢝⢎⣏⢞⢵⡱⡪⡳⣍⢗⢭⡳⡹⡱⡫⡮⠀⠀⠀⠑⡵⣕⢝⢎⢗⠼⡜⡝⢮⡪⢞⣜⢕⢧⢫\n");
        printf("\t\t⣗⠵⣕⢝⢦⡫⡺⡱⣎⢳⢕⣝⢜⡝⠎⠀⠀⠀⠀⠀⠈⢮⡣⣫⢳⡹⣪⢫⡺⡜⠃⢊⢁⣁⠁\n");
        printf("\t\t⡹⡪⣎⢗⢵⣙⢎⢗⢵⡙⡮⣪⠕⠃⠀⠀⠠⠀⠀⠀⠀⠀⢫⡪⣣⢳⡱⣣⡓⢀⡵⡫⣣⢝⡝\n");
        printf("\t\t⢝⢮⡪⣣⠳⣕⢭⠳⣕⢝⢮⠊⢀⠀⠄⠐⠀⠀⠀⠀⠀⠀⠀⠱⡕⢧⢫⠶⠀⡺⡜⡵⣱⢣⡫\n");
        printf("\t\t⢝⢦⡫⣪⢳⢕⡝⣕⢧⡫⠊⠀⠀⢀⠀⠄⠀⠁⠀⠀⠀⠀⠀⠀⠘⡣⣏⢽⠀⣮⢹⡪⣎⢧⢫\n");
        printf("\t\t⢗⢵⡹⣌⢗⢵⡹⡜⣖⢅⠀⠀⠄⢀⠀⠄⠈⠀⠀⠀⠀⠀⠀⠀⠀⢨⢎⣞⠀⢮⡣⡳⡪⣎⢧\n");
        printf("\t\t⢮⠳⣜⢎⡳⡕⡵⡹⣌⢗⠀⠀⠄⠀⡀⠀⠐⠀⠀⠀⠀⠀⠀⠀⠀⡺⡱⡵⠀⣗⢝⡼⡱⡕⢧\n");
        printf("\t\t⣎⢗⢵⡹⡜⡮⡺⡱⡕⡯⡂⠀⠄⢀⠀⠈⠀⠄⢀⠀⠀⠀⠀⠀⠀⣯⢪⣝⠀⡮⡳⣜⢵⡹⡕\n");
        printf("\t\t⢜⢵⢣⡳⡹⡜⡵⡹⡪⡞⡵⠀⠀⡀⡀⠡⠈⡂⠠⠀⠠⠀⠀⠀⢘⢎⢗⢼⠀⣝⢮⢪⢖⡵⣹\n");
        printf("\t\t⢮⠳⣕⢵⡹⡪⣎⢏⡞⣜⢽⡀⠐⠠⡀⠑⡀⢂⠐⠀⠂⠐⢀⠀⣜⢧⢫⢞⠀⣮⢪⡳⡕⡵⡣\n");
        printf("\t\t⢫⡳⡕⡵⣕⢝⡜⡮⣚⢮⢺⠄⢁⠂⠄⢱⠀⠂⠠⠨⠄⠁⡀⠀⠪⠎⡧⡻⠀⢮⡣⡳⡹⡜⡵\n");
        printf("\t\t⠧⡳⡹⡜⣎⢞⢼⠪⠃⠁⠀⢀⠂⡐⢈⢸⠈⠄⠁⢘⠂⠠⠀⡈⠀⠀⠀⠉⠀⣗⢝⡼⡱⣝⡪\n");
        printf("\t\t⣏⢞⡕⡽⣌⠗⠁⠀⠀⠀⠂⠐⡀⠢⠠⠀⢅⠈⡀⠂⠐⠀⠂⡀⠀⠀⠀⠀⢀⠈⠳⡜⡳⣜⢎\n");
        printf("\t\t⡜⣎⢞⠎⠀⠀⠀⠠⠀⠁⠀⠂⠐⡐⠠⠁⠄⢂⠀⡈⢀⠁⠄⠀⠀⠀⠀⠀⢀⠀⠀⠨⡳⣕⢝\n");
        printf("\t\t⢞⡜⣵⠀⠀⡀⠁⠀⠄⠈⢀⠈⠀⠠⠈⠂⠨⠀⠄⠠⠀⠀⠀⠀⠀⠀⠀⠀⠠⠀⠀⢈⢷⡸⣕\n");
        printf("\t\t⡪⡞⣜⠀⠀⡀⠐⠀⠐⠀⡀⠀⠂⠠⠀⠁⠈⢂⠀⠂⠀⠀⠀⠀⠀⠀⠀⠀⠐⠀⠀⠐⡧⡳⣜\n");
        printf("\t\t⡮⡺⣪⠀⢀⠀⠠⠀⠁⠀⡀⠐⠀⠠⠀⠈⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡁⠀⠀⠘⡮⡺⣬\n");
        printf("\n");
        break;
    case 2:
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⡻⡆⡌⠻⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣫⢺⡱⡇⢜⢑⠥⡙⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⡹⣪⢎⢗⢵⡣⡑⢅⠪⡒⢄⠛⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢜⡵⣣⡫⠳⠑⡅⡘⠌⢕⠜⡨⡃⢺⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠪⠞⡠⠡⡑⠡⠢⡈⠪⡠⠑⢌⠪⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡁⡊⢔⠁⢾⠈⢆⢑⢱⠔⠑⠤⡁⢾⣿⣿⣿⣿⠏⠈⢿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢂⢑⢄⠑⣽⢈⠢⡁⢜⡊⡘⠔⣈⢫⣿⣿⡿⠁⠀⠀⠀⠻⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠎⢔⢅⠪⡠⢂⠑⠌⡢⠰⡨⠪⢄⢽⢿⣿⠄⠀⠀⢀⠡⠀⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⡿⣻⢫⡳⣍⡢⡑⠕⡌⡪⢑⠕⡸⡈⢎⠪⡢⡗⡵⡹⠆⠀⠀⡐⠠⠂⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⢟⡼⡱⡳⣕⢵⡹⡪⡜⡰⢁⢇⠱⡨⡊⣦⢛⢮⡺⣕⢝⣇⠀⠀⠄⡡⢸⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⢫⢎⢮⡫⡺⡜⡵⡹⣪⢝⢖⡕⢌⢕⢼⡪⣎⢯⢺⢜⢮⡪⢮⠀⠀⠢⠀⣼⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⢱⡣⡳⡹⣜⢝⢮⢣⢕⡫⡮⣹⡪⡳⣕⢗⢜⡜⣕⢏⢮⢳⡹⠄⠀⠨⡀⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⠲⡕⣝⢮⣪⡣⡳⡱⡕⡵⣙⢦⡫⣞⠬⣎⢧⠺⡬⡣⣏⢞⣜⡅⠀⢂⠰⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⢪⠺⡜⣖⢵⡹⣜⢕⢝⡜⢮⡣⡳⣕⢏⢮⢪⡓⣝⢮⡪⡳⡜⡦⠀⢂⢸⣿⣿⣿\n");
        printf("\n");
        break;
    case 3:
        printf("\t\t⡺⣝⠾⣵⢻⡵⣫⣞⢧⡟⣾⡹⢮⡗⣯⣳⢽⡺⣝⡮⣗⢯⣳⢻⣼⢳⡽⣳⠽⣎⡷⣽⢳⡽⣺\n");
        printf("\t\t⡳⣏⣟⡞⣧⣻⠵⣞⢯⢾⣱⢟⡽⣺⡵⣫⢾⣹⢧⣛⡾⣭⣳⢻⡼⣳⡝⣧⢿⡹⣞⡵⣻⡼⣳\n");
        printf("\t\t⠷⣝⣮⠽⣞⡵⣻⡝⣞⡧⣟⢾⡹⢧⣻⠵⠏⠂⠙⢧⣛⡶⣝⡧⣟⡵⣫⡗⣯⣳⢽⣚⡧⣟⡵\n");
        printf("\t\t⡿⣹⢎⣟⡮⣗⢯⢾⡹⣞⡽⣺⡝⣯⠓⢉⠠⠁⠀⠀⠈⠳⣏⡾⣵⢫⢷⣹⢧⣛⣮⠷⣝⡾⣱\n");
        printf("\t\t⡵⣏⠿⣜⣳⡽⣞⢯⣳⢽⡺⡵⠋⠄⡐⢀⢂⠡⢀⠀⠀⠀⠈⠳⢭⣯⢳⣏⡾⣣⣏⣟⢮⡗⣯\n");
        printf("\t\t⣞⡽⣫⢟⡼⣳⢽⡺⣵⣫⠷⠁⡐⠠⠐⢂⠌⠤⢁⠂⠄⡀⠀⠀⠘⣮⢷⡹⣞⣵⢫⡞⣧⡻⡵\n");
        printf("\t\t⣎⡷⣏⢯⣳⣛⡮⠗⠓⢉⠃⡐⠠⢡⠉⢄⠊⡐⠠⠌⡐⢀⠂⠀⠀⠘⠃⠛⠱⣞⢯⡽⣺⢵⣛\n");
        printf("\t\t⣣⢟⡾⣭⡳⡽⡆⠐⡈⠄⠂⠄⡑⢠⠊⣴⢈⡐⢁⢢⡔⠂⠌⠀⠀⠀⠀⠀⠀⢸⢧⡻⣵⢫⡾\n");
        printf("\t\t⣛⣮⢗⣳⡽⣳⢳⢦⡐⢈⡐⡤⠘⡀⠆⣼⠠⠐⢂⢸⠆⡈⠐⠀⢰⠀⠀⢀⡴⢮⣳⢏⡷⣫⠷\n");
        printf("\t\t⣏⢾⣭⡳⣝⡧⢿⣭⠻⡍⠻⡅⢠⢁⠂⡀⢂⠍⡀⢂⠐⠠⢉⠀⢸⣏⢿⡹⣝⣳⢽⡺⣝⡧⣟\n");
        printf("\t\t⣞⣳⢮⣝⡧⡟⠇⡄⢃⠌⡱⢀⠆⠰⢀⡀⢁⡀⢀⠀⡀⢐⠠⠀⣏⡞⣧⢻⡼⣹⢮⡽⣞⡵⣏\n");
        printf("\t\t⢧⣛⣞⢮⣳⣝⠰⡈⢆⡘⠄⡃⠌⢂⠡⡐⠂⢌⠀⠆⡁⠂⠤⢁⢺⡝⣮⢳⣭⠳⣏⡾⣵⢻⣜\n");
        printf("\t\t⣭⢷⣚⢯⢶⡹⣆⠡⢂⠜⡠⢑⡈⢂⠡⡐⠉⠤⢈⠐⠠⢁⢂⣦⢻⡜⣧⢻⠌⠣⢌⢳⡭⣗⢯\n");
        printf("\t\t⣎⡷⣫⢞⣧⢻⣜⡳⣌⠢⢑⣢⠘⠤⣁⠂⠍⡰⢀⢁⢊⡴⣫⠞⢣⡟⡼⠃⡌⡑⢢⢘⡷⣹⡞\n");
        printf("\t\t⣝⡾⣹⢞⡼⣳⢎⡷⣹⢦⣁⠣⡘⡴⡄⡉⢆⡐⢠⣎⠿⡜⢣⡟⡼⢉⠔⡡⢂⡑⢢⠸⣝⣧⣛\n");
        printf("\t\t⣧⡟⣵⢫⡞⣵⢫⡞⣵⢫⣞⡳⣖⡰⢠⠙⠆⡌⡷⣌⡹⡞⢣⠑⡄⢣⠘⠤⢡⠘⡄⢹⡞⣶⣭\n");
        printf("\n");
        break;
    case 4:
        printf("\t\t⣣⡻⡪⣞⢼⡱⡳⣕⢯⡺⣱⢳⢕⢯⡺⡕⡯⡪⢞⡼⣱⢳⡕⡯⡺⣕⢵⢳⢝⢮⢳⢝⢮⡺⣜\n");
        printf("\t\t⡜⡮⡳⣕⢧⡻⣜⢵⡣⢯⡪⡳⣝⢮⡺⣪⢳⡙⢮⡺⡱⣇⢟⣜⢳⡕⣽⡱⣫⢳⢝⣕⢗⡵⣹\n");
        printf("\t\t⡪⣞⢵⢳⢕⢗⡵⣣⢻⡪⣝⢞⢮⠺⣜⢕⢵⣙⡕⣝⢮⢪⢧⡫⡺⣎⢮⢎⢷⡱⡳⣕⡝⡮⣺\n");
        printf("\t\t⡺⣜⢕⡗⡽⣱⢣⢯⡪⣳⢕⢯⣪⣷⣵⣽⣶⡵⣭⣮⢮⣳⣕⢝⢮⢎⣗⢝⢮⢳⢝⢮⢺⢕⢗\n");
        printf("\t\t⣕⢽⡪⣝⢮⢳⢝⢎⢷⣵⣿⢿⣿⣻⣯⣿⣾⣟⢷⡽⣫⡷⣽⡻⣧⢯⣪⢳⡝⣕⢏⡧⣫⢳⢝\n");
        printf("\t\t⡕⣧⢫⢎⣗⢝⢮⢳⡿⣯⣿⣟⣿⠻⠫⠛⠙⠋⠓⠛⠝⢟⡾⣝⣷⡻⣧⡳⣹⡪⣳⡹⣪⢳⢝\n");
        printf("\t\t⢗⢵⢝⡵⣕⢏⡗⡽⣿⣿⣯⡋⢀⠐⠠⠁⠌⠀⠀⠀⠀⠀⠈⢹⢮⡿⣵⢏⢮⢮⡣⣗⢵⡫⡮\n");
        printf("\t\t⡝⣕⢗⡵⡵⣹⡪⡳⡽⣷⣿⡇⠠⠈⢄⠡⠈⠄⠀⡀⠀⠀⠀⣸⡷⣻⢾⡍⡷⣱⢳⢕⢗⡵⣹\n");
        printf("\t\t⡪⣳⡹⡪⡞⡮⣪⢏⠾⣜⠽⡧⢈⠐⠠⠇⡈⠄⠀⠯⠀⠀⠀⡾⣹⢕⢧⡫⣞⢕⢗⣝⢵⢝⡼\n");
        printf("\t\t⢝⢮⡪⣏⢞⡵⣣⢏⣗⢝⢮⡳⠀⠌⢐⠀⡂⠀⠀⠀⠀⠀⠀⣯⡪⡳⣕⢗⢵⢝⡵⣣⡳⣣⢯\n");
        printf("\t\t⢗⢧⡫⣎⢷⡱⡳⡵⣱⣿⣾⣹⡀⠊⢀⠞⠛⠉⠑⠂⠀⠀⢘⡦⡙⡜⡪⢳⢝⢮⡺⡪⣞⢕⢧\n");
        printf("\t\t⡱⣫⢞⡼⡱⡝⠊⢐⣿⣿⣿⡢⣇⠈⠄⢸⡄⠆⠰⠀⠀⣠⢜⡎⣌⠪⡒⠀⠙⢪⡳⣝⢜⢧⡻\n");
        printf("\t\t⣝⢮⡺⡜⠡⠐⡈⢐⣿⣿⣾⣿⣮⣻⢦⣌⢦⡤⢌⡤⢞⢕⠣⡪⡰⣉⢎⠀⠀⠀⠈⢮⡫⣺⢪\n");
        printf("\t\t⢺⡪⣮⡃⠐⠠⠂⢨⣿⣿⢾⣯⣿⢿⣷⣮⣳⠲⡝⣌⢣⡑⢕⠜⡔⢅⢎⠀⠀⠀⠀⢰⢝⡎⣷\n");
        printf("\t\t⣪⢳⡪⡆⠈⠄⠡⢐⣿⣿⣟⣿⣻⣿⣻⣟⣿⡇⢎⢔⢅⢎⢪⢊⢎⠪⡒⠀⠀⠀⠀⢸⢕⡽⣜\n");
        printf("\t\t⣮⣳⡹⡆⣈⣄⡡⣐⣿⣯⣿⣻⣿⣽⣟⣿⢿⣇⣕⣪⣢⡱⣡⡣⣊⣎⣪⣀⣀⡁⣀⢸⣕⣵⣳\n");
        printf("\n");
        break;
    case 5:
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⡻⡆⡌⠻⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣫⢺⡱⡇⢜⢑⠥⡙⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⡹⣪⢎⢗⢵⡣⡑⢅⠪⡒⢄⠛⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢜⡵⣣⡫⠳⠑⡅⡘⠌⢕⠜⡨⡃⢺⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠪⠞⡠⠡⡑⠡⠢⡈⠪⡠⠑⢌⠪⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡁⡊⢔⠁⢾⠈⢆⢑⢱⠔⠑⠤⡁⢾⣿⣿⣿⣿⠏⠈⢿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢂⢑⢄⠑⣽⢈⠢⡁⢜⡊⡘⠔⣈⢫⣿⣿⡿⠁⠀⠀⠀⠻⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠎⢔⢅⠪⡠⢂⠑⠌⡢⠰⡨⠪⢄⢽⢿⣿⠄⠀⠀⢀⠡⠀⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⡿⣻⢫⡳⣍⡢⡑⠕⡌⡪⢑⠕⡸⡈⢎⠪⡢⡗⡵⡹⠆⠀⠀⡐⠠⠂⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⢟⡼⡱⡳⣕⢵⡹⡪⡜⡰⢁⢇⠱⡨⡊⣦⢛⢮⡺⣕⢝⣇⠀⠀⠄⡡⢸⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⢫⢎⢮⡫⡺⡜⡵⡹⣪⢝⢖⡕⢌⢕⢼⡪⣎⢯⢺⢜⢮⡪⢮⠀⠀⠢⠀⣼⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⢱⡣⡳⡹⣜⢝⢮⢣⢕⡫⡮⣹⡪⡳⣕⢗⢜⡜⣕⢏⢮⢳⡹⠄⠀⠨⡀⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⠲⡕⣝⢮⣪⡣⡳⡱⡕⡵⣙⢦⡫⣞⠬⣎⢧⠺⡬⡣⣏⢞⣜⡅⠀⢂⠰⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⢪⠺⡜⣖⢵⡹⣜⢕⢝⡜⢮⡣⡳⣕⢏⢮⢪⡓⣝⢮⡪⡳⡜⡦⠀⢂⢸⣿⣿⣿\n");
        printf("\n");
        break;
    case 6:
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣳⣟⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣽⣻⣽⢯⣟⣾⢷⣻⣯⢿⣽⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣟⡿⣽⣯⢿⣯⣟⡿⣯⣟⣿⢾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠿⠟⠃⠉⠛⠿⠿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⡿⠀⠀⠀⠀⠀⠠⢀⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⢈⠐⠂⠌⠠⠐⠈⠠⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣷⡄⠀⢀⠐⠂⢌⢈⡐⣡⡀⢁⠂⢁⠀⠀⠀⢀⣿⣿⣿⣿⣿⢿⣿⣿⣿\n");
        printf("\t\t⣿⣿⡿⣟⣾⣽⣿⣿⣿⡇⢀⠂⠌⡜⠁⠈⠡⠄⡌⠁⠈⠐⡈⠄⢂⢸⣿⣿⢿⣟⣾⢿⡿⣿⣿\n");
        printf("\t\t⣿⣾⣽⡿⣽⣾⣳⣯⣿⢷⣴⠈⡘⢄⠀⢀⡰⠃⢂⡀⠀⡠⠁⡐⢺⡞⣽⢫⣿⣾⢯⣟⣿⣳⣿\n");
        printf("\t\t⣿⣿⣿⡿⣽⣾⣟⡷⣫⣟⣾⠀⠐⠈⡉⠡⠐⡀⢂⠈⠁⠄⠁⠀⢹⡞⣵⢫⡽⢯⣿⣻⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣟⡿⣞⣳⡟⣷⣫⢾⠀⠀⠀⠠⠁⠁⠀⠀⠀⠁⠀⠀⠀⢪⡝⣮⢳⡝⣯⢞⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⢯⣟⣽⡳⣟⣳⡽⢯⣀⠀⠀⠀⠀⠈⡐⠀⠀⠀⠀⠀⠀⣮⠽⣜⢧⡻⣜⢯⢾⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣛⣮⢷⣻⡽⣳⣻⢏⡿⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⣠⢟⡲⢏⡞⣧⢻⡜⣯⢾⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⡽⣞⣯⢷⣛⡷⢯⣻⣳⣏⡷⣆⠀⠀⠀⠀⠀⣠⠞⣵⢫⡝⣽⢺⡵⣫⢞⡵⣻⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣽⣯⣞⣯⣯⣟⣯⣷⣳⣯⣽⣯⣷⣄⣴⣀⣶⣭⣻⣬⣳⣝⣮⣳⣝⣧⣻⣜⣿⣿⣿⣿\n");
        printf("\n");
        break;
    case 7:
        printf("\t\t⡹⡪⡳⡹⡪⣳⣙⢎⢗⢝⢎⢗⢝⢎⢗⢝⣎⠃⠹⣪⢳⣙⢎⢗⢝⣎⡳⡹⡪⣳⡙⣎⢗⢭⡛\n");
        printf("\t\t⢝⢎⣏⢞⢵⡱⡪⡳⣍⢗⢭⡳⡹⡱⡫⡮⠀⠀⠀⠑⡵⣕⢝⢎⢗⠼⡜⡝⢮⡪⢞⣜⢕⢧⢫\n");
        printf("\t\t⣗⠵⣕⢝⢦⡫⡺⡱⣎⢳⢕⣝⢜⡝⠎⠀⠀⠀⠀⠀⠈⢮⡣⣫⢳⡹⣪⢫⡺⡜⠃⢊⢁⣁⠁\n");
        printf("\t\t⡹⡪⣎⢗⢵⣙⢎⢗⢵⡙⡮⣪⠕⠃⠀⠀⠠⠀⠀⠀⠀⠀⢫⡪⣣⢳⡱⣣⡓⢀⡵⡫⣣⢝⡝\n");
        printf("\t\t⢝⢮⡪⣣⠳⣕⢭⠳⣕⢝⢮⠊⢀⠀⠄⠐⠀⠀⠀⠀⠀⠀⠀⠱⡕⢧⢫⠶⠀⡺⡜⡵⣱⢣⡫\n");
        printf("\t\t⢝⢦⡫⣪⢳⢕⡝⣕⢧⡫⠊⠀⠀⢀⠀⠄⠀⠁⠀⠀⠀⠀⠀⠀⠘⡣⣏⢽⠀⣮⢹⡪⣎⢧⢫\n");
        printf("\t\t⢗⢵⡹⣌⢗⢵⡹⡜⣖⢅⠀⠀⠄⢀⠀⠄⠈⠀⠀⠀⠀⠀⠀⠀⠀⢨⢎⣞⠀⢮⡣⡳⡪⣎⢧\n");
        printf("\t\t⢮⠳⣜⢎⡳⡕⡵⡹⣌⢗⠀⠀⠄⠀⡀⠀⠐⠀⠀⠀⠀⠀⠀⠀⠀⡺⡱⡵⠀⣗⢝⡼⡱⡕⢧\n");
        printf("\t\t⣎⢗⢵⡹⡜⡮⡺⡱⡕⡯⡂⠀⠄⢀⠀⠈⠀⠄⢀⠀⠀⠀⠀⠀⠀⣯⢪⣝⠀⡮⡳⣜⢵⡹⡕\n");
        printf("\t\t⢜⢵⢣⡳⡹⡜⡵⡹⡪⡞⡵⠀⠀⡀⡀⠡⠈⡂⠠⠀⠠⠀⠀⠀⢘⢎⢗⢼⠀⣝⢮⢪⢖⡵⣹\n");
        printf("\t\t⢮⠳⣕⢵⡹⡪⣎⢏⡞⣜⢽⡀⠐⠠⡀⠑⡀⢂⠐⠀⠂⠐⢀⠀⣜⢧⢫⢞⠀⣮⢪⡳⡕⡵⡣\n");
        printf("\t\t⢫⡳⡕⡵⣕⢝⡜⡮⣚⢮⢺⠄⢁⠂⠄⢱⠀⠂⠠⠨⠄⠁⡀⠀⠪⠎⡧⡻⠀⢮⡣⡳⡹⡜⡵\n");
        printf("\t\t⠧⡳⡹⡜⣎⢞⢼⠪⠃⠁⠀⢀⠂⡐⢈⢸⠈⠄⠁⢘⠂⠠⠀⡈⠀⠀⠀⠉⠀⣗⢝⡼⡱⣝⡪\n");
        printf("\t\t⣏⢞⡕⡽⣌⠗⠁⠀⠀⠀⠂⠐⡀⠢⠠⠀⢅⠈⡀⠂⠐⠀⠂⡀⠀⠀⠀⠀⢀⠈⠳⡜⡳⣜⢎\n");
        printf("\t\t⡜⣎⢞⠎⠀⠀⠀⠠⠀⠁⠀⠂⠐⡐⠠⠁⠄⢂⠀⡈⢀⠁⠄⠀⠀⠀⠀⠀⢀⠀⠀⠨⡳⣕⢝\n");
        printf("\t\t⢞⡜⣵⠀⠀⡀⠁⠀⠄⠈⢀⠈⠀⠠⠈⠂⠨⠀⠄⠠⠀⠀⠀⠀⠀⠀⠀⠀⠠⠀⠀⢈⢷⡸⣕\n");
        printf("\t\t⡪⡞⣜⠀⠀⡀⠐⠀⠐⠀⡀⠀⠂⠠⠀⠁⠈⢂⠀⠂⠀⠀⠀⠀⠀⠀⠀⠀⠐⠀⠀⠐⡧⡳⣜\n");
        printf("\t\t⡮⡺⣪⠀⢀⠀⠠⠀⠁⠀⡀⠐⠀⠠⠀⠈⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡁⠀⠀⠘⡮⡺⣬\n");
        printf("\n");
        break;
    case 8:
        printf("\t\t⡹⡪⡳⡹⡪⣳⣙⢎⢗⢝⢎⢗⢝⢎⢗⢝⣎⠃⠹⣪⢳⣙⢎⢗⢝⣎⡳⡹⡪⣳⡙⣎⢗⢭⡛\n");
        printf("\t\t⢝⢎⣏⢞⢵⡱⡪⡳⣍⢗⢭⡳⡹⡱⡫⡮⠀⠀⠀⠑⡵⣕⢝⢎⢗⠼⡜⡝⢮⡪⢞⣜⢕⢧⢫\n");
        printf("\t\t⣗⠵⣕⢝⢦⡫⡺⡱⣎⢳⢕⣝⢜⡝⠎⠀⠀⠀⠀⠀⠈⢮⡣⣫⢳⡹⣪⢫⡺⡜⠃⢊⢁⣁⠁\n");
        printf("\t\t⡹⡪⣎⢗⢵⣙⢎⢗⢵⡙⡮⣪⠕⠃⠀⠀⠠⠀⠀⠀⠀⠀⢫⡪⣣⢳⡱⣣⡓⢀⡵⡫⣣⢝⡝\n");
        printf("\t\t⢝⢮⡪⣣⠳⣕⢭⠳⣕⢝⢮⠊⢀⠀⠄⠐⠀⠀⠀⠀⠀⠀⠀⠱⡕⢧⢫⠶⠀⡺⡜⡵⣱⢣⡫\n");
        printf("\t\t⢝⢦⡫⣪⢳⢕⡝⣕⢧⡫⠊⠀⠀⢀⠀⠄⠀⠁⠀⠀⠀⠀⠀⠀⠘⡣⣏⢽⠀⣮⢹⡪⣎⢧⢫\n");
        printf("\t\t⢗⢵⡹⣌⢗⢵⡹⡜⣖⢅⠀⠀⠄⢀⠀⠄⠈⠀⠀⠀⠀⠀⠀⠀⠀⢨⢎⣞⠀⢮⡣⡳⡪⣎⢧\n");
        printf("\t\t⢮⠳⣜⢎⡳⡕⡵⡹⣌⢗⠀⠀⠄⠀⡀⠀⠐⠀⠀⠀⠀⠀⠀⠀⠀⡺⡱⡵⠀⣗⢝⡼⡱⡕⢧\n");
        printf("\t\t⣎⢗⢵⡹⡜⡮⡺⡱⡕⡯⡂⠀⠄⢀⠀⠈⠀⠄⢀⠀⠀⠀⠀⠀⠀⣯⢪⣝⠀⡮⡳⣜⢵⡹⡕\n");
        printf("\t\t⢜⢵⢣⡳⡹⡜⡵⡹⡪⡞⡵⠀⠀⡀⡀⠡⠈⡂⠠⠀⠠⠀⠀⠀⢘⢎⢗⢼⠀⣝⢮⢪⢖⡵⣹\n");
        printf("\t\t⢮⠳⣕⢵⡹⡪⣎⢏⡞⣜⢽⡀⠐⠠⡀⠑⡀⢂⠐⠀⠂⠐⢀⠀⣜⢧⢫⢞⠀⣮⢪⡳⡕⡵⡣\n");
        printf("\t\t⢫⡳⡕⡵⣕⢝⡜⡮⣚⢮⢺⠄⢁⠂⠄⢱⠀⠂⠠⠨⠄⠁⡀⠀⠪⠎⡧⡻⠀⢮⡣⡳⡹⡜⡵\n");
        printf("\t\t⠧⡳⡹⡜⣎⢞⢼⠪⠃⠁⠀⢀⠂⡐⢈⢸⠈⠄⠁⢘⠂⠠⠀⡈⠀⠀⠀⠉⠀⣗⢝⡼⡱⣝⡪\n");
        printf("\t\t⣏⢞⡕⡽⣌⠗⠁⠀⠀⠀⠂⠐⡀⠢⠠⠀⢅⠈⡀⠂⠐⠀⠂⡀⠀⠀⠀⠀⢀⠈⠳⡜⡳⣜⢎\n");
        printf("\t\t⡜⣎⢞⠎⠀⠀⠀⠠⠀⠁⠀⠂⠐⡐⠠⠁⠄⢂⠀⡈⢀⠁⠄⠀⠀⠀⠀⠀⢀⠀⠀⠨⡳⣕⢝\n");
        printf("\t\t⢞⡜⣵⠀⠀⡀⠁⠀⠄⠈⢀⠈⠀⠠⠈⠂⠨⠀⠄⠠⠀⠀⠀⠀⠀⠀⠀⠀⠠⠀⠀⢈⢷⡸⣕\n");
        printf("\t\t⡪⡞⣜⠀⠀⡀⠐⠀⠐⠀⡀⠀⠂⠠⠀⠁⠈⢂⠀⠂⠀⠀⠀⠀⠀⠀⠀⠀⠐⠀⠀⠐⡧⡳⣜\n");
        printf("\t\t⡮⡺⣪⠀⢀⠀⠠⠀⠁⠀⡀⠐⠀⠠⠀⠈⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡁⠀⠀⠘⡮⡺⣬\n");
        printf("\n");
        break;
    case 9:
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⡻⡆⡌⠻⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣫⢺⡱⡇⢜⢑⠥⡙⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⡹⣪⢎⢗⢵⡣⡑⢅⠪⡒⢄⠛⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢜⡵⣣⡫⠳⠑⡅⡘⠌⢕⠜⡨⡃⢺⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠪⠞⡠⠡⡑⠡⠢⡈⠪⡠⠑⢌⠪⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡁⡊⢔⠁⢾⠈⢆⢑⢱⠔⠑⠤⡁⢾⣿⣿⣿⣿⠏⠈⢿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢂⢑⢄⠑⣽⢈⠢⡁⢜⡊⡘⠔⣈⢫⣿⣿⡿⠁⠀⠀⠀⠻⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠎⢔⢅⠪⡠⢂⠑⠌⡢⠰⡨⠪⢄⢽⢿⣿⠄⠀⠀⢀⠡⠀⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⡿⣻⢫⡳⣍⡢⡑⠕⡌⡪⢑⠕⡸⡈⢎⠪⡢⡗⡵⡹⠆⠀⠀⡐⠠⠂⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⢟⡼⡱⡳⣕⢵⡹⡪⡜⡰⢁⢇⠱⡨⡊⣦⢛⢮⡺⣕⢝⣇⠀⠀⠄⡡⢸⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⢫⢎⢮⡫⡺⡜⡵⡹⣪⢝⢖⡕⢌⢕⢼⡪⣎⢯⢺⢜⢮⡪⢮⠀⠀⠢⠀⣼⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⢱⡣⡳⡹⣜⢝⢮⢣⢕⡫⡮⣹⡪⡳⣕⢗⢜⡜⣕⢏⢮⢳⡹⠄⠀⠨⡀⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⠲⡕⣝⢮⣪⡣⡳⡱⡕⡵⣙⢦⡫⣞⠬⣎⢧⠺⡬⡣⣏⢞⣜⡅⠀⢂⠰⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⢪⠺⡜⣖⢵⡹⣜⢕⢝⡜⢮⡣⡳⣕⢏⢮⢪⡓⣝⢮⡪⡳⡜⡦⠀⢂⢸⣿⣿⣿\n");
        printf("\n");
        break;
    case 10:
        printf("\t\t⣣⡻⡪⣞⢼⡱⡳⣕⢯⡺⣱⢳⢕⢯⡺⡕⡯⡪⢞⡼⣱⢳⡕⡯⡺⣕⢵⢳⢝⢮⢳⢝⢮⡺⣜\n");
        printf("\t\t⡜⡮⡳⣕⢧⡻⣜⢵⡣⢯⡪⡳⣝⢮⡺⣪⢳⡙⢮⡺⡱⣇⢟⣜⢳⡕⣽⡱⣫⢳⢝⣕⢗⡵⣹\n");
        printf("\t\t⡪⣞⢵⢳⢕⢗⡵⣣⢻⡪⣝⢞⢮⠺⣜⢕⢵⣙⡕⣝⢮⢪⢧⡫⡺⣎⢮⢎⢷⡱⡳⣕⡝⡮⣺\n");
        printf("\t\t⡺⣜⢕⡗⡽⣱⢣⢯⡪⣳⢕⢯⣪⣷⣵⣽⣶⡵⣭⣮⢮⣳⣕⢝⢮⢎⣗⢝⢮⢳⢝⢮⢺⢕⢗\n");
        printf("\t\t⣕⢽⡪⣝⢮⢳⢝⢎⢷⣵⣿⢿⣿⣻⣯⣿⣾⣟⢷⡽⣫⡷⣽⡻⣧⢯⣪⢳⡝⣕⢏⡧⣫⢳⢝\n");
        printf("\t\t⡕⣧⢫⢎⣗⢝⢮⢳⡿⣯⣿⣟⣿⠻⠫⠛⠙⠋⠓⠛⠝⢟⡾⣝⣷⡻⣧⡳⣹⡪⣳⡹⣪⢳⢝\n");
        printf("\t\t⢗⢵⢝⡵⣕⢏⡗⡽⣿⣿⣯⡋⢀⠐⠠⠁⠌⠀⠀⠀⠀⠀⠈⢹⢮⡿⣵⢏⢮⢮⡣⣗⢵⡫⡮\n");
        printf("\t\t⡝⣕⢗⡵⡵⣹⡪⡳⡽⣷⣿⡇⠠⠈⢄⠡⠈⠄⠀⡀⠀⠀⠀⣸⡷⣻⢾⡍⡷⣱⢳⢕⢗⡵⣹\n");
        printf("\t\t⡪⣳⡹⡪⡞⡮⣪⢏⠾⣜⠽⡧⢈⠐⠠⠇⡈⠄⠀⠯⠀⠀⠀⡾⣹⢕⢧⡫⣞⢕⢗⣝⢵⢝⡼\n");
        printf("\t\t⢝⢮⡪⣏⢞⡵⣣⢏⣗⢝⢮⡳⠀⠌⢐⠀⡂⠀⠀⠀⠀⠀⠀⣯⡪⡳⣕⢗⢵⢝⡵⣣⡳⣣⢯\n");
        printf("\t\t⢗⢧⡫⣎⢷⡱⡳⡵⣱⣿⣾⣹⡀⠊⢀⠞⠛⠉⠑⠂⠀⠀⢘⡦⡙⡜⡪⢳⢝⢮⡺⡪⣞⢕⢧\n");
        printf("\t\t⡱⣫⢞⡼⡱⡝⠊⢐⣿⣿⣿⡢⣇⠈⠄⢸⡄⠆⠰⠀⠀⣠⢜⡎⣌⠪⡒⠀⠙⢪⡳⣝⢜⢧⡻\n");
        printf("\t\t⣝⢮⡺⡜⠡⠐⡈⢐⣿⣿⣾⣿⣮⣻⢦⣌⢦⡤⢌⡤⢞⢕⠣⡪⡰⣉⢎⠀⠀⠀⠈⢮⡫⣺⢪\n");
        printf("\t\t⢺⡪⣮⡃⠐⠠⠂⢨⣿⣿⢾⣯⣿⢿⣷⣮⣳⠲⡝⣌⢣⡑⢕⠜⡔⢅⢎⠀⠀⠀⠀⢰⢝⡎⣷\n");
        printf("\t\t⣪⢳⡪⡆⠈⠄⠡⢐⣿⣿⣟⣿⣻⣿⣻⣟⣿⡇⢎⢔⢅⢎⢪⢊⢎⠪⡒⠀⠀⠀⠀⢸⢕⡽⣜\n");
        printf("\t\t⣮⣳⡹⡆⣈⣄⡡⣐⣿⣯⣿⣻⣿⣽⣟⣿⢿⣇⣕⣪⣢⡱⣡⡣⣊⣎⣪⣀⣀⡁⣀⢸⣕⣵⣳\n");
        printf("\n");
        break;
    case 11:
        printf("\t\t⢏⡲⣙⠦⣛⡜⣣⠝⡎⡖⢭⠲⣌⢻⡐⢯⡘⢮⡱⡍⡞⢥⣚⡱⢎⡎⡵⢋⠶⡱⣍⠮⣱⢋⡴\n");
        printf("\t\t⢎⡱⢣⡝⢆⡳⡱⢎⡵⣙⢎⡳⣌⢧⡙⢦⡙⢦⣓⡹⢜⡣⢖⢭⠲⣍⠞⣭⠚⡵⣌⠳⣥⠫⣜\n");
        printf("\t\t⢬⡓⢧⡚⣥⠳⣍⠞⡴⠉⠚⠐⠉⢂⠙⠦⡝⠂⠧⠙⠊⠘⠉⠊⠓⠬⠫⣔⢫⢖⣩⢓⢦⢛⡬\n");
        printf("\t\t⢪⡜⣣⠕⣮⠱⣎⠡⠀⠌⢀⠡⠈⡀⠄⠠⢀⠆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⡞⣜⢢⢏⢎⡞⣸\n");
        printf("\t\t⠖⣱⠣⣝⢢⡛⡴⠀⠐⡀⠂⠠⠐⠀⡄⠣⢌⠂⠡⢀⠀⠀⠀⠀⠀⠀⠀⠀⡼⢌⡳⣊⠞⣬⢱\n");
        printf("\t\t⡝⢦⢛⡬⣃⠷⣩⠐⠀⡐⠀⡁⠄⠡⢌⠱⡈⡌⠐⡀⢂⠀⠀⠀⠀⠀⠀⠀⣜⢣⠵⣩⠞⡴⢣\n");
        printf("\t\t⡜⣣⢫⠴⣩⢖⡱⡀⢂⠀⢂⠠⠀⡍⢢⠑⡰⠠⢁⠐⡀⠂⠀⠀⠀⠀⠀⢀⢎⠧⣛⠴⣋⡜⣣\n");
        printf("\t\t⠜⣥⢣⡛⡴⣩⢖⡱⡀⠐⡀⠄⠂⡜⢠⠃⠴⡁⢂⠐⠠⢁⠀⠀⠀⠀⠀⣎⢎⡳⣩⢖⡱⢎⡵\n");
        printf("\t\t⣛⠴⣣⡙⠶⣑⢮⡱⢣⠔⠀⡐⠰⡈⢆⡉⢆⡁⠂⠌⡐⠠⠀⠀⠀⢀⡼⣘⢎⡵⢢⢏⡜⣣⠖\n");
        printf("\t\t⣩⠖⣥⢋⡳⢍⠶⣉⢧⢫⣄⠰⢡⠘⠶⠬⢦⠄⡥⠶⠴⠁⢂⠀⣠⠳⣌⠳⢎⡜⣣⠞⡸⢥⡚\n");
        printf("\t\t⢥⡛⡴⢋⡼⡩⢞⡱⢎⡱⠜⡠⢃⠜⡠⠓⡄⠢⠐⡀⢂⠁⠂⠄⠁⠙⠨⠛⡼⣘⢥⡚⡵⢣⡝\n");
        printf("\t\t⣣⠞⣡⢏⢖⡹⡌⠓⠈⠀⠀⡰⢁⠎⡰⠡⠈⠁⠂⠐⠠⢈⠐⡈⠀⠀⠀⠀⠀⠩⢖⡹⡜⡱⢎\n");
        printf("\t\t⣥⢚⡵⣊⠮⠁⠀⠀⠀⠀⠀⠰⡁⢎⡀⢄⠨⠄⡀⠀⠀⠄⢂⠐⠀⠀⠀⠀⠀⠀⠈⠰⢍⡳⡍\n");
        printf("\t\t⣌⠳⢦⠉⠀⠀⠀⠀⠀⠀⠀⠑⠌⢢⠘⡄⢃⠆⠠⠁⠌⡐⠀⠂⠀⠀⢣⣄⡀⠀⠀⠀⠈⢶⡙\n");
        printf("\t\t⣌⠏⡰⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠊⡔⢡⠊⠄⡁⠂⠀⠀⠀⠀⠀⠀⠀⠁⠀⠀⠀⠠⠈⡝\n");
        printf("\t\t⡊⢆⠱⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠂⡅⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⠁⡺\n");
        printf("\t\t⡝⢠⢃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢂⢱\n");
        printf("\n");
        break;
    case 12:
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⡻⡆⡌⠻⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣫⢺⡱⡇⢜⢑⠥⡙⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⡹⣪⢎⢗⢵⡣⡑⢅⠪⡒⢄⠛⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢜⡵⣣⡫⠳⠑⡅⡘⠌⢕⠜⡨⡃⢺⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠪⠞⡠⠡⡑⠡⠢⡈⠪⡠⠑⢌⠪⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡁⡊⢔⠁⢾⠈⢆⢑⢱⠔⠑⠤⡁⢾⣿⣿⣿⣿⠏⠈⢿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢂⢑⢄⠑⣽⢈⠢⡁⢜⡊⡘⠔⣈⢫⣿⣿⡿⠁⠀⠀⠀⠻⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠎⢔⢅⠪⡠⢂⠑⠌⡢⠰⡨⠪⢄⢽⢿⣿⠄⠀⠀⢀⠡⠀⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⡿⣻⢫⡳⣍⡢⡑⠕⡌⡪⢑⠕⡸⡈⢎⠪⡢⡗⡵⡹⠆⠀⠀⡐⠠⠂⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⢟⡼⡱⡳⣕⢵⡹⡪⡜⡰⢁⢇⠱⡨⡊⣦⢛⢮⡺⣕⢝⣇⠀⠀⠄⡡⢸⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⢫⢎⢮⡫⡺⡜⡵⡹⣪⢝⢖⡕⢌⢕⢼⡪⣎⢯⢺⢜⢮⡪⢮⠀⠀⠢⠀⣼⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⢱⡣⡳⡹⣜⢝⢮⢣⢕⡫⡮⣹⡪⡳⣕⢗⢜⡜⣕⢏⢮⢳⡹⠄⠀⠨⡀⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⠲⡕⣝⢮⣪⡣⡳⡱⡕⡵⣙⢦⡫⣞⠬⣎⢧⠺⡬⡣⣏⢞⣜⡅⠀⢂⠰⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⢪⠺⡜⣖⢵⡹⣜⢕⢝⡜⢮⡣⡳⣕⢏⢮⢪⡓⣝⢮⡪⡳⡜⡦⠀⢂⢸⣿⣿⣿\n");
        printf("\n");
        break;
    case 13:
        printf("\t\t⡹⡪⡳⡹⡪⣳⣙⢎⢗⢝⢎⢗⢝⢎⢗⢝⣎⠃⠹⣪⢳⣙⢎⢗⢝⣎⡳⡹⡪⣳⡙⣎⢗⢭⡛\n");
        printf("\t\t⢝⢎⣏⢞⢵⡱⡪⡳⣍⢗⢭⡳⡹⡱⡫⡮⠀⠀⠀⠑⡵⣕⢝⢎⢗⠼⡜⡝⢮⡪⢞⣜⢕⢧⢫\n");
        printf("\t\t⣗⠵⣕⢝⢦⡫⡺⡱⣎⢳⢕⣝⢜⡝⠎⠀⠀⠀⠀⠀⠈⢮⡣⣫⢳⡹⣪⢫⡺⡜⠃⢊⢁⣁⠁\n");
        printf("\t\t⡹⡪⣎⢗⢵⣙⢎⢗⢵⡙⡮⣪⠕⠃⠀⠀⠠⠀⠀⠀⠀⠀⢫⡪⣣⢳⡱⣣⡓⢀⡵⡫⣣⢝⡝\n");
        printf("\t\t⢝⢮⡪⣣⠳⣕⢭⠳⣕⢝⢮⠊⢀⠀⠄⠐⠀⠀⠀⠀⠀⠀⠀⠱⡕⢧⢫⠶⠀⡺⡜⡵⣱⢣⡫\n");
        printf("\t\t⢝⢦⡫⣪⢳⢕⡝⣕⢧⡫⠊⠀⠀⢀⠀⠄⠀⠁⠀⠀⠀⠀⠀⠀⠘⡣⣏⢽⠀⣮⢹⡪⣎⢧⢫\n");
        printf("\t\t⢗⢵⡹⣌⢗⢵⡹⡜⣖⢅⠀⠀⠄⢀⠀⠄⠈⠀⠀⠀⠀⠀⠀⠀⠀⢨⢎⣞⠀⢮⡣⡳⡪⣎⢧\n");
        printf("\t\t⢮⠳⣜⢎⡳⡕⡵⡹⣌⢗⠀⠀⠄⠀⡀⠀⠐⠀⠀⠀⠀⠀⠀⠀⠀⡺⡱⡵⠀⣗⢝⡼⡱⡕⢧\n");
        printf("\t\t⣎⢗⢵⡹⡜⡮⡺⡱⡕⡯⡂⠀⠄⢀⠀⠈⠀⠄⢀⠀⠀⠀⠀⠀⠀⣯⢪⣝⠀⡮⡳⣜⢵⡹⡕\n");
        printf("\t\t⢜⢵⢣⡳⡹⡜⡵⡹⡪⡞⡵⠀⠀⡀⡀⠡⠈⡂⠠⠀⠠⠀⠀⠀⢘⢎⢗⢼⠀⣝⢮⢪⢖⡵⣹\n");
        printf("\t\t⢮⠳⣕⢵⡹⡪⣎⢏⡞⣜⢽⡀⠐⠠⡀⠑⡀⢂⠐⠀⠂⠐⢀⠀⣜⢧⢫⢞⠀⣮⢪⡳⡕⡵⡣\n");
        printf("\t\t⢫⡳⡕⡵⣕⢝⡜⡮⣚⢮⢺⠄⢁⠂⠄⢱⠀⠂⠠⠨⠄⠁⡀⠀⠪⠎⡧⡻⠀⢮⡣⡳⡹⡜⡵\n");
        printf("\t\t⠧⡳⡹⡜⣎⢞⢼⠪⠃⠁⠀⢀⠂⡐⢈⢸⠈⠄⠁⢘⠂⠠⠀⡈⠀⠀⠀⠉⠀⣗⢝⡼⡱⣝⡪\n");
        printf("\t\t⣏⢞⡕⡽⣌⠗⠁⠀⠀⠀⠂⠐⡀⠢⠠⠀⢅⠈⡀⠂⠐⠀⠂⡀⠀⠀⠀⠀⢀⠈⠳⡜⡳⣜⢎\n");
        printf("\t\t⡜⣎⢞⠎⠀⠀⠀⠠⠀⠁⠀⠂⠐⡐⠠⠁⠄⢂⠀⡈⢀⠁⠄⠀⠀⠀⠀⠀⢀⠀⠀⠨⡳⣕⢝\n");
        printf("\t\t⢞⡜⣵⠀⠀⡀⠁⠀⠄⠈⢀⠈⠀⠠⠈⠂⠨⠀⠄⠠⠀⠀⠀⠀⠀⠀⠀⠀⠠⠀⠀⢈⢷⡸⣕\n");
        printf("\t\t⡪⡞⣜⠀⠀⡀⠐⠀⠐⠀⡀⠀⠂⠠⠀⠁⠈⢂⠀⠂⠀⠀⠀⠀⠀⠀⠀⠀⠐⠀⠀⠐⡧⡳⣜\n");
        printf("\t\t⡮⡺⣪⠀⢀⠀⠠⠀⠁⠀⡀⠐⠀⠠⠀⠈⠀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡁⠀⠀⠘⡮⡺⣬\n");
        printf("\n");
        break;
    case 14:
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠟⠛⠛⠛⠉⢉⣽⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠁⠀⠀⠀⠀⣠⣾⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢿⣻⢄⠫⡝⡻⢿⠋⢀⢀⣀⣀⣀⣾⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣟⡿⣞⣯⢿⣈⠳⡌⡕⢪⡑⢫⡙⡻⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣝⣯⠿⣵⣻⠞⠏⠻⢷⣳⣼⣡⡚⢥⢊⠵⣹⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣽⡞⠟⠃⠁⠈⢀⠀⠀⠀⠉⠓⠻⢟⣮⢶⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡧⠀⢀⠐⠈⢀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣗⠀⢀⠀⠂⡄⠠⠀⠀⣀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠂⠀⠄⠋⢀⠀⠀⠙⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠐⠈⠀⡐⢀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⡧⠀⠂⠀⣷⡸⢽⠨⡑⠢⠄⠀⠀⢘⡛⠿⢿⣿⣿⣿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⣿⣿⡿⠋⢀⡿⣽⡇⠀⠄⠁⣶⠉⠀⠀⠈⢑⠂⠀⠀⠸⣌⢣⢣⠈⠛⢿⣿⣿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⡿⠋⠀⠀⠄⣿⣳⢯⣄⠀⠂⡷⠀⢀⠀⠀⢌⠂⠀⢀⡜⢤⢃⡎⠀⠀⠈⠙⢿⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⠆⠀⡀⠁⢈⡷⣯⣟⣾⣳⣄⣟⡹⣖⠰⢌⢢⠡⡜⣡⠚⣌⠖⣩⠀⠀⠀⠀⢸⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⡃⠀⠀⠌⢀⣿⣳⢯⣞⣷⣻⢾⡵⣮⠑⣌⠦⡓⡜⢤⢋⡴⣉⠖⠀⠀⠀⠀⢸⣿⣿⣿\n");
        printf("\t\t⣿⣿⣿⡅⢀⠈⡀⢠⡿⣭⣟⡾⣧⢿⡽⣾⣽⢊⡴⢣⡑⣎⡜⣢⢒⡥⣚⠀⢀⠀⠀⢸⣿⣿⣿\n");
        printf("\n");
        break;
    case 15:
        printf("\t\t⡺⣝⠾⣵⢻⡵⣫⣞⢧⡟⣾⡹⢮⡗⣯⣳⢽⡺⣝⡮⣗⢯⣳⢻⣼⢳⡽⣳⠽⣎⡷⣽⢳⡽⣺\n");
        printf("\t\t⡳⣏⣟⡞⣧⣻⠵⣞⢯⢾⣱⢟⡽⣺⡵⣫⢾⣹⢧⣛⡾⣭⣳⢻⡼⣳⡝⣧⢿⡹⣞⡵⣻⡼⣳\n");
        printf("\t\t⠷⣝⣮⠽⣞⡵⣻⡝⣞⡧⣟⢾⡹⢧⣻⠵⠏⠂⠙⢧⣛⡶⣝⡧⣟⡵⣫⡗⣯⣳⢽⣚⡧⣟⡵\n");
        printf("\t\t⡿⣹⢎⣟⡮⣗⢯⢾⡹⣞⡽⣺⡝⣯⠓⢉⠠⠁⠀⠀⠈⠳⣏⡾⣵⢫⢷⣹⢧⣛⣮⠷⣝⡾⣱\n");
        printf("\t\t⡵⣏⠿⣜⣳⡽⣞⢯⣳⢽⡺⡵⠋⠄⡐⢀⢂⠡⢀⠀⠀⠀⠈⠳⢭⣯⢳⣏⡾⣣⣏⣟⢮⡗⣯\n");
        printf("\t\t⣞⡽⣫⢟⡼⣳⢽⡺⣵⣫⠷⠁⡐⠠⠐⢂⠌⠤⢁⠂⠄⡀⠀⠀⠘⣮⢷⡹⣞⣵⢫⡞⣧⡻⡵\n");
        printf("\t\t⣎⡷⣏⢯⣳⣛⡮⠗⠓⢉⠃⡐⠠⢡⠉⢄⠊⡐⠠⠌⡐⢀⠂⠀⠀⠘⠃⠛⠱⣞⢯⡽⣺⢵⣛\n");
        printf("\t\t⣣⢟⡾⣭⡳⡽⡆⠐⡈⠄⠂⠄⡑⢠⠊⣴⢈⡐⢁⢢⡔⠂⠌⠀⠀⠀⠀⠀⠀⢸⢧⡻⣵⢫⡾\n");
        printf("\t\t⣛⣮⢗⣳⡽⣳⢳⢦⡐⢈⡐⡤⠘⡀⠆⣼⠠⠐⢂⢸⠆⡈⠐⠀⢰⠀⠀⢀⡴⢮⣳⢏⡷⣫⠷\n");
        printf("\t\t⣏⢾⣭⡳⣝⡧⢿⣭⠻⡍⠻⡅⢠⢁⠂⡀⢂⠍⡀⢂⠐⠠⢉⠀⢸⣏⢿⡹⣝⣳⢽⡺⣝⡧⣟\n");
        printf("\t\t⣞⣳⢮⣝⡧⡟⠇⡄⢃⠌⡱⢀⠆⠰⢀⡀⢁⡀⢀⠀⡀⢐⠠⠀⣏⡞⣧⢻⡼⣹⢮⡽⣞⡵⣏\n");
        printf("\t\t⢧⣛⣞⢮⣳⣝⠰⡈⢆⡘⠄⡃⠌⢂⠡⡐⠂⢌⠀⠆⡁⠂⠤⢁⢺⡝⣮⢳⣭⠳⣏⡾⣵⢻⣜\n");
        printf("\t\t⣭⢷⣚⢯⢶⡹⣆⠡⢂⠜⡠⢑⡈⢂⠡⡐⠉⠤⢈⠐⠠⢁⢂⣦⢻⡜⣧⢻⠌⠣⢌⢳⡭⣗⢯\n");
        printf("\t\t⣎⡷⣫⢞⣧⢻⣜⡳⣌⠢⢑⣢⠘⠤⣁⠂⠍⡰⢀⢁⢊⡴⣫⠞⢣⡟⡼⠃⡌⡑⢢⢘⡷⣹⡞\n");
        printf("\t\t⣝⡾⣹⢞⡼⣳⢎⡷⣹⢦⣁⠣⡘⡴⡄⡉⢆⡐⢠⣎⠿⡜⢣⡟⡼⢉⠔⡡⢂⡑⢢⠸⣝⣧⣛\n");
        printf("\t\t⣧⡟⣵⢫⡞⣵⢫⡞⣵⢫⣞⡳⣖⡰⢠⠙⠆⡌⡷⣌⡹⡞⢣⠑⡄⢣⠘⠤⢡⠘⡄⢹⡞⣶⣭\n");
        printf("\n");
        break;
    }
}

void print_text(int stageNum){
    switch(stageNum){
    case 1:
        printf("\t(대사제)\n");
        printf("\t\"전하, 올해 풍작을 바라신다면 농경 신께 제사를 올리는 것이 어떠하겠습니까? \n");
        printf("\t한해의 시작을 신과 함께 하신다면 두려울 것이 무엇이 있겠습니까?\"\n");
        break;
    case 2:
        printf("\t(경비대장)\n");
        printf("\t\"폐하, 봄은 병사들을 훈련하기에 가장 적합한 시기입니다. \n");
        printf("\t지금 새로운 캠프를 세우면 방비를 더욱 철저히 할 수 있을 것입니다. \n");
        printf("\t다만, 캠프 주변의 농사에 차질이 생길 수 있겠으나 \n");
        printf("\t국력이 가장 중요한 사안 아니겠습니까.\"\n");
        break;
    case 3:
        printf("\t(궁정 관리관)\n");
        printf("\t\"폐하, 긴 겨울을 견뎌낸 백성들에게 봄 축제를 선물하시지요. \n");
        printf("\t음악과 춤, 그리고 화려한 꽃내음들이 함께하는 축제는 민심을 크게 고양시킬 것입니다.\"\n");
        break;
    case 4:
        printf("\t(행정 관리관)\n");
        printf("\t\"폐하, 이번에 귀족들에게 세금을 감면시켜달라는 입장을 전달받았습니다. \n");
        printf("\t세금을 감면해준다면 군대를 지원해줄 것이라는 약속을 받아내었기에 \n");
        printf("\t국력이 한층 더 강화될 것입니다.\"\n");
        break;
    case 5:
        printf("\t(경비대장)\n");
        printf("\t\"폐하, 여름은 성벽 보수에 최적의 계절이옵니다. \n");
        printf("\t성벽을 강화하고 방어 태세를 더욱 견고히 한다면 \n");
        printf("\t외적들은 더 이상 왕국을 넘볼 수 없을 것입니다.\"\n");
        break;
    case 6:
        printf("\t(왕실 의원)\n");
        printf("\t\"폐하, 외곽지역 한 작은 마을에서 역병이 돌고 있습니다. \n");
        printf("\t왕실 자금을 사용해 의약품과 치료 시설을 마련한다면 \n");
        printf("\t많은 생명을 구할 수 있을 것입니다. 다만, 재정에 큰 부담이 될 것입니다.\"\n");
        break;
    case 7:
        printf("\t(대사제)\n");
        printf("\t\"폐하, 역병은 신의 시험일지도 모릅니다. \n");
        printf("\t지금 당장 신의 이름 앞에 사죄하며 제사를 치룬다면 \n");
        printf("\t왕국은 신의 가호 아래에서 보호받을 것입니다.\"\n");
        break;
    case 8:
        printf("\t(대사제)\n");
        printf("\t\"폐하, 가을은 신에게 감사를 드리는 중요한 계절입니다. \n");
        printf("\t올해 첫 추수된 작물과 함께 대제사를 드린다면 \n");
        printf("\t왕국은 다가오는 겨울을 신의 가호 아래에서 보호받을 것입니다.\"\n");
        break;
    case 9:
        printf("\t(경비 대장)\n");
        printf("\t\"폐하, 가을은 날씨가 선선하여 군사 훈련을 강화하기에 적합한 시기입니다. \n");
        printf("\t일부 백성들이 군사 훈련에 강제로 동원되는 것에 불만을 가질 수 있습니다만, \n");
        printf("\t국력이 가장 중요한 것 아니겠습니까.\"\n");
        break;
    case 10:
        printf("\t(행정 관리관)\n");
        printf("\t\"폐하, 가을이 되니 왕국의 농작물을 먹어치우는 야생동물들이 급증하고 있습니다. \n");
        printf("\t토속신앙을 믿는 몇몇 마을이 사냥을 반대하는 것으로 보이지만, \n");
        printf("\t이를 해결하지 않는다면 농업 생산에 지장이 있을 것으로 보입니다.\"\n");
        break;
    case 11:
        printf("\t(농업 관리관)\n");
        printf("\t\"폐하, 겨울이 다가오고 있습니다. 백성들이 고통받지 않도록 식량을 비축하는 것이 중요합니다. \n");
        printf("\t비축을 많이 해놓으면 겨울철 위기를 대비할 수 있겠지만, 사용되는 재정은 상당할 것입니다.\"\n");
        break;
    case 12:
        printf("\t(경비대장)\n");
        printf("\t\"폐하, 수도를 덮친 눈폭풍이 왕국의 교통을 마비시키고 있습니다. \n");
        printf("\t이를 해결하기 위해서는 군사를 움직여야 할 것입니다. \n");
        printf("\t하지만 그렇게 한다면 군사력 부족으로 인해 교역이 일시적으로 중단될 것입니다.\"\n");
        break;
    case 13:
        printf("\t(사제)\n");
        printf("\t\"폐하, 겨울은 신앙의 힘이 더욱 필요한 시기입니다. \n");
        printf("\t대제사를 통해 추위에 굳어진 몸에 활기를 넣고 생동감을 불어넣는다면 \n");
        printf("\t올해의 마무리는 걱정이 없을 것입니다.\"\n");
        break;
    case 14:
        printf("\t(재정 관리관)\n");
        printf("\t\"폐하, 겨울철에는 백성들의 생활이 더욱 힘듭니다. \n");
        printf("\t조세를 경감하거나 면제한다면 겨울철 굶주리고 있는 \n");
        printf("\t백성들의 지지를 얻을 수 있을 것입니다.\"\n");
        break;
    case 15:
        printf("\t(궁정 관리관)\n");
        printf("\t\"폐하, 겨울 동안 백성들이 많은 고통을 겪었습니다. \n");
        printf("\t점차 날씨가 풀리고 있는 지금 눈이 모두 녹기 전 눈축제를 연다면 \n");
        printf("\t백성들은 축제에 참여하여 폐하를 찬양할 것입니다.\"\n");   
        break;
    }
}

void print_gameScreen(Gamedata* gamedata){
    //UI 출력 부분
    print_UI(gamedata->kingdom);
    //아스키 아트 출력 부분
    print_art(gamedata->stageNum);
    //게임 텍스트 출력 부분
    print_text(gamedata->stageNum);
}

void print_selectText(int stageNum, int playerNum){ //선택지 선택 이후 결과 텍스트
    switch(stageNum){
    case 1:
        if(playerNum == 1){
            printf("\n\t1. 제사를 올린다 (재정↓↓, 신앙↑↑)\n");
            printf("\t\"신께 제사를 드림으로써 풍요로운 한 해를 약속받고자 한다. 신들의 가호는 백성들의 안녕을 보장할 것이다.\"\n\n");
            printf("\t2. 제사를 올리지 않는다 (신앙↓↓, 국력↑↑)\n");
            printf("\t\"제사는 불필요하다. 대신 실질적인 농업 정책을 강화하여 풍년을 이루겠다.\"\n");
            break;
        }
        else if(playerNum == 2){
            printf("\n\t1. 농업 개혁을 통해 자립을 강조한다 (국력↑, 재정↓↓, 민심↑, 신앙↓)\n");
            printf("\t\"전하, 제사 대신 농민들에게 새로운 농업 기술과 지원을 제공하는 개혁을 추진해야 하옵니다. \n\t신의 가호보다 백성의 땀이 풍년을 보장할 것이옵니다.\"\n\n");
            printf("\t2. 신전 주도의 농업 축제를 개최한다 (신앙↑↑, 재정↓↓, 민심↑↑)\n");
            printf("\t\"제사를 올리기보다 신전에서 농업 축제를 여는 것이 어떠하옵니까? \n\t이는 신의 축복을 구하면서도 백성들과 함께하는 기회가 될 것이옵니다.\"\n");
            break;
        }
    case 2:
        if(playerNum == 1){
            printf("\n\t1. 병사들을 훈련한다 (민심↓, 재정↓, 국력↑↑)\n");
            printf("\t\"군사력이 강해야 외적의 위협도 막을 수 있다. 훈련은 곧 왕국의 힘이다. 당장 명령을 내려라!\"\n\n");
            printf("\t2. 병사들을 훈련하지 않는다 (국력↓↓, 민심↑, 재정↑)\n");
            printf("\t\"지금은 방비보다 백성들의 삶이 먼저다. 병사들에게 휴식을 허락하라.\"\n");
            break;
        }
        else if(playerNum == 2){
            printf("\n\t1. 지역 방비 체계 정비를 제안한다 (민심↑, 재정↓, 국력↑)\n");
            printf("\t\"전하, 병사 훈련 대신 지역 방어 체계를 개선해 방비를 강화하면 \n");
            printf("\t백성들의 불만을 최소화할 수 있을 것이옵니다.\"\n\n");
            printf("\t2. 농업과 군사 지원을 병행한다 (민심↑, 재정↓↓, 국력↑)\n");
            printf("\t\"농사에 지장을 주지 않도록 훈련 대신 농업 지원을 강화하며 \n");
            printf("\t병사들을 도와줄 농민군을 구성해보는 것이 어떠하옵니까?\"\n");
            break;
        }
    case 3:
        if(playerNum == 1){
            printf("\n\t1. 봄 축제를 시작한다 (재정↓↓, 민심↑↑)\n");
            printf("\t\"축제는 백성의 사기를 북돋우는 가장 빠른 길이다. 화려하게 준비하도록 하라!\"\n\n");
            printf("\t2. 봄 축제를 열지 않는다 (민심↓↓, 재정↑)\n");
            printf("\t\"축제를 열 여유가 없다. 백성들에게 더 중요한 것은 안정된 삶이다.\"\n");
            break;
        }
        else if(playerNum == 2){
            printf("\n\t1. 지역 축제를 장려한다 (민심↑, 재정↓)\n");
            printf("\t\"대규모 축제 대신 각 지방에서 소규모 봄 축제를 장려하는 것이 \n");
            printf("\t백성들에게 더 직접적으로 다가갈 수 있을 것이옵니다.\"\n\n");
            printf("\t2. 자연 보호와 감사의 날을 제안한다 (민심↑, 재정↓, 신앙↑)\n");
            printf("\t\"축제 대신 봄의 자연에 감사를 표하는 행사를 마련하면 \n");
            printf("\t신앙과 민심을 함께 얻을 수 있을 것이옵니다.\"\n");
            break;
        }
    case 4:
        if(playerNum == 1){
            printf("\n\t1. 세금을 감면시켜준다 (민심↓↓, 재정↓, 국력↑↑)\n");
            printf("\t\"귀족들의 지원이 곧 왕국의 힘이다. 이번만큼은 그들의 요구를 들어주겠다.\"\n\n");
            printf("\t2. 세금을 감면시켜주지 않는다 (국력↓↓, 민심↑, 재정↑)\n");
            printf("\t\"귀족들이 힘을 모으지 않아도 왕국은 굳건하다. 백성의 부담을 우선 생각하겠다.\"\n");
            break;
        }
        else if(playerNum == 2){
            printf("\n\t1. 귀족에게 지원 조건을 추가한다 (국력↑, 재정↓↓, 민심↓)\n");
            printf("\t\"귀족들이 국력 증가에 실제로 기여하도록 구체적인 지원 조건을 약속받는 것이 중요하옵니다.\"\n\n");
            printf("\t2. 세금 감면 대신 공공 사업을 제안한다 (재정↓, 민심↑, 국력↓)\n");
            printf("\t\"세금 감면 대신 공공 사업을 추진해 귀족과 백성 모두에게 이익을 나눠주는 것이 어떠하옵니까?\"\n");
            break;
        }
    case 5:
        if(playerNum == 1){
            printf("\n\t1. 성벽 보수를 명령한다 (재정↓↓, 국력↑↑)\n");
            printf("\t\"성벽은 왕국의 얼굴이자 방패다. 금이 들더라도 반드시 보수해야 한다!\"\n\n");
            printf("\t2. 성벽 보수를 명령하지 않는다 (국력↓↓, 재정↑↑)\n");
            printf("\t\"지금은 방어보다 다른 부분에 재정을 써야 한다. 이후를 대비하겠다.\"\n");
            break;
        }
        else if(playerNum == 2){
            printf("\n\t1. 마을 방어선만을 보강한다 (재정↓, 국력↑, 민심↑)\n");
            printf("\t\"전국 성벽 보수 대신 주요 도시와 마을 방어선을 보강해 \n");
            printf("\t효과적으로 외적을 방어하는 것이 좋을 것이옵니다.\"\n\n");
            printf("\t2. 무기 체계를 개선한다 (국력↑, 재정↓↓)\n");
            printf("\t\"성벽 대신 검과 화살 제작 등 군사 물자를 강화하면 더욱 효율적인 방비가 될 것이옵니다.\"\n");
            break;
        }
    case 6:
        if(playerNum == 1){
            printf("\n\t1. 치료를 명령한다 (재정↓↓, 민심↑↑)\n");
            printf("\t\"백성의 생명을 외면한다면 왕의 자격이 없다. 즉시 필요한 자금을 할당하라!\"\n\n");
            printf("\t2. 치료를 명령하지 않는다 (민심↓↓, 재정↑↑)\n");
            printf("\t\"재정이 바닥나면 왕국 전체가 위험해진다. 부디 스스로 극복하기를 바란다.\"\n");
            break;
        }
        else if(playerNum == 2){
            printf("\n\t1. 지역 의료 사업을 추진한다 (재정↓↓, 민심↑, 국력↑)\n");
            printf("\t\"역병 대응을 계기로 전국적인 지역 의료사업을 추진하는 것은 \n");
            printf("\t백성을 안정시키는 좋은 기회가 될 것이옵니다.\"\n\n");
            printf("\t2. 피난민 수용소를 세운다 (재정↓, 민심↑↑, 신앙↓)\n");
            printf("\t\"환자들을 따로 수용하여 역병 확산을 막고 백성들의 신뢰를 얻어야 하옵니다.\"\n");
            break;
        }
    case 7:
        if(playerNum == 1){
            printf("\n\t1. 신의 이름으로 제사를 치른다 (재정↓↓, 국력↓, 신앙↑↑)\n");
            printf("\t\"신의 뜻을 따르는 것이 올바른 길이다. 지금 바로 제사를 준비하라.\"\n\n");
            printf("\t2. 제사를 치르지 않는다 (신앙↓↓, 재정↑↑, 국력↑)\n");
            printf("\t\"역병은 신의 시험이 아니다. 이 문제를 해결할 다른 방안을 찾겠다.\"\n");
            break;
        }
        else if(playerNum == 2){
            printf("\n\t1. 자연의 균형을 강조한다 (신앙↑, 민심↑, 국력↓)\n");
            printf("\t\"제사 대신 자연의 회복을 기도하며 균형을 되찾는 활동을 장려하는 것이 어떠하옵니까?\"\n\n");
            printf("\t2. 공공 위생을 강화한다 (재정↓, 민심↑, 국력↑)\n");
            printf("\t\"신의 가호도 중요하지만, 백성들에게 깨끗한 환경을 제공해야 \n");
            printf("\t역병을 근본적으로 막을 수 있을 것이옵니다.\"\n");
            break;
        }
    case 8:
        if(playerNum == 1){
            printf("\n\t1. 대제사를 명령한다 (국력↓, 재정↓↓, 신앙↑↑)\n");
            printf("\t\"올해의 풍요는 신의 은혜다. 감사의 마음을 담아 대제사를 준비하라.\"\n\n");
            printf("\t2. 대제사를 명령하지 않는다 (신앙↓↓, 국력↑, 재정↑)\n");
            printf("\t\"대제사보다 겨울 대비가 더 중요하다. 백성의 생존이 우선이다.\"\n");
            break;
        }
        else if(playerNum == 2){
            printf("\n\t1. 감사 연회를 제안한다 (재정↓, 민심↑, 신앙↑)\n");
            printf("\t\"대규모 제사 대신 백성들과 함께하는 감사 연회를 마련하여 \n");
            printf("\t신의 축복을 기리면 신앙과 민심 모두를 얻을 수 있을 것이옵니다.\"\n\n");
            printf("\t2. 농업 개선을 지원한다 (재정↓↓, 민심↑, 국력↑)\n");
            printf("\t\"제사 비용을 농업 혁신에 투자하여 백성들의 삶과 \n");
            printf("\t왕국의 힘을 동시에 개선할 방법을 찾아보는 것이 어떠하옵니까?\"\n");
            break;
        }
    case 9:
        if(playerNum == 1){
            printf("\n\t1. 군사 훈련을 명령한다 (민심↓, 재정↓, 국력↑↑)\n");
            printf("\t\"왕국의 안보를 위해 군사 훈련은 반드시 필요하다. \n");
            printf("\t백성들에게도 왕국의 안위를 설명하여 이해시키도록 하라.\"\n\n");
            printf("\t2. 군사 훈련을 명령하지 않는다 (국력↓↓, 민심↑, 재정↑)\n");
            printf("\t\"지금은 백성들의 불만을 잠재우는 것이 우선이다. \n");
            printf("\t군사 훈련은 추후 다시 논의하도록 하겠다.\"\n");
            break;
        }
        else if(playerNum == 2){
            printf("\n\t1. 백성 자치 방위를 제안한다 (민심↑, 국력↑, 재정↓)\n");
            printf("\t\"군사 훈련 대신 마을 단위의 자치 방위 체계를 구축한다면 \n");
            printf("\t민심을 얻으면서도 방비를 강화할 수 있을 것이옵니다.\"\n\n");
            printf("\t2. 훈련 대상을 축소시킨다. (민심↑, 재정↑, 국력↓)\n");
            printf("\t\"모든 백성을 동원하지 않고, 적합한 대상만 훈련에 참여시켜 불만을 최소화하는 것이 현명할 것입니다.\"\n");
            break;
        }
    case 10:
        if(playerNum == 1){
            printf("\n\t1. 사냥을 명령한다 (민심↓↓, 국력↑↑)\n");
            printf("\t\"농업은 왕국의 기반이다. 모든 수단을 동원해 농작물을 보호하라.\"\n\n");
            printf("\t2. 사냥을 명령하지 않는다 (국력↓↓, 민심↑↑)\n");
            printf("\t\"마을의 신앙을 존중하겠다. 농업 피해를 최소화할 다른 방안을 모색하라.\"\n");
            break;
        }
        else if(playerNum == 2){
            printf("\n\t1. 야생동물 대책 회의를 연다 (민심↑, 재정↓, 국력↑)\n");
            printf("\t\"사냥 대신 지역별 해결 방안을 도출하고 마을 주민들의 협력을 얻는 방법을 모색하옵니다.\"\n\n");
            printf("\t2. 농작물 보호 제도를 도입한다 (재정↓↓, 민심↑, 국력↑)\n");
            printf("\t\"농작물을 보호하는 새로운 제도를 통해 농업 피해를 줄이고 \n");
            printf("\t백성들의 불만을 잠재울 수 있을 것이옵니다.\"\n");
            break;
        }
    case 11:
        if(playerNum == 1){
            printf("\n\t1. 겨울을 대비한다 (재정↓↓, 민심↑, 국력↑)\n");
            printf("\t\"백성들이 굶주리는 것을 내버려 둘 수는 없다. \n");
            printf("\t지금 재정을 투자해 겨울의 위기를 미리 막아야 한다.\"\n\n");
            printf("\t2. 겨울을 대비하지 않는다 (민심↓, 국력↓, 재정↑↑)\n");
            printf("\t\"왕국의 재정은 바닥날 위기에 처해 있다. \n");
            printf("\t당장은 절약해야 하니, 백성들도 스스로 견뎌내길 바란다.\"\n");
            break;
        }
        else if(playerNum == 2){
            printf("\n\t1. 공동 창구를 만든다 (민심↑, 국력↑, 재정↓)\n");
            printf("\t\"왕국 전체의 자원을 통합적으로 관리할 수 있는 \n");
            printf("\t공동 창고를 마련하면 비축 효율을 높일 수 있을 것입니다.\"\n\n");
            printf("\t2. 농민들에게 비축 장려금을 지급한다 (재정↓↓, 민심↑↑)\n");
            printf("\t\"농민들이 자발적으로 식량을 비축하도록 지원금을 지급해 백성들의 신뢰를 얻어야 하옵니다.\"\n");
            break;
        }
    case 12:
        if(playerNum == 1){
            printf("\n\t1. 수도의 교통문제를 해결한다 (재정↓↓, 국력↓, 민심↑↑)\n");
            printf("\t\"백성의 불편을 외면할 수 없다. 모든 자원을 동원해 눈폭풍 문제를 해결하라.\"\n\n");
            printf("\t2. 수도의 교통문제를 해결하지 않는다 (민심↓↓, 재정↑↑, 국력↑)\n");
            printf("\t\"군사력을 낭비할 수 없다. 눈폭풍은 시간이 해결해줄 것이다.\"\n");
            break;
        }
        else if(playerNum == 2){
            printf("\n\t1. 자원 봉사 체계를 조직한다 (민심↑, 재정↓, 국력↓)\n");
            printf("\t\"군사 대신 민간 자원 봉사자들을 동원해 교통 문제를 해결하는 것이 어떠하옵니까?\"\n\n");
            printf("\t2. 지역 간 물자 배분을 조정한다 (민심↑, 국력↓, 재정↑)\n");
            printf("\t\"즉각적인 해결보다는 다른 지역에서 수도로 물자를 이송하여 위기를 극복해보는 것은 어떠하옵니까?\"\n");
            break;
        }
    case 13:
        if(playerNum == 1){
            printf("\n\t1. 겨울 대제사를 명령한다 (재정↓↓, 국력↓↓, 신앙↑↑)\n");
            printf("\t\"신의 가호 없이는 이 겨울을 넘길 수 없다. 대제사를 준비하라.\"\n\n");
            printf("\t2. 겨울 대제사를 명령하지 않는다 (신앙↓↓, 재정↑↑, 국력↑↑)\n");
            printf("\t\"신의 뜻은 스스로의 의지에서 찾는 법. 대제사는 불필요하다.\"\n");
            break;
        }
        else if(playerNum == 2){
            printf("\n\t1. 백성들의 신앙 활동을 장려한다 (민심↑, 신앙↑, 재정↓)\n");
            printf("\t\"대규모 제사 대신 신전에서 백성들이 자율적으로 신앙 활동을 할 수 있도록 장려하는 것이 좋을 것이옵니다.\"\n\n");
            printf("\t2. 신전을 통한 구호활동을 강화한다 (신앙↑, 재정↓↓, 민심↑)\n");
            printf("\t\"신전을 통해 겨울철 구호 활동을 강화하여 신앙심과 민심을 함께 얻어야 하옵니다.\"\n");
            break;
        }
    case 14:
        if(playerNum == 1){
            printf("\n\t1. 조세를 경감한다 (재정↓↓, 국력↓↓, 민심↑↑)\n");
            printf("\t\"백성의 고통을 덜어주기 위해 조세를 경감한다. 희생 없이 지지는 얻을 수 없다.\"\n\n");
            printf("\t2. 조세를 유지한다 (민심↓↓, 재정↑↑, 국력↑↑)\n");
            printf("\t\"재정은 왕국의 근본이다. 조세를 유지하되, 백성들의 어려움은 다른 방식으로 도울 것이다.\"\n");
            break;
        }
        else if(playerNum == 2){
            printf("\n\t1. 하층 계급의 조세를 감면한다 (민심↑, 재정↓, 국력↓)\n");
            printf("\t\"전하, 모든 계층이 아니라 농민 등 하층 계급의 조세를 경감한다면 효율적으로 민심을 얻을 수 있을 것입니다.\"\n\n");
            printf("\t2. 공공 식량 배급을 시행한다 (민심↑ 재정↓↓ 신앙↑)\n");
            printf("\t\"조세를 유지하되, 겨울철 공공 식량 배급으로 백성들의 고통을 줄여야 할 것입니다.\"\n");
            break;
        }
    case 15:
        if(playerNum == 1){
            printf("\n\t1. 눈축제를 연다 (재정↓↓, 민심↑↑)\n");
            printf("\t\"축제를 통해 백성들에게 희망을 주겠다. 화려하게 준비하라!\"\n\n");
            printf("\t2. 눈축제를 열지 않는다 (민심↓↓, 재정↑↑)\n");
            printf("\t\"지금은 재정이 우선이다. 축제는 내년에 다시 논의하도록 하자.\"\n");
            break;
        }
        else if(playerNum == 2){
            printf("\n\t1. 눈축제를 간소화한다 (재정↓, 민심↑)\n");
            printf("\t\"대규모 축제 대신 간소한 축제를 열어 재정을 절약하면서 민심을 얻을 수 있을 것입니다.\"\n\n");
            printf("\t2. 눈축제 대신 공공 행사를 제안한다 (민심↑↑, 재정↓)\n");
            printf("\t\"눈축제 대신 백성들과 함께 소통할 수 있는 공공 행사를 여는 것이 더 효과적일 것이옵니다.\"\n");
            break;
        }
    }
}

void gamedata_update(Gamedata* gamedata){
    switch(gamedata->stageNum){
        case 1:
            if(gamedata->p1select == 1){
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
                gamedata->kingdom.faith =  gamedata->kingdom.faith + 10;
            } else if(gamedata->p1select == 2){
                gamedata->kingdom.power = gamedata->kingdom.power + 10;
                gamedata->kingdom.faith =  gamedata->kingdom.faith - 10;
            }
            
            if(gamedata->p2select == 1){
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.faith =  gamedata->kingdom.faith - 5;
            } else if(gamedata->p2select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public + 10;
                gamedata->kingdom.faith =  gamedata->kingdom.faith + 10;
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
            }
            break;
        case 2:
            if(gamedata->p1select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public - 5;
                gamedata->kingdom.power =  gamedata->kingdom.power + 10;
                gamedata->kingdom.gold = gamedata->kingdom.gold - 5;
            } else if(gamedata->p1select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.power =  gamedata->kingdom.power - 10;
                gamedata->kingdom.gold = gamedata->kingdom.gold + 5;
            }
            
            if(gamedata->p2select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
                gamedata->kingdom.gold = gamedata->kingdom.gold - 5;
            } else if(gamedata->p2select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
            }
            break;
        case 3:
            if(gamedata->p1select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public + 10;
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
            } else if(gamedata->p1select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public - 10;
                gamedata->kingdom.gold = gamedata->kingdom.gold + 5;
            }
            
            if(gamedata->p2select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.gold = gamedata->kingdom.gold - 5;
            } else if(gamedata->p2select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.gold = gamedata->kingdom.gold - 5;
                gamedata->kingdom.faith =  gamedata->kingdom.faith + 5;
            }
            break;
        case 4:
            if(gamedata->p1select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public - 10;
                gamedata->kingdom.power =  gamedata->kingdom.power + 10;
                gamedata->kingdom.gold = gamedata->kingdom.gold - 5;
            } else if(gamedata->p1select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.power =  gamedata->kingdom.power - 10;
                gamedata->kingdom.gold = gamedata->kingdom.gold + 5;
            }
            
            if(gamedata->p2select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public - 5;
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
            } else if(gamedata->p2select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.power =  gamedata->kingdom.power - 5;
                gamedata->kingdom.gold = gamedata->kingdom.gold - 5;
            }
            break;
        case 5:
            if(gamedata->p1select == 1){
                gamedata->kingdom.power =  gamedata->kingdom.power + 10;
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
            } else if(gamedata->p1select == 2){
                gamedata->kingdom.power =  gamedata->kingdom.power - 10;
                gamedata->kingdom.gold = gamedata->kingdom.gold + 10;
            }
            
            if(gamedata->p2select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
                gamedata->kingdom.gold = gamedata->kingdom.gold - 5;
            } else if(gamedata->p2select == 2){
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
            }
            break;
        case 6:
            if(gamedata->p1select == 1){
                gamedata->kingdom.power =  gamedata->kingdom.power + 10;
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
            } else if(gamedata->p1select == 2){
                gamedata->kingdom.power =  gamedata->kingdom.power - 10;
                gamedata->kingdom.gold = gamedata->kingdom.gold + 10;
            }
            
            if(gamedata->p2select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
            } else if(gamedata->p2select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public + 10;
                gamedata->kingdom.faith =  gamedata->kingdom.faith - 5;
                gamedata->kingdom.gold = gamedata->kingdom.gold - 5;
            }
            break;
        case 7:
            if(gamedata->p1select == 1){
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
                gamedata->kingdom.faith =  gamedata->kingdom.faith + 10;
                gamedata->kingdom.power =  gamedata->kingdom.power - 5;
            } else if(gamedata->p1select == 2){
                gamedata->kingdom.gold = gamedata->kingdom.gold + 10;
                gamedata->kingdom.faith =  gamedata->kingdom.faith - 10;
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
            }
            
            if(gamedata->p2select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.faith =  gamedata->kingdom.faith + 5;
                gamedata->kingdom.power =  gamedata->kingdom.power - 5;
            } else if(gamedata->p2select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.gold =  gamedata->kingdom.gold - 5;
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
            }
            break;
        case 8:
            if(gamedata->p1select == 1){
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
                gamedata->kingdom.faith =  gamedata->kingdom.faith + 10;
                gamedata->kingdom.power =  gamedata->kingdom.power - 5;
            } else if(gamedata->p1select == 2){
                gamedata->kingdom.gold = gamedata->kingdom.gold + 5;
                gamedata->kingdom.faith =  gamedata->kingdom.faith - 10;
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
            }
            
            if(gamedata->p2select == 1){
                gamedata->kingdom.gold = gamedata->kingdom.gold - 5;
                gamedata->kingdom.faith =  gamedata->kingdom.faith + 5;
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
            } else if(gamedata->p2select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.gold =  gamedata->kingdom.gold - 10;
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
            }
            break;
        case 9:
            if(gamedata->p1select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public - 5;
                gamedata->kingdom.gold =  gamedata->kingdom.gold - 5;
                gamedata->kingdom.power =  gamedata->kingdom.power + 10;
            } else if(gamedata->p1select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.gold =  gamedata->kingdom.gold + 5;
                gamedata->kingdom.power =  gamedata->kingdom.power - 10;
            }
            
            if(gamedata->p2select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.gold =  gamedata->kingdom.gold - 5;
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
            } else if(gamedata->p2select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.gold =  gamedata->kingdom.gold + 5;
                gamedata->kingdom.power =  gamedata->kingdom.power - 5;
            }
            break;
        case 10:
            if(gamedata->p1select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public - 10;
                gamedata->kingdom.power =  gamedata->kingdom.power + 10;
            } else if(gamedata->p1select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public + 10;
                gamedata->kingdom.power =  gamedata->kingdom.power - 10;
            }
            
            if(gamedata->p2select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.gold =  gamedata->kingdom.gold - 5;
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
            } else if(gamedata->p2select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.gold =  gamedata->kingdom.gold - 10;
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
            }
            break;
        case 11:
            if(gamedata->p1select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.gold =  gamedata->kingdom.gold - 10;
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
            } else if(gamedata->p1select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public - 5;
                gamedata->kingdom.gold =  gamedata->kingdom.gold + 10;
                gamedata->kingdom.power =  gamedata->kingdom.power - 5;
            }
            
            if(gamedata->p2select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.gold =  gamedata->kingdom.gold - 5;
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
            } else if(gamedata->p2select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public + 10;
                gamedata->kingdom.gold =  gamedata->kingdom.gold - 10;
            }
            break;
        case 12:
            if(gamedata->p1select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public + 10;
                gamedata->kingdom.gold =  gamedata->kingdom.gold - 10;
                gamedata->kingdom.power =  gamedata->kingdom.power - 5;
            } else if(gamedata->p1select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public - 10;
                gamedata->kingdom.gold =  gamedata->kingdom.gold + 10;
                gamedata->kingdom.power =  gamedata->kingdom.power + 5;
            }
            
            if(gamedata->p2select == 1){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.gold =  gamedata->kingdom.gold - 5;
                gamedata->kingdom.power =  gamedata->kingdom.power - 5;
            } else if(gamedata->p2select == 2){
                gamedata->kingdom.public = gamedata->kingdom.public + 5;
                gamedata->kingdom.gold =  gamedata->kingdom.gold + 5;
                gamedata->kingdom.power =  gamedata->kingdom.power - 5;
            }
            break;
        case 13:
            if(gamedata->p1select == 1){
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
                gamedata->kingdom.faith =  gamedata->kingdom.faith + 10;
                gamedata->kingdom.power =  gamedata->kingdom.power - 10;
            } else if(gamedata->p1select == 2){
                gamedata->kingdom.gold = gamedata->kingdom.gold + 10;
                gamedata->kingdom.faith =  gamedata->kingdom.faith - 10;
                gamedata->kingdom.power =  gamedata->kingdom.power + 10;
            }
            
            if(gamedata->p2select == 1){
                gamedata->kingdom.gold = gamedata->kingdom.gold - 5;
                gamedata->kingdom.faith =  gamedata->kingdom.faith + 5;
                gamedata->kingdom.public =  gamedata->kingdom.public + 5;
            } else if(gamedata->p2select == 2){
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
                gamedata->kingdom.faith =  gamedata->kingdom.faith + 5;
                gamedata->kingdom.public =  gamedata->kingdom.public + 5;
            }
            break;
        case 14:
            if(gamedata->p1select == 1){
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
                gamedata->kingdom.public =  gamedata->kingdom.public + 10;
                gamedata->kingdom.power =  gamedata->kingdom.power - 10;
            } else if(gamedata->p1select == 2){
                gamedata->kingdom.gold = gamedata->kingdom.gold + 10;
                gamedata->kingdom.public =  gamedata->kingdom.public - 10;
                gamedata->kingdom.power =  gamedata->kingdom.power + 10;
            }
            
            if(gamedata->p2select == 1){
                gamedata->kingdom.gold = gamedata->kingdom.gold - 5;
                gamedata->kingdom.public =  gamedata->kingdom.public + 5;
                gamedata->kingdom.power =  gamedata->kingdom.power - 5;
            } else if(gamedata->p2select == 2){
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
                gamedata->kingdom.public =  gamedata->kingdom.public + 5;
                gamedata->kingdom.faith =  gamedata->kingdom.faith + 5;
            }
            break;
        case 15:
            if(gamedata->p1select == 1){
                gamedata->kingdom.gold = gamedata->kingdom.gold - 10;
                gamedata->kingdom.public =  gamedata->kingdom.public + 10;
            } else if(gamedata->p1select == 2){
                gamedata->kingdom.gold = gamedata->kingdom.gold + 10;
                gamedata->kingdom.public =  gamedata->kingdom.public - 10;
            }
            
            if(gamedata->p2select == 1){
                gamedata->kingdom.gold = gamedata->kingdom.gold - 5;
                gamedata->kingdom.public =  gamedata->kingdom.public + 5;
            } else if(gamedata->p2select == 2){
                gamedata->kingdom.gold = gamedata->kingdom.gold - 5;
                gamedata->kingdom.public =  gamedata->kingdom.public + 10;
            }
            break;
    } //스테이지 별 선택에 따른 능력치 변화
    
    if(gamedata->kingdom.gold <= 0){ //재정이 0이하인 경우
        gamedata->endNum = 2;
        gamedata->gameState = 3;
    }
    else if(gamedata->kingdom.gold >= 100){ //재정이 100이상인 경우
        gamedata->endNum = 3;
        gamedata->gameState = 3;
    }
    else if(gamedata->kingdom.public <= 0){ //민심이 0이하인 경우
        gamedata->endNum = 4;
        gamedata->gameState = 3;
    }
    else if(gamedata->kingdom.public >= 100){ //민심이 100이상인 경우
        gamedata->endNum = 5;
        gamedata->gameState = 3;
    }
    else if(gamedata->kingdom.power <= 0){ //국력이 0이하인 경우
        gamedata->endNum = 6;
        gamedata->gameState = 3;
    }
    else if(gamedata->kingdom.power >= 100){ //국력이 100이상인 경우
        gamedata->endNum = 7;
        gamedata->gameState = 3;
    }
    else if(gamedata->kingdom.faith <= 0){ //신앙이 0이하인 경우
        gamedata->endNum = 8;
        gamedata->gameState = 3;
    }
    else if(gamedata->kingdom.faith >= 100){ //신앙이 100이상인 경우
        gamedata->endNum = 9;
        gamedata->gameState = 3;
    }
    
    if(gamedata->endNum == 0){
        gamedata->stageNum++; //엔딩이 아니라면 스테이지 올려주기
    }
    
    if(gamedata->stageNum == 16){ //16스테이지까지 갔다면 해피엔딩
        gamedata->endNum = 1;
        gamedata->gameState = 3;
    }
    
    gamedata->p1select = 0;
    gamedata->p2select = 0;
}

void printChanges(Gamedata* gamedata) {
    printf("\n\t플레이어의 선택에 의해 왕국의 자원이 변경되었습니다.\n\n");
    printf("\t\t민심: %d -> %d\n", gamedata->beforeKingdom.public, gamedata->kingdom.public);
    printf("\t\t재정: %d -> %d\n", gamedata->beforeKingdom.gold, gamedata->kingdom.gold);
    printf("\t\t국력: %d -> %d\n", gamedata->beforeKingdom.power, gamedata->kingdom.power);
    printf("\t\t신앙: %d -> %d\n", gamedata->beforeKingdom.faith, gamedata->kingdom.faith);
    sleep(3);
}

void print_ending(int endNum){
    switch(endNum){
        case 1:
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣟⣿⣏⣿⣏⣿⡽⣿⢼⣿⣽⡿⣽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣽⣿⣞⣿⡞⣿⠈⠁⣿⡃⢘⣻⢧⢻⡹⣿⣳⣿⣻⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⡿⣷⣟⢿⣮⢿⣎⢿⡹⠆⢸⢾⡅⠸⣟⣼⢣⡽⣱⠿⣵⡿⣯⣿⢿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣻⢿⣾⡻⢷⣝⢧⣛⣎⠷⠀⠀⠀⠀⣹⠲⣌⠳⣼⢋⡿⣫⣾⢻⣵⡿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣟⡿⣷⣯⡻⢧⣛⢮⡱⢎⡹⠀⠀⠀⠀⣇⠫⡜⠳⣌⠯⡼⣹⢼⣻⣽⡾⢿⣻⣿⣿⣿\n");
            printf("\t⣿⣿⣽⣟⡿⢷⣮⣝⡳⢭⠲⣍⢮⡱⠀⠀⠆⠀⣌⠳⣌⠳⣌⠳⣍⠳⣞⣭⣷⠾⣿⣻⣿⣿⣿\n");
            printf("\t⣿⣿⣽⣻⣻⡟⡶⡭⣝⢣⡛⡴⢃⡞⠀⠀⠂⠀⣌⠳⣌⠳⣌⠳⣌⠳⡭⠶⢾⣻⣛⣿⣯⣿⣿\n");
            printf("\t⣿⣯⣿⣽⣏⣿⣙⣳⢎⡣⢵⣩⠳⡜⠀⠀⠀⠀⣎⠱⣌⠳⣌⠳⣌⢳⣙⣛⣏⣻⣽⣯⣿⣽⣿\n");
            printf("\t⣿⣿⣽⣭⣯⣭⡧⠷⣎⠵⢣⠒⠉⠈⠱⢠⡀⠐⣌⠳⣌⠳⣌⠳⣌⠳⡼⠼⣼⣭⣽⣭⣿⣿⣿\n");
            printf("\t⣿⣿⣯⣿⣷⠷⣞⣻⡥⣋⠧⡀⠀⠀⠀⠀⠘⠸⣌⠳⣌⠓⣌⡳⣌⠳⣭⣛⠶⣷⣯⣿⣽⣿⣿\n");
            printf("\t⣿⣿⣿⣷⣾⢿⣫⣵⠞⣥⠓⣅⠀⠀⠀⠀⠀⠰⣌⠳⣈⠳⣌⠷⡬⣓⢧⣯⣛⡷⣾⣯⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣽⣾⢿⣫⣾⢟⣥⠟⡤⠀⠀⠀⠀⠀⠰⣌⠳⣌⠳⣜⣣⠷⣭⡳⣾⣝⡿⣷⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⡿⣟⣵⡿⣱⢎⠲⣁⠀⠀⠀⠀⠰⣌⠳⣈⢳⡼⣡⢿⣜⢿⣮⡿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣻⣾⢟⡌⠳⣌⠀⠀⠀⠀⠰⣌⠳⣌⡳⣜⢧⣳⡻⣿⣽⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣻⣿⡌⠳⡌⡅⢀⠀⠀⠰⣌⢳⡰⡝⢮⠶⣽⣿⣟⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠑⠘⠌⠣⢎⡱⣛⣬⢳⡙⢞⡹⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠱⡌⢦⡙⢦⢹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\n\t엔딩 1 : 찬란한 미래 (게임 클리어)\n");
            printf("\n\t\"폐하, 지난 1년은 너무나도 혹독한 시기였습니다.\n");
            printf("\t역병과 눈폭풍, 약탈해오는 야생동물까지... 그러나 우리는, 그 모든 것을 넘겼습니다.\n");
            printf("\t겨울은 끝나고 봄이 왔습니다. 이 위대한 여정을 함께한 뱅석들에게 감사의 연설을 하고,\n");
            printf("\t더 나은 왕국을 위해 나아가야 할 때입니다.\"\n");
            break;
        case 2:
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⣀⣿⣏⠀⣎⣿⣿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏⠀⠿⢿⡏⢸⣾⣿⣇⠻⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⢠⢏⡟⡻⢇⣻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠁⡜⠮⣜⡱⣋⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⢢⡠⠀⠠⡜⡱⢢⠱⣽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⠐⠉⠆⠀⠴⣑⣿⢃⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⠹⣿⣿⣿⣿⣿⣿⣟⠀⠀⠀⠀⢠⠓⣌⠲⣉⣿⣿⣿⣿⡿⣿⣿⡇⢿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⠃⡜⣿⣿⣿⣿⣿⣿⣿⣧⡀⠀⠀⢦⡙⢤⢓⣹⣿⣿⣿⣿⠳⣿⣿⠇⢨⢿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣧⠀⣞⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⢰⣢⣽⣶⣿⣿⣿⣿⣿⡇⣼⣻⣿⣧⡐⣾⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⢾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣏⣾⣿⢿⣿⣿⣿⣿⣿⣿⡷⣿⣿⣿⣿⣿⢿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣏⣿⣿⣿⣿⣿⣿⣿⣿⣯⣿⣿⢸⣿⣯⣛⣿⣿⡿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣾⣿⣿⣿\n");
            printf("\t⣿⣿⣿⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢇⣿⣿⣿⣿⣿⣿⣿⣽⣿⣿⣾⣿⣿⣿⣿⣿⣧⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣟⣺⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⣿⣿⣿⣿⣿⣿⣿⣽⣿⣿\n");
            printf("\n\t엔딩 2 : 가난한 왕국 (재정 0 이하)\n");
            printf("\n\t\"폐하, 왕국의 재정은 완전히 고갈 되었습니다.\n");
            printf("\t백성들은 굶주리고 귀족들은 이미 왕국을 떠났습니다.\n");
            printf("\t이제는 백성들의 원망과 분노를 감당해야 할 때입니다.\"\n");
            break;
        case 3:
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⣀⣿⣏⠀⣎⣿⣿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏⠀⠿⢿⡏⢸⣾⣿⣇⠻⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⢠⢏⡟⡻⢇⣻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠁⡜⠮⣜⡱⣋⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⢢⡠⠀⠠⡜⡱⢢⠱⣽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⠐⠉⠆⠀⠴⣑⣿⢃⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⠹⣿⣿⣿⣿⣿⣿⣟⠀⠀⠀⠀⢠⠓⣌⠲⣉⣿⣿⣿⣿⡿⣿⣿⡇⢿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⠃⡜⣿⣿⣿⣿⣿⣿⣿⣧⡀⠀⠀⢦⡙⢤⢓⣹⣿⣿⣿⣿⠳⣿⣿⠇⢨⢿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣧⠀⣞⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⢰⣢⣽⣶⣿⣿⣿⣿⣿⡇⣼⣻⣿⣧⡐⣾⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⢾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣏⣾⣿⢿⣿⣿⣿⣿⣿⣿⡷⣿⣿⣿⣿⣿⢿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣏⣿⣿⣿⣿⣿⣿⣿⣿⣯⣿⣿⢸⣿⣯⣛⣿⣿⡿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣾⣿⣿⣿\n");
            printf("\t⣿⣿⣿⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢇⣿⣿⣿⣿⣿⣿⣿⣽⣿⣿⣾⣿⣿⣿⣿⣿⣧⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣟⣺⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⣿⣿⣿⣿⣿⣿⣿⣽⣿⣿\n");
            printf("\n\t엔딩 3 : 귀족의 배신 (재정 100 이상)\n");
            printf("\n\t\"폐하, 왕국의 재정은 더 이상 백성들의 삶을 위해 사용되지 않고 있습니다. \n");
            printf("\t부패한 귀족들이 모든 부를 독점하고 옆나라로 떠났으며,\n");
            printf("\t굶주린 백성들만이 남아 왕궁에 쳐들어오고 있습니다. \n");
            printf("\t이제는 백성들의 원망과 분노를 감당해야 할 때입니다.\"\n");
            break;
        case 4:
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠙⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠁⠀⡐⠀⠈⠙⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⠟⠁⠀⠠⡁⠀⠄⠁⡀⢂⣨⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⡿⠟⠁⠀⢠⣬⣿⣷⠀⠐⢀⡴⣊⠶⣩⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⡟⠁⠀⠀⢠⣿⣿⠟⠁⠀⡜⢎⠶⡙⢊⢁⠋⡽⣿⡟⠡⢸⡝⣿⣿⣿⢟⡻⢿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⡇⠀⠀⠀⠀⠛⠁⠀⣠⠚⡜⣬⣶⣡⠀⠀⢈⣱⣿⣷⡀⢹⣺⣿⣿⣧⢯⣙⢧⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣷⠀⠀⠀⠀⠀⣄⠳⣰⣭⣿⣿⡟⡍⠆⠀⣿⣿⣿⣿⠄⢣⣿⣿⣿⣿⡦⣙⣾⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⠀⠀⠀⣄⠳⣌⠣⡝⡛⣟⠱⡘⣼⠆⠀⠉⠉⠙⠛⠂⣣⢟⣛⣛⢻⡱⢣⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣇⣄⡫⢔⣣⣌⡳⢌⡱⢌⣧⣿⣿⠁⠀⣀⠀⠀⠀⠀⣧⢚⡴⡩⢖⣍⢳⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⣷⣿⣿⣿⣿⣤⣾⣿⣿⣆⡀⠀⣷⣯⡖⣙⢮⡜⣮⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\n\t엔딩 4 : 등을 돌린 백성 (민심 0 이하)\n");
            printf("\n\t\"폐하, 이제는 백성들은 당신을 왕으로 여기지 않습니다. \n");
            printf("\t백성들은 모두 왕국을 등지고 타국으로 이민을 떠났습니다.\n");
            printf("\t당신은 누군가의 위에 있어야 할 존재가 아닙니다.\"\n");
            break;
        case 5:
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⡏⢏⣹⣩⠭⡑⢎⡹⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⡓⡬⢒⡋⢧⠙⠦⠑⠣⢻⣿⣿⣿⡿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⡁⠈⠀⢀⢀⠀⠀⠀⢀⣾⣿⣿⣏⠀⠐⡹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣷⠀⠀⠐⣉⣀⣀⣠⣾⣿⣿⣿⣿⣧⡀⠐⢩⢿⣯⣟⡟⣿⣛⣿⢻⣻⢟⣻⡟⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠀⢊⢿⣿⣿⣧⣟⣞⣯⢯⣟⣳⡟⣧\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣿⣿⣿⣿⣿⣿⣿⠟⢁⠠⣮⣾⣿⣿⣿⣿⣾⣾⣻⣼⡳⣟⣳\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⡟⢹⢻⣿⣴⣩⣿⣿⣿⣿⠟⠁⡄⣮⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣿⣽⣳\n");
            printf("\t⣿⣿⣿⣿⡏⠉⢹⣷⣆⢻⣿⢿⡒⢿⣿⠟⠁⡔⣣⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣦⠘⠟⠛⠀⢏⠶⣙⠎⠁⠀⡘⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣆⠀⢂⣈⣬⣷⣿⣦⠀⠰⣡⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣾⣿⣿⣿⣿⣿⣿⣷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\n\t엔딩 5 : 오만한 백성 (민심 100 이상)\n");
            printf("\n\t\"폐하, 이제 왕국은 백성들의 통치 아래에 있습니다. \n");
            printf("\t그들의 욕심은 끝이 없었고 그들의 요구는 이제 명령이 되어 왕권을 짓누르고 있습니다. \n");
            printf("\t더 이상 이 왕국에 왕은 필요한 존재가 아닙니다.\"\n");
            break;
        case 6:
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⢿⣿⣿⣿⣿⣿⢰⣷⡎⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣇⠚⣿⣿⣿⢟⡱⢦⣵⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣮⡐⢌⡑⠮⣱⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⣿⣿⣿⡿⢠⣳⡇⠂⡀⠀⠹⠿⣿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣻⣿⣿⣿⣿⣿⣿⣿⣿⡿⢋⣥⣿⠿⠛⠀⣿⣦⣤⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⡻⠓⠋⢿⠝⣌⠣⠈⠀⣠⣶⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣷⡃⠀⢀⠤⡋⠄⠀⢠⣾⣿⠿⠛⠛⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⡿⣋⠜⠂⠁⢀⣴⡟⠉⠀⠀⠀⠀⢀⡀⠉⠛⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⠟⡱⠌⠂⠀⣠⣿⣿⣏⡱⢢⠄⡀⠀⠘⠘⠀⠀⠀⠀⠉⠛⢿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⡟⠬⠁⠀⢠⣾⣿⣿⣿⣷⢈⠇⣎⣱⣃⠦⣀⠀⠀⠀⠀⠀⢠⣦⡀⠁⠉⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⠉⣠⣴⣾⣿⣿⣿⣿⣿⣿⣮⣞⡤⢓⡞⡰⢣⡙⣎⠖⡤⡐⠻⣿⣿⣶⣾⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣮⣕⡣⡜⣬⠛⣴⣿⣷⡤⣀⣹⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣬⣏⠽⣻⠻⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⣯⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\n\t엔딩 6 : 타국의 침략 (국력 0 이하)\n");
            printf("\n\t\"폐하, 군은 이미 무너졌습니다. 외세의 침략은 더이상 겉잡을 수 없게 되었습니다. \n");
            printf("\t귀족들은 대다수가 이미 도망가버렸으며 백성들은 적들에게 짖밟혀 울부짖고 있습니다. \n");
            printf("\t이제는 저희의 차례입니다.\"\n");
            break;
        case 7:
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡫⡜⣿⣿⡉⢻⣿⡿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠳⣽⣿⣷⢠⣾⣿⠀⣈⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠱⣎⠲⡅⠈⠙⠛⢀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⠈⠀⠛⣀⣀⢀⡀⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠀⠀⠀⠀⡒⣌⠦⣱⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⢠⠀⠀⠰⡱⢌⠲⣽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠉⠀⠀⢲⠹⣏⢲⣿⣿⣿⣿⣿⣻⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡄⢠⣃⣳⣼⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣟⣯⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⡿⠿⢿⣿⣿⣿⢟⣫⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⡿⢉⡠⣉⢄⠢⠹⣥⣾⣿⣿⣿⣿⣿⣽⡻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⡇⣰⣕⣯⡞⢲⠈⣽⣿⣿⣿⣿⣿⣿⣷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⡿⢟⣭⡷⢛⠤⡙⠆⣘⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⡈⢻⣿⣷⣶⣴⣶⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\n\t엔딩 7 : 쿠데타 (국력 100 이상)\n");
            printf("\n\t\"폐하, 이 모든 군사적 팽창이 결국 왕국을 파멸로 이끌 것임을 저는 알고 있었습니다. \n");
            printf("\t전하의 명령은 군사력을 강화하려 했지만, 이제 저희 군은 더 이상 그 명령을 따르지 않을 것입니다. \n");
            printf("\t저는 이제 왕국을 새로운 방향으로 이끌어 갈 것입니다.\"\n");
            break;
        case 8:
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠛⠛⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡅⠀⠀⢀⣿⣿⡏⠁⢨⠽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏⠀⣰⣾⣿⣿⡇⢀⡏⣾⣿⣿⡟⣭⢹⢻⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠀⠀⠻⢿⣿⣿⠃⡸⣼⣿⣿⣿⡜⢦⣋⢾⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⠀⠀⡀⠀⠀⠉⢠⠳⢿⢿⣿⣿⢏⠶⣽⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⢀⠀⠄⡼⣙⢎⡖⡲⣜⢪⢽⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⠀⠀⠈⠀⠀⠀⢠⠳⡜⣎⠼⣑⢮⡱⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠀⣠⠀⢀⠀⠂⠀⢎⠡⠱⣈⠓⡩⠒⣽⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⠀⣘⣷⠚⠀⠀⡘⠤⣉⣲⢀⠣⠔⣹⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠀⠘⠉⠸⠆⠀⢀⠜⡰⢠⣹⡶⠛⡌⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⠀⠀⠀⠀⠀⠀⣈⠆⡑⢚⡉⢻⠥⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⠀⠀⠀⠀⣤⣴⣶⣧⠘⡌⠒⡌⢒⡰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⡟⠛⣿⣿⣿⣿⣷⣄⠀⢸⠟⠻⣟⠛⣿⠤⡉⠴⡁⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⠋⡝⣿⡟⣿⣿⠟⢿⣿⣿⣿⣧⡈⠒⠺⠽⡻⢁⠆⣑⣦⣵⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣾⣿⣿⠿⢿⠆⠚⠁⠀⠀⠈⠀⠀⠀⣧⠒⣿⣿⣿⣿⣿⣿⣿⣟⣉⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⣿⣿⣿⣿⣿⣤⣾⣄⠀⠀⠬⣁⠣⠀⠀⠀⠀⠀⠉⠙⢿⠿⠛⡃⢈⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n");
            printf("\t⡟⠛⣿⣿⣿⡏⣉⠻⠂⠀⠐⠀⠃⠀⠀⢂⠘⡌⠱⣀⠀⣀⠀⠀⠈⣿⣉⣿⣿⣿⠛⢻⣿⡿⠿\n");
            printf("\t⣷⣾⣿⣿⣿⣷⣦⠀⠀⢀⠀⡀⠀⡀⢚⠄⡣⢌⠱⣀⠣⠄⠈⢁⠀⣾⣿⣿⣿⣿⣷⣾⣿⣷⣼\n");
            printf("\n\t엔딩 8 : 도래한 역병 (신앙 0 이하)\n");
            printf("\n\t\"폐하, 신께서는 왕국을 떠나셨습니다. \n");
            printf("\t왕국은 신의 뜻을 경시하신 대가로 자연재해와 전염병에 휩싸였습니다. \n");
            printf("\t폐하의 치세는 모든것이 무너질 때까지 저주받은 치세로 불리울 것입니다.\"\n");
            break;
        case 9:
            printf("\t⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢿⡿⣿⣿⣿⣿\n");
            printf("\t⣟⣿⣻⣟⣿⣻⣟⣿⣻⣟⣿⣻⣟⣿⣻⣿⢏⢎⠽⣿⣻⣟⣿⣻⣟⣿⣻⣫⣾⡿⣿⣾⣮⡻⣿\n");
            printf("\t⣟⣿⣻⣟⣿⣻⣟⣿⣻⣟⣿⣻⣟⣿⣽⠏⡢⡫⡪⠹⣿⣟⣿⣻⣟⣿⡧⣿⣻⣟⣿⣽⣿⡯⣾\n");
            printf("\t⣿⣻⣿⣻⣿⣟⣿⣟⣿⣟⣿⣟⣿⢿⠏⠀⢱⢝⢜⠀⠹⣿⣟⣿⣟⣿⡳⣿⣿⣻⣿⣻⣾⢯⣾\n");
            printf("\t⣽⣿⣽⡿⣷⣿⣯⡟⣯⣿⣯⣿⢿⠟⠀⠀⠨⣎⠆⠀⠀⢹⣿⣯⣿⣿⡫⣿⣽⣿⣽⣟⣿⣾⣿\n");
            printf("\t⢷⣿⣯⣿⣿⣾⡯⡸⡸⣿⣯⣿⡟⠀⠀⠀⠐⡕⠅⠀⠀⠀⢹⣷⣿⣽⣟⠜⣿⣽⣯⣿⢿⡷⣿\n");
            printf("\t⡿⣿⣾⢿⣾⡟⡰⣑⢎⠼⣿⣻⣷⠀⠀⠀⠀⡫⠀⠀⠀⢀⣾⣿⣿⡏⣼⠪⡜⣿⣯⣿⡿⣿⣿\n");
            printf("\t⡿⣿⣽⣿⡿⢨⠪⡢⡣⢕⠽⣿⣿⡇⡠⠐⢔⠀⠂⢄⠀⣼⣿⣿⡝⡄⢿⡘⣔⠹⣿⣽⡿⣿⣾\n");
            printf("\t⣿⣿⣻⣾⣧⢱⡩⡪⣊⢎⢺⣷⢿⡧⢈⠊⢔⠁⡡⠠⠀⣿⣟⣾⡨⡪⣽⢨⢒⠕⣽⣿⣟⣿⣷\n");
            printf("\t⣾⣟⣿⣽⣿⣶⣿⣿⣿⢧⣿⣻⡯⡷⢈⠊⠢⡁⠀⢅⠀⣿⢾⣟⣵⣲⡵⣷⣧⣩⣿⣯⣿⣯⣿\n");
            printf("\t⣾⣟⣿⣽⣷⣟⢷⣯⡾⣯⢿⣮⢿⠇⡐⠡⡑⢀⠡⠀⠄⠻⣟⣾⢯⣾⣓⣿⣟⣿⢿⣽⣷⣿⣻\n");
            printf("\t⣯⣿⣻⣷⠯⡑⣿⣽⡻⢃⠽⠞⠁⠀⠈⠐⢄⠁⠠⠂⠀⠀⠘⢷⡻⣱⣏⣷⡿⠛⣯⣿⣯⣿⣻\n");
            printf("\t⣯⣿⠟⡌⢎⠪⡢⣺⠑⠕⠀⠀⠀⠀⠀⠀⠀⡾⠀⠀⠀⠀⠀⠀⢊⠌⣮⡷⢑⢍⡚⢿⣾⣟⣿\n");
            printf("\t⠞⡣⡪⡪⢱⣟⣇⠺⠉⠀⠀⠄⠠⠀⠀⠀⠀⢠⢳⡀⠀⠀⡠⢤⢄⠸⣢⡗⢜⠢⡩⡌⢷⣿⣿\n");
            printf("\t⢘⠜⡔⢕⢬⡏⠁⠀⠀⠀⢀⠁⠂⠀⠀⠀⠈⢰⠃⠀⠀⠀⢫⡪⡪⠃⡰⢇⢕⢍⠬⡪⡪⡹⣿\n");
            printf("\t⠪⣑⠕⢕⢼⡆⠀⠀⠀⠀⠀⡈⠀⠀⠀⠀⠀⢸⡁⠀⠀⠀⢐⢕⠅⠀⢜⠀⠰⡑⡜⡢⡪⡲⡸\n");
            printf("\t⡣⡱⡩⡪⣟⡃⠀⠀⠀⠀⠀⠠⠁⢂⠐⠠⠈⡸⡔⡔⣆⢖⠬⡳⡀⠀⣕⠀⠀⢘⠔⡕⢜⢔⠕\n");
            printf("\t⣑⢎⣪⢜⣿⡅⠀⠀⠀⢀⢀⡁⢂⠄⢂⠁⠄⢺⡘⣎⢖⢕⢝⡜⠀⠀⠲⠀⠀⢘⠜⢜⠢⡣⣹\n");
            printf("\n\t엔딩 9 : 넘치는 신앙 (신앙 100 이상)\n");
            printf("\n\t\"폐하, 이제 신의 뜻이 이 땅의 유일한 법이 되었습니다. \n");
            printf("\t\"왕권은 이제 신앙의 도구일 뿐입니다. \n");
            printf("\t왕국은 더욱 발전한여 신국으로 재건될 것입니다.\"\n");
            break;
    }
    printf("\n\n\t1. 게임을 재시작한다.\n");
    printf("\n\t2. 게임을 종료한다.\n");
}







