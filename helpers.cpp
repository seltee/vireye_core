#include <string.h>
#include "helpers.h"

void delayByLoop(uint32_t nCount){
	while(nCount--);
}

void reverse(char s[]){
	unsigned short i, j;
	char c;
	for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

void itoa(int n, char s[]){
	int i, sign;
	if ((sign = n) < 0){  /* записываем знак */
		n = -n;          /* делаем n положительным числом */
	}
	i = 0;
	do {       /* генерируем цифры в обратном порядке */
		s[i++] = n % 10 + '0';   /* берем следующую цифру */
	} while ((n /= 10) > 0);     /* удаляем */
	if (sign < 0){
		s[i++] = '-';
	}
	s[i] = '\0';
	reverse(s);
}

bool cmp(char *a, char *b){
	while(*a == *b){
		if (*a == 0){
			return true;
		}
		a++;
		b++;
	}
	return false;
}
