/* I, the copyright holder of this work, hereby release it into the public
   domain. This applies worldwide.

   In case this is not legally possible: I grant anyone the right to use this
   work for any purpose, without any conditions, unless such conditions are
   required by law. */

#include <stdio.h>
#include <termios.h>
#include <string.h>

const char *pi_digits = "14159265358979323846264338327950288419716939937510";

int getchar_blocking(void)
{
	// http://forums.macrumors.com/showthread.php?t=418230
	
	struct termios oldt, newt;
	int ch;
	tcgetattr( 0, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( 0, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( 0, TCSANOW, &oldt );
	return ch;
}

int main(void)
{
	printf("\033[2JLearn pi!\n\n");
	
	int max_digits = strlen(pi_digits);
	
	while (1) {
		printf("> 3.");
		
		int victory = 1;		
		for (int pos = 0; pos < max_digits; pos++) {
			char digit = pi_digits[pos];
			char answer = getchar_blocking();
			
			if (answer == 27) {
				putchar('\n');
				return 0;
			}

			if (answer == '\n') {
				printf("\nNext digit would have been %c. You got %i %s right.\n\n",
				       digit, pos, pos == 1 ? "digit" : "digits");
				
				victory = 0;
				break;
			}
			
			if (answer < '0' || answer > '9')
				continue;
			
			putchar(answer);
			
			if (answer != digit) {
				printf("\nSorry, next digit is %c, not %c. You got %i %s right.\n\n",
				       digit, answer, pos, pos == 1 ? "digit" : "digits");
				
				victory = 0;
				break;
			}
		}
		
		if (victory)
			printf("\nWin! You got 100 digits.\n\n");
		
		printf("Press return key to retry.");
		while (1) {
			char c = getchar_blocking();
			if (c == 27) {
				printf("\n");
				return 0;
			} else if (c == '\n') {
				break;
			}
		}	
			
		printf("\n\033[2J");
	}
	
	return 0;
}