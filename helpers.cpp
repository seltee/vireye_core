#include <string.h>
#include "helpers.h"
#include "hardware.h"

void delayByLoop(uint32_t nCount){
	while(nCount--);
}

void reverse(char s[]){
	unsigned int i, j;
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

char toUpper(char a){
	return (a >= 'a' && a <= 'z') ? a-0x20 : a;
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

bool cmpi(char *a, char *b){
	while(toUpper(*a) == toUpper(*b)){
		if (*a == 0){
			return true;
		}
		a++;
		b++;
	}
	return false;
}

unsigned int __umodsi3(unsigned int a, unsigned int b){
	return a % b;
}

int __modsi3(int a, int b){
	return a % b;
}

unsigned int __udivsi3(unsigned int n, unsigned int d){
	return n/d;
}

int __divsi3(int n, int d){
	return n/d;
}

unsigned int rand(){
	unsigned int number = 0;
	for (int i = 0; i < 32; i++){
		number += getAdcValueForXAxis() << i;
		number += getAdcValueForYAxis() << i;
		number += getTimer() << 1;
		if (number & 0x80000000 || number & 0x800000 || number & 0x8000 || number & 0x80){
			number >>= 1;
		}
	}
	return number;
}
