#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <unistd.h>
int file_count = 0;
int infected_count = 0;
char infected_files[1024][1024];
int reason[1024];
int search_signature(char* path) {
    unsigned char signature[]={0x1d, 0x00, 0x00, 0xec, 0x33, 0xff, 0xff, 0xfb, 0x06, 0x00, 0x00, 0x00, 0x46, 0x0e, 0x10};
    size_t bytes;
    unsigned char *pointer_0x98;
    unsigned char *buffer;
    long file_size;
    FILE* fp2=fopen(path, "rb");
    if (fp2!=NULL) {
        fseek(fp2, 0, SEEK_END);
        file_size=ftell(fp2);
        rewind(fp2);
        buffer=(unsigned char *)malloc(file_size);
        if (buffer==NULL) {
            perror("Error allocating memory");
            fclose(fp2);
            return 0;
        }
        bytes=fread(buffer, 1, file_size, fp2);
        if (bytes!=file_size) {
            perror("Error reading file");
            free(buffer);
            fclose(fp2);
            return 0;
        }
        fclose(fp2);
    }
    pointer_0x98=buffer;   //psaxnw na vrw to 0x98
    while ((pointer_0x98=memchr(pointer_0x98, 0x98, file_size-(pointer_0x98-buffer)))!=NULL) {
        if (pointer_0x98+16<=buffer+file_size && memcmp(pointer_0x98+1, signature, sizeof(signature))==0) { //an uparxoun 16 bytes kai tairiazoune me to signature
            return 1;       //an to vrw to sugkrinw me ton pinaka signature na dw an tairiazei
        }
        pointer_0x98++;     //alliws sunexizoume thn anazhthsh
    }
    free(buffer);
    return 0;
}

//Vriskei th prwth emfanish tou byte string s sto bytes string l
int search_bitcoin(char *s1, size_t s1_len){
    unsigned char bitcoin[100] = {0x62, 0x63, 0x31, 0x71, 0x61, 0x35, 0x77, 0x6b, 0x67, 0x61, 0x65, 0x77, 0x32, 0x64, 0x6b, 0x76, 0x35, 0x36, 0x6b, 0x66, 0x76, 0x6a, 0x34, 0x39, 0x6a, 0x30, 0x61, 0x76, 0x35, 0x6e, 0x6d, 0x6c, 0x34, 0x35, 0x78, 0x39, 0x65, 0x6b, 0x39, 0x68, 0x7a, 0x36, 0x00};
    int length=strlen(bitcoin);
    char *i;
    if(s1_len==0 || length==0 || s1_len<length) return 0;
    for (i=s1; i<=s1+s1_len-length; i++) {      //diatrexw ta data
        if (i[0]==bitcoin[0] && memcmp(i, bitcoin, length)==0) return 1;        //an to prwto char einai idio me to 0x62 elegxw olh thn akolouthia kai an einai idia vrhka to bitcoin kai epistrefw 1
    }
    return 0;
}

