#include <time.h>
#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <dirent.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>
#include <allegro5/keyboard.h>
#include <allegro5/timer.h>
#include <allegro5/color.h>
#include "boulder_dash.h"

// Valida se a alocação ocorreu de forma correta
void validade_alocation(void* ptr){
    if(ptr == NULL){
        fprintf(stderr, "Erro na alocação");
        exit(1);
    }
}

// Verifica se o arquivo foi aberto de forma correta
void validade_file(FILE* file){
    if(file == NULL){
        fprintf(stderr, "Erro ao abrir arquivo");
        exit(1);
    }
}

// Carrega bitmap do tamanho correto para a tela
ALLEGRO_BITMAP *load_bitmap_at_size(const char *filename, int w, int h)
{
  ALLEGRO_BITMAP *resized_bmp, *loaded_bmp, *prev_target;

  resized_bmp = al_create_bitmap(w, h);
  if (!resized_bmp)
    return NULL;

  loaded_bmp = al_load_bitmap(filename);

  if (!loaded_bmp)
  {
     al_destroy_bitmap(resized_bmp);
     return NULL;
  }

  prev_target = al_get_target_bitmap();
  al_set_target_bitmap(resized_bmp);

  al_draw_scaled_bitmap(loaded_bmp,
     0, 0,                                
     al_get_bitmap_width(loaded_bmp),     
     al_get_bitmap_height(loaded_bmp),    
     0, 0,                                
     w, h,                                
     0
  );

  al_set_target_bitmap(prev_target);
  al_destroy_bitmap(loaded_bmp);

  return resized_bmp;
}

