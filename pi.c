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
	printf("Learn pi!\n\n");
	
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
				printf("\nNext digit would have been %c. You got %i %s right.\n\n", digit, pos, pos == 1 ? "digit" : "digits");
				
				victory = 0;
				break;
			}
			
			if (answer < '0' || answer > '9')
				continue;
			
			putchar(answer);
			
			if (answer == '\r') {
				break;
			} else if (answer != digit) {
				printf("\nSorry, next digit is %c, not %c. You got %i %s right.\n\n", digit, answer, pos, pos == 1 ? "digit" : "digits");
				
				victory = 0;
				break;
			}
		}
		
		if (victory)
			printf("\nWin! You got 100 digits.");
	}
	
	return 0;
}