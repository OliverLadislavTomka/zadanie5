#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "rng.h"
#include <ctype.h>

char *readline() {
    const size_t BSIZE = 100;
    char *line = NULL;
    size_t capacity = 0;
    do {
        capacity += BSIZE;
        line = (char *) realloc(line, (capacity + 1) * sizeof(char));
        if (line == NULL)
            return line;
        if (fgets(line + capacity - BSIZE, (int)BSIZE + 1, stdin) == NULL) {
            free(line);
            return NULL;
        }
    } while (strlen(line + capacity - BSIZE) >= BSIZE && line[capacity - 1] != '\n');
    return line;
}

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
    zviera.type = &enemy_types[index];
    return zviera;
}

void nacitanieEnemyDoMapy(FILE* fptr,int height,Unit* hraciePole[][height],int numEnemy,Unit* enemies){
    for(int i =0; i< numEnemy; i++) {
        int posX = 0, posY = 0,index = 0, hp = 0, level = 0;
        fscanf(fptr,"%d %d %d %d %d",&posX,&posY,&index,&hp,&level);
        enemies[i]=generovanieEnemy(index,hp,level);
        hraciePole[posX][posY] = &enemies[i];
    }
}

void inicializaciaPola(int height,int width,Unit * hraciePole[][height]){
    for(int i=0;i<width;i++){
        for(int j=0;j<height;j++){
            hraciePole[i][j] = NULL;
        }
    }
}

int getSuradnice(int width,int height,Unit * hraciePole[][height],Unit * monster,char suradnica){
    for (int i=0;i<width;i++)
        for (int j=0;j<height;j++)
            if (hraciePole[i][j] == monster) {
                if (suradnica == 'x') return i;
                if (suradnica == 'y') return j;
            }
    return 0;
}
int checkIfFree(int x, int y, int height,Unit * hraciePole[][height]){
    if (hraciePole[x][y] == NULL) return 1;
    return 0;
}

int vypocetDMG(Unit *utocnik,Unit *obranca){
    return (((30+utocnik->level - obranca->level)*(100+rnd(1,utocnik->hp) + utocnik->type->att))/(100+rnd(1,obranca->hp)+obranca->type->def));
}

void utok(Unit * utocnik, Unit* obranca){
    int dmg=vypocetDMG(utocnik,obranca);
    obranca->hp -= dmg;
}

void utokVypis(Unit* utocnik, Unit *obranca){
    utok(obranca,utocnik);
    if (utocnik->hp > 0) utok(utocnik,obranca);
}

int skontrolujCiMonster(Unit * monster){
    for(int i =0; i< MONSTER_TYPE_COUNT;i++) if (monster->type == &monster_types[i]) return 1;
    return 0;
}

void boj(Unit* utocnik, Unit *obranca,int height, Unit * hraciePole[][height],int x1,int x2,int y1, int y2){
    while(utocnik->hp > 0 && obranca->hp > 0){
        utokVypis(utocnik,obranca);
        if(utocnik->level < 10 && utocnik->hp > 0) utocnik->level++;
        if (skontrolujCiMonster(obranca) && obranca->level < 10 && obranca->hp > 0) obranca->level++;
    }
    if (utocnik->hp > 0) hraciePole[x2][y2] = utocnik;
}

