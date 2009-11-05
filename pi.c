/* I, the copyright holder of this work, hereby release it into the public
   domain. This applies worldwide.

   In case this is not legally possible: I grant anyone the right to use this
   work for any purpose, without any conditions, unless such conditions are
   required by law. */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#if defined(_WIN32)

#include <conio.h>
#include <stdlib.h>

#define getchar_blocking (getch)
#define fmax(a, b) ((float)(a) > (float)(b) ? (float)(a) : (float)(b))

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

const char *PI_DIGITS = 
"14159265358979323846264338327950288419716939937510"
"58209749445923078164062862089986280348253421170679";

const int PI_DIGIT_COUNT = (100);
const int ESCAPE_KEYCODE = (27);
const float POINTS_PER_DIGITS = 10.0f; // points for every digit
const float POINTS_PER_DPS = 10.0f;    // for every digit per second

#ifndef TRUE
const int TRUE = 1;
#endif
#ifndef FALSE
const int FALSE = 0;
#endif
#ifndef BOOL
typedef int BOOL;
#endif

BOOL is_return(char c)
{
	return c == '\n' || c == '\r';
}

BOOL is_digit(char c)
{
	return '0' <= c && c <= '9';
}

void print_welcome()
{
	printf("Learn pi!\n\n");
}

void print_prompt()
{
	printf("> 3.");
}

typedef enum 
{
	ReadDigitCorrect,
	ReadDigitIncorrect,
	ReadDigitCancel,
	ReadDigitQuit
} ReadDigitResultType;

typedef enum
{
	PromptContinue,
	PromptQuit
} PromptContinueResult;

typedef struct 
{
	ReadDigitResultType type;
	int pos;
	char correct_answer;
	char given_answer;
} ReadDigitResult;

ReadDigitResult read_digit(int pos)
{
	ReadDigitResult result;
	result.pos = pos;
	result.correct_answer = PI_DIGITS[pos];
	result.given_answer = getchar_blocking();
	
	if (result.given_answer == ESCAPE_KEYCODE) {
		result.type = ReadDigitQuit;
		return result;
	}

	if (is_return(result.given_answer)) {
		result.type = ReadDigitCancel;
		return result;
	}
	
	if (!is_digit(result.given_answer))
		return read_digit(result.pos);

	putchar(result.given_answer);
	
	BOOL correct = result.correct_answer == result.given_answer;
	result.type = correct ? ReadDigitCorrect : ReadDigitIncorrect;
	
	return result;
}

int calculate_score(int num_correct, time_t time_taken)
{
	float dps = (float)num_correct / fmax(1, (float)time_taken);
	float fscore = num_correct * POINTS_PER_DIGITS + dps * POINTS_PER_DPS;
	
	return (int)fscore;
}

void print_score(int num_correct, time_t time_taken)
{
	if (time_taken == 0) {
		printf("You got %i %s right.\n", 
			num_correct, num_correct == 1 ? "digit" : "digits");
	} else {
		float dps = (float)num_correct / time_taken;
		printf("You got %i %s right in %lus (%.2f p/s).\n",
	           num_correct, num_correct == 1 ? "digit" : "digits",
			   (unsigned long)time_taken, dps);
	}
	
	int score = calculate_score(num_correct, time_taken);
	printf("\nYour score: %i\n\n", score);
}

void print_digit_result(ReadDigitResult result, time_t start_time)
{
	time_t taken = time(NULL) - start_time;
	
	switch (result.type) {
		case ReadDigitQuit:
			printf("\n");
			break;
			
		case ReadDigitCancel:
			if (result.pos > 0) {
				printf("\n\nThe next digit is %c.\n", result.correct_answer);
				print_score(result.pos, taken);
			} else {	
				printf("\n\nThe first digit is %c.\n\n", result.correct_answer);
			}
			break;
		
		case ReadDigitIncorrect:
			printf("\n\nThe next digit is %c, not %c.\n", result.correct_answer,
			       result.given_answer);
			print_score(result.pos, taken);
			break;
			
		default:
			// do nothing
			break;
	}
}

void print_victory(int num_correct, time_t start_time)
{
	time_t taken = time(NULL) - start_time;
	
	printf("Victory!\n\n");
	print_score(num_correct, taken);
}

PromptContinueResult prompt_continue()
{
	printf("Press return key to retry.");
	while (1) {
		char c = getchar_blocking();
			
		if (c == ESCAPE_KEYCODE) {			
			putchar('\n');
			return PromptQuit;
		} else if (is_return(c)) {
			putchar('\n');
			return PromptContinue;
		}
	}		
}

int main(void)
{
	clear_screen();
	print_welcome();
	
	while (TRUE) {
		print_prompt();
		
		BOOL victory = TRUE;
		time_t start_time = 0;
				
		for (int pos = 0; pos < PI_DIGIT_COUNT; pos++) {
			ReadDigitResult result = read_digit(pos);
			
			if (pos == 0)
				start_time = time(NULL);
			
			print_digit_result(result, start_time);
			
			BOOL exit_loop = FALSE;
			switch (result.type) {
				case ReadDigitIncorrect:				
				case ReadDigitCancel:
					victory = FALSE;
					exit_loop = TRUE;
					break;

				case ReadDigitQuit:
					return 0;

				default:
					// do nothing
					break;
			}
			
			if (exit_loop)
				break;
		}
		
		if (victory)
			print_victory(PI_DIGIT_COUNT, start_time);
		
		PromptContinueResult cont = prompt_continue();
		if (cont == PromptQuit)
			return 0;
		
		clear_screen();
	}
	
	return 0;
}