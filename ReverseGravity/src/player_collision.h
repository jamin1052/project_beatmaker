int get_tile_at(int tx, int ty);
void check_spike_collision(void);
int is_wall_tile(int t);
int check_wall_collision(void);
void resolve_horizontal_collision(void);
void resolve_vertical_tile_collision(void);
void check_goal_reach(void);
void check_interactive_tiles(void); // ★ [추가]

// ★ [추가] 스피드 타일 위에 있는지 확인하는 함수
int is_on_speed_tile(void);
int is_on_slow_tile(void);