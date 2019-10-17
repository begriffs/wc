#include <stdio.h>
#include <ctype.h>

int main(void)
{
	unsigned long long chars=0, words=0, lines=0;
	int c, prev;

	for (prev=' '; (c = getchar()) != EOF; prev=c)
	{
		chars++;
		if (isspace(prev) && !isspace(c))
			words++;
		if (c == '\n')
			lines++;
	}
	printf("%llu %llu %llu\n", lines, words, chars);
}
