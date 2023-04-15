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
	char timeString[10];
	char startTimeString[8];
	const char am[3] = "AM";
	const char pm[3] = "PM";

	struct tm tm = *localtime(&startTime);
	int startHour;
	if(tm.tm_hour < 1) {
		startHour = tm.tm_hour + 12;
	} else if(tm.tm_hour >= 13) {
		startHour = tm.tm_hour - 12;
	} else {
		startHour = tm.tm_hour;
	}
	sprintf(startTimeString, "%02d:%02d%s", startHour, tm.tm_min, (tm.tm_hour < 12) ? am : pm );
	char filename[25];
	int filenameSuffixInteger = 0;
	FILE *logFile;
	
	int lineNumber = 4;
	int exit = 0;

	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	
	while(1) {
		sprintf( filename, "tasktimer_%02d-%02d-%04d_%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, filenameSuffixInteger );
		logFile = fopen( filename, "r" );
		if( logFile != NULL ) {
			fclose(logFile);
			filenameSuffixInteger++;
		} else {
			initscr();
			printw("Log file: %s\n\nPress Spacebar to save a time, P to pause, X to exit.\n\nElapsed: 00:00:00 ", filename);
			refresh();
			break;
		}
	}

	start_color();
	use_default_colors();
	init_pair(1, COLOR_GREEN, -1);

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
				fprintf(logFile, "[ Start: %s | Elapsed: %s | Note: %s ]\n\r", startTimeString, timeString, note);
				fclose(logFile);
				timeout(0);
				noecho();
				lineNumber++;
				startTime = time(NULL);
				tm = *localtime(&startTime);
				int startHour;
				if(tm.tm_hour < 1) {
					startHour = tm.tm_hour + 12;
				} else if(tm.tm_hour >= 13) {
					startHour = tm.tm_hour - 12;
				} else {
					startHour = tm.tm_hour;
				}
				sprintf(startTimeString, "%02d:%02d%s", startHour, tm.tm_min, (tm.tm_hour < 12) ? am : pm );
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
