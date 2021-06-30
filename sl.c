/*========================================
 *    sl.c: SL version 5.03
 *        Copyright 1993,1998,2014-2015
 *                  Toyoda Masashi
 *                  (mtoyoda@acm.org)
 *        Last Modified: 2014/06/03
 *========================================
 */
/* sl version 5.03 : Fix some more compiler warnings.                        */
/*                                              by Ryan Jacobs    2015/01/19 */
/* sl version 5.02 : Fix compiler warnings.                                  */
/*                                              by Jeff Schwab    2014/06/03 */
/* sl version 5.01 : removed cursor and handling of IO                       */
/*                                              by Chris Seymour  2014/01/03 */
/* sl version 5.00 : add -c option                                           */
/*                                              by Toyoda Masashi 2013/05/05 */
/* sl version 4.00 : add C51, usleep(40000)                                  */
/*                                              by Toyoda Masashi 2002/12/31 */
/* sl version 3.03 : add usleep(20000)                                       */
/*                                              by Toyoda Masashi 1998/07/22 */
/* sl version 3.02 : D51 flies! Change options.                              */
/*                                              by Toyoda Masashi 1993/01/19 */
/* sl version 3.01 : Wheel turns smoother                                    */
/*                                              by Toyoda Masashi 1992/12/25 */
/* sl version 3.00 : Add d(D51) option                                       */
/*                                              by Toyoda Masashi 1992/12/24 */
/* sl version 2.02 : Bug fixed.(dust remains in screen)                      */
/*                                              by Toyoda Masashi 1992/12/17 */
/* sl version 2.01 : Smoke run and disappear.                                */
/*                   Change '-a' to accident option.                         */
/*                                              by Toyoda Masashi 1992/12/16 */
/* sl version 2.00 : Add a(all),l(long),F(Fly!) options.                     */
/*                                              by Toyoda Masashi 1992/12/15 */
/* sl version 1.02 : Add turning wheel.                                      */
/*                                              by Toyoda Masashi 1992/12/14 */
/* sl version 1.01 : Add more complex smoke.                                 */
/*                                              by Toyoda Masashi 1992/12/14 */
/* sl version 1.00 : SL runs vomiting out smoke.                             */
/*                                              by Toyoda Masashi 1992/12/11 */

#include <curses.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>


int add_art(int x);
int MAX_COL = 0;
int MAX_ROW = 0;

char **buffer;

int my_mvaddstr(int y, int x, char *str);

int my_mvaddstr(int y, int x, char *str)
{
    for ( ; x < 0; ++x, ++str)
        if (*str == '\0')  return ERR;
    for ( ; *str != '\0'; ++str, ++x)
        if (mvaddch(y, x, *str) == ERR)  return ERR;
    return OK;
}

void returnNthAsciiFileName(const char *path, int count, char* fileName)
{
    struct dirent *dp;
    DIR *dir = opendir(path);
    int index = 0;

    // Unable to open directory stream
    if (!dir) 
        return; 

    while ((dp = readdir(dir)) != NULL)
    {
        if( dp->d_name[0] == '@' )
            index++;
        if( index == count ) {
            strcpy(fileName,path);
            strcpy(fileName+strlen(path), dp->d_name);
            break;
        }
    }

    // Close directory stream
    closedir(dir);
}

int returnAsciiArtCount(const char *path)
{
    struct dirent *dp;
    DIR *dir = opendir(path);
    int index = 0;

    // Unable to open directory stream
    if (!dir) 
        return 0; 

    while ((dp = readdir(dir)) != NULL)
    {
        if( dp->d_name[0] == '@' )
            index++;
    }

    // Close directory stream
    closedir(dir);
    return index;
}

int main(int argc, char *argv[])
{
    if( argc != 1 && argc != 3 ) {
        printf("Bad Parameter\n");
        exit(1);
    }

    int x, i;
    int artNum = 0;
    const char* path = "/usr/share/ascii_art/";

    int total_art = returnAsciiArtCount(path);
    if( total_art == 0 ) {
        printf("No Ascii Art Found\n");
        return 0;
    }

    if( argc == 3 ) {
        if (strcmp(argv[1], "-n") == 0 ) {
            artNum = atoi(argv[2])% total_art;
        }
        else {
            printf("Bad Parameter\n");
            exit(1);
        }
    }

    srand(time(0));
    if( artNum == 0 ) {
        artNum = rand()%total_art +1;
    }

    initscr();
    signal(SIGINT, SIG_IGN);
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    leaveok(stdscr, TRUE);
    scrollok(stdscr, FALSE);

    char fileName[50];
    returnNthAsciiFileName(path, artNum, fileName);

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    size_t lCount = 0;
    fp = fopen(fileName, "r");
    if (fp == NULL) {
        printf("File not found\n");
        return 1;
    }

    buffer = (char **)malloc(LINES * sizeof(char *));
    for (int i=0; i<LINES; i++)
         buffer[i] = (char *)malloc(COLS * sizeof(char));

    //Assumption that animation always fits in the screen
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
        memcpy(buffer[lCount++],line, read);
        if( read > MAX_COL )
            MAX_COL = read;
        //printf("%s", line);
    }
    MAX_ROW = lCount;

    fclose(fp);

    for (x = COLS - 1; ; --x) {
        if (add_art(x) == ERR) break;
        getch();
        refresh();
        usleep(40000);
    }
    mvcur(0, COLS - 1, LINES - 1, 0);
    endwin();

    for(int i=0; i<LINES; i++)
        free(buffer[i]);
    free(buffer);

    return 0;
}

int add_art(int x)
{
    int y, i, dy = 0;

    if (x < - MAX_COL)  return ERR;
    y = LINES / 2 - 5;

    for (i = 0; i <= MAX_ROW; ++i) {
        my_mvaddstr(y + i + dy, x + 53, buffer[i]);
    }
    return OK;
}




