 

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

void set_is_input_flag();
void clear_is_input_flag();
int has_input_flag();

void ws_sync(unsigned int frame,int user,unsigned int* cmds,int size);
unsigned char* get_frame(unsigned int frame);
void intToByteArray(unsigned value, unsigned char *byteArray);
unsigned int byteArrayToInt(const unsigned char *byteArray);
#endif

