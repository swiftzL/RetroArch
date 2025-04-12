#include "ws.h"
#include <emscripten.h>
#include "drivers/emulatorjs_input.h"
#include<stdio.h>
#include<stdlib.h>


static int simulate_input_flag = 0;

static unsigned char* cmds = NULL;

int CMD_BYTES = 16;//一小时3m 了不起玩10个小时 30m

int TEN_HOUR_FRAME = 60 * 60 *60 * 10;

void intToByteArray(unsigned value, unsigned char *byteArray) {
    byteArray[0] = (value >> 24) & 0xFF; // 取最高字节
    byteArray[1] = (value >> 16) & 0xFF; // 取次高字节
    byteArray[2] = (value >> 8) & 0xFF;  // 取次低字节
    byteArray[3] = value & 0xFF;         // 取最低字节
}
unsigned int byteArrayToInt(const unsigned char *byteArray) {
    unsigned int value = 0;
    value |= (byteArray[0] << 24); // 最高字节
    value |= (byteArray[1] << 16); // 次高字节
    value |= (byteArray[2] << 8);  // 次低字节
    value |= byteArray[3];         // 最低字节
    return value;
}

void printBytes(unsigned char* array,int size){
    for (int i = 0; i < size; i++)
    {
        printf("%02x ", array[i]);
    }
    printf("\n");
}
void reciveWsCmds(unsigned char* array,int size) {
   if (array == NULL)
   {
      return;
   }
   printBytes(array,size);
   if(cmds == NULL){
       cmds = (unsigned char *)malloc(CMD_BYTES);
   }
   int currentFrame = byteArrayToInt(array);
   printf("接收到frame:%d",currentFrame);
   if (currentFrame >= TEN_HOUR_FRAME)
   {
      return;
   } 
   int cmd_pos = (currentFrame-1)*CMD_BYTES;
   for(int i = 0; i < size; i++) {
      cmds[cmd_pos++] = array[i];
   }
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

static void sendData(unsigned char* data, int size) {
    EM_ASM_({
        // 创建一个 Uint8Array 视图，指向传递的内存
        var dataView = new Uint8Array(HEAPU8.buffer, $0, $1);
        
        // 创建一个新的 ArrayBuffer 和复制数据
        // 这很重要，因为 HEAPU8.buffer 会随着内存增长而变化
        var arraybuffer = new ArrayBuffer($1);
        var copy = new Uint8Array(arraybuffer);
        copy.set(dataView);
        
        // 调用 JavaScript 函数处理 ArrayBuffer
        window.sendFrameData(arraybuffer);
        
        // 如果您想要保留对 HEAPU8.buffer 的直接引用，请确保数据不会被释放
        // 或者考虑使用下面的其他方法
    }, data, size);

}

//bind js send 没有指令修改就不要同步 节约流量
void ws_sync(unsigned int frame,int user,unsigned int* cmds,int size) {
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
      if (cmds[1] == 0)
      {
         return;
      }
      intToByteArray(1, data + 4);
      intToByteArray(cmds[1], data + 8); //同步其他 
   }
   //send data
   sendData(data,16);
}

unsigned char* get_frame(unsigned int frame){
   if (frame >= TEN_HOUR_FRAME)
   {
      return NULL;
   }
   int cmd_pos = (frame-1)*CMD_BYTES;
  if(byteArrayToInt(cmds+cmd_pos) != frame){
   return NULL;
  }
  return cmds+cmd_pos;
}

static int current_user = -1;

int set_current_user(int num) {
   current_user = num;
}
int currentUser(){
   
   return current_user;
}
int isWs(){
   return current_user != -1;
}