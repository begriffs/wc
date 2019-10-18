#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

struct counts
{
	long chars;
	long words;
	long lines;
};

struct chunk
{
	FILE *f;
	long offset;
	long length;
};

struct counts *count_chunk(struct chunk *chunk)
{
	struct counts *ret = calloc(1, sizeof(struct counts));
	int c='\0', prev;

	fseek(chunk->f, chunk->offset, SEEK_SET);
	for (prev=' ';
		 ret->chars < chunk->length && (c = fgetc(chunk->f)) != EOF;
		 prev=c)
	{
		ret->chars++;
		if (isspace(prev) && !isspace(c))
			ret->words++;
		if (c == '\n')
			ret->lines++;
	}

	/* peek ahead and if we stopped mid-word, don't count the
	 * word because another thread will count the end of it */
	if (!isspace(c))
	   if ((c = fgetc(chunk->f)) != EOF && !isspace(c))
		ret->words--;

	return ret;
}

int main(int argc, const char **argv)
{
	long filesz;
	int i, nWorkers = 4;
	pthread_t *workers;
	struct chunk *chunks;
	struct counts *result, sum = {0};
	struct stat in_stat;
	const char *path;

	if (argc == 3)
	{
		if (sscanf(argv[1], "%d", &nWorkers) < 1 || nWorkers < 1)
		{
			fputs("Workers argument must be integer >= 1\n", stderr);
			return EXIT_FAILURE;
		}
		path = argv[2];
	}
	else
	{
		fprintf(stderr, "Usage: %s nWorkers path\n", argv[0]);
		return EXIT_FAILURE;
	}

	stat(path, &in_stat);
	filesz = (long)in_stat.st_size;

	workers = malloc(nWorkers * sizeof(pthread_t));
	chunks = malloc(nWorkers * sizeof(struct chunk));
	for (i = 0; i < nWorkers; i++)
	{
		chunks[i].f = fopen(path, "r");
		chunks[i].offset = i*(filesz/nWorkers);
		/* due to division rounding, have the last thread read to EOF */
		chunks[i].length = (i < nWorkers-1) ? filesz/nWorkers : LONG_MAX;
		pthread_create(&workers[i], NULL,
			(void *(*)(void *))count_chunk, &chunks[i]);
	}

	/* run free, threads! */

	for (i = 0; i < nWorkers; i++)
	{
		pthread_join(workers[i], (void **)&result);
		fclose(chunks[i].f);
		sum.lines += result->lines;
		sum.words += result->words;
		sum.chars += result->chars;
		free(result);
	}

	printf("%ld %ld %ld\n", sum.lines, sum.words, sum.chars);
	free(workers);
	free(chunks);
}
