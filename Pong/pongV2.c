#include <stdio.h>

#define WIDTH 80
#define HEIGHT 25
#define PADDLE_SIZE 3
#define WIN_SCORE 21

typedef struct {
    int y;
} Paddle;

typedef struct {
    int x, y;
    int dx, dy;
} Ball;

typedef struct {
    Paddle left, right;
    Ball ball;
    int score_left;
    int score_right;
} Game;

Game init_game() {
    Game game;
    game.left.y = HEIGHT / 2 - PADDLE_SIZE / 2;
    game.right.y = HEIGHT / 2 - PADDLE_SIZE / 2;
    game.ball.x = WIDTH / 2;
    game.ball.y = HEIGHT / 2;
    game.ball.dx = -1;
    game.ball.dy = 1;
    game.score_left = 0;
    game.score_right = 0;
    return game;
}

void draw(Game game) {
    
    printf("\033[H\033[J");

    for (int i = 0; i < WIDTH; ++i) putchar('-');
    putchar('\n');

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            if (x == 0 || x == WIDTH - 1) {
                putchar('|');
            }
            else if (x == 2 && y >= game.left.y && y < game.left.y + PADDLE_SIZE) {
                putchar('|');
            }
            else if (x == WIDTH - 3 && y >= game.right.y && y < game.right.y + PADDLE_SIZE) {
                putchar('|');
            }
            else if (x == game.ball.x && y == game.ball.y) {
                putchar('O');
            }
            else {
                putchar(' ');
            }
        }
        putchar('\n');
    }

    for (int i = 0; i < WIDTH; ++i) putchar('-');
    putchar('\n');
    printf("Score: %d - %d\n", game.score_left, game.score_right);
    printf("Controls: A/Z for left, K/M for right, SPACE to skip, Q to quit\n");
    printf("Enter your move: ");
}

Paddle move_paddle(Paddle paddle, int up) {
    if (up && paddle.y > 0)
        paddle.y--;
    else if (!up && paddle.y + PADDLE_SIZE < HEIGHT)
        paddle.y++;
    return paddle;
}

Game update_ball(Game game) {
    game.ball.x += game.ball.dx;
    game.ball.y += game.ball.dy;

    if (game.ball.y <= 0 || game.ball.y >= HEIGHT - 1) 
        game.ball.dy = -game.ball.dy;

    if (game.ball.x == 3 && game.ball.y >= game.left.y && 
        game.ball.y < game.left.y + PADDLE_SIZE) {
        game.ball.dx = 1;
    }

    if (game.ball.x == WIDTH - 4 && game.ball.y >= game.right.y &&
        game.ball.y < game.right.y + PADDLE_SIZE) {
        game.ball.dx = -1;
    }

    if (game.ball.x <= 0) {
        game.score_right++;
        game.ball.x = WIDTH / 2;
        game.ball.y = HEIGHT / 2;
        game.ball.dx = 1;
    } 
    else if (game.ball.x >= WIDTH - 1) {
        game.score_left++;
        game.ball.x = WIDTH / 2;
        game.ball.y = HEIGHT / 2;
        game.ball.dx = -1;
    }
    
    return game;
}

int main() {
    Game game = init_game();
    char input;

    while (game.score_left < WIN_SCORE && game.score_right < WIN_SCORE) {
        draw(game);

        input = getchar();
        while (getchar() != '\n');

        switch (input) {
            case 'a':
            case 'A':
                game.left = move_paddle(game.left, 1);
                break;
            case 'z':
            case 'Z':
                game.left = move_paddle(game.left, 0);
                break;
            case 'k':
            case 'K':
                game.right = move_paddle(game.right, 1);
                break;
            case 'm':
            case 'M':
                game.right = move_paddle(game.right, 0);
                break;
            case 'q':
            case 'Q':
                printf("Game stopped by user.\n");
                return 0;
            default:
                break;
        }

        game = update_ball(game);
    }

    draw(game);

    if (game.score_left == WIN_SCORE)
        printf("Player Left Wins!\n");
    else
        printf("Player Right Wins!\n");

    return 0;
}