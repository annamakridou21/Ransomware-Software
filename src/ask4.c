#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

int polynomyal(int a2, int a1, int a0, int x) {
    return (a2*x*x)+(a1*x)+a0;      //upologismos polywnymou 2ou vathmou
}

void coordinates(char *point, int *x, int *y) {
    point++;        //agnow thn parenthesh 
    *x=atoi(point);             //to x einai to prwto stoixeio meta th parenthesh
    while (*point && *point!=',') {
        point++;
    }
    point++;  //proxwraw mexri meta to komma 
    *y=atoi(point);     //apothikeuw to y
}

//mathimatikes sunarthseis
double determinant(double a, double b, double c, double d) {
    return a*d-b*c;
}

//mathimatikes sunarthseis
double determinant3x3(double matrix[3][3]) {
    return matrix[0][0]*determinant(matrix[1][1], matrix[1][2], matrix[2][1], matrix[2][2])-matrix[0][1]*determinant(matrix[1][0], matrix[1][2], matrix[2][0], matrix[2][2])+matrix[0][2]*determinant(matrix[1][0], matrix[1][1], matrix[2][0], matrix[2][1]);
}

//Cramer's Rule gia 3X3 system
void cramer(int a1, int a2, int x1, int x2, int x3, int y1, int y2, int y3,int points) {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo=localtime(&rawtime);
    double coefficient_matrix[3][3]={{x1, x1*x1, 1},{x2, x2*x2, 1},{x3, x3*x3, 1}};
    double det_coefficient=determinant3x3(coefficient_matrix);

    double c_matrix[3][3]={{x1, x1*x1, y1},{x2, x2*x2, y2},{x3, x3*x3, y3}};
    double det_c=determinant3x3(c_matrix);

    int result_c=det_c/det_coefficient;
    
    printf("[INFO] [9096] [%02d-%02d-%04d %02d:%02d] Application Started\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, 1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min);
    printf("[INFO] [9096] [%02d-%02d-%04d %02d:%02d] Received %d different shares\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, 1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min,points);
    printf("[INFO] [9096] [%02d-%02d-%04d %02d:%02d] Computed that a2=%d and a1=%d\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, 1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min,a2,a1);
    printf("[INFO] [9096] [%02d-%02d-%04d %02d:%02d] Encryption key is: %d\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, 1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min,result_c);
}

void function(int argc, char *argv[]) {
    time_t rawtime;
    struct tm *timeinfo;
    int a1,a2,i,secret,x[10],y[10],result;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    srand(time(NULL));
    if (strcmp(argv[1], "slice")==0) {
        secret=atoi(argv[2]);
        a1=(rand()%100)+1;      //dhmiourgw ta random a1,a2
        a2=(rand()%100)+1;

        printf("[INFO] [9096] [%02d-%02d-%04d %02d:%02d] Application Started\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, 1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min);
        printf("[INFO] [9096] [%02d-%02d-%04d %02d:%02d] Generating shares for key '%d'\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, 1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min, atoi(argv[2]));

        for (i=1; i<=10; i++) {
            result=polynomyal(a2, a1, secret, i);       //kalw 10 fores gia ta 10 shmeia to polywnimo
            x[i-1]=i;       //apothikeuw to x
            y[i-1]=result;      //apothikeuw to f(x)
        }

        for (i=0; i<10; i++) {
            printf("(%d,%d)\n", x[i], y[i]);    //printing ta 10 shmeia
        }
    } 
    else if (strcmp(argv[1], "unlock")==0) {    
        char*point1,*point2,*point3;
        int x1,y1,x2,y2,x3,y3;

        if (argc<5) {
            printf("Correct use: %s unlock (x1,y1) (x2,y2) (x3,y3).\n", argv[0]);
            return;
        }
        
        point1=strdup(argv[2]);     //apothikeuw ta 3 shmeia
        point2=strdup(argv[3]);
        point3=strdup(argv[4]);

        coordinates(point1,&x1,&y1);    //diaspaw ta 3 shmeia se x1,x2,x3,y1,y2,y3
        coordinates(point2,&x2,&y2);
        coordinates(point3,&x3,&y3);

        a1=(rand()%100)+1;      //dhmiourgw ta 2 tuxaia a1,a2
        a2=(rand()%100)+1;

        cramer(a1,a2,x1,x2,x3,y1,y2,y3,argc-2);   //plhthos twn shmeiwn: argc-2

        free(point1);
        free(point2);
        free(point3);
    }
}
