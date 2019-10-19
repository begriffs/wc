#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <unistd.h>

struct counts
{
	long chars;
	long words;
	long lines;

	char cfirst;
	char clast;
};

struct chunk
{
	int fd;
	off_t offset;
	size_t length;
};

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define BUFSZ 1024*1024

struct counts *count_chunk(struct chunk *chunk)
{
	long chars=0, words=0, lines=0;
	struct counts *ret = calloc(1, sizeof(struct counts));
	bool gotspace = true;
	char c = '\0', *buf = malloc(BUFSZ);
	ssize_t index, n, remaining = chunk->length;

	lseek(chunk->fd, chunk->offset, SEEK_SET);
	while ((n = read(chunk->fd, buf, MIN(BUFSZ, remaining))) > 0)
	{
		if (!ret->cfirst)
			ret->cfirst = buf[0];
		for (index = 0; index < n; ++index)
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
		remaining -= n;
	}
	free(buf);

	ret->chars = chars;
	ret->words = words;
	ret->lines = lines;
	ret->clast = c;
	return ret;
}

int main(int argc, const char **argv)
{
	off_t filesz;
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
	filesz = in_stat.st_size;

	workers = malloc(nWorkers * sizeof(pthread_t));
	chunks = malloc(nWorkers * sizeof(struct chunk));
	for (i = 0; i < nWorkers; i++)
	{
		chunks[i].fd = open(path, O_RDONLY);
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
		close(chunks[i].fd);
		sum.lines += result->lines;
		sum.words += result->words;
		sum.chars += result->chars;
		/* don't double-count a word split between chunks */
		if (sum.clast && !isspace(sum.clast) && !isspace(result->cfirst))
			sum.words--;
		sum.cfirst = result->cfirst;
		sum.clast = result->clast;
		free(result);
	}

	printf("%ld %ld %ld\n", sum.lines, sum.words, sum.chars);
	free(workers);
	free(chunks);
}
