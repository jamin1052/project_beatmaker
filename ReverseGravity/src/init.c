#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "defs.h"
#include "init.h"
#include "map.h"

Mix_Music* interface_bgm = NULL;
Mix_Music *gameplayBGM = NULL;
Mix_Chunk* checkpoint_effect = NULL;
Mix_Chunk* death_effect = NULL;
Mix_Music *ending_bgm = NULL;

// 전역 변수 정의
App app;
Entity player;
Camera camera = {0, 0};

// 타일 텍스처 전역 정의 (defs.h 에 extern 있음)
SDL_Texture* g_tile_textures[16]; // ★ [수정] 10 -> 11 ->12

SDL_Texture* player_texture_normal = NULL;
SDL_Texture* player_texture_reverse = NULL;

// ★ [추가] 여기 두 줄을 꼭 넣어야 합니다!
SDL_Texture* g_laser_beam_texture = NULL;   // 레이저 빔 텍스처 실제 생성
Projectile g_projectiles[MAX_PROJECTILES];  // 투사체 배열 실제 생성
// 폰트 전역 정의 (scene_title.c 에서 extern)
TTF_Font* font_normal = NULL;
TTF_Font* font_selected = NULL;

// ----------------------------------------
// 메모리 초기화
// ----------------------------------------
void InitMemorySet(void) {
    memset(&app, 0, sizeof(App));
    memset(&player, 0, sizeof(Entity));
}

// ----------------------------------------
// PNG 텍스처 로드 함수
// ----------------------------------------
static void load_tile_texture(SDL_Texture** tex_ptr, const char* path) {
    *tex_ptr = IMG_LoadTexture(app.renderer, path);
    if (*tex_ptr == NULL) {
        printf("Error loading texture %s: %s\n", path, IMG_GetError()); 
        exit(1);
    }
    SDL_SetTextureBlendMode(*tex_ptr, SDL_BLENDMODE_BLEND);
}

