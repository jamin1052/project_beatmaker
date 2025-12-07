#include "defs.h"
#include "init.h"
#include "player_move.h"
#include "player_collision.h"
#include "map.h"

// MOVE_SPEED 기본값 정의 (혹시 없으면 600.0으로 설정)
#ifndef MOVE_SPEED
#define MOVE_SPEED 600.0 
#endif

void move_player_horzontal(double dt)
{
    int dir = 0;

    if (app.key_left && !app.key_right)
        dir = -1;
    else if (app.key_right && !app.key_left)
        dir = 1;

    // ★ [핵심] 수평 이동 속도 계산
    double current_speed = MOVE_SPEED;

    // 발 밑에 스피드 타일이 있으면 속도 2배!
    if (is_on_speed_tile()) {
        current_speed *= 2.0;
    }
    
    if (is_on_slow_tile()) {
        current_speed *= 0.5;
    }

    player.v_x = dir * current_speed;

    // 원래 위치 저장
    int old_x = player.pos.x;

    // 먼저 이동
    player.pos.x += (int)(player.v_x * dt);

    // 벽 충돌 체크 (Speed 타일은 벽이지만 위에서 걷는 건 괜찮음, 옆은 막힘)
    if (check_wall_collision())
    {
        // 충돌했으면 이동 취소
        player.pos.x = old_x;
    }
}

extern Entity player;

void UpdatePlayer()
{
    int screen_w = MAP_WIDTH * TILE_SIZE;
    int screen_h = MAP_HEIGHT * TILE_SIZE;

    // 방 전환 처리
    if (player.pos.x >= screen_w)
    {
        ChangeRoom(0, +1);
        player.pos.x = 0;
    }
    else if (player.pos.x + player.pos.w <= 0)
    {
        ChangeRoom(0, -1);
        player.pos.x = screen_w - player.pos.w;
    }

    if (player.pos.y >= screen_h)
    {
        ChangeRoom(+1, 0);
        player.pos.y = 0;
    }
    else if (player.pos.y + player.pos.h <= 0)
    {
        ChangeRoom(-1, 0);
        player.pos.y = screen_h - player.pos.h;
    }
}