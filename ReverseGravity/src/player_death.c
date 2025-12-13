#include "defs.h"
#include "init.h"
#include "player_death.h"
#include "map.h"   // LoadRoom 쓰려고 필요
#include "input.h" // ResetInputFlags 쓰려면 필요(선택)

void kill_player(void)
{
    player.health = 0;
    player.v_x = 0;
    player.v_y = 0;

    if (death_effect)
        Mix_PlayChannel(-1, death_effect, 0);
    game_state = STATE_GAMEOVER;
}

// 추가: 체크포인트(또는 시작점)로 되돌리기 공용 함수
void reset_player_to_checkpoint(void)
{
    // 체크포인트 방으로 이동
    LoadRoom(player.checkpoint_room_row, player.checkpoint_room_col);

    // 위치 이동
    player.pos.x = player.checkpoint_x;
    player.pos.y = player.checkpoint_y;

    // 물리/상태 초기화
    player.v_x = 0.0;
    player.v_y = 0.0;
    player.is_grounded = 0;

    // 중력/스킨 초기화
    player.gravity_inverted = 0;
    player.texture = player_texture_normal;

    // 살아있는 상태로 복구
    player.health = 1;

    // (선택) 끼임 방지 목적이면 중력실 재트리거 방지용으로 초기화 추천
    player.last_string_row = -1;
    player.last_string_col = -1;

    // 입력 플래그 정리 (특히 key_r)
    ResetInputFlags();

    // 게임 상태를 게임으로 (죽었을 때 호출하면 GameOver에서 빠져나옴)
    game_state = STATE_GAME;
}