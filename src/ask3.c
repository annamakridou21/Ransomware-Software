#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>

struct node {
    const char *name;
    int  mask;                  //apothikeuw ta events se mia lista gia na psaksw gia to pattern
    int count;
    struct node*next;
};

struct node *Head=NULL;
int count=0;

void Free(struct node *head) {
    struct node *temp;

    while (head != NULL) {
        temp = head;                    // Store the current node to be freed
        head = head->next;              // Move to the next node
        free((char*)temp->name);        // Free the memory allocated for the name
        free(temp);                     // Free the memory allocated for the node
    }
}


struct node *Lookup(const char *name, int mask) {
    struct node *current=Head;
    while (current!=NULL) {
        if (strcmp(current->name, name)==0 && current->mask==mask) {        //anazhthsh tou pattern
            return current;
        }
        current=current->next;
    }
    return NULL;
}


int Insert(const char *path, int event_mask) {          //insertion tou event sth lista
    struct node *temp;
    struct node *newnode=(struct node *)malloc(sizeof(struct node));
    if (!newnode) {
        perror("Memory allocation failed.\n");
        return 0;
    }
    newnode->name=strdup(path);
    newnode->mask=event_mask;
    newnode->count=count++;
    newnode->next=NULL;
    if (Head==NULL) {
        Head=newnode;
    } 
    else {
        temp=Head;
        while (temp->next!=NULL) {
            temp=temp->next;
        }
        temp->next=newnode;
    }
    return 1;
}

void Delete(const char *name) {
    struct node *current = Head;
    struct node *prev = NULL;
    struct node *temp;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            if (prev == NULL) {
                temp = current;
                Head = current->next;
                current = current->next;
                free((char*)temp->name);
                free(temp);
            } 
            else {
                temp = current;
                prev->next = current->next;
                current = current->next;
                free((char*)temp->name);
                free(temp);
            }
        } 
        else {
            prev = current;
            current = current->next;
        }
    }
}

static void handle_events(int fd, int *wd, int argc, char* argv[]) {            //sunarthsh handle_events apto manual
    char buf[4096] __attribute__ ((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    struct node* p1,*p2,*p3;
    char str1[100];
    char str2[] = ".locked";
    char result[200]; 
    ssize_t len;
    int length;
    char *ptr;
    while (1) {
        len=read(fd, buf, sizeof(buf));
        if (len==-1 && errno!=EAGAIN) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (len<=0)break;

        for (ptr=buf; ptr<buf+len; ptr+=sizeof(struct inotify_event)+event->len) {
            event=(const struct inotify_event *)ptr;
            //my extra code
            length=strlen(event->name);
            //an to onoma tou arxeiou einai adeio h teleiwnei se swp to agnow(to .swp to paragei mono tou sto mac)
            if(length==0 || event->name==NULL || (event->name[length-3]=='s' && event->name[length-2]=='w' && event->name[length-1]=='p')) continue;   
            //diaxeirizomai ta events pou anaferontai sto paradeigma
            if (event->mask & IN_CLOSE_WRITE) {
                printf("File opened for writing was closed: %s\n", event->name);
                Insert(event->name, event->mask);
            }
            else if (event->mask & IN_CLOSE_NOWRITE) {
                printf("File not opened for writing was closed: %s\n", event->name);
                Insert(event->name, event->mask);
            }
            else if (event->mask & IN_CREATE) {
                if (event->mask & IN_ISDIR) {
                    printf("Directory created in watched directory: %s\n", event->name);
                } 
                else {
                    printf("File created in watched directory: %s\n", event->name);
                }
                Insert(event->name, IN_CREATE);
            }
            else if (event->mask & IN_DELETE) { //se periptwsh diagrafhs energopoieitai o mhxanismos elegxou gia ransomware attack
                strcpy(str1,event->name);       //paradeigma str1=a.txt kai result=a.txt.locked
                strcpy(result, str1);
                strcat(result, str2);
                p1=Lookup(str1,IN_OPEN);
                p2=Lookup(result,IN_CREATE);
                p3=Lookup(result,IN_MODIFY);
                if(p1!=NULL && p2!=NULL && p3!=NULL && p1->count<p2->count && p2->count<p3->count){      //psaxnw to pattern: OPEN file.txt CREATE file.txt.locked MODIFY file.txt.locked
                    printf("\033[1;31m[WARN] Ransomware attack detected on file %s\033[0m\n", str1);        //an to vrw tote proeidopoiw
                    Delete(str1);       //diagrafw apth lista to arxeio pou esvhsa gia na mh mperdeuetai me metagenestera events
                }
                if (event->mask & IN_ISDIR) {
                    printf("Directory deleted from watched directory: %s\n", event->name);
                } 
                else {
                    printf("File deleted from watched directory: %s\n", event->name);
                }
                Insert(event->name, event->mask);
            }
            else if (event->mask & IN_MODIFY) {
                printf("File was modified: %s\n", event->name);
                Insert(event->name, event->mask);
            }
            else if (event->mask & IN_OPEN) {
                printf("File was opened: %s\n", event->name);
                Insert(event->name, event->mask);
            }
        }
    }
}
//o kwdikas ths main apo to manual pou kalei thn handle_events
void monitor_directory(int argc, char *argv[]) {
    char buf;
    int fd, i, poll_num;
    int *wd;
    nfds_t nfds;
    struct pollfd fds[2];
    fd=inotify_init1(IN_NONBLOCK);
    if (fd==-1) {
        perror("inotify_init1");
        exit(EXIT_FAILURE);
    }
    wd=calloc(1, sizeof(int));
    if (wd==NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    wd[0]=inotify_add_watch(fd, argv[2], IN_ACCESS | IN_CLOSE_WRITE | IN_CLOSE_NOWRITE | IN_CREATE | IN_DELETE | IN_MODIFY | IN_OPEN);
    if (wd[0]==-1) {
        fprintf(stderr, "Cannot watch '%s': %s\n", argv[2], strerror(errno));
        exit(EXIT_FAILURE);
    }
    nfds=2;
    fds[0].fd=STDIN_FILENO; 
    fds[0].events=POLLIN;
    fds[1].fd=fd;          
    fds[1].events=POLLIN;
    while (1) {
        poll_num=poll(fds, nfds, -1);
        if (poll_num==-1) {
            if (errno==EINTR)continue;
            perror("poll");
            exit(EXIT_FAILURE);
        }

        if (poll_num>0) {
            if (fds[0].revents & POLLIN) {
                while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    continue;
                break;
            }
            if (fds[1].revents & POLLIN) {
                handle_events(fd, wd, argc, argv);
            }
        }
    }
    close(fd);
    free(wd);
    exit(EXIT_SUCCESS);
}
/*
int main(int argc, char *argv[]) {
    if (argc < 3 || strcmp(argv[1], "monitor") != 0) {
        printf("Correct use: %s monitor <directory_path>\n", argv[0]);
        return 1;
    }
    monitor_directory(argc, argv);
    return 0;
}
*/
