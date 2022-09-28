#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace UTILS {

#define LINE_SIZE 128

int _hexdump(void *addr, size_t len, char **buffer)
{
    size_t my_str_ite = 0;
    int i = 0;
    int j = 0;
    int line_ite = 0;
    int line_nbr = 0;
    char buf[17] = {'\0'};
    char *line = NULL;
    unsigned char *my_str = (unsigned char *)addr;

    if (addr == NULL || len == 0 || buffer == NULL) {
        return -1;
    }

    if (len % 16 == 0) {
        line_nbr = len / 16;
    } else {
        line_nbr = (len / 16) + 1;
    }

    line = (char*)calloc(line_nbr, LINE_SIZE);
    if (line == NULL) {
        return -1;
    }

    for (j = 0; j < line_nbr; j++) {
        line_ite += sprintf(&line[line_ite], "%08x  ", j);

        for (i = 0; i < 16; i++) {
            if (i + my_str_ite < len) {
                line_ite += sprintf(&line[line_ite], "%02x ", my_str[i + my_str_ite]);
                buf[i] = my_str[i + my_str_ite];
            } else if (j != 0) {
                line_ite += sprintf(&line[line_ite], "   ");
                buf[i] = ' ';
            }
        }

        line_ite += sprintf(&line[line_ite], " |%s|\n", buf);
        my_str_ite += 16;
    }

    line[line_ite - 1] = '\0';
    *buffer = line;

    return line_nbr * LINE_SIZE;
}

void hexdump(void *addr, size_t len) {
  char* dump;
  _hexdump(addr,len,&dump);
  printf("buffer len:%d\n", len);
  printf("%s\n",dump);
  free(dump);
}

}
