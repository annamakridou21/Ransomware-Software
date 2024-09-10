#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <regex.h>
#include <curl/curl.h>

/*node ths listas pou apothikeuw ta dedomena pou xreiazomai*/
struct ask2 {
    char *domain;
    char* directory;
    char* filename;
    char* malware;
    struct ask2 *next;
};

/*Global Variables*/
struct ask2 *head=NULL;
int file_cnt=0;
time_t rawtime;
struct tm *timeinfo;

//afairw ta domain names pou vriskw polles fores sto idio arxeio kai den kanw request gia auta
void remove_duplicates(){
    struct ask2 *current=head,*temp,*tmp,*cur;
    while (current!=NULL && current->next!=NULL) {
        cur= current;
        while (cur->next != NULL) {
            if (strcmp(current->filename, cur->next->filename)==0 && strcmp(current->domain, cur->next->domain)==0) {
                temp=cur->next;
                cur->next=cur->next->next;
                free(temp);
            } 
            else {
                cur=cur->next;
            }
        }
        current=current->next;
    }
}
//anazhthsh sth lista
int lookup_ask2(char *domain,char*filename) {
    struct ask2 *current=head;
    while (current!=NULL) {
        if (strcmp(current->domain, domain)==0 && strcmp(current->filename,filename)==0) {       //anazhthsh domain name sth lista
            return 1; 
        }
        current=current->next;
    }
    return 0;
}

//eisagwgh sth lista
int insert_ask2(char *domain, char* dir, char* filename) {
    struct ask2 *temp;
    if (lookup_ask2(domain,filename)) return 0; //an uparxei hdh de ksanakanw request gia to idio domain sto idio file
    struct ask2 *newask2=(struct ask2 *)malloc(sizeof(struct ask2));
    if (!newask2) {
        perror("Memory allocation failed.\n");
        return 0;
    }
    newask2->domain=strdup(domain);
    newask2->filename=strdup(filename);
    newask2->directory=strdup(dir);
    newask2->next=NULL;
    if (head==NULL) {
        head=newask2;
    } 
    else {
        temp=head;
        while (temp->next!=NULL) {
            temp=temp->next;
        }
        temp->next=newask2;
    }
    return 1;
}

//tupwnw ta apotelesmata
void print_ask2(){
    struct ask2*tmp=head;
    while (tmp!=NULL) {        //Diatrexw th lista kai tupwnw ola ta domains, ta arxeia sta opoia ta vrhka kai to directory
        printf("%-30s %-80s %-30s", tmp->domain, tmp->directory, tmp->filename);
        if (strcmp(tmp->malware, "Malware") == 0) {
            printf("\033[1;31m%s\033[0m\n", tmp->malware); //Red color gia malware
        } 
        else {
            printf("%s\n", tmp->malware); //Default color gia safe
        }
        tmp = tmp->next;
    }
}

