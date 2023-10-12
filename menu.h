#pragma once
#include <string>

inline static float dpi_scale = 1.f;
inline static int dpi_scale_new = 0.f;
inline static auto menu_setupped = false;
inline bool update_dpi = false;
inline int width = 820, height = 630;
inline float public_alpha;
inline int active_tab_index;
inline float child_height = 471;
inline float preview_alpha = 1.f;
inline int active_tab;
inline bool menuShow = true;
inline bool need_update_cfg = true;

inline int ScreenHeight = NULL;
inline int ScreenWidth = NULL;
inline int ScreenLeft = NULL;
inline int ScreenRight = NULL;
inline int ScreenTop = NULL;
inline int ScreenBottom = NULL;

inline bool need_to_draw_board = false;
inline bool trigger_is_working = false;
inline bool aim_is_working = false;

inline std::string username;

//from lua
inline bool loaded_script;