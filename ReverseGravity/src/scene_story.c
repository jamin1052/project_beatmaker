#include "scene_story.h"
#include "defs.h"           
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "map.h"
#include "init.h"
#include "defs.h"

extern App app;
extern GameState game_state;
extern Camera camera;
extern TTF_Font* font_normal;

static SDL_Texture* story_img = NULL;
static SDL_Texture* hint_tex = NULL;
static SDL_Rect hint_rect;
static Uint32 story_enter_ticks = 0;

static void story_destroy(void)
{
    if (story_img) {
        SDL_DestroyTexture(story_img);
        story_img = NULL;
    }
    if(hint_tex){
        SDL_DestroyTexture(hint_tex);
        hint_tex = NULL;
    }
}

void story_init(void)
{
    story_destroy();

    story_img = IMG_LoadTexture(app.renderer, "gfx/scene_story.jpg");
    
    if (!story_img) {
        printf("Story image load failed: %s\n", IMG_GetError());
        // 스토리 이미지 없으면 그냥 게임으로 넘어가게
        game_state = STATE_GAME;
        return;
    }
    // ===== 말풍선 영역(이미지 기준) =====
    // SCREEN_WIDTH=1600, SCREEN_HEIGHT=1280 기준으로 비율로 잡음
    SDL_Rect bubble = {
        (int)(SCREEN_WIDTH  * 0.35f), // x
        (int)(SCREEN_HEIGHT * 0.30f), // y
        (int)(SCREEN_WIDTH  * 0.68f), // w
        (int)(SCREEN_HEIGHT * 0.30f)  // h
    };
    int padding = 40; // 말풍선 테두리랑 글자 간격
        // ---- 텍스트(안내 문구) 생성 시작 ----
    SDL_Color white = {255, 255, 255, 255};
    const char* hint = 
        "\n"
        "       Hello, player~. I'm virus!  \n"
        "       We're going to hack this computer. Let's go~ \n "
        "       If you want to start, press spacebar! \n"
        "\n"
        "       -> : Right \n"
        "       <- : Left \n"
        "       Spacebar : Gravity Reverse \n";
    // 말풍선 폭 안에서 자동 줄바꿈
    int wrap = bubble.w - padding * 2;

    SDL_Surface* s = TTF_RenderUTF8_Blended_Wrapped(font_normal, hint, white, wrap);
    if (!s) {
        printf("TTF render failed: %s\n", TTF_GetError());
    } else {
        hint_tex = SDL_CreateTextureFromSurface(app.renderer, s);
        if (!hint_tex) {
            printf("CreateTextureFromSurface failed: %s\n", SDL_GetError());
        } else {
            hint_rect.w = s->w;
            hint_rect.h = s->h;
            // ✅ 말풍선 안 “정중앙” 배치
            hint_rect.x = bubble.x + (bubble.w - hint_rect.w) / 2;
            hint_rect.y = bubble.y + (bubble.h - hint_rect.h) / 2;
        }
        SDL_FreeSurface(s);
    }
    // ---- 텍스트 생성 끝 ----


    story_enter_ticks = SDL_GetTicks();
    ResetInputFlags(); // 타이틀에서 눌렀던 키가 남아 바로 넘어가는 것 방지
}

void story_update(void)
{
    // 타이틀에서 SPACE로 들어왔을 때, 바로 스킵되는 걸 막기 위한 아주 짧은 딜레이
    if (SDL_GetTicks() - story_enter_ticks < 200) return;

    if (app.key_space || app.menu_enter) {
        ResetInputFlags();

        LoadRoom(0,0);
        InitPlayer();

        camera.x = 0;
        camera.y = 0;
        game_state = STATE_GAME;
    }
}

void story_render(void)
{
    // 스토리 이미지 전체 화면 출력
    SDL_Rect dst = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(app.renderer, story_img, NULL, &dst);

    if (hint_tex) SDL_RenderCopy(app.renderer, hint_tex, NULL, &hint_rect);

}