void print_results(int file_count, int infected_count) {
    time_t rawtime;
    struct tm *timeinfo;
    int i;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("[INFO] [9096] [%02d-%02d-%04d %02d:%02d] Found %d files\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, 1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min, file_count);  
    printf("[INFO] [9096] [%02d-%02d-%04d %02d:%02d] Searching...\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, 1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min);
    printf("[INFO] [9096] [%02d-%02d-%04d %02d:%02d] Operation finished\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, 1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min);
    printf("[INFO] [9096] [%02d-%02d-%04d %02d:%02d] Processed %d files. Found %d infected\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, 1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min, file_count, infected_count);
    if (infected_count>0) {
        for (i=0; i<infected_count; ++i) {
            if (reason[i]==1) printf("%s:REPORTED_BITCOIN\n", infected_files[i]);
            if (reason[i]==2) printf("%s:REPORTED_VIRUS\n", infected_files[i]);
            if (reason[i]==3) printf("%s:REPORTED_MD5_HASH\n", infected_files[i]);
            if (reason[i]==4) printf("%s:REPORTED_SHA256_HASH\n", infected_files[i]);
        }
    }
    else return;
}

void scan_directory(const char *dir_path) {
    DIR *dir;
    struct dirent *read_dir;
    struct stat file;
    FILE *fp;
    size_t bytes;
    unsigned char md5_result[MD5_DIGEST_LENGTH];
    unsigned char sha256_result[SHA256_DIGEST_LENGTH];
    char path[1024];
    unsigned char data[8192];
    MD5_CTX md5_context;
    SHA256_CTX sha256_context;
    //Dhlwsh twn dedomenwn ths askhshs
    unsigned char md5[MD5_DIGEST_LENGTH] = {0x85, 0x57, 0x8c, 0xd4, 0x40, 0x4c, 0x6d, 0x58, 0x6c, 0xd0, 0xae, 0x1b, 0x36, 0xc9, 0x8a, 0xca};
    unsigned char sha256[SHA256_DIGEST_LENGTH] = {0xd5, 0x6d, 0x67, 0xf2, 0xc4, 0x34, 0x11, 0xd9, 0x66, 0x52, 0x5b, 0x32, 0x50, 0xbf, 0xaa, 0x1a, 0x85, 0xdb, 0x34, 0xbf, 0x37, 0x14, 0x68, 0xdf, 0x1b, 0x6a, 0x98, 0x82, 0xfe, 0xe7, 0x88, 0x49};
    dir=opendir(dir_path);
    if (dir==NULL) {
        fprintf(stderr, "Error opening directory %s\n", dir_path);
        return;
    }
    while ((read_dir=readdir(dir)) != NULL) {
        snprintf(path, sizeof(path), "%s/%s", dir_path, read_dir->d_name);
        if (stat(path, &file)==-1) {
            continue;
        }
        if (S_ISDIR(file.st_mode)) {
            if (strcmp(read_dir->d_name, ".")!=0 && strcmp(read_dir->d_name, "..")!=0) scan_directory(path);
        } 
        else {
            fp=fopen(path, "rb");   
            if (fp!=NULL) {
                file_count++;
                //upologizw ta MD5, SHA256
                MD5_Init(&md5_context);
                SHA256_Init(&sha256_context);
                while ((bytes=fread(data, 1, sizeof(data), fp))!=0) {
                    MD5_Update(&md5_context, data, bytes);
                    SHA256_Update(&sha256_context, data, bytes);
                    if (search_bitcoin(data, bytes)==1) {
                          strcpy(infected_files[infected_count], path);
                          reason[infected_count]=1;     //an vrhka to bitcoin vgainw apto loop
                          infected_count++;
                          break;
                    }
                }
                MD5_Final(md5_result, &md5_context);
                SHA256_Final(sha256_result, &sha256_context);
                fclose(fp);
                if (memcmp(md5_result, md5, MD5_DIGEST_LENGTH)==0) {        //afou skanara olo to arxeio elegxw an vrhka md5
                    strcpy(infected_files[infected_count], path);
                    reason[infected_count]=3;
                    infected_count++;
                }
                if (memcmp(sha256_result, sha256, SHA256_DIGEST_LENGTH)==0) {
                    strcpy(infected_files[infected_count], path);       //h an vrhka sha256
                    reason[infected_count]=4;
                    infected_count++;
                }
            }
            if (search_signature(path)==1) {
                strcpy(infected_files[infected_count], path);
                reason[infected_count]=2;     //an vrhka to bitcoin vgainw apto loop
                infected_count++;
                break;
            }
        }
    }
    closedir(dir);
}
/*
int main(int argc, char *argv[]) {
    if (argc<3 || strcmp(argv[1], "scan") != 0) {
        printf("Correct use: %s scan <directory_path>\n", argv[0]);
        return 1;
    }
    char *dir_path = argv[2];
    scan_directory(dir_path);
    print_results(file_count, infected_count);
    return 0;
}*/

