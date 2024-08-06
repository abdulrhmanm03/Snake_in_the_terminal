#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>
#include <time.h>


typedef struct {
    int x;
    int y;
}food;

typedef struct {
    int x;
    int y; 
}snakeCell;


void set_raw_mode(struct termios *original_tty);
void reset_mode(struct termios *original_tty);
void clearScreen();
void drawGame(int width, int height, snakeCell *snake, int snakeLength, food f);
int direction(int lastDir);
void movement(snakeCell *snake, int snakeLength, int dir);
food generateFood(int width, int height);
snakeCell addCell(snakeCell tail);
snakeCell* updateSnake(snakeCell *snake, snakeCell newCell, int *snakeLength);


int main(){

    struct termios original_tty;
    srand(time(NULL)); 

    int width = 60;
    int height = 30;

    int snakeLength = 4;

    snakeCell *snake = (snakeCell*) malloc(snakeLength * sizeof(snakeCell));
    snake[0].x = 10;
    snake[0].y = 10;
    snake[1].x = 9;
    snake[1].y = 10;
    snake[2].x = 8;
    snake[2].y = 10;
    snake[3].x = 7;
    snake[3].y = 10;

    int dir = 3;

    set_raw_mode(&original_tty);

    food f = generateFood(width, height);

    int score = 0;

    while (1) {
        
        dir = direction(dir);  // Get direction from input
        if (dir == 0) {  // Check for quit command
            break;
        }
        if(snake[0].x == 0 || snake[0].y == 0 || snake[0].x == width-1 || snake[0].y == height-1){
            break;
        }
        int collision = 0;
        for (int i = 1; i < snakeLength; i++){
            if (snake[0].x == snake[i].x && snake[0].y == snake[i].y){
                collision = 1;
            }            
        }
        if (collision){
            break;
        }
        if(snake[0].x == f.x && snake[0].y == f.y ){
            snakeCell newCell = addCell(snake[snakeLength-1]);
            snake = updateSnake(snake, newCell, &snakeLength);
            f = generateFood(width, height);
            score++;
        }

        clearScreen();  // Clear the screen
        drawGame(width, height, snake, snakeLength, f);  // Draw game state
        printf("Score: %d\n", score);
        movement(snake, snakeLength, dir);  // Update snake position
        usleep(10000);  // Sleep to control speed
    }
    printf("Game over\n");

    free(snake);

    reset_mode(&original_tty);
    return 0;
}

void drawGame(int width, int height, snakeCell *snake, int snakeLength, food f) {

    printf("\n");
    printf("w to move up, s to move down, d to move right, a to move left, q to quit\n");
    printf("\n");
    
    char map[height][width];

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            map[i][j] = ' ';
        }
    }

    for (int i = 0; i < width; i++) {
        map[0][i] = '#';
        map[height - 1][i] = '#';
    }
    for (int i = 0; i < height; i++) {
        map[i][0] = '#';
        map[i][width - 1] = '#';
    } 

    map[f.y][f.x] = '@';

    for (int k = 0; k < snakeLength; k++) {
        int x = snake[k].x;
        int y = snake[k].y;
        
        map[y][x] = 'O';
    }


    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            
            putchar(map[i][j]);
        }
        printf("\n");
    }
}

void movement(snakeCell *snake, int snakeLength, int dir){

    int ix = snake[0].x;
    int iy = snake[0].y;
    int iix;
    int iiy;
    if (dir == 1){
        snake[0].y--;
    }else if (dir == 2){
        snake[0].y++;
    }else if (dir == 3){
        snake[0].x++;
    }else if (dir == 4){
        snake[0].x--;
    }
    for (int i = 1; i < snakeLength; i++)
    {
        iix = snake[i].x;
        iiy = snake[i].y;
        snake[i].x = ix;
        snake[i].y = iy;
        ix = iix;
        iy = iiy;
    }
}

snakeCell addCell(snakeCell tail){

    snakeCell newCell;

    newCell.x = tail.x;
    newCell.y = tail.y;

    return newCell;
}

snakeCell* updateSnake(snakeCell *snake, snakeCell newCell, int *snakeLength) {
    int oldSnakeLength = *snakeLength;

    snakeCell *newSnake = (snakeCell*) malloc((*snakeLength + 1) * sizeof(snakeCell));

    for (int i = 0; i < oldSnakeLength; i++) {
        newSnake[i].x = snake[i].x;
        newSnake[i].y = snake[i].y;
    }

    newSnake[*snakeLength].x = newCell.x;
    newSnake[*snakeLength].y = newCell.y;

    free(snake);

    (*snakeLength)++;

    return newSnake;
}

int direction(int lastDir) {
    char ch;
    struct timeval tv;
    fd_set readfds;

    tv.tv_sec = 0;
    tv.tv_usec = 100000;  // 100ms

    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);

    if (result == -1) {
        perror("select");
        exit(EXIT_FAILURE);
    } else if (result > 0) {
        read(STDIN_FILENO, &ch, 1);
        if (ch == 'q') {
            return 0;
        } else if (ch == 'w' && lastDir != 2) {
            return 1;
        } else if (ch == 's' && lastDir !=1) {
            return 2;
        } else if (ch == 'd' && lastDir !=4) {
            return 3;
        } else if (ch == 'a' && lastDir !=3) {
            return 4;
        }
    }
    return lastDir;
}

food generateFood(int width, int height){
    food f;
    f.x = (rand() % (width-2)) +1;
    f.y = (rand() % (height-2)) +1;
    return f;
}

void set_raw_mode(struct termios *original_tty) {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    *original_tty = tty;  // Save the original terminal settings
    tty.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
    tty.c_cc[VMIN] = 0;  // Minimum number of characters for non-canonical read
    tty.c_cc[VTIME] = 1; // Timeout for non-canonical read (0.1 seconds)
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void reset_mode(struct termios *original_tty) {
    tcsetattr(STDIN_FILENO, TCSANOW, original_tty);
}

void clearScreen() {
    printf("\033[H\033[J");
    fflush(stdout);
}