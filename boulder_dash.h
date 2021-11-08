#ifndef __boulder_dash__
#define __boulder_dash__

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>

#define M_FPS 10.0
#define R_FPS 6.0
#define S_FPS 3.0
#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define SIZE 40.0

typedef enum{UP,DOWN, RIGHT, LEFT, PLAY, PAUSE} STATES;
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
    unsigned int diamond, score, life;

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
    int ObjectID;
    ALLEGRO_BITMAP *image;
    float speed;
    Position p;
    bool alive;
}MovingObject;


typedef struct
{
    int number;
    int total_diamond;
    char file_name[25];
    int row, col;
    int time;
    float y_radius, x_radius;
}Level;


ALLEGRO_BITMAP *load_bitmap_at_size(const char *filename, int w, int h);

/*  miners mechanics, generally due to keyboard changing, this movement effects rocks, diamonds which are parameters*/ 
void update_miner(Object **map, ALLEGRO_BITMAP *texture[10], ALLEGRO_SAMPLE_INSTANCE *walk_empty,ALLEGRO_SAMPLE_INSTANCE *walk_earth, ALLEGRO_SAMPLE_INSTANCE *boulder, ALLEGRO_SAMPLE_INSTANCE *collect_diamond, Miner *m, FallingObject *rock, FallingObject *diamond, int r_num, int d_num, int move, int *result);


void init_map(Object **map, ALLEGRO_BITMAP *texture[10], int r, int c, char *file_name);

/* one and only draw function,                                               *
 * drawing is basically drawing the map and the map changes due to mechanics */
void draw_map(Object **map, int row, int col, Miner m);

/* quickly add diamonds to diamond list */
void add_diamond(FallingObject *diamond, ALLEGRO_BITMAP *texture[10], int index, int x, int y);

/* rock mechanics, free fall and can be used for diamond list to since they have the same movement mechs */
void update_rock(Object **map, ALLEGRO_BITMAP *texture[10], ALLEGRO_SAMPLE_INSTANCE *boulder, int row, int col, FallingObject *rock, int r_num, Miner *m, FallingObject *diamond, int *d_num, MovingObject *spider, int s_num, MovingObject *monster, int m_num);

/* spider goes top, bottom and left or right */
void update_spider(Object **map, ALLEGRO_BITMAP *texture[10], int row, int col, MovingObject *spider, int s_num, Miner *m);

/* monster finds the closest way to miner */  
void update_monster(Object **map, ALLEGRO_BITMAP *texture[10],int row, int col, Miner *m, MovingObject *monster, int m_num);

/* water spreads to map */
void update_water(Object **map, ALLEGRO_BITMAP *texture[10], MovingObject *water, int *water_len);

/* initialization of objects due to map, creating simple array data types from map */
void find_objects_len(Object **map, int row, int col, int *s_num, int *m_num, int *w_num, int *r_num, int *d_num);
void init_object(Object **map, ALLEGRO_BITMAP *texture[10], int row, int col, Miner *m, MovingObject *w_list, MovingObject *m_list, MovingObject *s_list, FallingObject *r_list, FallingObject *d_list);
void camera_update(Position *camera_position, int x, int y, int width, int height, int row , int col, Level lvl);
void draw_score(ALLEGRO_FONT *text_font, ALLEGRO_BITMAP *texture[10], ALLEGRO_BITMAP *icon_diamond, ALLEGRO_SAMPLE_INSTANCE *music, Miner *m, int time_left, Level level, Position camera_position);
bool check_dead(Miner *m);
int is_over(Object **map, int row, int col, Miner *m, int time_left, Level level);
int create_door(Object **map, ALLEGRO_BITMAP *texture[10], Miner *m, Level level);
void change_level(Level *curr_level, const Level next_level);
void init_level(Level level[10]);

void destroy_music();

#endif