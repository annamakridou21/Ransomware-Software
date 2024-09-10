#include <stdio.h>
#include <string.h>
extern int file_count;
extern int infected_count;
void scan_directory(char *dir_path);
void print_results(int file_count, int infected_count);
void scan(char *dir_path);
void print_prompt(char *dir_path);
void print_ask2();
void making_all_requests();
void monitor_directory(int argc, char *argv[]);
void function(int argc, char *argv[]);
int main(int argc, char *argv[]) {
    if(argc<3) {
        printf("Correct use: %s scan/inspect/monitor <directory_path> or %s slice secret or %s unlock point1 point2 point3\n", argv[0]);
        return 1;
    }
    if(strcmp(argv[1],"scan")==0){
        char *dir_path = argv[2];
        scan_directory(dir_path);
        print_results(file_count, infected_count);
    }
    else if (strcmp(argv[1],"inspect")==0) {
        char *dir_path = argv[2];
        scan(dir_path);
        print_prompt(dir_path);
	 making_all_requests();
	print_ask2();
    }
    else if (strcmp(argv[1], "monitor")==0) {
        monitor_directory(argc, argv);
    }
    else if (strcmp(argv[1],"slice")==0 || strcmp(argv[1],"unlock")==0){
	function(argc, argv);
    }	
    else printf("Correct use: %s scan/inspect/monitor <directory_path>\n", argv[0]);
    return 0;
}
