#include "ws.h"
#include "../input/drivers/emulatorjs_input.h"
#include<stdio.h>


static int simulate_input_flag = 0;

static unsigned char* cmds = NULL;

int CMD_BYTES = 16;//一小时3m 了不起玩10个小时 30m

int TEN_HOUR_FRAME = 60 * 60 *60 * 10;

static void intToByteArray(unsigned value, unsigned char *byteArray) {
    byteArray[0] = (value >> 24) & 0xFF; // 取最高字节
    byteArray[1] = (value >> 16) & 0xFF; // 取次高字节
    byteArray[2] = (value >> 8) & 0xFF;  // 取次低字节
    byteArray[3] = value & 0xFF;         // 取最低字节
}
static unsigned int byteArrayToInt(const unsigned char *byteArray) {
    unsigned int value = 0;
    value |= (byteArray[0] << 24); // 最高字节
    value |= (byteArray[1] << 16); // 次高字节
    value |= (byteArray[2] << 8);  // 次低字节
    value |= byteArray[3];         // 最低字节
    return value;
}
void reciveWsCmds(unsigned char* array,int size) {
   if (array == NULL)
   {
      return;
   }
   
   if(cmds == NULL){
       cmds = (unsigned char *)malloc(CMD_BYTES * );
   }
   int currentFrame = byteArrayToInt(array);
   if (currentFrame >= TEN_HOUR_FRAME)
   {
      return;
   }
    for (int i = 0; i < size; i++) {
        array[i] = HEAPU8[i]; // 从 HEAPU8 中获取字节
    }
    processByteArray(array, ARRAY_SIZE);
    free(array); // 释放内存
}


void set_is_input_flag(){
   simulate_input_flag = 1;
}
void clear_is_input_flag() {
   simulate_input_flag = 0;
}

int has_input_flag(){
   return simulate_input_flag == 1;
}

//bind js send
void sync(unsigned int frame,int user,unsigned int* cmds,int size) {
   printf("开始同步... %d\n user:%d",frame,user);
   unsigned char data[16];
   intToByteArray(frame, data);
   if (user==0)//sync all
   {
      intToByteArray(size, data + 4);
      for (int i = 0; i < size; i++)
      {
         intToByteArray(cmds[i], data + 8 + i * 4);
      }
      
   }else{
      intToByteArray(1, data + 4);
      intToByteArray(cmds[1], data + 8); //同步其他 
   }
   //send data
}


