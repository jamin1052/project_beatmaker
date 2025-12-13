#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

// ----------------------------------------
// 창 및 전역 설정
// ----------------------------------------
#define SCREEN_WIDTH 1600       
#define SCREEN_HEIGHT 1280      
#define BUFSIZE 1024

// ----------------------------------------
// 맵 및 타일 관련 상수
// ----------------------------------------
#define TILE_SIZE 64            
#define MAP_WIDTH 25            
#define MAP_HEIGHT 20           

#define PLAYER_WIDTH  64
#define PLAYER_HEIGHT 64

// ★ 타일 종류 정의 (기존 + Speed 추가)
#define TILE_EMPTY 0          
#define TILE_FLOOR 1          
#define TILE_SPIKE 2          // 바닥 가시 (위쪽 방향)
#define TILE_SPIKE_REVERSE 3  // 천장 가시 (아래쪽 방향)
#define TILE_SPIKE_LEFT 4     // 왼쪽 벽 가시 (오른쪽 방향)
#define TILE_SPIKE_RIGHT 5    // 오른쪽 벽 가시 (왼쪽 방향)
#define TILE_CHECKPOINT 6     // 깃발 (체크포인트)
#define TILE_GRAVITY_STRING 7 // 중력 반전 실
#define TILE_START 8
#define TILE_GOAL 9
#define TILE_SPEED 10         // ★ [추가] 속도 증가 타일
#define TILE_SLOW 11

// ... (기존 타일 정의) ...
#define TILE_LASER_FLOOR 12   
#define TILE_LASER_CEILING 13 
// ★ [추가] 벽면 레이저 타일
#define TILE_LASER_LEFT 14    // 왼쪽 벽 (오른쪽으로 발사)
#define TILE_LASER_RIGHT 15   // 오른쪽 벽 (왼쪽으로 발사)

// ★ [추가] 최대 투사체 개수
#define MAX_PROJECTILES 100

extern SDL_Texture* g_laser_beam_texture;
// ----------------------------------------
// 구조체 정의
// ----------------------------------------
typedef struct {
    SDL_Renderer *renderer;
    SDL_Window *window;
    TTF_Font *font;
    
    int key_up;
    int key_down;
    int key_left;
    int key_right;
    int key_r;
    int key_space;
    int key_enter;

    int menu_up;
    int menu_down;
    int menu_enter;
} App;

typedef struct {
    SDL_Texture *texture;
    SDL_Rect pos;           
    double v_x;             
    double v_y;             
    int health;             
    
    int is_grounded;        
    int gravity_inverted;   

    // ★ 체크포인트 및 기믹용 변수
    int checkpoint_x;       // 저장된 부활 위치 X
    int checkpoint_y;       // 저장된 부활 위치 Y
    int checkpoint_room_row;  // 체크포인트가 위치한 방의 행 인덱스
    int checkpoint_room_col;  // 체크포인트가 위치한 방의 열 인덱스

    int last_string_row; 
    int last_string_col;
    int last_checkpoint_id; 
    //double gravity_cooldown; // 중력 반전 쿨타임 (초 단위)
    int has_checkpoint; 
    // double speed_timer; (만약 스피드 타이머 쓰고 계시다면 유지)
} Entity;

typedef struct {
    SDL_Rect pos;
    SDL_Color color;
    SDL_Surface *surface;
    SDL_Texture *texture;
} Text;

// ----------------------------------------
// 전역 변수 선언
// ----------------------------------------
extern App app;
extern Entity player;
extern Mix_Music *bgm;
extern Mix_Music *gameplayBGM;
extern Mix_Chunk *checkpoint_effect;
extern Mix_Chunk *death_effect;
extern Mix_Music *ending_bgm;

extern int g_map_data[MAP_HEIGHT][MAP_WIDTH];
extern SDL_Texture* g_tile_textures[16]; // ★ [수정] 12 -->>맵 배열 원소 개수 각 이미지

// ★ [추가] 투사체 구조체 정의
typedef struct {
    double x, y;      
    double vx, vy;    
    int active;       
    double angle;     
    int w; // 히트박스 너비
    int h; // 히트박스 높이
} Projectile;


// ★ [추가] 투사체 관련 전역 변수
extern SDL_Texture* g_laser_beam_texture;
extern Projectile g_projectiles[MAX_PROJECTILES];

extern SDL_Texture* player_texture_normal;
extern SDL_Texture* player_texture_reverse;

extern Mix_Music *gameplayBGM;

typedef enum {
    STATE_TITLE,
    STATE_STORY,
    STATE_GAME,
    STATE_GAMEOVER,
    STATE_ENDING,
    STATE_EXIT
} GameState;

extern GameState game_state;

void ResetInputFlags(void);

typedef struct {
    int x;
    int y;
} Camera;

extern Camera camera;

