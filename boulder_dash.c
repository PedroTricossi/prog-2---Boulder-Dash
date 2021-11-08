#include <time.h>
#include <stdio.h>
#include <time.h>
#include <stdio.h>
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

ALLEGRO_BITMAP *load_bitmap_at_size(const char *filename, int w, int h)
{
  ALLEGRO_BITMAP *resized_bmp, *loaded_bmp, *prev_target;

  // 1. create a temporary bitmap of size we want
  resized_bmp = al_create_bitmap(w, h);
  if (!resized_bmp) return NULL;
  // 2. load the bitmap at the original size
  loaded_bmp = al_load_bitmap(filename);
  if (!loaded_bmp)
  {
     al_destroy_bitmap(resized_bmp);
     return NULL;
  }

  // 3. set the target bitmap to the resized bmp
  prev_target = al_get_target_bitmap();
  al_set_target_bitmap(resized_bmp);

  // 4. copy the loaded bitmap to the resized bmp
  al_draw_scaled_bitmap(loaded_bmp,
     0, 0,                                // source origin
     al_get_bitmap_width(loaded_bmp),     // source width
     al_get_bitmap_height(loaded_bmp),    // source height
     0, 0,                                // target origin
     w, h,                                // target dimensions
     0                                    // flags
  );

  // 5. restore the previous target and clean up
  al_set_target_bitmap(prev_target);
  al_destroy_bitmap(loaded_bmp);

  return resized_bmp;
}


void update_miner(Object **map, ALLEGRO_BITMAP *texture[10], ALLEGRO_SAMPLE_INSTANCE *walk_empty,ALLEGRO_SAMPLE_INSTANCE *walk_earth, ALLEGRO_SAMPLE_INSTANCE *boulder, ALLEGRO_SAMPLE_INSTANCE *collect_diamond, Miner *m, FallingObject *rock, FallingObject *diamond, int r_num, int d_num, int move, int *result)
{
    Object *ob_up, *ob_down, *ob_right, *ob_left;

    /* shortcut: nicknames :) */
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

void init_map(Object **map, ALLEGRO_BITMAP *texture[10], int r, int c, char *file_name)
{
    FILE *fp = fopen("resources/levels/LEVEL_2.txt", "r");
    char s;

    do
    {
        s = fgetc(fp);
    }while(s != '-'); /* map stars flag */

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
                case 'S': map[i][j].ObjectID = SPIDER; map[i][j].image = texture[SPIDER];    break;
                case '!': map[i][j].ObjectID = MONSTER; map[i][j].image = texture[MONSTER];  break;
                case 'W': map[i][j].ObjectID = WATER; map[i][j].image = texture[WATER];      break;
                case 'D': map[i][j].ObjectID = DOOR; map[i][j].image = texture[DOOR];        break;
                default: j--;
            }
        }
    }

    fclose(fp);

}

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

void add_diamond(FallingObject *diamond, ALLEGRO_BITMAP *texture[10], int index, int x, int y)
{
    diamond[index].alive = true;
    diamond[index].image = texture[DIAMOND];
    diamond[index].is_falling= false;
    diamond[index].ObjectID = DIAMOND;
    diamond[index].p.x = x;
    diamond[index].p.y = y;
}

