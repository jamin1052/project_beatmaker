// src/main.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "defs.h"
#include "init.h"
#include "draw.h"
#include "action.h"
#include "input.h"
#include "scene_title.h"
#include "map.h"
#include "scene_story.h"
#include "player_death.h"


Mix_Music *bgm;
extern Mix_Chunk *death_effect;


GameState game_state = STATE_TITLE;

int main(void) {
    InitMemorySet(); 
    InitSDL();
    LoadRoom(0, 0);     // ★ 먼저 방을 로드해야 함
    InitPlayer();
    title_init();    
    
    for (;;) {
        ClearWindow();

        if (game_state == STATE_ENDING) 
        {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT)
                {
                    game_state = STATE_EXIT;
                }
                if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                {
                    game_state = STATE_EXIT;
                }
            }
        }
        else GetInput();

        switch (game_state) 
        {
            case STATE_TITLE: title_update(); title_render(); break;
            case STATE_STORY: story_update(); story_render(); break;
            case STATE_GAME: ActGame(); DrawGame(); break;
            case STATE_GAMEOVER: ActGameOver(); DrawGameOver(); break;
            case STATE_ENDING:
                DrawEnding();
                break;
            case STATE_EXIT: QuitSDL(); exit(0);
        }

        ShowWindow();
        SDL_Delay(16);
    }
    return 0;
}

void InitGameOver(void) {}
void InitBullet(void) {}
void InitScoreBoard(void) {}
void InitSound(void) {}
void LoadSound(void) {}
void PlayBGM(void) {}
void InitTTF(void) {}