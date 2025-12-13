#include "scene_story.h"
#include "defs.h"           
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "map.h"
#include "init.h"
#include "defs.h"

extern App app;
extern GameState game_state;
extern Camera camera;

static SDL_Texture* story_img = NULL;
static Uint32 story_enter_ticks = 0;

static void story_destroy(void)
{
    if (story_img) {
        SDL_DestroyTexture(story_img);
        story_img = NULL;
    }
}

void story_init(void)
{
    story_destroy();

    story_img = IMG_LoadTexture(app.renderer, "gfx/story.png");
    if (!story_img) {
        printf("Story image load failed: %s\n", IMG_GetError());
        // 스토리 이미지 없으면 그냥 게임으로 넘어가게
        game_state = STATE_GAME;
        return;
    }

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
}
