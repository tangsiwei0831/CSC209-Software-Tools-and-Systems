#include <stdio.h>

int main(){
    char phone[11];
    int number;

    scanf("%s %d", phone, &number);

	if (number == -1){
		printf("%s\n", phone);
		return 0;
	}else if (number>=0 && number <= 9){
		printf("%c\n", phone[number]);
		return 0;
	} else {
		printf("ERROR\n");
		return 1;
	}
}