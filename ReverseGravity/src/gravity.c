#include "defs.h"
#include "init.h"
#include "gravity.h"
#include "player_collision.h"

// ★ [수정] 가속도(4500) 대신 고정 낙하 속도(1000) 정의
#define FALL_SPEED 1000.0 

#define GROUND_Y (SCREEN_HEIGHT - TILE_SIZE)
#define CEILING_Y 0

void gravity_inverted(double dt)
{
    // 중력 방향 결정 (1: 바닥으로, -1: 천장으로)
    double g_dir = player.gravity_inverted ? -1.0 : 1.0;

    // ★ [수정] 가속도 없이 '고정 속도' 계산
    double current_speed = FALL_SPEED;

    // Speed 타일 통과 시 속도 1.5배 (1000 -> 1500)
    if (is_on_speed_tile()) {
        current_speed *= 1.5;
    }
    
    if (is_on_slow_tile()) {
        current_speed *= 0.5;
    }

    // 공중에 떠 있을 때만 속도 적용
    if (!player.is_grounded) {
        // 더하기(+=)가 아니라 대입(=)하여 등속 운동 구현
        player.v_y = current_speed * g_dir;
    }

    // 위치 업데이트 (이미 속도에 배율이 적용되었으므로 dt만 곱함)
    player.pos.y += (int)(player.v_y * dt);
}

void resolve_vertical_collision(void)
{
    // 기존 충돌 처리 유지
    resolve_vertical_tile_collision();
}
