/*
 * This program will only work on macOS and Linux.
 */

#include <stdlib.h>
#include <unistd.h>
#include <curses.h>

#define HEIGHT 32
#define WIDTH 32

void input();

void draw();

void logic();

typedef struct Position {
    int x, y;
} position;

typedef struct Snake {
    int len;
    position *pos;
} snake_type;

typedef struct Game {
    snake_type snake;
    position fruit;
    int running;
    int score;
    char buffer[HEIGHT][WIDTH];

} game_type;


int i, j, len;
game_type game;
position dir = {1, 0};


// Generate new fruit
void gen_new_fruit() {
    retry_fruit:
    game.fruit.x = rand() % WIDTH;
    game.fruit.y = rand() % HEIGHT;
    if (game.buffer[game.fruit.y][game.fruit.x] != ' ') goto retry_fruit;
}


// Setting up the game
void setup() {
    game.running = 1;
    game.snake.len = 5;
    game.snake.pos = calloc(game.snake.len, sizeof(position));
    if (game.snake.pos == NULL) {
        mvprintw("Memory not allocated.\n");
        exit(0);
    }
    game.snake.pos->x = 2;
    game.snake.pos->y = 2;
    draw();
    gen_new_fruit();
}

// Take input from the user
void input() {
    int c = getch();
    switch (c) {
        case 'w':
            dir = (position) {0, -1};
            break;
        case 'a':
            dir = (position) {-1, 0};
            break;
        case 's':
            dir = (position) {0, 1};
            break;
        case 'd':
            dir = (position) {1, 0};
            break;
        default:
            break;
    }
}

// Logical operations like moving the snake, checking if it's hitting an obstacle, checking if it's eating the fruit etc.
void logic() {
    // Move Snake
    position old_pos = *game.snake.pos;
    game.snake.pos->x += dir.x;
    game.snake.pos->y += dir.y;

    // Checking if the head is hitting the borders
    if (game.snake.pos->x == 0 || game.snake.pos->y == 0 || game.snake.pos->x == WIDTH - 1 ||
        game.snake.pos->y == HEIGHT - 1)
        game.running = 0;

    for (len = 1; len < game.snake.len; len++) {
        position temp = *(game.snake.pos + len);
        *(game.snake.pos + len) = old_pos;
        old_pos = temp;
        // Check if the head of the snake is eating the fruit
        if (game.snake.pos->x == game.fruit.x && game.snake.pos->y == game.fruit.y) {
            game.score++;
            game.snake.len++;
            game.snake.pos = realloc(game.snake.pos, game.snake.len * sizeof(position));
            if (game.snake.pos == NULL) {
                mvprintw("Memory not allocated.\n");
                exit(0);
            }
            gen_new_fruit();
        }

        // Checking if the snake is hitting itself
        if (game.buffer[game.snake.pos->y][game.snake.pos->x] == 'O') {
            game.running = 0;
        }
    }

}

void draw() {
    curs_set(0);
    // Drawing to our buffer
    for (j = 0; j < HEIGHT; j++) {
        for (i = 0; i < HEIGHT; i++) {
            if (j == 0 || j == HEIGHT - 1 || i == 0 || i == WIDTH - 1) {
                game.buffer[j][i] = '#';
            } else {
                game.buffer[j][i] = ' ';
            }
        }
    }

    // Draw Fruit
    game.buffer[game.fruit.y][game.fruit.x] = '*';

    // Draw Snake
    position pos;
    for (len = 0; len < game.snake.len; len++) {
        pos = *(game.snake.pos + len);
        game.buffer[pos.y][pos.x] = 'O';
    }

    // Draw the buffer to the screen
    for (j = 0; j < HEIGHT; j++) {
        for (i = 0; i < HEIGHT; i++) {
            mvprintw(j, i, "%c", game.buffer[j][i]);
        }
    }
    mvprintw(0, HEIGHT + 1, "Score: %d\n", game.score);
    refresh();
}

// Game Loops until it's over
void loop() {

    while (game.running == 1) {
        input();
        logic();
        draw();
        usleep(100 * 1000);
    }
    if (game.running == 0) {
        free(game.snake.pos);
    }
}


int main() {
    initscr();
    cbreak();
    noecho();
    timeout(1);
    setup();
    loop();
    return 0;

}
