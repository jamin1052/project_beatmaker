#include "defs.h"
#include "init.h"
#include "player_collision.h"
#include "player_death.h"
#include "scene_title.h"
#include "map.h"

int get_tile_at(int tx, int ty) {
    if (tx < 0 || tx >= MAP_WIDTH || ty < 0 || ty >= MAP_HEIGHT) return TILE_EMPTY;
    return g_map_data[ty][tx];
}

// 1. 벽 판정: Speed 타일(10)은 제외 (통과 가능)
int is_wall_tile(int t) {
    return (t == TILE_FLOOR); // || t == TILE_SPEED --------------> 스피드 블록 벽 판정
}

// ★ [핵심 수정] 플레이어의 몸체(사각형) 전체를 검사합니다.
// 이전처럼 점 하나만 찍어서 검사하면, 겹쳐 있는데도 인식을 못 할 수 있습니다.
int is_on_speed_tile(void) {
    // 플레이어가 차지하고 있는 타일의 범위를 계산 (좌상단 ~ 우하단)
    int start_tx = player.pos.x / TILE_SIZE;
    int end_tx   = (player.pos.x + player.pos.w - 1) / TILE_SIZE;
    int start_ty = player.pos.y / TILE_SIZE;
    int end_ty   = (player.pos.y + player.pos.h - 1) / TILE_SIZE;

    // 플레이어 몸이 걸쳐있는 모든 타일을 확인
    for (int ty = start_ty; ty <= end_ty; ty++) {
        for (int tx = start_tx; tx <= end_tx; tx++) {
            if (get_tile_at(tx, ty) == TILE_SPEED) {
                return 1; // 하나라도 스피드 타일이면 True!
            }
        }
    }
    return 0; // 안 닿음
}
/*
int is_on_slow_tile(void) {
    int cx = player.pos.x + player.pos.w / 2;
    int check_y = 0;

    if (player.gravity_inverted) {
        check_y = player.pos.y - 2; 
    } else {
        check_y = player.pos.y + player.pos.h + 2; 
    }

    int tx = cx / TILE_SIZE;
    int ty = check_y / TILE_SIZE;

    if (get_tile_at(tx, ty) == TILE_SLOW) {
        return 1;
    }
    return 0;
}
*/ // --> 슬로우 타일 벽처리
int is_on_slow_tile(void) {
    int start_tx = player.pos.x / TILE_SIZE;
    int end_tx   = (player.pos.x + player.pos.w - 1) / TILE_SIZE;
    int start_ty = player.pos.y / TILE_SIZE;
    int end_ty   = (player.pos.y + player.pos.h - 1) / TILE_SIZE;

    // 플레이어 몸이 걸쳐있는 모든 타일을 확인
    for (int ty = start_ty; ty <= end_ty; ty++) {
        for (int tx = start_tx; tx <= end_tx; tx++) {
            if (get_tile_at(tx, ty) == TILE_SLOW) {
                return 1; // 닿았다!
            }
        }
    }
    return 0;
}

// ... (아래는 기존 충돌 함수들 유지) ...

int check_wall_collision(void) {
    int x1 = player.pos.x;
    int x2 = player.pos.x + player.pos.w - 1;
    int mid_y = player.pos.y + player.pos.h / 2;
    int mid_ty = mid_y / TILE_SIZE;
    int tx1 = x1 / TILE_SIZE;
    int tx2 = x2 / TILE_SIZE;

    if (is_wall_tile(get_tile_at(tx1, mid_ty))) return 1;
    if (is_wall_tile(get_tile_at(tx2, mid_ty))) return 1;
    return 0;
}

void resolve_horizontal_collision(void) {
    SDL_Rect p = player.pos;
    int left_x = p.x;
    int right_x = p.x + p.w - 1;
    int top_y = p.y;
    int bottom_y = p.y + p.h - 1;

    int left_tx = left_x / TILE_SIZE;
    int right_tx = right_x / TILE_SIZE;
    int top_ty = top_y / TILE_SIZE;
    int bottom_ty = bottom_y / TILE_SIZE;

    int t1 = get_tile_at(left_tx, top_ty);
    int t2 = get_tile_at(left_tx, bottom_ty);

    if (is_wall_tile(t1) || is_wall_tile(t2)) {
        player.pos.x = (left_tx + 1) * TILE_SIZE;
    }

    t1 = get_tile_at(right_tx, top_ty);
    t2 = get_tile_at(right_tx, bottom_ty);

    if (is_wall_tile(t1) || is_wall_tile(t2)) {
        player.pos.x = right_tx * TILE_SIZE - player.pos.w;
    }
}

