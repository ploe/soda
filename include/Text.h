#ifndef SODA_TEXT_H
#define SODA_TEXT_H

typedef char *Text;

Text TextNew(const char *format, ...);
Text TextFromFile(const char *path);
Text TextFree(Text s);

#endif
