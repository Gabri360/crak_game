#ifndef STATE_STATISTICS_H
#define STATE_STATISTICS_H



void statistics_load(void);
void statistics_init(void);
void statistics_enter(void);
void statistics_update(double dt);
void statistics_run(void);
void statistics_handle_events(GLFWwindow* window);
void statistics_esc(void);
void statistics_shutdown(void);

#endif
