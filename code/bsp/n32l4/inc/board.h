#ifndef __BOARD_H__
#define __BOARD_H__


void board_init(void);
void enter_lowpower(void);
void exit_lowpower(void);
void enter_offcpu(void);
void exit_offcpu(void);
void board_debug_init(void);
void board_debug_deinit(void);
#endif
