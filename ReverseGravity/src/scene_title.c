#include "scene_title.h"
#include "defs.h"
#include "init.h"
#include "draw.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


static const char* story_text =
"드디어 컴퓨터 내부에 들어왔어.\n"
"백신들을 뚫고 컴퓨터를 해킹하자!";

static const char* controls_text =
"[이동] ← →\n"
"[점프] ↓\n"
"[중력 반전] SPACE\n"
"[리셋] R";


extern App app;
extern TTF_Font* font_normal;
extern TTF_Font* font_selected;
extern Mix_Music* interface_bgm;

SDL_Texture* title_image = NULL;
SDL_Texture* ending_texture = NULL;

int menu_index = 0;   // 0 = START, 1 = EXIT

void title_init()
{
    title_image = IMG_LoadTexture(app.renderer, "gfx/titlescreen.png");
    if (!title_image) {
        printf("Title image load failed: %s\n", IMG_GetError());
        exit(1);
    }

    menu_index = 0;

    Mix_PlayMusic(interface_bgm, -1);
}

void title_update()
{
    // ===== 메뉴 이동 =====
    if (app.menu_up) {
        if (menu_index > 0) menu_index--;
        app.menu_up = 0;   // ★ key_up이 아니라 menu_up을 리셋해야 함
    }

    if (app.menu_down) {
        if (menu_index < 1) menu_index++;
        app.menu_down = 0; // ★ key_down이 아니라 menu_down 리셋
    }

    // ===== 메뉴 선택 =====
    if (app.menu_enter || app.key_space) {

        if (menu_index == 0){
            Mix_HaltMusic();       // ★ 타이틀 음악 정지
            ResetInputFlags();     // ← 안전하게 입력만 초기화
            game_state = STATE_GAME;
        }
        else
            game_state = STATE_EXIT;

        app.menu_enter = 0;   // ★ menu_enter 리셋
        app.key_space = 0;
    }
}

// void title_render()
// {
//     SDL_RenderCopy(app.renderer, title_image, NULL, NULL);

//     SDL_Color normal   = {255, 255, 255, 255};
//     SDL_Color selected = {255, 255, 0, 255};

//     // START
//     TTF_Font* font_s = (menu_index == 0 ? font_selected : font_normal);
//     SDL_Color col_s  = (menu_index == 0 ? selected : normal);

//     SDL_Surface* s_start = TTF_RenderText_Solid(font_s, "START", col_s);
//     SDL_Texture* tex_start = SDL_CreateTextureFromSurface(app.renderer, s_start);

//     // EXIT
//     TTF_Font* font_e = (menu_index == 1 ? font_selected : font_normal);
//     SDL_Color col_e  = (menu_index == 1 ? selected : normal);

//     SDL_Surface* s_exit = TTF_RenderText_Solid(font_e, "EXIT", col_e);
//     SDL_Texture* tex_exit = SDL_CreateTextureFromSurface(app.renderer, s_exit);

//     // 위치 설정
//     SDL_Rect r_start = { 240, 300, s_start->w, s_start->h };
//     SDL_Rect r_exit  = { 240, 360, s_exit->w, s_exit->h };

//     SDL_RenderCopy(app.renderer, tex_start, NULL, &r_start);
//     SDL_RenderCopy(app.renderer, tex_exit, NULL, &r_exit);

//     SDL_FreeSurface(s_start);
//     SDL_FreeSurface(s_exit);

//     SDL_DestroyTexture(tex_start);
//     SDL_DestroyTexture(tex_exit);
// }

void title_render()
{
    SDL_RenderCopy(app.renderer, title_image, NULL, NULL);

    // ================================
//  타이틀: "Gravity Hacker"
// =============================

    SDL_Color normal   = {255, 255, 255, 255};
    SDL_Color selected = {255, 255, 0, 255};

    // START
    TTF_Font* font_s = (menu_index == 0 ? font_selected : font_normal);
    SDL_Color col_s  = (menu_index == 0 ? selected : normal);

    SDL_Surface* s_start = TTF_RenderText_Solid(font_s, "START", col_s);
    SDL_Texture* tex_start = SDL_CreateTextureFromSurface(app.renderer, s_start);

    // EXIT
    TTF_Font* font_e = (menu_index == 1 ? font_selected : font_normal);
    SDL_Color col_e  = (menu_index == 1 ? selected : normal);

    SDL_Surface* s_exit = TTF_RenderText_Solid(font_e, "EXIT", col_e);
    SDL_Texture* tex_exit = SDL_CreateTextureFromSurface(app.renderer, s_exit);

    // 버튼 크기 2배
    SDL_Rect r_start;
    r_start.w = s_start->w * 2;
    r_start.h = s_start->h * 2;
    r_start.x = (SCREEN_WIDTH - r_start.w) / 2;   // 화면 중앙
    r_start.y = 700;                               // ★ 아래 위치로 이동

    SDL_Rect r_exit;
    r_exit.w = s_exit->w * 2;
    r_exit.h = s_exit->h * 2;
    r_exit.x = (SCREEN_WIDTH - r_exit.w) / 2;     // 화면 중앙
    r_exit.y = 900;                                // ★ START보다 더 아래

    SDL_RenderCopy(app.renderer, tex_start, NULL, &r_start);
    SDL_RenderCopy(app.renderer, tex_exit, NULL, &r_exit);

    SDL_FreeSurface(s_start);
    SDL_FreeSurface(s_exit);

    SDL_DestroyTexture(tex_start);
    SDL_DestroyTexture(tex_exit);
}


void InitEnding(void)
{
    ending_texture = IMG_LoadTexture(app.renderer, "./gfx/ending.png");
    if (!ending_texture)
    {
        printf("Error loading ending.png: %s\n", IMG_GetError());
    }
}

void DrawEnding(void)
{
    SDL_Rect dest = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(app.renderer, ending_texture, NULL, &dest);
}