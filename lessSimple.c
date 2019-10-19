#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>

#define BUFFERSIZE (size_t)1024*1024

int main(void)
{
    unsigned long long chars=0, words=0, lines=0;
    int c;
    bool gotspace = true;

    char *buf = malloc(BUFFERSIZE);
    int length;

    while ((length = read(0, buf, BUFFERSIZE))) {

        for (int index = 0; index < length; ++index) {
            chars++;
            c = buf[index];
            if (isspace(c))
            {
                gotspace = true;
                if (c == '\n')
                    lines++;
            }
            else if(gotspace)
            {
                gotspace = false;
                words++;
            }
        }
    }
    free(buf);
    printf("%llu %llu %llu\n", lines, words, chars);
}
