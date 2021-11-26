#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <time.h>
#include <stdio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>
#include <allegro5/keyboard.h>
#include <allegro5/timer.h>
#include <allegro5/color.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "boulder_dash.h"


int main()
{
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_TIMER *miner_timer, *rock_timer, *level_timer;
    ALLEGRO_FONT *score_text, *pause_text;
    ALLEGRO_TRANSFORM camera;
    ALLEGRO_BITMAP *texture_icon_diamond;
    ALLEGRO_BITMAP *texture[8];
    ALLEGRO_SAMPLE *sound_walk_earth, *sound_walk_empty, *sound_boulder, *sound_diamond, *sound_melody, *sound_start;
    ALLEGRO_SAMPLE_INSTANCE *walk_empty, *walk_earth, *boulder, *collect_diamond, *music, *starting_music;
    FallingObject *rock, *diamond;
    Level *level;
    Miner miner;
    Position camera_position;

    bool doexit = false;
    bool redraw = true;
    bool keys[4] = {false,false,false, false};

    int door = 0;
    int pressed = 0;
    int result = 0; 
    int score = 0;
    int game_state=PAUSE;
    int top_scores[6];

    int row, col;
    int time_left;
    int r_num=0, d_num=0;
    int i;

    Level curr_level;
    int lvl_i=0; 
    
    /* random */
    srand(time(NULL));

    /*initialization */
    al_init();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    //al_init_primitives_addon();
    al_install_keyboard();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(20);
   
    /* Objects */

    level = init_level();

    /* graphics */
    texture[EMPTY] = load_bitmap_at_size("resources/textures/Empty.png",SIZE, SIZE);
    texture[EARTH] = load_bitmap_at_size("resources/textures/sand.jpg", SIZE, SIZE);
    texture[MINER] = load_bitmap_at_size("resources/textures/Miner.png", SIZE, SIZE);
    texture[ROCK] = load_bitmap_at_size("resources/textures/Rock.png", SIZE, SIZE);
    texture[BORDER] = load_bitmap_at_size("resources/textures/wood.jpg", SIZE, SIZE);
    texture[DIAMOND] = load_bitmap_at_size("resources/textures/Diamond.png", SIZE,SIZE);
    texture[DOOR] = load_bitmap_at_size("resources/textures/Door.png", SIZE, SIZE);
    texture_icon_diamond = load_bitmap_at_size("resources/textures/Diamond.png", 3*SIZE/5, 3*SIZE/5);

    /* Sound samples, instances; basicly all necessary loading stuff */
    
    sound_walk_earth = al_load_sample("resources/sounds/walk_earth.ogg");
    sound_walk_empty = al_load_sample("resources/sounds/walk_empty.ogg");
    sound_boulder = al_load_sample("resources/sounds/boulder.ogg");
    sound_diamond = al_load_sample("resources/sounds/collect_diamond.ogg");
    sound_melody = al_load_sample("resources/sounds/melody.ogg");
    sound_start = al_load_sample("resources/sounds/bdmusic.ogg");

    walk_empty = al_create_sample_instance(sound_walk_empty);
    walk_earth = al_create_sample_instance(sound_walk_earth);
    boulder = al_create_sample_instance(sound_boulder);
    collect_diamond = al_create_sample_instance(sound_diamond);
    music = al_create_sample_instance(sound_melody);
    starting_music = al_create_sample_instance(sound_start);

    al_set_sample_instance_playmode(music, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_playmode(starting_music, ALLEGRO_PLAYMODE_LOOP);
    al_attach_sample_instance_to_mixer(walk_empty,al_get_default_mixer());
    al_attach_sample_instance_to_mixer(walk_earth,al_get_default_mixer());
    al_attach_sample_instance_to_mixer(boulder,al_get_default_mixer());
    al_attach_sample_instance_to_mixer(collect_diamond,al_get_default_mixer());
    al_attach_sample_instance_to_mixer(music,al_get_default_mixer());
    al_attach_sample_instance_to_mixer(starting_music, al_get_default_mixer());

    change_level(&curr_level, level); /* starting from level 1 */

    row = curr_level.row;
    col = curr_level.col;

    Object **map = (Object **)malloc(row * sizeof(Object*));
    for (int i=0; i<row; i++)
         map[i] = (Object *)malloc(col * sizeof(Object));

    time_left = curr_level.time;
    
    init_map(map, texture, row,col, curr_level.file_name);

    find_objects_len(map,row,col, &r_num, &d_num);


    rock = (FallingObject*)malloc(sizeof(FallingObject)*r_num);
    
    diamond = (FallingObject*)malloc((sizeof(FallingObject))*d_num);


    init_object(map, texture, row, col, &miner, rock, diamond);


    /* font loading */
    score_text = al_load_font("resources/fonts/zig.ttf", 27, 0);
    pause_text = al_load_font("resources/fonts/zig.ttf", 42, 0);


    /* event queue for event handling and timers init. */
    event_queue = al_create_event_queue();
    miner_timer = al_create_timer(1.0 / (FPS / 7.5));
    rock_timer = al_create_timer(1.0 / (FPS / 6.0));
    level_timer = al_create_timer(1.0);


    /* created the big picture */
    display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);

    /* adding inputs, timers and outputs to event queue to handle them later */
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(miner_timer));
    al_register_event_source(event_queue, al_get_timer_event_source(rock_timer));
    al_register_event_source(event_queue, al_get_timer_event_source(level_timer));

    /* cleared the screen before we start */
    al_clear_to_color(al_map_rgb(0,0,0));
    al_flip_display();

    /* started the times */
    al_start_timer(miner_timer);
    al_start_timer(rock_timer);
    al_start_timer(level_timer);
    ALLEGRO_EVENT ev;


    /* starting menu, not a quite menu but prevents game from starting suddenly and credits the author */ 
    al_play_sample_instance(starting_music);
    while(game_state != PLAY)
    {
        al_wait_for_event(event_queue,&ev);
        al_draw_textf(pause_text, al_map_rgb(147,81,178), SCREEN_WIDTH/2, SCREEN_HEIGHT/2, ALLEGRO_ALIGN_CENTER, "PRESS SPACE TO PLAY.");
        al_flip_display();
        al_clear_to_color(al_map_rgb(0,0,0));

        if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if(ev.keyboard.keycode == ALLEGRO_KEY_SPACE) game_state = PLAY;
        }
    }
    al_stop_sample_instance(starting_music);


    while(!doexit)
    {
        al_wait_for_event(event_queue,&ev);
        result = is_over(map, row, col, &miner, time_left, curr_level);

        if(game_state == PLAY && ev.timer.source == miner_timer)
        {
            redraw = true;
            if(keys[UP]) update_miner(map, texture, walk_empty, walk_earth, boulder, collect_diamond, &miner, rock, diamond, r_num, d_num, UP, &result);
            else if(keys[DOWN]) update_miner(map, texture, walk_empty, walk_earth, boulder, collect_diamond, &miner,rock, diamond, r_num, d_num, DOWN,&result);
            else if(keys[RIGHT]) update_miner(map, texture, walk_empty, walk_earth, boulder, collect_diamond, &miner, rock, diamond, r_num, d_num, RIGHT,&result);
            else if(keys[LEFT]) update_miner(map, texture, walk_empty, walk_earth, boulder, collect_diamond, &miner, rock, diamond, r_num, d_num, LEFT,&result);

            camera_update(&camera_position, miner.p.x, miner.p.y, (int)SIZE, (int)SIZE, row, col, curr_level);
            al_identity_transform(&camera);
            al_translate_transform(&camera, (float)-camera_position.x, (float)-camera_position.y);
            al_use_transform(&camera);
        }

        if(game_state == PLAY && ev.timer.source == rock_timer)
        {
            redraw = true;
            update_rock(map, texture, boulder, row, col, rock, r_num, &miner, diamond, &d_num);
            update_rock(map, texture, boulder, row, col, diamond, d_num, &miner, diamond, &d_num); /* diamond can free fall too */

        }
        if(game_state == PLAY && ev.timer.source == level_timer) time_left--;

        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            doexit = true;
        
        else if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            switch(ev.keyboard.keycode)
            {
                case ALLEGRO_KEY_UP:
                    keys[UP] = true;
                    pressed++;
                    break;
                case ALLEGRO_KEY_DOWN:
                    keys[DOWN]= true;
                    pressed++;  
                    break;

                case ALLEGRO_KEY_RIGHT:
                    keys[RIGHT]= true;
                    pressed++;
                    break;

                case ALLEGRO_KEY_LEFT:
                    keys[LEFT]= true;
                    pressed++;
                    break;

                case ALLEGRO_KEY_ESCAPE:
                    game_state = PAUSE;
                    break;

                case ALLEGRO_KEY_SPACE:
                    game_state = PLAY;
                    break;
                
                case ALLEGRO_KEY_H:
                    game_state = HELP;
                    break;
            }
        }

        else if(ev.type == ALLEGRO_EVENT_KEY_UP)
        {
            switch(ev.keyboard.keycode)
            {
                case ALLEGRO_KEY_UP:    keys[UP] = false;    break;
                case ALLEGRO_KEY_DOWN:  keys[DOWN]= false;   break;
                case ALLEGRO_KEY_RIGHT: keys[RIGHT]= false;  break;
                case ALLEGRO_KEY_LEFT: keys[LEFT]= false;    break;
            }

        }

        if(game_state == PAUSE)
        {
            al_draw_textf(pause_text, al_map_rgb(255,223,0), SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 150, ALLEGRO_ALIGN_CENTER, "GAME PAUSED.");
            al_draw_textf(pause_text, al_map_rgb(255,223,0), SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 150, ALLEGRO_ALIGN_CENTER, "PRESS SPACE TO PLAY.");
            al_flip_display();
            al_clear_to_color(al_map_rgb(0,0,0));
        }

        if(game_state == HELP)
        {
            al_draw_textf(pause_text, al_map_rgb(136, 56, 255), SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 342, ALLEGRO_ALIGN_CENTER, "TELA DE AJUDA");
            al_draw_textf(score_text, al_map_rgb(136, 56, 255), SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 250, ALLEGRO_ALIGN_CENTER, "BEM-VINDO AO BOULDER DASH!!! ");
            al_draw_textf(score_text, al_map_rgb(136, 56, 255), SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 120, ALLEGRO_ALIGN_CENTER, "SEU OBJETIVO É PEGAR TODOS OS DIAMANTES, SEM MORRER.");
            al_draw_textf(score_text, al_map_rgb(136, 56, 255), SCREEN_WIDTH/2, SCREEN_HEIGHT/2 , ALLEGRO_ALIGN_CENTER, "VOCÊ PODE USAR AS SETAS DO TECLADO PARA SE MOVER");
            al_draw_textf(score_text, al_map_rgb(136, 56, 255), SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 30, ALLEGRO_ALIGN_CENTER, "'ESC' PARA PAUSAR.");

            al_draw_textf(score_text, al_map_rgb(136, 56, 255), SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 280, ALLEGRO_ALIGN_CENTER, "DESENVOLVIDO POR:");
            al_draw_textf(score_text, al_map_rgb(136, 56, 255), SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 310, ALLEGRO_ALIGN_CENTER, "PEDRO DOMINGOS TRICOSSI DOS SANTOS");
            al_draw_textf(score_text, al_map_rgb(136, 56, 255), SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 342, ALLEGRO_ALIGN_CENTER, "@UFPR - 2021/PROGRAMAÇÃO 2");
            al_flip_display();
            al_clear_to_color(al_map_rgb(0,0,0));
        }
        


        if(game_state == PLAY && redraw && al_is_event_queue_empty(event_queue))
        {
            redraw = false;
            check_dead(&miner);
            draw_map(map,row,col,miner);
            
            if(!door && (curr_level.time - 5) > time_left && pressed < 2 && lvl_i == 0)
                door = create_door(map, texture, &miner, curr_level);

            if(!door && miner.diamond == curr_level.total_diamond) 
                door = create_door(map, texture, &miner, curr_level);

            draw_score(score_text, texture, texture_icon_diamond, music, &miner, time_left, curr_level, camera_position);
            al_set_sample_instance_speed(music, 1+10/(time_left+1));
            al_flip_display();
            al_clear_to_color(al_map_rgb(0,0,0));

            if(result == 1 || result == -1)
            {
                fprintf(stderr, "entrou resultado \n");
                if(result == -1)
                {
                    score -= miner.diamond*10;
                    lvl_i--; /* not changing */
                }

                else if(pressed > 3)
                {
                    fprintf(stderr, "entrou pressed\n");
                    doexit = true;
                    break;
                }

                else
                    miner.score += (time_left*20)+(miner.life*10);
                
                fprintf(stderr, "entrou EG \n");

                score = miner.score;

                level++;
                change_level(&curr_level, level);

                row = curr_level.row;
                col = curr_level.col;
                time_left = curr_level.time;
                init_map(map, texture, row,col, curr_level.file_name);
                find_objects_len(map,row,col,&r_num, &d_num);

                free(rock);
                free(diamond);

                rock = (FallingObject*)malloc(sizeof(FallingObject)*r_num);
                diamond = (FallingObject*)malloc((sizeof(FallingObject))*(d_num));
                
                init_object(map, texture, row, col, &miner, rock, diamond);
                
                miner.score = score;
                miner.life=3+lvl_i;
                result = 0;
                door = 0;
            }
        }
    }


    get_score(miner.score, top_scores);
    doexit = false;
    while(!doexit)
    {
        al_clear_to_color(al_map_rgb(42,42,42));
        al_draw_textf(pause_text, al_map_rgb(172,84,242), SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 342, ALLEGRO_ALIGN_CENTER, "THANKS FOR PLAYING.");
        al_draw_textf(pause_text, al_map_rgb(104,94,242), SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 280, ALLEGRO_ALIGN_CENTER, "YOUR SCORE: %.6d", miner.score);

        al_draw_textf(pause_text, al_map_rgb(136,56,255), SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 150, ALLEGRO_ALIGN_CENTER, "TOP SCORES:");

        for(i = 0; i<5 ;i++){
            int r = 136 + 23*i;
            int g = 150 - 15*i;
            int b = 255 - 25*i;

            al_draw_textf(pause_text, al_map_rgb(r, g, b), SCREEN_WIDTH/2, SCREEN_HEIGHT/2 - 80 + (60 * i), ALLEGRO_ALIGN_CENTER,"%.6d", top_scores[i]);
        }

        al_flip_display();
    }
    
    /* all destroy stuff */
    al_destroy_display(display);
    al_destroy_bitmap(texture[BORDER]);
    al_destroy_bitmap(texture[DIAMOND]);
    al_destroy_bitmap(texture[EARTH]);
    al_destroy_bitmap(texture[EMPTY]);
    al_destroy_bitmap(texture[MINER]);
    al_destroy_timer(miner_timer);
    al_destroy_timer(level_timer);
    al_destroy_timer(rock_timer);
    al_destroy_event_queue(event_queue);
    al_destroy_sample(sound_boulder);
    al_destroy_sample(sound_diamond);
    al_destroy_sample(sound_walk_earth);
    al_destroy_sample(sound_walk_empty);
    al_destroy_sample(sound_start);
    al_destroy_sample_instance(walk_earth);
    al_destroy_sample_instance(walk_empty);
    al_destroy_sample_instance(collect_diamond);
    al_destroy_sample_instance(boulder);
    al_destroy_sample_instance(music);
    al_destroy_sample_instance(starting_music);

    free(map);
    free(rock);
    free(diamond);

    return 0;

}