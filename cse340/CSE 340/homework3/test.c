#include <stdio.h>
int i;
void g();
void f()
{
 int i;
 i = 3;
 {
 int i;
 i = 2;
 g();
 }
 g();
}
void g()
{
 i = i + 1;
 printf("g: %d\n", i);
}
int main()
{
 i = 5;
 g();
 f();
 return 0;
} 
