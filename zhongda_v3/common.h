#ifndef COMMON_H
#define COMMON_H

void intToStr(int num, char* str, int width) ;
void log(const char* title, const char* text);
void log(const char* text);
void log(const char* title, const char* text, const char* content);
void log(String title, const char* text);
void log(IPAddress ip, const char* prefix = "IP Address", Print& output = Serial);
void formatDateTime(DateTime dt, char* buffer);
int formatDate(char* buffer, size_t bufferSize);
void setPinsState(int* pinArr, int count, uint8_t state);
void setPinsMode(int* pinArr, int count, uint8_t mode);
#endif