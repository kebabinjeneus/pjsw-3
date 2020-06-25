#ifndef USART_H
#define USART_H

void USART_init();
void writeChar(char data);
void writeString(char* Pstring);
void writeInt(int i);
void checkInput();

#endif