// Atualiza o mineiro em relação ao mapa
void update_miner(Object **map, ALLEGRO_BITMAP *texture[10], ALLEGRO_SAMPLE_INSTANCE *walk_empty,ALLEGRO_SAMPLE_INSTANCE *walk_earth, ALLEGRO_SAMPLE_INSTANCE *boulder, ALLEGRO_SAMPLE_INSTANCE *collect_diamond, Miner *m, FallingObject *rock, FallingObject *diamond, int r_num, int d_num, int move, int *result)
{
    Object *ob_up, *ob_down, *ob_right, *ob_left;

    ob_up = &map[m->p.y-1][m->p.x];
    ob_down = &map[m->p.y+1][m->p.x];
    ob_right = &map[m->p.y][m->p.x+1];
    ob_left = &map[m->p.y][m->p.x-1];

    if(move == UP)
    {
        switch(ob_up->ObjectID)
        {
            case DOOR: *result = 1; return;
            case EMPTY: al_play_sample_instance(walk_empty); break;
            case EARTH : al_play_sample_instance(walk_earth); break;
            case DIAMOND:
                al_play_sample_instance(collect_diamond);
                for(int i=0; i<d_num; i++)
                {
                    if(diamond[i].p.x == m->p.x && diamond[i].p.y == m->p.y - 1)
                        diamond[i].alive = false;
                }
                m->diamond++; m->score+=10; break;
            case ROCK:
            case WATER:
            case BORDER: return;

        }
    }else if(move == DOWN)
    {
        switch(ob_down->ObjectID)
        {
            case DOOR: *result = 1; return;
            case EMPTY: al_play_sample_instance(walk_empty); break;
            case EARTH : al_play_sample_instance(walk_earth); break;
            case DIAMOND:
                al_play_sample_instance(collect_diamond);
                for(int i=0; i<d_num; i++)
                {
                    if(diamond[i].p.x == m->p.x && diamond[i].p.y == m->p.y + 1)
                        diamond[i].alive = false;
                }
                m->diamond++; m->score+=10; break;
            case ROCK:
            case WATER:
            case BORDER: return;

        }
    }else if(move == RIGHT)
    {
        switch(ob_right->ObjectID)
        {
            case DOOR: *result = 1; return;
            case EMPTY: al_play_sample_instance(walk_empty); break;
            case EARTH : al_play_sample_instance(walk_earth); break;
            case DIAMOND:
                al_play_sample_instance(collect_diamond);
                for(int i=0; i<d_num; i++)
                {
                    if(diamond[i].p.x == m->p.x+1 && diamond[i].p.y == m->p.y)
                        diamond[i].alive = false;
                }
                m->diamond++; m->score+=10; break;

            case ROCK:
                if(map[m->p.y][m->p.x+2].ObjectID == EMPTY)
                {
                    al_play_sample_instance(boulder);
                    for(int i=0; i<r_num; i++)
                    {
                        if(rock[i].p.x == m->p.x+1 && rock[i].p.y == m->p.y)
                            rock[i].p.x = m->p.x+2;
                    }
                    ob_right->ObjectID = EMPTY;
                    ob_right->image = texture[EMPTY];
                    map[m->p.y][m->p.x+2].ObjectID = ROCK;
                    map[m->p.y][m->p.x+2].image = texture[ROCK];
                    map[m->p.y][m->p.x+1].ObjectID = MINER;
                    map[m->p.y][m->p.x+1].image = texture[MINER];
                    break;
                }
            case WATER:
            case BORDER: return;

        }
    }else if(move == LEFT)
    {
        switch(ob_left->ObjectID)
        {
            case DOOR: *result = 1; return;
            case EMPTY: al_play_sample_instance(walk_empty); break;
            case EARTH : al_play_sample_instance(walk_earth); break;
            case DIAMOND:
                al_play_sample_instance(collect_diamond);
                for(int i=0; i<d_num; i++)
                {
                    if(diamond[i].p.x == m->p.x-1 && diamond[i].p.y == m->p.y)
                        diamond[i].alive = false;
                }
                m->diamond++; m->score+=10; break;
            case ROCK:
                 if(map[m->p.y][m->p.x-2].ObjectID == EMPTY)
                {
                    al_play_sample_instance(boulder);
                    for(int i=0; i<r_num; i++)
                    {
                        if(rock[i].p.x == m->p.x-1 && rock[i].p.y == m->p.y)
                            rock[i].p.x = m->p.x-2;
                    }
                    ob_left->ObjectID = MINER;
                    ob_left->image = texture[MINER];
                    map[m->p.y][m->p.x-2].ObjectID = ROCK;
                    map[m->p.y][m->p.x-2].image = texture[ROCK];
                    map[m->p.y][m->p.x-1].ObjectID = MINER;
                    map[m->p.y][m->p.x-1].image = texture[MINER];
                    break;
                }
            case WATER:
            case BORDER: return;

        }
    }

    map[m->p.y][m->p.x].ObjectID = EMPTY;
    map[m->p.y][m->p.x].image = texture[EMPTY];

    switch(move)
    {
        case UP: (m->p.y)--;    break;
        case DOWN: (m->p.y)++;  break;
        case RIGHT: (m->p.x)++;  break;
        case LEFT: (m->p.x)--;   break;

    }

    map[m->p.y][m->p.x].ObjectID = MINER;
    map[m->p.y][m->p.x].image = texture[MINER];
}


// Inicia o mapa
void init_map(Object **map, ALLEGRO_BITMAP *texture[10], int r, int c, char *file_name)
{
    FILE *fp = fopen(file_name, "r");
    validade_file(fp);

    char s;

    do
    {
        s = fgetc(fp);
    }while(s != '-');

    for(int i=0; i<r; i++)
    {
        for(int j=0; j<c; j++)
        {
            switch(s = fgetc(fp))
            {
                case 'M': map[i][j].ObjectID = MINER; map[i][j].image = texture[MINER];      break;
                case ' ': map[i][j].ObjectID = EMPTY; map[i][j].image = texture[EMPTY];      break;
                case 'E': map[i][j].ObjectID = EARTH; map[i][j].image = texture[EARTH];      break;
                case 'B': map[i][j].ObjectID = BORDER; map[i][j].image = texture[BORDER];    break;
                case 'R': map[i][j].ObjectID = ROCK; map[i][j].image = texture[ROCK];        break;
                case '*': map[i][j].ObjectID = DIAMOND; map[i][j].image = texture[DIAMOND];  break;
                case 'D': map[i][j].ObjectID = DOOR; map[i][j].image = texture[DOOR];        break;
                default: j--;
            }
        }
    }

    fclose(fp);
}

