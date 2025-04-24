#include "ppb.h"
#include <stdio.h>
#include <string.h>

static char progress_bar[10];
// [####>.....] 40% 
void draw_progress_bar(int percent){
    memset(progress_bar, '.', 10);
    memset(progress_bar, '#', percent);
    progress_bar[percent] = '>';
    if(percent == 10){
        progress_bar[10] = '#'; 
    }
    printf("[%s] %d%\r", progress_bar, percent*10);
    fflush(stdout);
}