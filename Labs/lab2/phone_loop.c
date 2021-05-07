#include<stdio.h>  
  
int main(){  
    char phone[11];  
    int number;  
    int res = 0;  
  
    scanf("%s", phone);  
  
    while (scanf("%d", &number) != EOF){  
  
        if (number == -1){  
            printf("%s\n", phone);  
        } else if (number>=0 && number <= 9){  
            printf("%c\n", phone[number]);  
        } else {  
            printf("ERROR\n");  
            res = 1;  
        }  
          
    }  
      
    return res;  
}  