void resolve_vertical_tile_collision(void) {
    SDL_Rect p = player.pos;
    int feet_x1 = p.x;
    int feet_x2 = p.x + p.w - 1;
    int feet_y = p.y + p.h;
    int head_y = p.y - 1;

    int tx1 = feet_x1 / TILE_SIZE;
    int tx2 = feet_x2 / TILE_SIZE;
    int ty_feet = feet_y / TILE_SIZE;
    int ty_head = head_y / TILE_SIZE;

    if (!player.gravity_inverted) {
        if (is_wall_tile(get_tile_at(tx1, ty_feet)) || is_wall_tile(get_tile_at(tx2, ty_feet))) {
            player.pos.y = ty_feet * TILE_SIZE - p.h;
            player.v_y = 0;
            player.is_grounded = 1;
            return;
        }
    } else {
        if (is_wall_tile(get_tile_at(tx1, ty_head)) || is_wall_tile(get_tile_at(tx2, ty_head))) {
            player.pos.y = (ty_head + 1) * TILE_SIZE;
            player.v_y = 0;
            player.is_grounded = 1;
            return;
        }
    }
    player.is_grounded = 0;
}

void check_spike_collision(void) {
    int x1 = player.pos.x;
    int y1 = player.pos.y;
    int x2 = player.pos.x + player.pos.w - 1;
    int y2 = player.pos.y + player.pos.h - 1;

    int tx1 = x1 / TILE_SIZE;
    int ty1 = y1 / TILE_SIZE;
    int tx2 = x2 / TILE_SIZE;
    int ty2 = y2 / TILE_SIZE;

    int tiles_x[2] = { tx1, tx2 };
    int tiles_y[2] = { ty1, ty2 };

    for (int iy = 0; iy < 2; ++iy) {
        for (int ix = 0; ix < 2; ++ix) {
            int t = get_tile_at(tiles_x[ix], tiles_y[iy]);
            if (t == TILE_SPIKE || t == TILE_SPIKE_REVERSE || 
                t == TILE_SPIKE_LEFT || t == TILE_SPIKE_RIGHT) {
                kill_player();
                return;
            }
        }
    }
}

void check_goal_reach(void) {
    int tx1 = player.pos.x / TILE_SIZE;
    int ty1 = player.pos.y / TILE_SIZE;
    int tx2 = (player.pos.x + player.pos.w) / TILE_SIZE;
    int ty2 = (player.pos.y + player.pos.h) / TILE_SIZE;

    if (g_map_data[ty1][tx1] == TILE_GOAL || g_map_data[ty1][tx2] == TILE_GOAL ||
        g_map_data[ty2][tx1] == TILE_GOAL || g_map_data[ty2][tx2] == TILE_GOAL) {
        InitEnding();
        game_state = STATE_ENDING;
    }
}

void check_interactive_tiles(void)
{
    int cx = player.pos.x + player.pos.w / 2;
    int cy = player.pos.y + player.pos.h / 2;
    int tx = cx / TILE_SIZE;
    int ty = cy / TILE_SIZE;

    int tile = get_tile_at(tx, ty);

    if (tile == TILE_CHECKPOINT) {
        player.checkpoint_x = tx * TILE_SIZE;
        player.checkpoint_y = ty * TILE_SIZE;
        player.checkpoint_room_row = current_room_row;
        player.checkpoint_room_col = current_room_col;
    }

    if (tile == TILE_GRAVITY_STRING) {
        if (player.gravity_cooldown <= 0.0) {
            player.gravity_inverted = !player.gravity_inverted;
            if (player.gravity_inverted)
                player.texture = player_texture_reverse;
            else
                player.texture = player_texture_normal;
            player.gravity_cooldown = 0.5; 
        }
    }
}