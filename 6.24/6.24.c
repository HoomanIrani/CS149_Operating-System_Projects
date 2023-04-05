#include <stdio.h>

void SwapValues (int* val1, int* val2, int* val3, int* val4) {

    int temp;
    temp = *val1;
    *val1 = *val2;
    *val2 = temp;

    temp = *val3;
    *val3 = *val4;
    *val4 = temp;

}

int main () {

    int val1, val2, val3, val4;

    scanf("%d",&val1);
    scanf("%d",&val2);
    scanf("%d",&val3);
    scanf("%d",&val4);

    SwapValues(&val1, &val2, &val3, &val4);
    printf("%d %d %d %d\n",val1,val2,val3,val4);

    return 0;
}