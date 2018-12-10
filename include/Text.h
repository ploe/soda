#ifndef TR_TEXT
#define TR_TEXT

typedef char *Text;

Text TextNew(const char *format, ...);
Text TextFromFile(const char *path);
Text TextFree(Text s);

#endif