// Desenha o mapa
void draw_map(Object **map, int row, int col, Miner m)
{
    for(int y=0; y<row; y++)
    {
        for(int x=0; x<col; x++)
        {
            switch(map[y][x].ObjectID)
            {
                default: al_draw_bitmap(map[y][x].image, x*SIZE, y*SIZE,0);
            }
        }
    }
}

// Atualiza a posição das pedras em relação ao mapa (também usado para os diamantes)
void update_rock(Object **map, ALLEGRO_BITMAP *texture[10], ALLEGRO_SAMPLE_INSTANCE *boulder, int row, int col, FallingObject *rock, int r_num, Miner *m, FallingObject *diamond, int *d_num)
{
    int x,y,j=0;
    ALLEGRO_BITMAP *image;
    int ID;
    if(rock[0].ObjectID == ROCK)
    {
        image = texture[ROCK];
        ID = ROCK;
    }
    else
    {
        image = texture[DIAMOND];
        ID = DIAMOND;
    }

    while(j<*d_num)
        j++;
    


    for(int i=0; i<r_num; i++)
    {
        x = rock[i].p.x;
        y = rock[i].p.y;

        if(rock[i].alive)
        {
            if(rock[i].is_falling)
            {
                if(map[y+1][x].ObjectID == MINER)
                {
                    m->alive = false;
                    map[y][x].ObjectID = EMPTY;
                    map[y][x].image = texture[EMPTY];
                    map[y+1][x].ObjectID = ID;
                    map[y+1][x].image = image;
                    rock[i].p.y = y+1;
                }
                else
                {
                    if(map[y+1][x].ObjectID == EMPTY)
                    {
                        if(rock[i].ObjectID == ROCK)  al_play_sample_instance(boulder);
                        map[y][x].ObjectID = EMPTY;
                        map[y][x].image = texture[EMPTY];
                        map[y+1][x].ObjectID = ID;
                        map[y+1][x].image = image;
                        rock[i].p.y = y+1;
                    }
                    else rock[i].is_falling = false;
                }
            }
            else if(map[y+1][x].ObjectID == EMPTY)
            {
                if(rock[i].ObjectID == ROCK)  al_play_sample_instance(boulder);
                map[y][x].ObjectID = EMPTY;
                map[y][x].image = texture[EMPTY];
                map[y+1][x].ObjectID = ID;
                map[y+1][x].image = image;
                rock[i].p.y = y+1;
                rock[i].is_falling = true;
            }
            else    rock[i].is_falling = false;
        }
    }
}

// Carrega o numero de pedras e de diamantes no mapa
void find_objects_len(Object **map, int row, int col, int *r_num, int *d_num)
{
    *r_num = 0;
    *d_num = 0;

    for(int y=0; y<row-1; y++)
    {
        for(int x=0; x<col; x++)
        {
            switch(map[y][x].ObjectID)
            {
                case ROCK: (*r_num)++; break;
                case DIAMOND: (*d_num)++; break;
            }
        }
    }

}