void update_rock(Object **map, ALLEGRO_BITMAP *texture[10], ALLEGRO_SAMPLE_INSTANCE *boulder, int row, int col, FallingObject *rock, int r_num, Miner *m, FallingObject *diamond, int *d_num, MovingObject *spider, int s_num, MovingObject *monster, int m_num)
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
                else if(map[y+1][x].ObjectID == SPIDER || map[y+1][x].ObjectID == MONSTER)
                {
                    rock[i].alive = false;
                    
                    if(map[y+1][x].ObjectID == MONSTER)
                    {

                        for(int k=0; k<m_num; k++)
                        {
                            if(monster[k].p.x == x && monster[k].p.y == y+1)
                                monster[k].alive = false;
                        }
                        if(y>2)
                        {
                           if(map[y-2][x].ObjectID == EARTH || map[y-2][x].ObjectID == EMPTY)
                            {
                                map[y-2][x].ObjectID = DIAMOND;
                                map[y-2][x].image = texture[DIAMOND];
                                add_diamond(diamond, texture, j++, x,y-2);
                            }
                        }

                        if(x<col-2)
                        {
                            if(map[y+1][x+2].ObjectID == EARTH || map[y+1][x+2].ObjectID == EMPTY)
                            {
                                map[y+1][x+2].ObjectID = DIAMOND;
                                map[y+1][x+2].image = texture[DIAMOND];
                                add_diamond(diamond, texture, j++, x+2,y+1);
                            }
                        }

                        if(x>2)
                        {
                            if(map[y+1][x-2].ObjectID == EARTH || map[y+1][x-2].ObjectID == EMPTY)
                            {
                                map[y+1][x-2].ObjectID = DIAMOND;
                                map[y+1][x-2].image = texture[DIAMOND];
                                add_diamond(diamond, texture, j++, x-2,y+1);
                            }
                        }
                        if(y<row-2)
                        {
                           if(map[y+2][x].ObjectID == EARTH || map[y+2][x].ObjectID == EMPTY)
                            {
                                map[y+2][x].ObjectID = DIAMOND;
                                map[y+2][x].image = texture[DIAMOND];
                                add_diamond(diamond, texture, j++, x,y+2);
                            }
                        }
                    }

                    else
                    {

                        for(int k=0; k<s_num; k++)
                        {
                            if(spider[k].p.x == x && spider[k].p.y == y+1)
                                spider[k].alive = false;
                        }

                    }


                    map[y+1][x].ObjectID = DIAMOND;
                    map[y+1][x].image = texture[DIAMOND];
                    add_diamond(diamond, texture, j++, x,y+1);

                    map[y][x].ObjectID = DIAMOND;
                    map[y][x].image = texture[DIAMOND];
                    add_diamond(diamond, texture, j++, x,y);


                    if(map[y][x+1].ObjectID == EARTH || map[y][x+1].ObjectID == EMPTY)
                    {
                        map[y][x+1].ObjectID = DIAMOND;
                        map[y][x+1].image = texture[DIAMOND];
                        add_diamond(diamond, texture, j++, x+1,y);
                    }
                    if(map[y][x-1].ObjectID == EARTH || map[y][x-1].ObjectID == EMPTY)
                    {
                        map[y][x-1].ObjectID = DIAMOND;
                        map[y][x-1].image = texture[DIAMOND];
                        add_diamond(diamond, texture, j++, x-1,y);
                    }
                    if(map[y+1][x+1].ObjectID == EARTH || map[y+1][x+1].ObjectID == EMPTY)
                    {
                        map[y+1][x+1].ObjectID = DIAMOND;
                        map[y+1][x+1].image = texture[DIAMOND];
                        add_diamond(diamond, texture, j++, x+1,y+1);
                    }
                    if(map[y+1][x-1].ObjectID == EARTH || map[y+1][x-1].ObjectID == EMPTY)
                    {
                        map[y+1][x-1].ObjectID = DIAMOND;
                        map[y+1][x-1].image = texture[DIAMOND];
                        add_diamond(diamond, texture, j++, x-1,y+1);
                    }

                    if(y+2<row)
                    {
                        if(map[y+2][x+1].ObjectID == EARTH || map[y+2][x+1].ObjectID == EMPTY)
                        {
                            map[y+2][x+1].ObjectID = DIAMOND;
                            map[y+2][x+1].image = texture[DIAMOND];
                            add_diamond(diamond, texture, j++, x+1,y+2);
                        }
                        if(map[y+2][x-1].ObjectID == EARTH || map[y+2][x-1].ObjectID == EMPTY)
                        {
                            map[y+2][x-1].ObjectID = DIAMOND;
                            map[y+2][x-1].image = texture[DIAMOND];
                            add_diamond(diamond, texture, j++, x-1,y+2);
                        }
                        if(map[y+2][x].ObjectID == EARTH || map[y+2][x].ObjectID == EMPTY)
                        {
                            map[y+2][x].ObjectID = DIAMOND;
                            map[y+2][x].image = texture[DIAMOND];
                            add_diamond(diamond, texture, j++, x,y+2);
                        } 
                    }
                    

                    *d_num = j;


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

void update_spider(Object **map, ALLEGRO_BITMAP *texture[10], int row, int col, MovingObject *spider, int s_num, Miner *m)
{
    int r = rand(); /* random integer for left or top */
    int x, y;

    for(int i=0; i<s_num; i++)
    {
        x = spider[i].p.x;
        y = spider[i].p.y;

        if(spider[i].alive)
        {
                if(map[y][x+1].ObjectID == EMPTY || map[y][x+1].ObjectID == MINER)
           {
                if(map[y][x+1].ObjectID == MINER) m->alive = false;
                map[y][x].ObjectID = EMPTY;
                map[y][x].image = texture[EMPTY];
                map[y][x+1].ObjectID = SPIDER;
                map[y][x+1].image = texture[SPIDER];
                spider[i].p.x = x+1;
           }
           else if(map[y+1][x].ObjectID == EMPTY || map[y+1][x].ObjectID == MINER)
           {
                if(map[y+1][x].ObjectID == MINER) m->alive = false;
                map[y][x].ObjectID = EMPTY;
                map[y][x].image = texture[EMPTY];
                map[y+1][x].ObjectID = SPIDER;
                map[y+1][x].image = texture[SPIDER];
                spider[i].p.y = y+1;

           }
           else if(r%2 == 0)
           {
               if(map[y][x-1].ObjectID == EMPTY || map[y][x-1].ObjectID == MINER)
               {
                    if(map[y][x-1].ObjectID == MINER) m->alive = false;
                    map[y][x].ObjectID = EMPTY;
                    map[y][x].image = texture[EMPTY];
                    map[y][x-1].ObjectID = SPIDER;
                    map[y][x-1].image = texture[SPIDER];
                    spider[i].p.x = x-1;
               }
           }
           else
           {
               if(map[y-1][x].ObjectID == EMPTY || map[y-1][x].ObjectID == MINER)
               {
                    if(map[y-1][x].ObjectID == MINER) m->alive = false;
                    map[y][x].ObjectID = EMPTY;
                    map[y][x].image = texture[EMPTY];
                    map[y-1][x].ObjectID = SPIDER;
                    map[y-1][x].image = texture[SPIDER];
                    spider[i].p.y = y-1;
               }
           }
        }



    }


}


void update_monster(Object **map, ALLEGRO_BITMAP *texture[10], int row, int col, Miner *m, MovingObject *monster, int m_num)
{
    int x, y, diff_x, diff_y;
    bool is_moved = false;


    for(int i=0; i<m_num; i++)
    {
        is_moved = false;
        x = monster[i].p.x;
        y = monster[i].p.y;

        diff_y = m->p.y - y;
        diff_x = m->p.x - x;

        if(monster[i].alive)
        {
            if(diff_y<0) /* monster is under the miner */
            {
               if(map[y-1][x].ObjectID == EMPTY || map[y-1][x].ObjectID == MINER)
               {
                    if(map[y-1][x].ObjectID == MINER) m->alive = false;
                    is_moved = true;
                    map[y][x].ObjectID = EMPTY;
                    map[y][x].image = texture[EMPTY];
                    map[y-1][x].ObjectID = MONSTER;
                    map[y-1][x].image = texture[MONSTER];
                    monster[i].p.y = y-1;
               }
            }
            if(!is_moved && diff_y>0)
            {
                if(map[y+1][x].ObjectID == EMPTY || map[y+1][x].ObjectID == MINER)
               {
                    if(map[y+1][x].ObjectID == MINER) m->alive = false;
                    is_moved = true;
                    map[y][x].ObjectID = EMPTY;
                    map[y][x].image = texture[EMPTY];
                    map[y+1][x].ObjectID = MONSTER;
                    map[y+1][x].image = texture[MONSTER];
                    monster[i].p.y = y+1;
               }
            }
            if(!is_moved && diff_x<0) /* miner is is at left */
            {
                if(map[y][x-1].ObjectID == EMPTY || map[y][x-1].ObjectID == MINER)
                {
                    if(map[y][x-1].ObjectID == MINER) m->alive = false;
                    is_moved = true;
                    map[y][x].ObjectID = EMPTY;
                    map[y][x].image = texture[EMPTY];
                    map[y][x-1].ObjectID = MONSTER;
                    map[y][x-1].image = texture[MONSTER];
                    monster[i].p.x = x-1;
                }
            }
            if(!is_moved && diff_x>0)
            {
                if(map[y][x+1].ObjectID == EMPTY || map[y][x+1].ObjectID == MINER)
                {
                    if(map[y][x+1].ObjectID == MINER) m->alive = false;
                    map[y][x].ObjectID = EMPTY;
                    map[y][x].image = texture[EMPTY];
                    map[y][x+1].ObjectID = MONSTER;
                    map[y][x+1].image = texture[MONSTER];
                    monster[i].p.x = x+1;
                }
            }
        }

        
    }


}

void update_water(Object **map, ALLEGRO_BITMAP *texture[10], MovingObject *water, int *water_len)
{
    int f_water_len = *water_len; /* first water length for not to update screen immediately */

    for(int i=0; i<f_water_len; i++)
    {
        if(map[water[i].p.y-1][water[i].p.x].ObjectID == EARTH)
        {
            (*water_len)++;
            map[water[i].p.y-1][water[i].p.x].ObjectID = WATER;
            map[water[i].p.y-1][water[i].p.x].image = texture[WATER];
            water[*water_len-1].ObjectID = WATER;
            water[*water_len-1].image = texture[WATER];
            water[*water_len-1].p.x = water[i].p.x;
            water[*water_len-1].p.y = water[i].p.y-1;
            water[*water_len-1].speed = water[i].speed;
        }
        if(map[water[i].p.y+1][water[i].p.x].ObjectID == EARTH)
        {
            (*water_len)++;
            map[water[i].p.y+1][water[i].p.x].ObjectID = WATER;
            map[water[i].p.y+1][water[i].p.x].image = texture[WATER];
            water[*water_len-1].ObjectID = WATER;
            water[*water_len-1].image = texture[WATER];
            water[*water_len-1].p.x = water[i].p.x;
            water[*water_len-1].p.y = water[i].p.y+1;
            water[*water_len-1].speed = water[i].speed;
        }

        if(map[water[i].p.y][water[i].p.x-1].ObjectID == EARTH)
        {
            (*water_len)++;
            map[water[i].p.y][water[i].p.x-1].ObjectID = WATER;
            map[water[i].p.y][water[i].p.x-1].image = texture[WATER];
            water[*water_len-1].ObjectID = WATER;
            water[*water_len-1].image = texture[WATER];
            water[*water_len-1].p.x = water[i].p.x-1;
            water[*water_len-1].p.y = water[i].p.y;
            water[*water_len-1].speed = water[i].speed;
        }

        if(map[water[i].p.y][water[i].p.x+1].ObjectID == EARTH)
        {
            (*water_len)++;
            map[water[i].p.y][water[i].p.x+1].ObjectID = WATER;
            map[water[i].p.y][water[i].p.x+1].image = texture[WATER];
            water[*water_len-1].ObjectID = WATER;
            water[*water_len-1].image = texture[WATER];
            water[*water_len-1].p.x = water[i].p.x+1;
            water[*water_len-1].p.y = water[i].p.y;
            water[*water_len-1].speed = water[i].speed;
        }


    }
}

void find_objects_len(Object **map, int row, int col, int *s_num, int *m_num, int *w_num, int *r_num, int *d_num)
{
    /* reinitialization */
    *s_num = 0;
    *m_num = 0;
    *w_num = 0;
    *r_num = 0;
    *d_num = 0;

    for(int y=0; y<row-1; y++)
    {
        for(int x=0; x<col; x++)
        {
            switch(map[y][x].ObjectID)
            {
                case SPIDER: (*s_num)++; break;
                case MONSTER: (*m_num)++; break;
                case WATER: (*w_num)++; break;
                case ROCK: (*r_num)++; break;
                case DIAMOND: (*d_num)++; break;
            }
        }
    }

}

void init_object(Object **map, ALLEGRO_BITMAP *texture[10], int row, int col, Miner *m, MovingObject *w_list, MovingObject *m_list, MovingObject *s_list, FallingObject *r_list, FallingObject *d_list)
{

    int w_i=0, m_i=0, s_i=0, r_i=0, d_i=0;

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
                    case SPIDER:
                        s_list[s_i].ObjectID = SPIDER;
                        s_list[s_i].image = texture[SPIDER];
                        s_list[s_i].speed = SIZE;
                        s_list[s_i].alive = true;
                        s_list[s_i].p.x = x;
                        s_list[s_i++].p.y = y;
                        break;
                    case MONSTER:
                        m_list[m_i].ObjectID = MONSTER;
                        m_list[m_i].image = texture[MONSTER];
                        m_list[m_i].speed = SIZE;
                        m_list[m_i].alive = true;
                        m_list[m_i].p.x = x;
                        m_list[m_i++].p.y = y;
                        break;
                    case WATER:
                        w_list[w_i].ObjectID = WATER;
                        w_list[w_i].image = texture[WATER];
                        w_list[w_i].speed = SIZE;
                        w_list[w_i].p.x = x;
                        w_list[w_i++].p.y = y;
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

int is_over(Object **map, int row, int col, Miner *m, int time_left, Level level)
{
    int result=0;

    if(check_dead(m) || time_left==0) result = -1;

    else
    {
        for(int y=0; y<col; y++)
        {
            for(int x=0; x<row; x++)
            {
                 if(map[y][x].ObjectID == DOOR && m->p.x == x && m->p.y == y) result = 1;
                 return result;
            }
        }

    }

    return result;
}

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

void change_level(Level *curr_level, const Level next_level)
{
    curr_level->row = next_level.row;
    curr_level->col = next_level.col;
    curr_level->time = next_level.time;
    curr_level->total_diamond = next_level.total_diamond;
    curr_level->x_radius = next_level.x_radius;
    curr_level->y_radius = next_level.y_radius;
    curr_level->number = next_level.number;
    strcpy(curr_level->file_name, next_level.file_name);
}

void init_level(Level level[10])
{
    strcpy(level[0].file_name, "resources/levels/LEVEL_2.txt");
    strcpy(level[1].file_name, "resources/levels/LEVEL_2.txt");
    strcpy(level[2].file_name, "resources/levels/LEVEL_3.txt");
    strcpy(level[3].file_name, "resources/levels/LEVEL_4.txt");
    strcpy(level[4].file_name, "resources/levels/LEVEL_5.txt");
    strcpy(level[5].file_name, "resources/levels/LEVEL_6.txt");
    strcpy(level[6].file_name, "resources/levels/LEVEL_7.txt");
    strcpy(level[7].file_name, "resources/levels/LEVEL_8.txt");
    strcpy(level[8].file_name, "resources/levels/LEVEL_9.txt");
    strcpy(level[9].file_name, "resources/levels/LEVEL_10.txt");

    FILE *fp;



    for(int i=0; i<10;i++)
    {
        fp = fopen(level[i].file_name, "r");
        level[i].number = i+1;
        fscanf(fp,"%d\n%d %d\n", &level[i].total_diamond, &level[i].row, &level[i].col);
        fscanf(fp,"%d %f %f", &level[i].time, &level[i].x_radius, &level[i].y_radius);
        fclose(fp);
    }


}
