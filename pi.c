/* I, the copyright holder of this work, hereby release it into the public
   domain. This applies worldwide.

   In case this is not legally possible: I grant anyone the right to use this
   work for any purpose, without any conditions, unless such conditions are
   required by law. */

#include <stdio.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32)

#include <conio.h>
#include <stdlib.h>

#define getchar_blocking getch

void clear_screen(void)
{
	system("cls");
}

#else

#include <termios.h>

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

void clear_screen(void)
{
	printf("\033[2J");
}

#endif

const char *pi_digits = 
"14159265358979323846264338327950288419716939937510"
"58209749445923078164062862089986280348253421170679";

int is_return(char c)
{
	return c == '\n' || c == '\r';
}

int main(void)
{
	clear_screen();
	printf("Learn pi!\n\n");
	
	int max_digits = strlen(pi_digits);
	
	while (1) {
		printf("> 3.");
		
		int victory = 1;
		time_t start_time = 0;
				
		for (int pos = 0; pos < max_digits; pos++) {
			char digit = pi_digits[pos];
			char answer = getchar_blocking();
			
			if (answer == 27) {
				putchar('\n');
				return 0;
			}

			if (is_return(answer)) {
				if (pos > 0) {
					time_t taken = time(NULL) - start_time;
					if (taken == 0) {
						printf("\n\nThe next digit is %c.\n"
						       "You got %i %s right.\n\n",
					           digit, pos, pos == 1 ? "digit" : "digits");
					} else {
						float dps = (float)pos / taken;		
						printf("\n\nThe next digit is %c.\n"
						       "You got %i %s right in %lus (%.2f p/s).\n\n",
					           digit, pos, pos == 1 ? "digit" : "digits",
							   (unsigned long)taken, dps);
					}					
				} else {
					printf("\nThe first digit is %c.\n\n", digit);
				}
				
				victory = 0;
				break;
			}
			
			if (answer < '0' || answer > '9')
				continue;

			if (pos == 0)
				start_time = time(NULL);
			
			putchar(answer);
			
			if (answer != digit) {
				time_t taken = time(NULL) - start_time;
				if (taken == 0) {
					printf("\n\nThe next digit is not %c but %c.\n"
					       "You got %i %s right.\n\n",
					       answer, digit, pos, pos == 1 ? "digit" : "digits");
				} else {
					float dps = (float)pos / taken;
					printf("\n\nThe next digit is not %c but %c.\n"
					       "You got %i %s right in %lus (%.2f p/s).\n\n",
					       answer, digit, pos, pos == 1 ? "digit" : "digits",
						   (unsigned long)taken, dps);
				}
				 
				victory = 0;
				break;
			}
		}
		
		if (victory) {
			time_t taken = time(NULL) - start_time;
			float dps = (float)max_digits / taken;		
			printf("\n\nWin! You got 100 digits right in %lus (%.2f p/s).\n\n",
			       (unsigned long)taken, dps);
		}
		
		printf("Press return key to retry.");
		while (1) {
			char c = getchar_blocking();
			if (c == 27) {
				printf("\n");
				return 0;
			} else if (is_return(c)) {
				break;
			}
		}	
			
		printf("\n");
		clear_screen();
	}
	
	return 0;
}