void posunMonstra(int width,int height, Unit * hraciePole[][height],char operator,Unit * monsters){
    int x,y,x2,y2;
    switch (operator){
        case 'd':
            x = getSuradnice(width,height,hraciePole,&monsters[0],'x');
            y = getSuradnice(width,height,hraciePole,&monsters[0],'y');
            x2 = x + 1;
            if (x2 == height)  x2 = 0;
            if (checkIfFree(x2,y,height,hraciePole)) hraciePole[x2][y] = &monsters[0];
            else boj(hraciePole[x][y],hraciePole[x2][y],height,hraciePole,x,x2,y,y);
            hraciePole[x][y] = NULL;
            break;
        case 'u':
            x = getSuradnice(width,height,hraciePole,&monsters[0],'x');
            y = getSuradnice(width,height,hraciePole,&monsters[0],'y');
            x2 = x - 1;
            if (x2 == -1)  x2 = height-1;
            if (checkIfFree(x2,y,height,hraciePole)) hraciePole[x2][y] = &monsters[0];
            else  boj(hraciePole[x][y],hraciePole[x2][y],height,hraciePole,x,x2,y,y);
            hraciePole[x][y] = NULL;
            break;
        case 'l':
            x = getSuradnice(width,height,hraciePole,&monsters[0],'x');
            y = getSuradnice(width,height,hraciePole,&monsters[0],'y');
            y2 = y - 1;
            if (y2 == -1)  y2 = height-1;
            if (checkIfFree(x,y2,height,hraciePole)) hraciePole[x][y2] = &monsters[0];
            else  boj(hraciePole[x][y],hraciePole[x][y2],height,hraciePole,x,x,y,y2);
            hraciePole[x][y] = NULL;
            break;
        case 'r':
            x = getSuradnice(width,height,hraciePole,&monsters[0],'x');
            y = getSuradnice(width,height,hraciePole,&monsters[0],'y');
            y2 = y + 1;
            if (y2 == height)  y2 = 0;
            if (checkIfFree(x,y2,height,hraciePole)) hraciePole[x][y2] = &monsters[0];
            else  boj(hraciePole[x][y],hraciePole[x][y2],height,hraciePole,x,x,y,y2);
            hraciePole[x][y] = NULL;
            break;
        case 'D':
            x = getSuradnice(width,height,hraciePole,&monsters[1],'x');
            y = getSuradnice(width,height,hraciePole,&monsters[1],'y');
            x2 = x + 1;
            if (x2 == height)  x2 = 0;
            if (checkIfFree(x2,y,height,hraciePole)) hraciePole[x2][y] = &monsters[1];
            else  boj(hraciePole[x][y],hraciePole[x2][y],height,hraciePole,x,x2,y,y);
            hraciePole[x][y] = NULL;
            break;
        case 'U':
            x = getSuradnice(width,height,hraciePole,&monsters[1],'x');
            y = getSuradnice(width,height,hraciePole,&monsters[1],'y');
            x2 = x - 1;
            if (x2 == -1)  x2 = height-1;
            if (checkIfFree(x2,y,height,hraciePole)) hraciePole[x2][y] = &monsters[1];
            else  boj(hraciePole[x][y],hraciePole[x2][y],height,hraciePole,x,x2,y,y);
            hraciePole[x][y] = NULL;
            break;
        case 'L':
            x = getSuradnice(width,height,hraciePole,&monsters[1],'x');
            y = getSuradnice(width,height,hraciePole,&monsters[1],'y');
            y2 = y - 1;
            if (y2 == -1)  y2 = height-1;
            if (checkIfFree(x,y2,height,hraciePole)) hraciePole[x][y2] = &monsters[1];
            else  boj(hraciePole[x][y],hraciePole[x][y2],height,hraciePole,x,x,y,y2);
            hraciePole[x][y] = NULL;
            break;
        case 'R':
            x = getSuradnice(width,height,hraciePole,&monsters[1],'x');
            y = getSuradnice(width,height,hraciePole,&monsters[1],'y');
            y2 = y + 1;
            if (y2 == height)  y2 = 0;
            if (checkIfFree(x,y2,height,hraciePole)) hraciePole[x][y2] = &monsters[1];
            else  boj(hraciePole[x][y],hraciePole[x][y2],height,hraciePole,x,x,y,y2);
            hraciePole[x][y] = NULL;
            break;
        default:
            break;
    }
}

void behaniePoMape(int width,int height, Unit * hraciePole[][height], char * ovladaciRetazec, Unit * monsters){
    for (int i=0;i<strlen(ovladaciRetazec);i++){
        posunMonstra(width,height,hraciePole,ovladaciRetazec[i],monsters);
    }
}

int pocet(Unit * pole,int velkost){
    int pocet = 0;
    for(int i = 0; i< velkost;i++){
        if (pole[i].hp > 0) pocet++;
    }
    return pocet;
}

void vypisMonstier(Unit * pole,int velkost,int width, int height,Unit *hraciePole[][height]){
    for(int i = 0; i< velkost;i++)
        if (pole[i].hp > 0) printf("%d %d %s %d %d\n", getSuradnice(width,height,hraciePole,&pole[i],'x'),getSuradnice(width,height,hraciePole,&pole[i],'y'),pole[i].type->name,pole[i].hp,pole[i].level);
}

int isEnemy(Unit *unit){
    for (int i = 0; i < ENEMY_TYPE_COUNT ; i++) {
        if (unit->type->name == enemy_types[i].name) return i;
    }
    return ENEMY_TYPE_COUNT;
}

void vypisEnemies(int width, int height, Unit *hraciePole[][height]){
    for (int i = 0; i < width; i++){
        for (int j = 0; j < height; j++){
            if (hraciePole[i][j] != NULL && isEnemy(hraciePole[i][j]) != ENEMY_TYPE_COUNT && hraciePole[i][j]->hp > 0)
                printf("%d %d %d %d %d\n", getSuradnice(width, height, hraciePole,(Unit *) hraciePole[i][j], 'x'), getSuradnice(width, height, hraciePole,(Unit *) hraciePole[i][j], 'y'),
                       isEnemy(hraciePole[i][j]), hraciePole[i][j]->hp, hraciePole[i][j]->level);
        }
    }
}

void zaverecnyVypis(int width,int height,Unit * hraciePole[][height],Unit * monsters, Unit * enemies,int numMon, int numEnem){
    printf("%d %d %d %d\n",width,height, pocet(monsters,numMon),pocet(enemies,numEnem));
    vypisMonstier(monsters,numMon,width,height,hraciePole);
    vypisEnemies(width,height,hraciePole);
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
    nacitanieMonstierDoMapy(fptr,height,hraciePole,numMon,monsters);
    nacitanieEnemyDoMapy(fptr,height,hraciePole,numEnem,enemies);
    char * ovladaciRetazec = readline();
    behaniePoMape(width,height,hraciePole,ovladaciRetazec,monsters);
    zaverecnyVypis(width,height,hraciePole,monsters,enemies,numMon,numEnem);
    fclose(fptr);
}

int main(int argc, char *argv[]) {
    gamecycle(argv);
    return 0;
}