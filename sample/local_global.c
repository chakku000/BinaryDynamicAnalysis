#include <stdio.h>

int a;

int main(){
    int b;
    printf("&a = %p\n",&a);
    printf("&b = %p\n",&b);
    a = 10;
    b = 20;
    printf("a = %d\n",a);
    printf("b = %d\n",b);
}
