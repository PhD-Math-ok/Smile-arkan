#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
 
 
 
#define MASSIZE(x) (sizeof(x)/sizeof(x[0]))
 
void newGame();
void game();
void menu(unsigned);
 
const char levels [][13][26] = {
 {
        "                       ",
        "       x    x          ",
        "                       ",
        "                       ",
        "                       ",
        "   x             x     ",
        "     x         x       ",
        "       x x x x         ",
        "                       ",
        "                       ",
        "                       ",
 }
    };
 
char buffer[45*90];
char blocks[13][26];
 
int scadd, move, level,  ballnum;
int pwidth, pcoord;
int keys[256] = {0}, frames = 0;
 
typedef struct {
    int x, y, predx, predy;
    int dx, dy;
} Ball;
 
Ball balls[100], O1 = {40, 22, 0, 22, 1, -1};
 
HANDLE hStdo = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE hStdi = GetStdHandle(STD_INPUT_HANDLE);
 
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
 
void drawChars (const char *buf, DWORD num, int color, int x, int y) {
    COORD crd = {x, y};
    SetConsoleTextAttribute(hStdo, color);
    SetConsoleCursorPosition(hStdo, crd);
    WriteConsoleA(hStdo, buf, num, &num, NULL);
    SetConsoleTextAttribute(hStdo, 0x7);
}
 
void drawBlock (int x, int y){
    int i=y-2, j=x/4;
    if (i>=10) return;
    if (blocks[i][j] == 'x'){
        drawChars("    ", 4, ((i+j)%+7)*16, j*4, i+2);
    }
}
 
void drawBlocks (int b) {
    for (int i=0; i<=10; ++i) {
        for (int j=0; j<20; ++j) {
            if (b && blocks[i][j] == 'x'){
                drawChars("    ", 4, ((i+j)%2+7)*16, j*4, i+2);
            }
            else if (/*(b && blocks[i][j] == ' ') ||*/
                     (blocks[i][j] == '-')) {
                drawChars("    ", 4, 0, j*4, i+2);
                blocks[i][j] = ' ';
            }
        }
    }
}
 
bool catchBall(Ball &B){
    for (int i=0; i<10; ++i){
        for (int j=0; j<20; ++j){
            if (blocks[i][j]=='x') return false;
        }
    }
    level = (level+1) % MASSIZE(levels);
    memcpy(blocks, levels[level], 21*10);
    drawBlocks(1);
    B.predx = B.x;
    B.predy = B.y;
    balls[0] = B;
    ballnum = 1;
    move = 0;
    game();
    return true;
}
 
void destroyBlock(int y, int x) {
    blocks[y][x] = '-';
   }
 
void moveBall(Ball &O) {
    int x, y;
    for(;;) {
        if ((O.x+O.dx < 11) && O.dx<0) {
            O.dx = -O.dx;
         
            continue;
        }
        if ((O.x+O.dx > 68) && O.dx>0) {
            O.dx = -O.dx;
     
            continue;
        }
        if ((O.y+O.dy < 2) && O.dy<0){
            O.dy = -O.dy;
          
            continue;
        }
        if ((O.y+O.dy == 23) && O.dy>0
                && (O.x+O.dx >= pcoord) && (O.x+O.dx < pcoord+pwidth)) {
            O.dy = -O.dy;
 
            if (O.x < pcoord + 3) O.dx = -1;
            else if (O.x >= pcoord+pwidth - 3) O.dx = 1;
            scadd = 0;
          
            if (catchBall(O)) return;
            continue;
        }
        x = O.x + O.dx;
        y = O.y + O.dy;
 
        if (y>0 && y<12){
            if (blocks[y-2][O.x/4] == 'x'){
                destroyBlock(y-2, O.x/4);
                O.dy = -O.dy;
           
           
                continue;
            }
            if (blocks[O.y-2][x/4] == 'x'){
                destroyBlock(O.y-2, x/4);
                O.dx = -O.dx;
  
              
                continue;
            }
            if (blocks[y-2][x/4] == 'x'){
                destroyBlock(y-2, x/4);
                O.dy = -O.dy;
                O.dx = -O.dx;
             
           
                continue;
            }
        }
        break;
    }
    O.predx = O.x;
    O.predy = O.y;
    O.x += O.dx;
    O.y += O.dy;
 
    if (O.y > 23) {
        O = balls[--ballnum];
    }
 
    if (ballnum == 0) {
        O.predy = O.y = 22;
        O.predx = O.x = pcoord + pwidth/2;
        balls[0] = O;
       
        ballnum = 1;
        move = 0;
    }
}
 
void scanInput() {
    DWORD n;
    INPUT_RECORD ir[100];
    PeekConsoleInput(hStdi, ir, 100, &n);
    if (n == 0) return;
    ReadConsoleInput(hStdi, ir, 100, &n);
    for (DWORD i=0; i<n; ++i) {
        if (ir[i].EventType == KEY_EVENT) {
            KEY_EVENT_RECORD ker = ir[i].Event.KeyEvent;
            keys[ker.wVirtualKeyCode] = ker.bKeyDown;
        }
    }
}
 
