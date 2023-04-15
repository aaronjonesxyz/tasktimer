#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

int main() {
	time_t startTime = time(NULL);
	time_t currentTime;
	time_t elapsedTime;
	time_t previousElapsedTime = 0;
	struct tm* elapsedTimeInfo;
	char timeString[9];

	struct tm tm = *localtime(&startTime);
	char filename[25];
	int filenameSuffixInteger = 0;
	FILE *logFile;
	
	int lineNumber = 4;
	int exit = 0;

	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);

	while(!exit) {
		currentTime = time(NULL);
		elapsedTime = currentTime - startTime;
		if( elapsedTime != previousElapsedTime ) {
			elapsedTimeInfo = gmtime(&elapsedTime);
			strftime(timeString, sizeof(timeString), "%H:%M:%S", elapsedTimeInfo);
			mvprintw(lineNumber, 0, "\rElapsed: %s ", timeString);
			refresh();
			previousElapsedTime = elapsedTime;
		}
		char input = getch();

		switch(input) {
			case ' ':
				logFile = fopen(filename, "a");
				char note[100];
				attron(COLOR_PAIR(1));
				printw("Note: ");
				refresh();
				echo();
				timeout(-1);
				getstr(note);
        printw("%s",note);
				fprintf(logFile, "[ %s | %s ]\n\r", timeString, note);
				fclose(logFile);
				timeout(0);
				noecho();
				lineNumber++;
				startTime = time(NULL);
				attroff(COLOR_PAIR(1));
				break;
			case 'p':
				getch();
				timeout(-1);
				mvprintw(lineNumber, 0, "\r Paused: %s ", timeString);
				refresh();
				while(1) {
					if( getch() == 'p' ) {
						break;
					}
				}
				startTime += time(NULL) - currentTime;
				timeout(0);
				break;
			case 'x':
				exit = 1;
				break;
			default:
				break;
		}
	}
	endwin();
	return 0;
}
