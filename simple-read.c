#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#define BUFSZ 1024*1024

int main(void)
{
	unsigned long long chars=0, words=0, lines=0;
	bool gotspace = true;
	char *buf = malloc(BUFSZ), c;
	ssize_t index, length;

	while ((length = read(STDIN_FILENO, buf, BUFSZ)) > 0)
	{
		for (index = 0; index < length; ++index)
		{
			chars++;
			if (isspace(c = buf[index]))
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
