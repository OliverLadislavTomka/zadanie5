#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "rng.h"

UnitType* findMonsterInDataset(UnitType *poleMonstier, const char * menoMonstra){
    for (int i=0;i < MONSTER_TYPE_COUNT;i++){
        if (!strcmp(menoMonstra,poleMonstier[i].name)){
            return &poleMonstier[i];
        }
    }
    return NULL;
}

Unit generovanieMonstra(const char * menoMonstra,int hp, int level){
    Unit zviera;
    UnitType* monster=NULL;
    zviera.hp=hp;
    zviera.level=level;
    monster = findMonsterInDataset(monster_types, menoMonstra);
    zviera.type = monster;
    return zviera;
}


void nacitanieMonstierDoMapy(FILE* fptr,int height,Unit* hraciePole[][height],int numMon,Unit* monsters){
    for(int i =0; i< numMon; i++) {
        int posX = 0, posY =0, hp =0, level =0;
        char meno[31];
        fscanf(fptr,"%d %d %s %d %d",&posX,&posY,meno,&hp,&level);
        monsters[i]=generovanieMonstra(meno,hp,level);
        hraciePole[posX][posY] = &monsters[i];
    }
}

Unit generovanieEnemy(int index,int hp, int level){
    Unit zviera;
    zviera.hp=hp;
    zviera.level=level;
    zviera.type = &monster_types[index];
    return zviera;
}

void nacitanieEnemyDoMapy(FILE* fptr,int height,Unit* hraciePole[][height],int numEnemy,Unit* enemies){
    for(int i =0; i< numEnemy; i++) {
        int posX = 0, posY =0,index =0, hp =0, level =0;
        fscanf(fptr,"%d %d d %d %d",&posX,&posY,&index,&hp,&level);
        enemies[i]=generovanieEnemy(index,hp,level);
        hraciePole[posX][posY] = &enemies[i];
    }
}

void vypis(int width,int height, Unit* hraciePole[][height]){
    for (int i=0;i<width;i++) {
        for (int j = 0; j < height; j++){
            if (hraciePole[i][j]== NULL) printf("0");
            else printf("1");
        }
        printf("\n");
    }
}

void inicializaciaPola(int height,int width,Unit* hraciePole[width][height]){
    for(int i=0;i<width;i++){
        for(int j=0;j<height;j++){
            hraciePole[width][height] = NULL;
        }
    }
}

void gamecycle(char **argv){
    srnd(strtol(argv[1],NULL,0));
    FILE *fptr = fopen(argv[2],"r");
    int width=0,height=0,numMon=0,numEnem=0;
    fscanf(fptr,"%d %d %d %d",&width,&height,&numMon,&numEnem);
    Unit* hraciePole[width][height];
    inicializaciaPola(height,width,hraciePole);
    Unit monsters[numMon];
    Unit enemies[numEnem];
    //nacitanieMonstierDoMapy(fptr,height,hraciePole,numMon,monsters);
    vypis(width,height,hraciePole);
    //nacitanieEnemyDoMapy(fptr,height,hraciePole,numEnem,enemies);
    //vypis(width,height,hraciePole);
    fclose(fptr);
}

int main(int argc, char *argv[]) {
    gamecycle(argv);
    return 0;
}
