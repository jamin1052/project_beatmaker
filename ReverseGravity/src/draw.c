#include "defs.h"
#include <SDL2/SDL.h>
#include "projectile.h"
// DrawGame에서 사용되는 함수의 프로토타입을 미리 선언합니다.
void RenderEntity(Entity *object); 

extern TTF_Font* font_normal;

// ----------------------------------------
// 맵 렌더링 함수 (수정됨)
// ----------------------------------------
void DrawMap(void) {
    SDL_Rect destRect = {0, 0, TILE_SIZE, TILE_SIZE};
    SDL_Texture* texture_to_draw = NULL;

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int tile_type = g_map_data[y][x];

            // 빈 공간이면 건너뜀
            if (tile_type == TILE_EMPTY) continue;

            double angle = 0.0;
            SDL_RendererFlip flip = SDL_FLIP_NONE;
            texture_to_draw = NULL;

            // ----------------------------------------------------
            // 1. 타일 종류에 따라 텍스처와 회전 각도 결정
            // ----------------------------------------------------
            
            // --- (A) 가시 (Spikes) ---
            if (tile_type == TILE_SPIKE) { // 2: 바닥 가시
                texture_to_draw = g_tile_textures[TILE_SPIKE];
                angle = 0.0;
            }
            else if (tile_type == TILE_SPIKE_REVERSE) { // 3: 천장 가시
                texture_to_draw = g_tile_textures[TILE_SPIKE];
                angle = 180.0;
            }
            else if (tile_type == TILE_SPIKE_LEFT) { // 4: 왼쪽 벽 가시
                texture_to_draw = g_tile_textures[TILE_SPIKE];
                angle = 90.0;
            }
            else if (tile_type == TILE_SPIKE_RIGHT) { // 5: 오른쪽 벽 가시
                texture_to_draw = g_tile_textures[TILE_SPIKE];
                angle = 270.0;
            }
            
            // --- (B) 레이저 본체 (Lasers) ---
            else if (tile_type == TILE_LASER_FLOOR) { // 12: 바닥 레이저
                texture_to_draw = g_tile_textures[TILE_LASER_FLOOR];
                angle = 0.0;
            }
            else if (tile_type == TILE_LASER_CEILING) { // 13: 천장 레이저
                texture_to_draw = g_tile_textures[TILE_LASER_FLOOR];
                angle = 180.0;
            }
            else if (tile_type == TILE_LASER_LEFT) { // 14: 왼쪽 벽 레이저
                texture_to_draw = g_tile_textures[TILE_LASER_FLOOR];
                angle = 90.0;
            }
            else if (tile_type == TILE_LASER_RIGHT) { // 15: 오른쪽 벽 레이저
                texture_to_draw = g_tile_textures[TILE_LASER_FLOOR];
                angle = 270.0;
            }

            // --- (C) 기타 일반 타일들 ---
            else {
                // 바닥(1), 체크포인트, 실, 아이템 등등
                // 배열 범위 안전 체크
                if (tile_type >= 0 && tile_type < 16) {
                    texture_to_draw = g_tile_textures[tile_type];
                }
                angle = 0.0;
            }

            // ----------------------------------------------------
            // 2. 그리기 수행
            // ----------------------------------------------------
            if (texture_to_draw != NULL) {
                
                // (아이템들은 크기 조정)
                if (tile_type == TILE_CHECKPOINT || tile_type == TILE_GRAVITY_STRING || 
                    tile_type == TILE_SPEED || tile_type == TILE_SLOW) {
                    int w, h;
                    SDL_QueryTexture(texture_to_draw, NULL, NULL, &w, &h);
                    destRect.w = w;
                    destRect.h = h;
                } else {
                    destRect.w = TILE_SIZE;
                    destRect.h = TILE_SIZE;
                }

                // 위치 계산 (카메라 적용)
                destRect.x = x * TILE_SIZE - camera.x;
                destRect.y = y * TILE_SIZE - camera.y;

                // (중앙 정렬 보정)
                if (tile_type == TILE_CHECKPOINT) {
                    destRect.x += (TILE_SIZE - destRect.w) / 2;
                    destRect.y += (TILE_SIZE - destRect.h); 
                }
                else if (tile_type == TILE_GRAVITY_STRING) {
                    destRect.x += (TILE_SIZE - destRect.w) / 2;
                    destRect.y += (TILE_SIZE - destRect.h) / 2; 
                }

                // 화면 밖 컬링
                if (destRect.x + destRect.w < 0 || destRect.x > SCREEN_WIDTH ||
                    destRect.y + destRect.h < 0 || destRect.y > SCREEN_HEIGHT) {
                    continue;
                }

                SDL_RenderCopyEx(app.renderer, texture_to_draw, NULL, &destRect, angle, NULL, flip);
            }
        }
    }
}
// ----------------------------------------
// DrawGame 함수
// ----------------------------------------
void DrawGame(void) {
    DrawMap();
    RenderProjectiles(); // 2. ★ 투사체(빔)는 맵 위에 그리기
    RenderEntity(&player); // 3. 플레이어는 제일 위에
}

// ----------------------------------------
// DrawGameOver 함수
// ----------------------------------------
void DrawGameOver(void)
{
    SDL_Rect dst = {
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    };

    SDL_RenderCopy(app.renderer, death_texture, NULL, &dst);
}

// ----------------------------------------
// RenderEntity 함수
// ----------------------------------------
void RenderEntity(Entity *object)
{
    SDL_Rect r = object->pos;
    r.x -= camera.x;
    r.y -= camera.y;

    SDL_RenderCopy(app.renderer, object->texture, NULL, &r);
}

void ClearWindow(void) {
    SDL_SetRenderDrawColor(app.renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(app.renderer);
}

void ShowWindow(void) {
    SDL_RenderPresent(app.renderer);
}