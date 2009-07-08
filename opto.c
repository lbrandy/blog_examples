#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define WIDTH 8192
//#define HEIGHT 8192

#define WIDTH 4096
#define HEIGHT 4096

//#define WIDTH 256
//#define HEIGHT 16384

float elapsed(struct timeval before, struct timeval after)
{
  float ans = (float) (after.tv_sec - before.tv_sec);
  ans += ((float) (after.tv_usec - before.tv_usec)) / 1000000;
  return ans;
}

void randomize(float *mat)
{
  int i;
  for (i=0;i<WIDTH*HEIGHT;i++)
    mat[i]=random();
}

float gogo( void (*func)(float*,float*,float*), float* one, float* two, float* ans)
{
  struct timeval before,after;
  int i;

  gettimeofday(&before,NULL);
  for (i=0;i<10;i++)
    func(one,two,ans);
  gettimeofday(&after,NULL);

  return elapsed(before,after);
}

void fast(float *in1, float *in2, float*out)
{
  int i;
  for (i=0;i<HEIGHT*WIDTH;i++)
    out[i]=in1[i]+in2[i];
}

void horiz_add(float *in1, float *in2, float *out)
{
  int i,j;
  for (i=0;i<HEIGHT;i++)
    for (j=0;j<WIDTH;j++)
      {
	int index = i * WIDTH + j;
	out[index]=in1[index]+in2[index];
      }
}

void vert_add(float *in1, float *in2, float *out)
{
  int i,j;
  for (j=0;j<WIDTH;j++)
    for (i=0;i<HEIGHT;i++)
      {
	int index = i * WIDTH + j;
	out[index]=in1[index]+in2[index];
      }
}

void vert_add2(float *in1, float *in2, float *out)
{
  int i,j;
  for (j=0;j<WIDTH/16;j++)
    for (i=0;i<HEIGHT;i++)
      {
	int index = i * WIDTH + j*16;
	out[index]=in1[index]+in2[index];
	out[index+1]=in1[index+1]+in2[index+1];
	out[index+2]=in1[index+2]+in2[index+2];
	out[index+3]=in1[index+3]+in2[index+3];
	out[index+4]=in1[index+4]+in2[index+4];
	out[index+5]=in1[index+5]+in2[index+5];
	out[index+6]=in1[index+6]+in2[index+6];
	out[index+7]=in1[index+7]+in2[index+7];
	out[index+8]=in1[index+8]+in2[index+8];
	out[index+9]=in1[index+9]+in2[index+9];
	out[index+10]=in1[index+10]+in2[index+10];
	out[index+11]=in1[index+11]+in2[index+11];
	out[index+12]=in1[index+12]+in2[index+12];
	out[index+13]=in1[index+13]+in2[index+13];
	out[index+14]=in1[index+14]+in2[index+14];
	out[index+15]=in1[index+15]+in2[index+15];
      }
}

int main(void)
{
  float *one;
  float *two;
  float *ans;

  one = valloc(sizeof(float) * WIDTH * HEIGHT);
  two = valloc(sizeof(float) * WIDTH * HEIGHT);
  ans = valloc(sizeof(float) * WIDTH * HEIGHT);
  
  randomize(one);
  randomize(two);

  printf("horiz: %f\n", gogo(horiz_add,one,two,ans));
  printf("vert1: %f\n", gogo(vert_add,one,two,ans));
  printf("vert2: %f\n", gogo(vert_add2,one,two,ans));
  printf("fast: %f\n", gogo(fast,one,two,ans));

  return 1;
}