// ----------------------------------------
// SDL 초기화
// ----------------------------------------
void InitSDL(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL 초기화 실패: %s\n", SDL_GetError());
        exit(1);
    }

    // 창 생성
    app.window = SDL_CreateWindow("Reverse Gravity", 
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!app.window) { printf("창 생성 실패: %s\n", SDL_GetError()); exit(1); }

    // 렌더러 생성
    app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED);

    if (!app.renderer) { printf("렌더러 생성 실패: %s\n", SDL_GetError()); exit(1); }

    // 이미지 초기화
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) { 
        printf("SDL_image 초기화 실패: %s\n", IMG_GetError()); exit(1); 
    }

    // TTF 폰트 시스템 초기화
    if (TTF_Init() < 0) {
        printf("SDL_ttf 초기화 실패: %s\n", TTF_GetError());
        exit(1);
    }

    // 폰트 로드
    font_normal = TTF_OpenFont("ttf/LiberationSans-Regular.ttf", 32);
    font_selected = TTF_OpenFont("ttf/LiberationSans-Regular.ttf", 40);
    if (!font_normal) {
        printf("폰트 로드 실패: %s\n", TTF_GetError());
        exit(1);
    }
    if (!font_selected) {
        printf("폰트 로드 실패: %s\n", TTF_GetError());
        exit(1);
    }

    // 타일 그래픽 로드
    load_tile_texture(&g_tile_textures[TILE_FLOOR], "./gfx/floor_tile.png");
    load_tile_texture(&g_tile_textures[TILE_SPIKE], "./gfx/spike.png");
    load_tile_texture(&g_tile_textures[TILE_GOAL], "./gfx/GoalPoint.png");
    load_tile_texture(&g_tile_textures[TILE_START], "./gfx/StartPoint.png");
    load_tile_texture(&g_tile_textures[TILE_CHECKPOINT], "./gfx/CheckPoint.png"); 
    load_tile_texture(&g_tile_textures[TILE_GRAVITY_STRING], "./gfx/String.png");

    // ★ [추가] Speed 타일 이미지 로드 (파일 경로 확인 필수!)
    load_tile_texture(&g_tile_textures[TILE_SPEED], "./gfx/Speed.png");
    load_tile_texture(&g_tile_textures[TILE_SLOW], "./gfx/Slow.png");
    // 사운드 초기화

    load_tile_texture(&g_tile_textures[TILE_LASER_FLOOR], "./gfx/Laser.png");
    g_tile_textures[TILE_LASER_CEILING] = g_tile_textures[TILE_LASER_FLOOR]; 

    // ★ [추가] 벽면 레이저도 같은 이미지 사용 (그릴 때 회전)
    g_tile_textures[TILE_LASER_LEFT] = g_tile_textures[TILE_LASER_FLOOR];
    g_tile_textures[TILE_LASER_RIGHT] = g_tile_textures[TILE_LASER_FLOOR];

    load_tile_texture(&g_laser_beam_texture, "./gfx/LaserBeam.png");
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer 초기화 실패: %s\n", Mix_GetError());
        exit(1);
    }

    interface_bgm = Mix_LoadMUS("sound/interfaceBGM.wav");
    if (!interface_bgm) {
        printf("interface.wav 로드 실패: %s\n", Mix_GetError());
    }

    gameplayBGM = Mix_LoadMUS("sound/gameplayBGM.wav");
    if (!gameplayBGM) {
        printf("gameplayBGM.wav 로드 실패: %s\n", Mix_GetError());
    }

    checkpoint_effect = Mix_LoadWAV("sound/checkpoint.wav");
    if (!checkpoint_effect) {
        printf("checkpoint.wav 로드 실패: %s\n", Mix_GetError());
    }


    death_effect = Mix_LoadWAV("sound/deathsound.wav");
    if (!death_effect) {
        printf("사운드 로드 실패: %s\n", Mix_GetError());
        exit(1);
    }

    ending_bgm = Mix_LoadMUS("sound/ending.wav");
    if (!ending_bgm) {
        printf("ending.wav 로드 실패: %s\n", Mix_GetError());
    }
}

// ----------------------------------------
// SDL 종료 함수
// ----------------------------------------
void QuitSDL(void)
{
    if (app.renderer) SDL_DestroyRenderer(app.renderer);
    if (app.window) SDL_DestroyWindow(app.window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

// ----------------------------------------
// 플레이어 초기화 관련
// ----------------------------------------
void PlacePlayerAtStart(void)
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            if (g_map_data[y][x] == TILE_START)
            {
                player.pos.x = x * TILE_SIZE;
                player.pos.y = y * TILE_SIZE;
                return; 
            }
        }
    }

    // 만약 StartPoint가 없을 경우 디폴트 위치
    printf("[WARN] StartPoint tile not found! Using default position.\n");
    player.pos.x = SCREEN_WIDTH / 4;
    player.pos.y = SCREEN_HEIGHT / 4;
}

void InitPlayer(void) {
    // 두 이미지 모두 로드해두기
    load_tile_texture(&player_texture_normal, "./gfx/Player.png");
    load_tile_texture(&player_texture_reverse, "./gfx/ReversePlayer.png");

    // 처음에는 일반 이미지 사용
    player.texture = player_texture_normal;

    // 플레이어 위치를 StartPoint 위치로 설정
    PlacePlayerAtStart();

    player.pos.w = PLAYER_WIDTH;
    player.pos.h = PLAYER_HEIGHT;
    player.health = 1;

    // 게임 시작 시 체크포인트를 현재 시작 위치로 설정
    player.checkpoint_x = player.pos.x;
    player.checkpoint_y = player.pos.y;
    player.checkpoint_room_row = current_room_row;
    player.checkpoint_room_col = current_room_col;
    //player.gravity_cooldown = 0.0; 
    // ★ [수정] 플래그 초기화
    player.last_string_row = -1;
    player.last_string_col = -1;

    player.has_checkpoint = 0;
}

void InitTiles(void) {}