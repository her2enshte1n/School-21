#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 80
#define HEIGHT 25

// Паттерны
#define PATTERN_COUNT 5
const char* pattern_files[PATTERN_COUNT] = {
    "glider.txt",
    "exploder.txt",
    "gosper_gun.txt",
    "random.txt",
    "pulsar.txt"
};

const char* pattern_names[PATTERN_COUNT] = {
    "Glider",
    "Small Exploder",
    "Gosper Glider Gun",
    "Random Pattern",
    "Pulsar"
};

int field[HEIGHT][WIDTH] = {0};
int new_field[HEIGHT][WIDTH] = {0};

// Прототипы функций
int count_neighbors(int y, int x);
void draw_field();
void update_field();
void draw_borders();
void show_menu();
int load_pattern(const char* filename);
void initialize_field();

int count_neighbors(int y, int x) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0) continue;
            int ny = (y + dy + HEIGHT) % HEIGHT;
            int nx = (x + dx + WIDTH) % WIDTH;
            count += field[ny][nx];
        }
    }
    return count;
}

void draw_borders() {
    for (int x = 0; x < WIDTH + 1; x++) {
        mvaddch(0, x, '-');
        mvaddch(HEIGHT + 1, x, '-');
    }
    for (int y = 1; y <= HEIGHT; y++) {
        mvaddch(y, 0, '|');
        mvaddch(y, WIDTH + 1, '|');
    }
    mvaddch(0, 0, '+');
    mvaddch(0, WIDTH + 1, '+');
    mvaddch(HEIGHT + 1, 0, '+');
    mvaddch(HEIGHT + 1, WIDTH + 1, '+');
}

void draw_field() {
    clear();
    draw_borders();
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            mvaddch(y + 1, x + 1, field[y][x] ? '#' : ' ');
        }
    }
    refresh();
}

void update_field() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            new_field[y][x] = 0;
        }
    }
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int neighbors = count_neighbors(y, x);
            if (field[y][x] == 1) {
                new_field[y][x] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
            } else {
                new_field[y][x] = (neighbors == 3) ? 1 : 0;
            }
        }
    }
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            field[y][x] = new_field[y][x];
        }
    }
}

void initialize_field() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            field[y][x] = 0;
        }
    }
}

int load_pattern(const char* filename) {
    initialize_field();
    FILE* file = fopen(filename, "r");
    if (!file) return 0;
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            char c = fgetc(file);
            if (c == EOF) break;
            field[y][x] = (c == '1') ? 1 : 0;
        }
        while (fgetc(file) != '\n' && !feof(file));
        if (feof(file)) break;
    }
    fclose(file);
    return 1;
}

void show_menu() {
    int highlight = 0;
    int choice = -1;
    
    while (1) {
        clear();
        mvprintw(0, 0, "Select starting pattern:");
        
        for (int i = 0; i < PATTERN_COUNT; i++) {
            if (i == highlight) attron(A_REVERSE);
            mvprintw(i + 2, 2, "%d. %s", i + 1, pattern_names[i]);
            if (i == highlight) attroff(A_REVERSE);
        }
        
        mvprintw(PATTERN_COUNT + 3, 2, "Use arrow keys to navigate, ENTER to select");
        refresh();
        
        int ch = getch();
        switch (ch) {
            case KEY_UP:
                highlight = (highlight - 1 + PATTERN_COUNT) % PATTERN_COUNT;
                break;
            case KEY_DOWN:
                highlight = (highlight + 1) % PATTERN_COUNT;
                break;
            case 10: // ENTER
                choice = highlight;
                break;
                return;
        }
        
        if (choice != -1) {
            if (load_pattern(pattern_files[choice])) {
                return;
            } else {
                mvprintw(PATTERN_COUNT + 5, 2, "Failed to load pattern!");
                refresh();
                getch();
                choice = -1;
            }
        }
    }
}

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    
    show_menu();
    
    nodelay(stdscr, TRUE);
    int delay = 150000;
    
    while (1) {
        draw_field();
        update_field();

        char ch = getch();
        switch (ch) {
            case ' ':
                endwin();
                return 0;
            case 'a':
            case 'A':
                if (delay <= 15000) break;
                delay -= 10000;
                break;
            case 'z':
            case 'Z':
                if (delay == 5000) break;
                delay += 10000;
                break;
            case 'm':
            case 'M':
                show_menu();
                break;
        }
        usleep(delay);
    }
}