// Inicia os objetos
void init_object(Object **map, ALLEGRO_BITMAP *texture[10], int row, int col, Miner *m, FallingObject *r_list, FallingObject *d_list)
{

    int r_i=0, d_i=0;

    for(int y=0; y<row; y++)
        {
            for(int x=0; x<col; x++)
            {
                switch(map[y][x].ObjectID)
                {
                    case MINER:
                        m->ObjectID = MINER;
                        m->image = texture[MINER];
                        m->p.x = x;
                        m->p.y = y;
                        m->score = 0;
                        m->diamond = 0;
                        m->alive = true;
                        m->life = 3;
                        m->speed = SIZE;
                        break;
                    case ROCK:
                        r_list[r_i].ObjectID = ROCK;
                        r_list[r_i].image = texture[ROCK];
                        r_list[r_i].p.x = x;
                        r_list[r_i].p.y = y;
                        r_list[r_i].alive = true;
                        r_list[r_i++].is_falling = false;
                        break;
                    case DIAMOND:
                        d_list[d_i].ObjectID = DIAMOND;
                        d_list[d_i].image = texture[DIAMOND];
                        d_list[d_i].p.x = x;
                        d_list[d_i].p.y = y;
                        d_list[d_i].alive = true;
                        d_list[d_i++].is_falling = false;
            }
        }
    }

}

// atualiza a camera para uma melhor jogabilidade
void camera_update(Position *camera_position, int x, int y, int width, int height, int row , int col, Level lvl)
{

    if(x> (float)col/lvl.x_radius) x=(float)col/lvl.x_radius+0.5;
    if(y> (float)row/lvl.y_radius) y=(float)row/lvl.y_radius+0.5;

    camera_position->x = -(SCREEN_WIDTH/2) + (x*SIZE+(width/2));
    camera_position->y = -(SCREEN_HEIGHT/2) + (y*SIZE+(height/2));

    if(camera_position->x <0)
        camera_position->x = 0;
    if(camera_position->y <0)
        camera_position->y = 0;
}

// Desenha a pontuação em tempo real
void draw_score(ALLEGRO_FONT *text_font,ALLEGRO_BITMAP *texture[10], ALLEGRO_BITMAP *icon_diamond, ALLEGRO_SAMPLE_INSTANCE *music, Miner *m, int time_left, Level level, Position camera_position)
{
    int x,y;
    x = camera_position.x;
    y = camera_position.y;


    for(int i=0; i<SCREEN_WIDTH/SIZE; i++)
        al_draw_bitmap(texture[EMPTY], (i*SIZE)+x, y,0);

    al_draw_bitmap(icon_diamond, x+7*SIZE/5,y,0);
    al_draw_textf(text_font, al_map_rgb(255,223,0), x+6*SIZE/5, y, ALLEGRO_ALIGN_RIGHT, "%.2d", level.total_diamond);
    al_draw_textf(text_font, al_map_rgb(255,223,0), 17*SIZE/5+x , y, ALLEGRO_ALIGN_RIGHT, "%.2d",m->diamond);
    al_draw_textf(text_font, al_map_rgb(255,223,0), 9*SIZE+x , y, ALLEGRO_ALIGN_RIGHT, "LEVEL:%.2d", level.number);
    al_draw_textf(text_font, al_map_rgb(255,223,0), 15*SIZE+x, y, ALLEGRO_ALIGN_RIGHT, "LIFE: %.2d", m->life);
    al_draw_textf(text_font, al_map_rgb(255,223,0), 22*SIZE+x, y, ALLEGRO_ALIGN_RIGHT, "TIME: %.3d", time_left);
    al_draw_textf(text_font, al_map_rgb(255,223,0), 32*SIZE+x, y, ALLEGRO_ALIGN_RIGHT, "SCORE: %.6d", m->score);
    al_play_sample_instance(music);
}

// Checa se o mineiro está morto
bool check_dead(Miner *m)
{
    bool over = false;

    if(m->alive == false)
    {
        if(m->life > 0)
        {
            m->life--;
            m->alive = true;
            m->p.x = 1;
            m->p.y = 1;
        }
        else over = true;

    }

    return over;
}

