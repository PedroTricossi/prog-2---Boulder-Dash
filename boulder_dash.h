#ifndef __boulder_dash__
#define __boulder_dash__

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>

#define FPS 60.0
#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define SIZE 40.0
#define WORDSIZE 1024
#define NUMTEXTURES 8
#define PATH "./resources/levels/"
#define SCOREPATH "./resources/scores/scores.txt"

typedef enum{UP,DOWN, RIGHT, LEFT, PLAY, PAUSE, HELP} STATES;
typedef enum {EMPTY, MINER, EARTH, BORDER, ROCK, DIAMOND, SPIDER, MONSTER, WATER, DOOR}ID;


typedef struct
{
    int x, y;
}Position;

typedef struct
{
    int ObjectID;
    ALLEGRO_BITMAP *image;

}Object;

typedef struct
{
    int ObjectID;
    ALLEGRO_BITMAP *image;
    float speed;
    Position p;
    bool alive;
    int diamond, score, life;

}Miner;

typedef struct
{
    int ObjectID;
    ALLEGRO_BITMAP *image;
    Position p;
    bool is_falling, alive;
}FallingObject;


typedef struct
{
    int number;
    int total_diamond;
    char file_name[25];
    int row, col;
    int time;
    float y_radius, x_radius;
}Level;

// Valida se a alocação ocorreu de forma correta
void validade_alocation(void* ptr);

// Carrega bitmap do tamanho correto para a tela
ALLEGRO_BITMAP *load_bitmap_at_size(const char *filename, int w, int h);

// Atualiza o mineiro em relação ao mapa
void update_miner(Object **map, ALLEGRO_BITMAP *texture[NUMTEXTURES], ALLEGRO_SAMPLE_INSTANCE *walk_empty,ALLEGRO_SAMPLE_INSTANCE *walk_earth, ALLEGRO_SAMPLE_INSTANCE *boulder, ALLEGRO_SAMPLE_INSTANCE *collect_diamond, Miner *m, FallingObject *rock, FallingObject *diamond, int r_num, int d_num, int move, int *result);

// Inicia o mapa
void init_map(Object **map, ALLEGRO_BITMAP *texture[NUMTEXTURES], int r, int c, char *file_name);

// Desenha o mapa
void draw_map(Object **map, int row, int col, Miner m);

// Atualiza a posição das pedras em relação ao mapa (também usado para os diamantes)
void update_rock(Object **map, ALLEGRO_BITMAP *texture[NUMTEXTURES], ALLEGRO_SAMPLE_INSTANCE *boulder, int row, int col, FallingObject *rock, int r_num, Miner *m, FallingObject *diamond, int *d_num);

// Carrega o numero de pedras e de diamantes no mapa
void find_objects_len(Object **map, int row, int col, int *r_num, int *d_num);

// Inicia os objetos
void init_object(Object **map, ALLEGRO_BITMAP *texture[NUMTEXTURES], int row, int col, Miner *m, FallingObject *r_list, FallingObject *d_list);

// atualiza a camera para uma melhor jogabilidade
void camera_update(Position *camera_position, int x, int y, int width, int height, int row , int col, Level lvl);

// Desenha a pontuação em tempo real
void draw_score(ALLEGRO_FONT *text_font, ALLEGRO_BITMAP *texture[NUMTEXTURES], ALLEGRO_BITMAP *icon_diamond, ALLEGRO_SAMPLE_INSTANCE *music, Miner *m, int time_left, Level level, Position camera_position);

// Checa se o mineiro está morto
bool check_dead(Miner *m);

// Checa se chegou ao final do jogo
int shall_we_pass(Object **map, int row, int col, Miner *m, int time_left, Level level);

// Desenha a porta de saida
int create_door(Object **map, ALLEGRO_BITMAP *texture[NUMTEXTURES], Miner *m, Level level);

// Muda o nivel atual para o proximo nível
void change_level(Level *curr_level, Level *next_level);

// Inicia o nível
Level* init_level();

// Le a pontuação salva em disco e atualiza, caso necessário
void get_score(int score, int* top_scores);

#endif