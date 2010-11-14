#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

typedef struct {
  char *data;
  int len;
} data_t;

float elapsed(struct timeval before, struct timeval after)
{
  float ans = (float) (after.tv_sec - before.tv_sec);
  ans += ((float) (after.tv_usec - before.tv_usec)) / 1000000;
  return ans;
}

void compare(data_t* one, data_t* two, int num)
{
  int i;
  for (i=0; i<num; i++)
  {
    if (one[i].len != two[i].len)
      printf("lengths screwy\n");
    if (strcmp(one[i].data, two[i].data))
      printf("data screwy: %s  \\ %s\n", one[i].data, two[i].data);
  }  
}

data_t* create(int num, int length)
{
  int i,j;
  data_t* answer = malloc(sizeof(data_t) * num);

  for (i=0; i<num; i++)
  {
    answer[i].len = length - (i % 3);   /* "random" length */
    answer[i].data = malloc(sizeof(char) * answer[i].len);
    for (j=0; j<answer[i].len; j++)
      answer[i].data[j] = (char) (91 + (17*i+3*j)%26);  /* "random" character */
    answer[i].data[answer[i].len-1]= '\0';
  }
  
  return answer;
}

void naive_write(char* filename, data_t* data, int n)
{
  FILE *f;
  int i;
  f = fopen (filename, "wb" );
  fwrite(&n, sizeof(n), 1, f);
  for (i=0;i<n;i++)
  {
    fwrite(&data[i].len, sizeof(data[i].len), 1, f);
    fwrite(data[i].data, sizeof(char), data[i].len, f);
  }
  fclose(f);
}

void flat_write(char* filename, data_t* data, int n)
{
  FILE *f;
  int i;
  f = fopen (filename, "wb" );
  fwrite(&n, sizeof(n), 1, f);
  for (i=0;i<n;i++)
    fwrite(&data[i].len, sizeof(data[i].len), 1, f);

  for (i=0;i<n;i++)
    fwrite(data[i].data, sizeof(char), data[i].len, f);

  fclose(f);
}

data_t* onecopy_read(char* filename)
{
  data_t* answer;
  FILE *f;
  int i, n, total;
  
  f = fopen (filename, "rb");
  fread(&n, sizeof(n), 1, f);
  answer = malloc(n * sizeof(data_t));

  total = 0;
  for (i=0;i<n;i++)
  {
    fread(&answer[i].len, sizeof(answer[i].len), 1, f);
    total += answer[i].len;
  }

  answer[0].data = malloc(sizeof(char)*total);
  fread(answer[0].data, sizeof(char), total, f);  /* one giant fread */

  total=0;
  for (i=0;i<n;i++)
  {
    answer[i].data = answer[0].data + sizeof(char)*total;      /* set all the ptrs */
    total += answer[i].len;
  }
  
  return answer;
}

data_t* zerocopy_read(char* filename)
{
  data_t* answer;
  int f;
  int i, n, total;
  char *data_ptr;
  int* hdr_ptr;
  struct stat sb;
  
  f = open (filename, O_RDONLY);
  fstat(f, &sb);
  hdr_ptr = mmap(0, sb.st_size, PROT_READ, MAP_PRIVATE, f, 0);

  n = *hdr_ptr++;
  answer = malloc(n*sizeof(data_t));
  
  for (i=0;i<n;i++)
    answer[i].len = *hdr_ptr++;

  data_ptr = (char*) hdr_ptr;

  total=0;
  for (i=0;i<n;i++)
  {
    answer[i].data = data_ptr + sizeof(char)*total;
    total += answer[i].len;
  }
  
  return answer;
}

data_t* naive_read(char* filename)
{
  data_t* answer;
  FILE *f;
  int i, n;
  f = fopen (filename, "rb");
  fread(&n, sizeof(n), 1, f);
  answer = malloc(n * sizeof(data_t));
  for (i=0;i<n;i++)
  {
    fread(&answer[i].len, sizeof(answer[i].len), 1, f);
    answer[i].data = malloc(sizeof(char) * answer[i].len);
    fread(answer[i].data, sizeof(char), answer[i].len, f);
  }
  return answer;
}

int main(int argc, char *argv[])
{
  struct timeval before,after;
  int i,j;
  int n = 1000000;  /* number of strings */
  int len = 150;    /* "length" of strings (mild variation) */
  data_t* orig = create(n, len);
  naive_write("naive.bin", orig, n);
  flat_write("flat.bin", orig, n);

  gettimeofday(&before,NULL);
  data_t* naive = naive_read("naive.bin");
  gettimeofday(&after,NULL);
  printf(" naive time: %f\n", elapsed(before,after));

  gettimeofday(&before,NULL);
  data_t* onecopy = onecopy_read("flat.bin");
  gettimeofday(&after,NULL);
  printf(" 1copy time: %f\n", elapsed(before,after));

  gettimeofday(&before,NULL);
  data_t* zerocopy = zerocopy_read("flat.bin");
  gettimeofday(&after,NULL);
  printf(" 0copy time: %f\n", elapsed(before,after));
  
  compare(naive,onecopy, n);
  compare(naive,zerocopy,n);
  return 0;
}