void print_prompt(char *dir_path) {
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("[INFO] [9096] [%02d-%02d-%04d %02d:%02d] Application Started\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, 1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min);
    printf("[INFO] [9096] [%02d-%02d-%04d %02d:%02d] Scanning directory %s\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, 1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min,dir_path);
    printf("[INFO] [9096] [%02d-%02d-%04d %02d:%02d] Found %d files\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, 1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min, file_cnt);
    printf("[INFO] [9096] [%02d-%02d-%04d %02d:%02d] Searching...\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, 1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min);
    printf("[INFO] [9096] [%02d-%02d-%04d %02d:%02d] Operation finished\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, 1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min);
    printf("[INFO] [9096] [%02d-%02d-%04d %02d:%02d] Processed %d files.\n", timeinfo->tm_mday, timeinfo->tm_mon + 1, 1900 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min, file_cnt);
}

//sunarthsh pou kaleitai gia thn CURLOPT_WRITEFUNCTION gia thn apothikeush tou data pou gurnaei to request
size_t handle_response(void *ptr, size_t size, size_t num_elems, char **response) {
    size_t response_size=size*num_elems;
    *response=(char *)realloc(*response, response_size + 1);
    if (*response) {
        memcpy(*response, ptr, response_size);
        (*response)[response_size]='\0'; 
    }
    return response_size;
}

//sunarthsh pou kanei to request
void request(const char *domain,struct ask2* current) {
    CURL *curl;
    CURLcode result;
    char url[512];
    char *response=NULL;
    struct curl_slist *headers=NULL;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl=curl_easy_init();
    if (curl) {
        snprintf(url, 512, "https://family.cloudflare-dns.com/dns-query?name=%s", domain);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        headers=curl_slist_append(headers, "accept: application/dns-json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, handle_response); //edw apothikeuontai ta data apo request
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);      //to apotelesma tou request to grafoume sto response
        result=curl_easy_perform(curl);
        if (result!=CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
        } 
        else {
            if (response && strstr(response, "Censored")!=NULL) {       //an sto response san comment uparxei to censored, exoume malware
                current->malware="Malware";
            }
            else current->malware="Safe";       //An den vrei censored, einai safe
        }
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    curl_global_cleanup();
    free(response);
}


void making_all_requests(){
    curl_global_init(CURL_GLOBAL_DEFAULT);
    struct ask2* current=head;
    while (current!=NULL) {
        request(current->domain,current);   //kalw thn request gia kathe ask2 ths listas dhladh gia kathe domain name 
        current=current->next;
    }
    curl_global_cleanup();
}

int count_spaces(char* string) {
    int count = 0;
    while (*string) {
        if (*string==' ') { 
            count++;
        }
        string++;
    }
    return count;
}

//afairw whitespace kai to https apta domain names
void process_domains() {
    int i,j,size,shift=0;
    char*start,*end;
    if (head!=NULL) {
        struct ask2 *current=head;
        while (current!=NULL) { //krataw mono otidhpote vriskw anamesa sto http kai sto .com
            start=strstr(current->domain, "http"); 
            if (start!=NULL) {
                end=strstr(start, ".com"); 
                if (end!=NULL) {
                    memmove(current->domain, start, end-start+4);   //4 gia to mhkos tou .com
                    current->domain[end-start+4]='\0'; 
                }
            }   //afairw to https giati den to thelw sto request
            char *https = strstr(current->domain, "https://");
            if (https != NULL && https == current->domain) {
                memmove(current->domain, https + 8, strlen(https + 8) + 1);
            }
            char *last=strrchr(current->domain, ' ');   //svhnw ta spaces prin to domain name
            if (last!=NULL) {
                char result[100]; 
                j=0;
                for (i=0; i<strlen(last); i++) {
                    if (!isspace(last[i])) {
                        result[j++]=last[i];    //elegxw ksana gia tuxon kena
                    }
                }
                result[j]='\0';
                size=strlen(result);
                for (i=0; i < size; i++) {
                    if (result[i]=='"') {     //afairw tuxon quotes pou perikleioun to url
                        shift++;
                    } 
                    else {
                        result[i-shift]=result[i];
                    }
                }
                size=size-shift;
                result[size]='\0'; 
                char *last = strstr(result, ".com");
                if(last != NULL) {
                    last[4] = '\0'; //afairw otidhpote meta to .com kai termatizw to string
                }
                free(current->domain); 
                current->domain = strdup(result);   //ananewnw to field domain ths listas mou meta thn epeksergasia
            }
            current=current->next;
        }
    }
    
}

//euresh kai apothikeush twn domain names sth lista
void extract_domains(const unsigned char *data, size_t size,char* dir,char*filename) {
    //otidhpote pou den periexei sumvolo + opoiodhpote apta top domain extensions
    char *domain_pattern = "[a-z]+\\.(com|gr$|net$|gov$|edu$|info$|biz$|io$|co$|uk$|de$|jp$|cn$|ru$|au$|mobi$|name$|pro$|xxx$|tv$|travel$|us$|ca$|fr$|it$|es$|nl$|br$|mx$|in$)";
    regex_t regex;      //dexetai otidhpote ektos apo ta parapanw sumvola !@#$^&* epitrepetai grammata kai arithmoi kai to dash. meta thn teleia (.) akolouthoun .com,.net kai diafora alla suxna emfanizomena domain extensions
    int reti;
    char str[1024];
    size_t i;
    reti=regcomp(&regex, domain_pattern, REG_EXTENDED);
    if (reti) return;
    for (i=0; i<size; i++) {
        if (isprint(data[i])) {
            strncat(str, (char*)&data[i], 1);      //Apothikeuw tous printable xarakthres sto str
        } 
        else {
            if (strlen(str)>0) {        //Psaxnw gia domain name me to regex pou exw ftiaksei. An vrw to vazw sth lista
                reti=regexec(&regex, str, 0, NULL, 0);
                if (!reti) {
                    insert_ask2(str,dir,filename);
                }
                str[0]='\0';
            }
        }
    }
    //Kanw ton idio elegxo se periptwsh pou perisepse kapoio string
    if (strlen(str)>0) {
        reti=regexec(&regex, str, 0, NULL, 0);
        if (!reti) {
            insert_ask2(str,dir,filename);
        }
    }
    regfree(&regex);
}

//h kuria sunarthsh pou scanarei ola ta arxeia
void scan(const char *dir_path) {
    DIR *dir;
    const char* last_slash;
    char* directory;
    struct dirent *read;
    struct stat file;
    char* filename;
    FILE *fp;
    size_t bytes;
    size_t length;
    unsigned char data[1024];
    char file_path[1024];

    dir=opendir(dir_path);
    if (dir==NULL) {
        perror("Error opening directory.\n");
        return;
    }
    //diavazw kathe entry sto directory
    while ((read=readdir(dir))!=NULL) {
        //ftiaxnw to path
        snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, read->d_name);
        if (stat(file_path, &file)==-1) {
            continue;
        }
        if (S_ISDIR(file.st_mode)) {  //an einai directory kalw thn sunarthsh anadromika
            if (strcmp(read->d_name, ".")!=0 && strcmp(read->d_name, "..")!=0) scan(file_path);
        } 
        else { //an einai arxeio auksanw ton counter kai anoigw to arxeio
            fp=fopen(file_path, "rb");
            if (fp!=NULL) {
                file_cnt++;
                last_slash=strrchr(file_path, '/');  //kanw extract to onoma tou arxeiou kai tou directory
                directory=NULL;
                if (last_slash!=NULL) {
                    length=last_slash-file_path;
                    directory=(char*)malloc((length + 1)*sizeof(char));
                    if (directory!=NULL) {
                        strncpy(directory, file_path, length);
                        directory[length]='\0'; 
                    } 
                    else {
                        perror("Memory allocation failed");
                        exit(EXIT_FAILURE);
                    }
                }
                if (last_slash!=NULL) {
                    length=strlen(last_slash + 1);
                    filename=(char*)malloc((length + 1)*sizeof(char));
                    if (filename!=NULL) {
                        strcpy(filename, last_slash+1);
                    } 
                    else {
                        perror("Memory allocation failed");
                    }
                }
                //diavazw olo to arxeio kai to epeksergazomai me thn extract_domains pou vriskei ta domain names
                while ((bytes = fread(data, 1, sizeof(data), fp)) != 0) {
                    extract_domains(data, bytes,directory,filename);
                }
                fclose(fp);
            }
        }
    }
    closedir(dir);
    process_domains(); //afou vrei ola ta domain names ta epeksergazetai
}

int main(int argc, char *argv[]) {
    if (argc<3 || strcmp(argv[1], "inspect") != 0) {
        printf("Correct use: %s inspect <directory_path>\n", argv[0]);
        return 1;
    }
    char *dir_path = argv[2];
    scan(dir_path);
    print_prompt(dir_path);
    remove_duplicates();
    making_all_requests();
    print_ask2();
    return 0;
}
