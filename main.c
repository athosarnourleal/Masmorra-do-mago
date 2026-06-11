#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <termios.h>
#include <unistd.h>

enum possibleGameStates {
    INGAME,
    DEAD,
    WIN
};

enum possibleGameStates gameState = INGAME;

int W;
int H; // screen size
int *map;
int presetNumber = 1;

int frames = 0;

char ERRORSprite = '?';

char entitySprite[] = {'O', 'k', 'D', '=', '@', 'X', 'Y', 'Z', 'V', 'o',' '};

char mapSprites[] = {' ','*','#','L'};

int numberOfCommands = 6;
char knownCommands[] ={'w', 's', 'a','d','i','o'};
int curCommand;

int currentRoom = 0;
int universalButtonCounter = 0;

int villageMap[100] = {
    0, 0, 1, 0, 0, 0, 1, 1, 1, 1,
    0, 1, 0, 1, 0, 0, 1, 0, 3, 1,
    1, 0, 0, 0, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 1, 4, 1, 1,
    1, 0, 0, 0, 4, 0, 0, 0, 0, 0,
    1, 0,12, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 1, 0,
    1, 1, 1, 1, 1, 0, 0, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 4, 0,12,
    0, 0, 0, 0,-1, 0, 0, 1, 0, 0
};

