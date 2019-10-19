#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

int main(void)
{
	unsigned long long chars=0, words=0, lines=0;
	int c;
	bool gotspace = true;

	while((c = getchar()) != EOF)
	{
		chars++;
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
	printf("%llu %llu %llu\n", lines, words, chars);
}
