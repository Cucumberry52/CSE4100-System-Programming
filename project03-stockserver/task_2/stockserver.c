/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "csapp.h"


// 내가 정의한 크기
#define SBUFSIZE 100
#define NTHREADS 200


// 정희함
void *thread(void *vargp);

struct item {
    int ID;
    int left_stock;
    int price;
    int readcnt;      
   
    sem_t mutex;       
    sem_t w;           
    struct item *left;
    struct item *right;
};

struct item *root = NULL;



void for_show(struct item *root, char *buf);
struct item* BYsearch(struct item *root, int ID);
void inorder_search(struct item *root, FILE *fp);
struct item* new_item(int ID, int left_stock, int price);
struct item* insert_item(struct item* root, struct item* new_item);

void for_show(struct item *root, char *buf) {
    if (!root) return;
    for_show(root->left, buf);
    
    char line[MAXLINE];
    sprintf(line, "%d %d %d\n", root->ID, root->left_stock, root->price);
    strcat(buf, line);
    
    for_show(root->right, buf);
}

struct item* new_item(int ID, int left_stock, int price) {
	
    struct item* i = malloc( sizeof (struct item) ); 
    //할당 실패면 NULL 반환
    if (!i) return NULL;
	//
    i->ID = ID;
	i->left_stock = left_stock;
	i->price = price;
	
    i->readcnt = 0;
	
    sem_init(&i->mutex, 0, 1);
	sem_init(&i->w, 0, 1);
	
    i->left = NULL;
    i->right = NULL;
	
    return i;
}

struct item* insert_item(struct item* root, struct item* new_item) {
	if (!root) return new_item;
	
    if (new_item->ID == root->ID)  return root;
    

    if (new_item->ID < root->ID) root->left = insert_item(root->left, new_item);
    else root->right = insert_item(root->right, new_item);
	    
    return root;
}

struct item* BYsearch(struct item *root, int ID) {
	
    if (!root) return NULL; 
    if(root->ID == ID) return root;
	
    if (ID < root->ID) return BYsearch(root->left, ID);
	
    return BYsearch(root->right, ID);
}


void inorder_search(struct item *root, FILE *fp) {
	if (!root) return;
	
    // 파일이 열려 있으면 파일에 쓰고, 그렇지 않으면 콘솔에 출력
    inorder_search(root->left, fp);
	
    if(fp) fprintf(fp, "%d %d %d\n", root->ID, root->left_stock, root->price);
    else printf("%d ", root->ID);
	
    inorder_search(root->right, fp);
    
}


void sigint_handler(int sig) {
    FILE *fp = fopen("stock.txt", "w"); 
    
    if (fp) {
        inorder_search(root, fp);
        fclose(fp);

        printf("\n sigkill is received\n");
    } 
    
    if(!fp)fprintf(stderr, "\n fail to backup\n");
    
    
    exit(0); 
}

















// 여기까지 강의자료에 있던 구조체 및 함수
typedef struct{
    int*buf;          
    int n;            
    int front;        
    int rear;         
    sem_t mutex;      
    sem_t slots;       
    sem_t items;      
} sbuf_t;

void sbuf_init(sbuf_t*sp, int n);
void sbuf_deinit(sbuf_t*sp);
void sbuf_insert(sbuf_t*sp, int item);
int sbuf_remove(sbuf_t*sp);

void sbuf_init(sbuf_t*sp, int n)
{
    sp->buf= Calloc(n, sizeof(int));
    sp->n = n;                  
    sp->front = sp->rear = 0;   
    Sem_init(&sp->mutex, 0, 1);
    Sem_init(&sp->slots, 0, n); 
    Sem_init(&sp->items, 0, 0); 
}   

void sbuf_deinit(sbuf_t*sp)
{
Free(sp->buf);
}

void sbuf_insert(sbuf_t*sp, int item)
{
    P(&sp->slots);                   
    P(&sp->mutex);                    
    sp->buf[(++sp->rear)%(sp->n)]  = item;                      
    V(&sp->mutex);                   
    V(&sp->items);                    
}

int sbuf_remove(sbuf_t*sp)
{
    int item;
    P(&sp->items);                    
    P(&sp->mutex);                    
    item = sp->buf[(++sp->front)%(sp->n)]; 
    
    V(&sp->mutex);                   
    V(&sp->slots);                   
    return item;
}

// 여기까지 강의자료에 있던 










void echo(int connfd);




sbuf_t sbuf; 

int main(int argc, char **argv) 
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr; 
    char client_hostname[MAXLINE], client_port[MAXLINE];
    pthread_t tid;


    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(0);
    }

    Signal(SIGINT, sigint_handler);




    FILE *fp = fopen("stock.txt", "r");
    int id, stock, price;
    if (fp) {
        while (fscanf(fp, "%d %d %d", &id, &stock, &price) != EOF) {
            struct item* temp = new_item(id, stock, price);
            root = insert_item(root, temp);
        }
        fclose(fp);
    }

    listenfd = Open_listenfd(argv[1]);

    sbuf_init(&sbuf, SBUFSIZE); 
   
    for( int i= 0; i< NTHREADS; i++ )  Pthread_create(&tid, NULL, thread, NULL);  

    while (1) {

        clientlen = sizeof(struct sockaddr_storage); 
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", client_hostname, client_port);
        
        sbuf_insert(&sbuf, connfd);
       // echo(connfd);
        //Close(connfd);
   
    }
    exit(0);
}
/* $end echoserverimain */


void*  thread(void*vargp) {

    Pthread_detach(pthread_self());
   
    while( 1 ) {
        int connfd= sbuf_remove(&sbuf); 
        echo(connfd);                
        
        
        Close(connfd);
        printf("Connection closed (fd: %d)\n", connfd);
    }
}