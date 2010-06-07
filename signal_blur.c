#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LENGTH 5000

// a bad impersonation of a gaussian filter
const float filter[] = {0.01f, 0.2f, 0.58f, 0.2f, 0.01f};

void naive(float* in, float* out)
{
  int i,j;
  for (i=0;i<LENGTH-4;i++)
  {
    out[i]=0.0f;
    for (j = 0;j<5;j++)
      out[i] += filter[j] * in[i+j];
  }
}

void naive_restrict(float *restrict in, float *restrict out)
{
  int i,j;
  for (i=0;i<LENGTH-4;i++)
  {
    out[i]=0.0f;
    for (j = 0;j<5;j++)
      out[i] += filter[j] * in[i+j];
  }
}


void sum_variable(float* in, float* out)
{
  int i,j;
  for (i=0;i<LENGTH-4;i++)
  {
    float sum = 0.0f;
    for (j = 0;j<5;j++)
      sum += filter[j] * in[i+j];
    out[i] = sum;
  }
}

float elapsed(struct timeval before, struct timeval after)
{
  float ans = (float) (after.tv_sec - before.tv_sec);
  ans += ((float) (after.tv_usec - before.tv_usec)) / 1000000;
  return ans;
}

float gogo( void (*func)(float*,float*), float* in, float* out)
{
  struct timeval before,after;
  int i;

  func(in,out);
  gettimeofday(&before,NULL);
  for (i=0;i<10000;i++)
    func(in,out);
  gettimeofday(&after,NULL);

  return elapsed(before,after);
}


int main(int argc, char *argv[])
{
  float *in;
  float *ans;
  int i;

  in = valloc(sizeof(float) * LENGTH);
  ans = valloc(sizeof(float) * LENGTH);

  for (i=0;i<LENGTH;i++)
    in[i]=(float) i;

  printf("%s\n", argv[0]);
  printf("  naive: %f\n", gogo(naive,in,ans));
  printf("  naive_restrict: %f\n", gogo(naive_restrict,in,ans));
  printf("  sum_variable: %f\n", gogo(sum_variable,in,ans));
  printf("\n\n");
  return 1;
}