// Checa se chegou ao final do jogo
int shall_we_pass(Object **map, int row, int col, Miner *m, int time_left, Level level)
{
    int result=0;

    if(check_dead(m) || time_left==0) result = -1;

    else
    {
        for(int y=0; y<col; y++)
        {
            for(int x=0; x<row; x++)
            {
                if(map[y][x].ObjectID == DOOR && m->p.x == x && m->p.y == y)
                    result = 1;
                return result;
            }
        }
    }

    return result;
}

// Desenha a porta de saida
int create_door(Object **map, ALLEGRO_BITMAP *texture[10], Miner *m, Level level)
{
    int x,y;
    x = m->p.x;
    y = m->p.y;


    if(map[y+1][x].ObjectID == EMPTY || map[y+1][x].ObjectID == EARTH)
    {
        map[y+1][x].ObjectID = DOOR;
        map[y+1][x].image = texture[DOOR];
    }
    else if(map[y][x+1].ObjectID == EMPTY || map[y][x+1].ObjectID == EARTH)
    {
        map[y][x+1].ObjectID = DOOR;
        map[y][x+1].image = texture[DOOR];
    }
     else if(map[y-1][x].ObjectID == EMPTY || map[y-1][x].ObjectID == EARTH)
    {
        map[y-1][x].ObjectID = DOOR;
        map[y-1][x].image = texture[DOOR];
    }
     else if(map[y][x-1].ObjectID == EMPTY || map[y][x-1].ObjectID == EARTH)
    {
        map[y][x-1].ObjectID = DOOR;
        map[y][x-1].image = texture[DOOR];
    }

    return 1;
}

// Muda o nivel atual para o proximo nível
void change_level(Level *curr_level, Level *next_level)
{
    curr_level->row = next_level->row;
    curr_level->col = next_level->col;
    curr_level->time = next_level->time;
    curr_level->total_diamond = next_level->total_diamond;
    curr_level->x_radius = next_level->x_radius;
    curr_level->y_radius = next_level->y_radius;
    curr_level->number = next_level->number;
    strcpy(curr_level->file_name, next_level->file_name);
}

// Inicia o nível
Level* init_level()
{
    struct dirent** dirent = NULL;
    Level* level;
    FILE *fp;
    int numLevel;
    char *levelpath;
    int i;

    numLevel = scandir(PATH, &dirent, NULL, alphasort);

    level  = malloc(numLevel * sizeof(level));
    validade_alocation(level);

    levelpath  = malloc(WORDSIZE * sizeof(char));
    validade_alocation(levelpath);

    for(i=2; i< numLevel; i++){
        strcpy(levelpath, PATH);

        strcat(levelpath, dirent[i]->d_name);

        strcpy(level[i-2].file_name, levelpath);

        free(dirent[i]);
    }

    for(int i=0; i<(numLevel - 2);i++)
    {
        fp = fopen(level[i].file_name, "r");
        validade_file(fp);
        
        level[i].number = i+1;
        fscanf(fp,"%d\n%d %d\n", &level[i].total_diamond, &level[i].row, &level[i].col);
        fscanf(fp,"%d %f %f", &level[i].time, &level[i].x_radius, &level[i].y_radius);

        fclose(fp);
    }
    
    return level;
}

// Le a pontuação salva em disco e atualiza, caso necessário
void get_score(int score, int* top_scores){
    int i, j;

    FILE* fp = fopen (SCOREPATH, "r");
    validade_file(fp);


    for (i=0; i<5; i++)
    {
        fscanf (fp, "%d\n", &top_scores[i]);
    }

    if(score > top_scores[4]){
        while (score > top_scores[i] && i > 0)
        {
            i--;
        }

        for(j = 4; j > i - 1;j--){
            top_scores[j + 1] = top_scores[j];
        }

        top_scores[i] = score;

        fp = fopen (SCOREPATH, "w");
        validade_file(fp);

        for (i=0; i<5; i++)
        {
            fprintf(fp, "%d\n", top_scores[i]);
        }
    }

    fclose (fp) ;
}