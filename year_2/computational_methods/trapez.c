#include <stdio.h>
#include <math.h>
#define I 0.016832889701405 //(аналитически [1,2])
double f(double x)
{
    return 1/((25*x*x + 1)*sqrt(x));
}

double integral(int n, int a, int b)
{
    double h = (b - a)/(double)n;
    double s = 1/2*(f(a) + f(b));
    double x = a + h;

    for (int i = 1; i < n; i++)
  {
      s = s + f(x);
      x = x + h;
  }
    return s*h;  
}

int main() 
{
  double T16 = integral(16,1,2);
  double T32 = integral(32,1,2);
  double T64 = integral(64,1,2);

  printf("T16 = %lf\nT32 = %lf\nT64 = %lf\n", T16, T32, T64);
  printf("betta16 = %lf\nbetta32 = %lf\nbetta64 = %lf\n", I - T16, I - T32, I - T64);
}





