

#ifndef __WS_H
#define __WS_H


typedef  struct ws_cmd
{
   unsigned int user;
   unsigned int key;
   unsigned int down;
};

int currentUser();
int isWs();
void sendCmd(unsigned long long cmd);//这里还是插入到下一帧好点

void set_is_input_flag();
void clear_is_input_flag();
int has_input_flag();

void sync(unsigned int frame,int user,unsigned int* cmds,int size);
#endif