int floor1[100] = {// PRONTO
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 1,
    1, 0,-1, 0, 1, 0, 3, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 1,
    1, 6, 6, 6, 1, 1, 1, 4, 1, 1,
    1, 0, 0, 0, 6, 6, 0, 0, 0, 1,
    1, 6, 6, 0, 6, 6, 0, 0, 0, 1,
    1, 6, 6, 0, 6, 6, 6, 6, 6, 1,
    1, 6, 6, 0, 6, 6, 6, 7, 6, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

int floor2[255] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 5, 0, 0, 0, 0, 2, 2, 0, 4, 0, 0, 0, 3, 1,
    1, 0, 0, 0, 0, 0, 2, 2, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 2, 2, 2, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 8, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 5, 0, 0, 0, 0, 0, 0, 5, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 8, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 9, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,-1, 0, 1,
    1, 5, 0, 0, 0, 0, 0, 0, 5, 1, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

int bossFloor[625] =
{
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 1, 0, 0, 0, 1,
    1, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 7, 1, 0,-1, 0, 1,
    1, 6, 6, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6, 6, 6, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 6, 7, 6, 0, 0, 0, 0, 0, 6, 6, 6, 1, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 6, 6, 6, 0, 1, 1, 1, 0, 6, 6, 6, 1, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 6, 6, 6, 0, 1,11, 1, 0, 6, 6, 6, 1, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 4, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0,10, 0, 0, 0, 0, 0, 0, 0,10, 0, 1, 0, 0, 0, 1,10, 0,10, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 6, 6, 0, 0, 0, 0, 0, 6, 6, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0,10, 0, 1, 0, 6, 6, 0, 0, 0, 0, 0, 6, 6, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 1, 0, 0, 0, 1,
    1, 6, 6, 6, 0,10, 0, 0, 0, 0, 0, 0, 0, 0, 0,10, 0, 6, 6, 6, 1, 0, 0, 0, 1,
    1, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 1, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1,
    1, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 6, 6, 0, 0, 0, 0,10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0,10,10, 1,
    1, 6, 6, 0, 0, 0, 0,10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
    1, 7, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 8, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};
int bossMaxLife = 6, bossPhase = 0, spikeDirection = 0;

enum orientation {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
    UPLEFT = 4,
    UPRIGHT = 5,
    DOWNLEFT = 6,
    DOWNRIGHT = 7
};

enum weaponType {
    NOWEAPON = 0,
    SWORD = 1,
    BOW = 2,
    WAND = 3
};

char weaponNames[4][7] = {"NADA","ESPADA","ARCO","CAJADO"};

int maxLife = 3;
char playerSprites[] = {'^','v','<','>'};

struct player {
    int x;
    int y;
    enum orientation facing;
    int life;
    enum weaponType weapon;
    int keys;
};

struct player p;
int playerdmg = 1;

enum entityType {// +4
    DOOR = 0,
    BUTTON = 1,
    BOX = 2,
    BOXWITHKEY = 3,
    KEY = 4,
    ENEMYDUMB = 5,
    ENEMY = 6,
    BOSS = 7,
    VILLAGER = 8,
    FLYINGSPIKE = 9
};

struct entity {
    int x;
    int y;
    int life;
    enum entityType type; // kind of entity
    char sprite;
    int aux;
};

struct entity *entities; // entity vector
int entityNumber;

// ######################################################################################### UTILITARY FUNCTIONS

void flushInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int abs(int v) {
    if (v < 0) {
        return v*-1;
    }
    return v;
}

void pause_text() {
    //printf("Press Enter to continue...");
    getchar();
}

void clear() { system("clear"); }

void error(char *message) {
    printf("ERROR: %s \n\n", message);

    pause_text();
    exit(0);
}

int mygetch () { // CODIGO COPIADO.
  int ch;
  struct termios oldt, newt;

  tcgetattr ( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr ( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );

  return ch;
}

int getGameCommand() {
    int i, found = -1;


    do {

        char command = mygetch();
        for (i = 0; i < numberOfCommands; i++) {// CHECK IF COMMAND EXISTS
            if (command == knownCommands[i]) {
                found = i;
                break;
            }
        }
    } while (found == -1);



    return found;
}

int getDirectionX(enum orientation e) {

    switch(e) {
        case RIGHT:
            return 1;
        break;
        case LEFT:
            return -1;
        break;
        case UPRIGHT:
            return 1;
        break;
        case UPLEFT:
            return -1;
        break;
        case DOWNRIGHT:
            return 1;
        break;
        case DOWNLEFT:
            return -1;
        break;
        default:
            return 0;
    }

    return 0;
}

int getDirectionY(enum orientation e) {
    switch (e) {
        case DOWN:
            return 1;
        break;
        case UP:
            return -1;
        break;
        case UPRIGHT:
            return -1;
        break;
        case UPLEFT:
            return -1;
        break;
        case DOWNRIGHT:
            return 1;
        break;
        case DOWNLEFT:
            return 1;
        break;
        default:
            return 0;
    }
}

int inBounds(int x, int y) {
    if (x < 0 || y < 0 || x >= W || y >= H) {
        return 0;// off bounds
    }
    return 1; // in bounds
}

int getMap(int x,int y) {
    if (inBounds(x,y) == 0) {
        error("PLACE CANT BE GET SINCE IT IT OUT OF BOUNDS(getMap)");
        return -1;
    }
    return map[x + y*W];
}

void setMap(int val, int x, int y) {
    if (inBounds(x,y) == 0) {
        error("PLACE CANT BE SET SINCE IT IT OUT OF BOUNDS(setMap)");
    }

    map[x + y*W] = val;
}


// ######################################################################################### ENTITY ARRAY MANAGING

int entityPointColision(int pointx, int pointy) {
    int i;
    for (i = 0; i < entityNumber; i++) {
        if (entities[i].x == pointx && entities[i].y == pointy) {
            return i; // someone collided --> return their index;
        }
    }
    return -1; // no one collided
}

void pushEntity(int x, int y, enum entityType type) {
    if (entityNumber == 0) {
        entities = (struct entity*)malloc((entityNumber+1)*sizeof(struct entity));
    } else {
        entities = (struct entity*)realloc(entities, (entityNumber+1)*sizeof(struct entity));
    }

    if (inBounds(x,y) == 0) {
        error("IMPOSSIVEL CRIAR ENTIDADE NESSAS COORDENADAS(pushEntity)");
    }

    // start
    entities[entityNumber].x = x;
    entities[entityNumber].y = y;
    entities[entityNumber].type = type;
    entities[entityNumber].life = 1;
    entities[entityNumber].aux = 0;

    entities[entityNumber].sprite = ERRORSprite;

    switch(type) {
        case DOOR:
            entities[entityNumber].sprite = entitySprite[2];
            setMap(1,x,y); // activate colision
        break;
        case KEY:
            entities[entityNumber].sprite = entitySprite[4];
        break;
        case BOX:
            entities[entityNumber].sprite = entitySprite[1];
            setMap(1,x,y); // activate colision
        break;
        case BUTTON:
            entities[entityNumber].sprite = entitySprite[0];
            entities[entityNumber].aux = universalButtonCounter;
            universalButtonCounter++;
            //entities[entityNumber].sprite = 48+entities[entityNumber].aux;
        break;
        case BOXWITHKEY:
            entities[entityNumber].sprite = entitySprite[1];
            setMap(1,x,y); // activate colision
        break;
        case ENEMYDUMB:
            entities[entityNumber].sprite = entitySprite[5];
            setMap(1,x,y); // activate colision
        break;
        case ENEMY:
            entities[entityNumber].sprite = entitySprite[6];
            setMap(1,x,y); // activate colision
        break;
        case VILLAGER:
            entities[entityNumber].sprite = entitySprite[8];
            setMap(1,x,y); // activate colision

            int houseDoor = entityPointColision(4,4);
            if (houseDoor != -1) {
                setMap(0, 4, 4);
                entities[houseDoor].aux = 1;
                entities[houseDoor].sprite = entitySprite[3];
            }
        break;
        case FLYINGSPIKE:
            entities[entityNumber].sprite = mapSprites[2];
            entities[entityNumber].aux = spikeDirection;
        break;
        case BOSS:
            entities[entityNumber].sprite = entitySprite[7];
            entities[entityNumber].life = 9999999;
            bossPhase = 0;
            spikeDirection = 0;
        break;
    }


    entityNumber++;
}

void popEntity(int index) {
    if (index >= entityNumber) {
        error("IMPOSSIVEL REMOVER ESSE INDEX(popEntity)");
    }

    entityNumber--;

    if (entityNumber == 0) {
        free(entities);
        return;
    }

    // create the new array
    int i;
    for (i = 0; i < entityNumber; i++) {
        if (i >= index) {
            entities[i] = entities[i+1];
        }
    }

    entities = (struct entity*) realloc(entities, (entityNumber+1)*sizeof(struct entity));
}

// ######################################################################################### MAP MANIPULATION

void placeInMap(int x, int y, int ID) {
    if (ID == -2) {
        return;
    }

    if (ID == -1) {
        setMap(0,x,y);
        p.x = x;
        p.y = y;
        return;
    }

    if (ID < 4) {// is a map placement
        setMap(ID,x,y);
    }else {
        setMap(0,x,y);
        pushEntity(x,y,ID-4);
    }
}

void setMapAsPreset(int mapID) {

    int size;
    int *source;

    while(entityNumber > 0) { // clear all entities
        popEntity(0);
    }

    switch(mapID) {
        case 0:
            W = 10;
            H = 10;
            source = (int*)villageMap;
        break;
        case 1:
            W = 10;
            H = 10;
            source = (int*)floor1;
        break;
        case 2:
            W = 15;
            H = 15;
            source = (int*)floor2;
        break;
        case 3:
            W = 25;
            H = 25;
            source = (int*)bossFloor;
        break;
    }

    size = W*H;

    if (map == NULL) {
        map = (int*)malloc(size*sizeof(unsigned int));
    } else {
        map = (int*)realloc(map, size*sizeof(unsigned int));
    }

    int x,y;
    for(y = 0; y < H; y++) {
        for(x = 0; x < W; x++) {
            placeInMap(x, y, source[x + y*W]);
        }
    }
}

void resetMap() {
    // fix button arrangements
    int i, numberOfButtons = 0;
    for(i = 0; i < entityNumber; i++) {
        if (entities[i].type == BUTTON) {
            numberOfButtons++;
        }
    }
    universalButtonCounter-=numberOfButtons;

    setMapAsPreset(currentRoom);
}

void goDownstairs() {
    currentRoom++;
    setMapAsPreset(currentRoom);
}

// ######################################################################################### ENTITY INTERACTION

void damage(int dmg) {
    p.life-=dmg;

    resetMap();

    if (p.life <= 0) {
        gameState = DEAD;
    }
}

void keyInteraction(int ID) {
    if (entities[ID].type != KEY) return;

    p.keys++;// get key
    popEntity(ID); // kill itself
}

void doorInteraction(int ID) {
    if (entities[ID].type != DOOR) return;

    if (entities[ID].aux == 0 && p.keys > 0) { // if door is closed
        entities[ID].aux = 1;// OPEN DOOR
        entities[ID].sprite = entitySprite[3];
        setMap(0, entities[ID].x, entities[ID].y); // deactivate colision
        p.keys--;
    }
}

void villagerInteraction(int ID) {
    if (entities[ID].aux == 1) {
        return;
    }
    clear();
    printf("V:\n");
    printf("se você deseja entrar nas masmorras, tome cuidado.\n");
    printf("Muitos outros tolos cegos pela ganancia e pela estupidez encontraram a morte nas mãos do mago\n");
    pause_text();
    clear();
    printf("V:\n");
    printf("Olha isso! Nem uma arma você tem!\n");
    pause_text();
    int escolha = 0;
    while (escolha < 1 || escolha > 3) {
        clear();
        printf("V:\n");
        printf("se nada pode te impedir de entrar, ao menos pegue uma arma antes de partir...\n");
        printf("1: espada\n2: arco\n3: cajado\n");
        scanf(" %d", &escolha);
    }
    p.weapon = escolha;
    clear();
    printf("pelo menos com essa arma você terá chances maiores de ganhar o desafio.\n");
    printf("deixe me destrancar a porta da masmorra para você.\n");
    p.keys++;
    doorInteraction(entityPointColision(7,3));
    flushInputBuffer();
    pause_text();
    clear();
    printf("espero que você consiga trazer um fim para essa confusão.");
    pause_text();
    clear();


    entities[ID].aux = 1;
}

void interact(int x, int y, enum orientation dir) {
    int ID;

    int nx = x + getDirectionX(dir);
    int ny = y + getDirectionY(dir);
    ID = entityPointColision(nx,ny);

    if (ID == -1) {
        ID = entityPointColision(x,y);

        if (ID == -1) {
            return;
        }
    }

    struct entity e = entities[ID];

    switch(e.type) {
        case KEY:
            keyInteraction(ID);
        break;
        case DOOR:
            doorInteraction(ID);
        break;
        case VILLAGER:
            villagerInteraction(ID);
        break;
        default:
            return;
        break;
    }
}

void buttonInteract(int ID) {
    if (entities[ID].aux == -1) {
        return;
    }

    int i,j;

    switch(entities[ID].aux) {
        case 0:
            for(i = 6; i <= 8; i++) {// remove spikes
                for(j = 1; j <= 4; j++) {
                    if (getMap(i,j) == 2) setMap(0,i,j);
                }
            }
            clear();
            printf("\nvocê escuta um som de clique ecoando do outro lado da sala...\n");
            pause_text();
        break;
        case 1:
            int enemiesLeftToSpawn = 13;

            while (enemiesLeftToSpawn > 0) {
                int x = rand() % (1-8) + 1;
                int y = rand() % (1-13) + 1;

                if (abs(x - p.x) <= 1 && abs(y - p.y) <= 1) {
                    continue;
                }

                if (getMap(x,y) == 0 && entityPointColision(x,y) == -1) {
                    placeInMap(x,y,9);
                    enemiesLeftToSpawn--;
                }
            }

            clear();
            printf("\nO teto abre e vários inimigos caem na sala!\n");
            pause_text();
        break;
        case 2:
            for(i = 7; i <= 9; i++) {
                for(j = 6; j <= 8; j++) {
                    if (getMap(i,j) == 0 && entityPointColision(i,j) == -1) {
                        placeInMap(i,j,6);
                    }
                }
            }

            clear();
            printf("\nCom o clique do botão, varias caixas aparentemente inofensivas caem do teto\n");
            pause_text();
        break;
        case 3:
            for(i = 1; i <= 2; i++) {// add spikes
                for(j = 12; j <= 13; j++) {
                    if (getMap(i,j) == 0 && entityPointColision(i,j) == -1) placeInMap(i,j,2);
                }
            }
            clear();
            printf("\nVários espinhos saem do chão e te prendem\n");
            pause_text();
        break;
        case 4:
            for(i = 11; i <= 13; i++) {// remove spikes
                if (getMap(i,5) == 2) setMap(0, i, 5);
            }
            clear();
            printf("\nvocê escuta o som de um clique em algum lugar da masmorra...\n");
            pause_text();

        break;
    }

    entities[ID].aux = -1;// setar botão como usado
    entities[ID].sprite = entitySprite[9];
}

// ######################################################################################### ENTITY BEHAVIOR

void buttonTick(int ID) {
    if (entities[ID].aux == -1) return;// 2 --> botao ja apertado

    if (entities[ID].x == p.x && entities[ID].y == p.y) {
        buttonInteract(ID);
    }
}

void smartEnemyTick(int ID) {

    setMap(0, entities[ID].x, entities[ID].y); // remove colision

    int nx = entities[ID].x;
    int ny = entities[ID].y;

    if (p.x < nx) {
        nx--;
    } else if (p.x > nx) {
        nx++;
    } else {
        if (p.y < ny) {
            ny--;
        } else if (p.y > ny) {
            ny++;
        }
    }

    if (inBounds(nx,ny) && (getMap(nx,ny) == 0 || getMap(nx, ny) == 2) && entityPointColision(nx, ny) == -1) {
        if (nx == p.x && ny == p.y) {
            damage(1);
        } else {
            entities[ID].x = nx;
            entities[ID].y = ny;
        }
    }

    setMap(1, entities[ID].x, entities[ID].y); // reinstate colision
}

void dumbEnemyTick(int ID) {
    if (entities[ID].type != ENEMYDUMB) {
        return;
    }

    setMap(0, entities[ID].x, entities[ID].y); // remove colision

    enum orientation dir = rand() % 4;

    int nx = entities[ID].x + getDirectionX(dir);
    int ny = entities[ID].y + getDirectionY(dir);

    if (getMap(nx,ny) == 0 && inBounds(nx, ny)) {
        if (nx == p.x && ny == p.y) {
            damage(1);
        } else {
            entities[ID].x = nx;
            entities[ID].y = ny;
        }
    }

    setMap(1, entities[ID].x, entities[ID].y); // reinstate colision
}


void flyingSpikeTick(int ID) {

    clear();
    enum orientation facing = entities[ID].aux;
    int nx = entities[ID].x + getDirectionX(facing);
    int ny = entities[ID].y + getDirectionY(facing);

    int hitSomething = 0;

    if (inBounds(nx,ny) == 0) {
        popEntity(ID);
        hitSomething = 1;
    }
    if (nx == p.x && ny == p.y) {
        damage(1);
        popEntity(ID);
        hitSomething = 1;
    }
    if (getMap(nx,ny) != 0) {
        popEntity(ID);
        hitSomething = 1;
    }

    if(hitSomething == 0) {
        entities[ID].x = nx;
        entities[ID].y = ny;
    }
}

void bossTeleport(int ID) {
    int nx, ny;

    do {
        nx = rand()%(19-1)+1;
        ny = rand()%(17-1)+1;
    } while (abs(nx-p.x) <= 2 && abs(ny-p.y) <= 2);

    entities[ID].x = nx;
    entities[ID].y = ny;
}

void bossDialogue() {
    clear();
    printf("Z:\nQuem diria que algum dos tolos enviados pelo rei \nconseguiria chegar no meus aposentos\n");
    pause_text();
    clear();
    printf("Z:\nVarios outros guerreiros trilharam o mesmo caminho que você, \nmas nenhum tinha atingido essa sala antes\n");
    pause_text();
    clear();
    printf("Z:\nSaiba que planejo tratar os lacaios daquele traidor \ncom a mesma crueldade que ele me mostrou!\n");
    pause_text();
    clear();
    printf("Com uma exploão de luz do mago, todas a estrutura da sala é refeita \npara dar espaço para um campo de batalha...");
    pause_text();
}

void bossTick(int ID) {

    if (entities[ID].aux > 0) {
        entities[ID].aux--;
        return;
    }

    int i,j,k;
    switch (bossPhase) {
        case 0: // in jail
            int doorID = entityPointColision(entities[ID].x,entities[ID].y+1);
            if (entities[doorID].aux == 1) {// door is opened --> START BOSS FIGHT

                // break jail
                for(i = 1; i <= 19; i++) {
                    for(j = 1; j <= 17; j++) {
                        setMap(0,i,j);

                        for(k = 0; k < entityNumber; k++) {
                            if (entities[k].x == i && entities[k].y == j && entities[k].type != BOSS) {
                                popEntity(k);
                            } else if (entities[k].type == BOSS) {
                                ID = k;// update boss ID
                            }

                        }
                    }
                }
                // lock jail
                for(i = 0; i < entityNumber; i++) {
                    if (entities[i].type == DOOR) {
                        placeInMap(entities[i].x, entities[i].y, 4);
                        popEntity(i);
                        break;
                    }
                }

                entities[ID].life = bossMaxLife;
                bossTeleport(ID);
                bossPhase++;
                bossDialogue();
            }
        break;
        case 1: // phase 1
            if (entities[ID].life < 4) {
                bossPhase++;
                break;
            }
            bossTeleport(ID);
            entities[ID].aux = 7;

            int i;
            for (i = 0; i < 4; i++) {
                placeInMap(entities[ID].x, entities[ID].y, 13);
                spikeDirection = (spikeDirection+1)%4;
            }
        break;
        case 2: // phase 2
            bossTeleport(ID);
            entities[ID].aux = 5;

            for (i = 0; i < 8; i++) {
                placeInMap(entities[ID].x, entities[ID].y, 13);
                spikeDirection = (spikeDirection+1)%8;
            }
        break;

    }
}

void damageEntity(int ID, int dmg) {
    enum entityType type = entities[ID].type;
    if (type != ENEMYDUMB && type != ENEMY && type != BOX && type != BOXWITHKEY && type != BOSS && type != FLYINGSPIKE) {
        return;
    }

    entities[ID].life -= dmg;

    if (type == BOSS) {
        entities[ID].aux = 0;
    }

    if (entities[ID].life <= 0) {
        if (getMap(entities[ID].x,entities[ID].y) == 1) {
            setMap(0, entities[ID].x,entities[ID].y);
        }

        if (type == BOXWITHKEY) {
            pushEntity(entities[ID].x, entities[ID].y, KEY);
        } else if (type == BOSS) {
            gameState = WIN;
        }

        popEntity(ID);
    }
}

// ######################################################################################### MAIN GAME LOOP

void entitiesTick() {
    int i;
    for (i = 0; i < entityNumber; i++) {
        int lastEntityCount = entityNumber;
        switch(entities[i].type){
            case BUTTON:
                buttonTick(i);
            break;
            case ENEMY:
                smartEnemyTick(i);
            break;
            case ENEMYDUMB:
                dumbEnemyTick(i);
            break;
            case BOSS:
                bossTick(i);
            break;
            case FLYINGSPIKE:
                flyingSpikeTick(i);
            break;
            default:
            break;
        }
        i += entityNumber - lastEntityCount;
    }
}

void atackPosition(int x, int y) {
    int i;

    for (i = 0; i < entityNumber; i++) {
        if (entities[i].x == x && entities[i].y == y) {
            damageEntity(i,playerdmg);
        }
    }
}

void atackSword() {
    int startx, starty;
    int w, h;

    switch(p.facing) {
        case UP:
            w = 3;
            h = 2;
            startx =p.x-1;
            starty = p.y-2;
        break;
        case DOWN:
            w = 3;
            h = 2;
            startx = p.x-1;
            starty = p.y+1;
        break;
        case LEFT:
            w = 2;
            h = 3;
            startx = p.x-2;
            starty = p.y-1;
        break;
        case RIGHT:
            w = 2;
            h = 3;
            startx = p.x+1;
            starty = p.y-1;
        break;
    }

    int i,j;
    for (i = 0; i < w; i++) {
        for (j = 0; j < h; j++) {
            atackPosition(startx+i, starty+j);
        }
    }
}

void atackBow() {
    int range = 4, i;

    int vx = getDirectionX(p.facing);
    int vy = getDirectionY(p.facing);

    int atkx = p.x+vx, atky = p.y+vy;

    for (i = 0; i < range;i++) {
        atackPosition(atkx,atky);

        atkx+=vx;
        atky+=vy;
    }
}

void atackWand() {
    int i,j;
    for (i = -1; i <= 1; i++) {
        for (j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;
            int atkx = p.x + i, atky = p.y + j;

            atackPosition(atkx,atky);
        }
    }
}

void playerTick() {
    if (curCommand == -1) return;

    int nextx = p.x, nexty = p.y;

    if (curCommand >= 0 && curCommand <= 3) {
        p.facing = curCommand;
        nextx += getDirectionX(p.facing);
        nexty += getDirectionY(p.facing);
    }

    // try to move
    if (inBounds(nextx,nexty)) {
        int destination = getMap(nextx, nexty);
        if (destination != 1) {
            p.x = nextx;
            p.y = nexty;

            if (destination == 2) { // spikes
                damage(1);
            } else if (destination == 3) { // stairs
                goDownstairs();
            }
        }
    }

    if (curCommand == 4) { // i
        interact(p.x, p.y, p.facing);
    }

    if (curCommand == 5) { // o --> atack
        switch(p.weapon) {
            case NOWEAPON: break;
            case WAND:
                atackWand();
            break;
            case BOW:
                atackBow();
            break;
            case SWORD:
                atackSword();
            break;
        }
    }
}

void tick() {

    curCommand = getGameCommand();

    // PLAYER COMMANDS
    playerTick();

    // ENTITY TICK
    entitiesTick();
}

void render() {
    int i,j;

    clear();
    printf("MASMORRA DO MAGO\n");
    if (currentRoom == 3 && bossPhase > 0 && bossPhase < 4) {// boss room
        int bossID = -1;

        for(i = 0; i < entityNumber;i++) {
            if (entities[i].type == BOSS) {
                bossID = i;
                break;
            }
        }

        int barWidth = 5, j;
        printf("BOSS: [");
        for(i = 0; i < bossMaxLife; i++) {
            char c = (i <= entities[bossID].life) ? '#':'.';

            for (j = 0; j < barWidth; j++) {
                printf("%c", c);
            }
        }
        printf("]\n");
    }

    char *screen = (char*)malloc(W*H*sizeof(char));

    for(i = 0; i < W*H; i++) { // DRAW FLOOR PAN
        screen[i] = mapSprites[map[i]];
    }

    for (i = 0; i < entityNumber; i++) { // DRAW ENTITIES
        screen[entities[i].x + entities[i].y*W] = entities[i].sprite;
    }

    screen[p.x + p.y*W] = playerSprites[p.facing]; // DRAW PLAYER

    // DRAW MAP IN SCREEN

    for(i = 0; i < H; i++) {
        for(j = 0; j < W; j++) {
            char p = screen[j + i*W];
            printf("%c ",p);
        }
        printf("\n");
    }

    // RENDER UI

    printf("VIDA: ");
    for (i = 1; i <= maxLife; i++) {
        if (i <= p.life) {
            printf("<3  ");
        } else {
            printf("</3 ");
        }
    }
    printf("   ");
    printf("CHAVES: ");
    for (i = 1; i <= p.keys; i++) {
        printf("@");
    }
    printf("\n");

    printf("ARMA: %s \n", weaponNames[p.weapon]);
}

// ######################################################################################### GAME SETUP AND LOOP START

void deathScreen();
void winScreen();

void startingDialogue() {
    clear();
    printf("no ano de 1820, o maior reino de toda a terra era oficialmente formado...");
    pause_text();
    clear();
    printf("as lendas dizem que a chave para a grandiosidade do reino foi\na atuação do mago conselheiro\nconsultor direto do rei");
    pause_text();
    clear();
    printf("entretando, o rei, temendo o poder do mago, \norganizou uma armadilha para tentar tirar seus poderes e sua vida");
    pause_text();
    clear();
    printf("tentativa essa que foi falha, permitindo o mago fugir. \n");
    pause_text();
    clear();
    printf("o mago usou o pouco que restou de seus poderes para se trancar em uma masmorra\nlugar onde ele se recuperaria até que a hora da sua vingança chegasse.\n");
    pause_text();
    clear();
    printf("temendo a iminente ira do mago, \no rei decretou que teria como seu sucessor aquele \nque conseguisse trazer a cabeça do mago...\n");
    pause_text();
}

void game() {

    // setup
    map = nullptr;

    srand(time(0));
    entityNumber = 0;

    p.facing = 1;
    p.life = 3;
    p.weapon = NOWEAPON;

    universalButtonCounter = 0;

    currentRoom = 0;
    setMapAsPreset(currentRoom);

    clear();
    gameState = INGAME;
    render();
    while(gameState == INGAME) {
        tick();
        render();
    }

    if (gameState == DEAD) { // dead
        deathScreen();
    } else if (gameState == WIN) { // won
        winScreen();
    }

}

// ######################################################################################### SCREENS

void deathScreen() {
    clear();

    printf("\nVOCE MORREU!\n\n");

    printf("O mago, após recuperar seus poderes, transformou o reino todo em cinzas...");
    pause_text();
    flushInputBuffer();
    clear();
}

void winScreen() {
    clear();

    printf("VOCE GANHOU!\n\n");

    printf("Coroado poucos dias depois da sua batalha, você sucediu o rei e trouxe uma era de paz para o reino\n");

    flushInputBuffer();
    pause_text();
    clear();
}

void tutorialScreen() {
    // draw tutorial
    clear();
    printf("SIGNIFICADO DOS OBJETOS:\n\n");
    printf("v, <, ^, > : jogador \n");
    printf("* : parede \n");
    printf("# : espinho \n");
    printf("k : caixa \n");
    printf("o : botao \n");
    printf("D : porta fechada \n");
    printf("= : porta aberta \n");
    printf("@ : chave \n");
    printf("L : escada \n");
    printf("V : aldeão \n");
    printf("X : monstro burro \n");
    printf("Y : monstro \n");
    printf("Z : mago \n");
    printf("\npage 1/3\n");
    pause_text();
    clear();

    printf("CONTROLES: \n\n");
    printf("w, a, s, d: movimento\n");
    printf("i: interagir\n");
    printf("o: atacar\n");
    printf("\npage 2/3\n");
    flushInputBuffer();
    pause_text();
    clear();

    startingDialogue();
}

void mainMenu() {
    int exit = 0;
    int choice = -100;
    int option = 0;

    while (exit == 0) {
        option = 0;
        choice = -100;
        do {
            clear();
            printf("\n###### MASMORRA DO MAGO #####\n\n");

            if (option == 0) {
                printf("\t> ");
            } else {
                printf("\t  ");
            }
            printf("iniciar jogo\n");

            if (option == 1) {
                printf("\t> ");
            } else {
                printf("\t  ");
            }
            printf("tutorial\n");

            if (option == 2) {
                printf("\t> ");
            } else {
                printf("\t  ");
            }
            printf("sair\n\n");

            printf("'i': selecionar\n");
            printf("'w','s': mover\n");

            int command = mygetch();
            if (command == 'w') option = (option+2)%3;
            if (command == 's') option = (option+1)%3;
            if (command == 'i') choice = option + 1;

        } while (choice < 1 || choice > 3);

        switch(choice) {
            case 1: // start game
                game();
            break;
            case 2:
                tutorialScreen();
            break;
            case 3:
                exit = 1;
            break;
        }
        clear();
    }
}

int main() {

    setlocale(LC_ALL, "");

    mainMenu();

    printf("obrigado por jogar! \n");
    return 0;
}