void waitKey() {
    FlushConsoleInputBuffer(hStdi);
    INPUT_RECORD ir[10];
    for(DWORD n=0; n==0;) {
        Sleep(100);
        PeekConsoleInput(hStdi, ir, 10, &n);
    }
}
 
 
 
void game() {
    FlushConsoleInputBuffer(hStdi);
 
    clock_t tfps = clock(), t1 = tfps;
    memset(buffer+80, 0xCD, 80);
    for (int i=2; i<24; ++i) {
        buffer[i*80+10] = buffer[i*80+69] = 0xBA;
    }
    memset(buffer+160+11, ' ', 58);
    buffer[80+10] = buffer[80+69] = 0xCB;
    drawChars(buffer, 80*24, 0xF, 0, 0);
    drawBlocks(1);
 
    int steps = 0;
 
    while (2010){
        if (clock()-tfps > 500) {
            printf(&buffer[13], "FPS: %5.1f", (float)frames/(clock()-tfps)*1000);
            tfps = clock();
            frames = 0;
        }
 
        Sleep(12);
        t1 = clock();
        frames++;
        steps++;
 
        //    ///============\\\     _
        memset(buffer+160+11, '    ', 58);
        memset(buffer+160+pcoord, 0xCD, pwidth);
        memset(buffer+160+pcoord, '/ ', 3);
        memset(buffer+160+pcoord+pwidth-3, '\\ ', 3);
        drawChars(buffer+160, 100, 15, 0, 23);
          drawBlocks(0);
      
        
        drawChars(buffer, 80, 10, 0, 0);
 
      
 
        // balls  O O O
        for (int i=0; i<ballnum; ++i){
            Ball &B = balls[i];
            if (B.x!=B.predx || B.y!=B.predy)
                drawChars(" ", 1, 0, B.predx, B.predy);
            drawChars("O", 1, 14, B.x, B.y);
            if ((steps % 4 == 0) && move)
                moveBall(B);
        }
 
 
 
        scanInput();
        if ((keys[VK_LEFT] || keys['A']) && pcoord > 11) {
            pcoord --;
            if (move==0) balls[0].predx = balls[0].x--;
        }
        if ((keys[VK_RIGHT] || keys['D'])  && pcoord+pwidth < 69) {
            pcoord ++;
            if (move==0) balls[0].predx = balls[0].x++;
        }
        if (keys[VK_SPACE]) move = 1;
        if (keys[VK_ESCAPE]) menu(1);
 
       
    }
}
 
void newGame() {
    balls[0] = O1;
    pcoord = (80-pwidth)/2;
    pwidth = 10;
    scadd = 0;
 
    move = 0;
 
    ballnum = 1;
    memcpy(blocks, levels[level=0], 21*10);
    return ;
}
 
 
 
void menu(unsigned m) {
    system("cls");
    srand(clock());
    char rus[100] = {0};
 
 
    const char *menu[] = {
        "         ",
        "        ",
        "--------------------------",
        "         New Game   ",
        "         Continue   ",
        "           Help     ",
        "           Exit     "
    };
    for (;;) {
        for (unsigned i=0; i < MASSIZE(menu); ++i) {
            static const char *st[] = {" ", " "};
            int color = (i<3) ? 8 : ((i == m+3) ? 15 : 7);
            drawChars(menu[i], strlen(menu[i]), color, 25, i+5);
            if (i==5) {
                const char *str = st[m];
                drawChars(str, 3, color, 25+strlen(menu[i]), i+5);
            }
            if (i==6) {
                const char *str = st[m];
                drawChars(str, 3, color, 25+strlen(menu[i]), i+5);
            }
        }
 
        scanInput();
        if (keys[VK_UP] && m > 0) {
            m--;
            keys[VK_UP] = 0;
        }
        if (keys[VK_DOWN] && m < MASSIZE(menu)-3) {
            m++;
            keys[VK_DOWN] = 0;
        }
        if (keys[VK_RETURN] || keys[VK_SPACE]) {
            if (m==0) {
                newGame();
                game();
            }
            else if (m==1) game();
            else if (m==2) ;
            else if (m==3) ;
              
 
            
            else if (m==4) ;
            else if (m==5) exit(0);
            keys[VK_RETURN] = 0;
        }
        SetConsoleCP(1251);
        drawChars(rus, strlen(rus), 7, 4, 22);
        Sleep(10);
    }
}
 
 
 
 
 
int main (int argc, char *argv[]) {
    
    //CreateThread(NULL, 0, playMusik, NULL, 0, NULL);
 
#ifdef DEBUG
    printf("  ");
    for (int i=0; i<16; ++i){
        printf(" %X", i);
    }
    for (int i=2; i<16; ++i){
        printf("\n %X", i);
        for (int j=0; j<16; ++j){
            printf(" %c", (char)(i*16+j));
        }
    }
    waitKey();
#endif
 
    CONSOLE_CURSOR_INFO CCI = {1, FALSE};
    SetConsoleCursorInfo(hStdo, &CCI);
    newGame();
    menu(0);
    
}