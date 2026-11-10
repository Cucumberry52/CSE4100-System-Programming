/* 
 * echoserveri.c - An iterative echo server 
 */ 
/* $begin echoserverimain */
#include "csapp.h"

struct item {
    int ID;             
    int left_stock;         
    int price;          

    int readcnt;        
    //sem_t mutex;            
    //sem_t w;            
    
    struct item *left;
    struct item *right;
};

struct item *root = NULL;

typedef struct {
    int maxfd;
    fd_set read_set;
    fd_set ready_set;
    int nready;
    int maxi;
    int clientfd[FD_SETSIZE];
    rio_t clientrio[FD_SETSIZE];
} pool;

// 기존에 있던 함수
void init_pool(int listenfd, pool *p);
void add_client(int connfd, pool *p);
void check_clients(pool *p);


void for_show(struct item *root, char *buf);
struct item* BYsearch(struct item *root, int ID);
void inorder_search(struct item *root, FILE *fp);
struct item* new_item(int ID, int left_stock, int price);
struct item* insert_item(struct item* root, struct item* new_item);


void init_pool(int listenfd, pool *p)
{
    int i;
    p->maxi = -1;
    for (i = 0; i < FD_SETSIZE; i++)
        p->clientfd[i] = -1;

    p->maxfd = listenfd;
    FD_ZERO(&p->read_set);
    FD_SET(listenfd, &p->read_set);
}

void add_client(int connfd, pool *p)
{
    int i;
    p->nready--;
    for (i = 0; i < FD_SETSIZE; i++) {
        if (p->clientfd[i] < 0) {
            p->clientfd[i] = connfd;
            Rio_readinitb(&p->clientrio[i], connfd);
            FD_SET(connfd, &p->read_set);

            if (connfd > p->maxfd) p->maxfd = connfd;
            if (i > p->maxi) p->maxi = i;
            break;
        }
    }
    if (i == FD_SETSIZE) app_error("add_client error: Too many clients");
}




void for_show(struct item *root, char *buf) {
    if (!root) return;
    
    for_show(root->left, buf);
    
    sprintf(buf + strlen(buf), "%d %d %d\n", root->ID, root->left_stock, root->price);
    
    for_show(root->right, buf);
}



void check_clients(pool *p)
{
    int i, connfd, n;
    
    char buf[MAXLINE];
    char what_to_do[MAXLINE];

    int id, amount;
    char response_buf[MAXLINE];

    for (i = 0; (i <= p->maxi) && (p->nready > 0); i++) {
        connfd = p->clientfd[i];

        if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set))) {
            p->nready--;

            if ((n = Rio_readlineb(&p->clientrio[i], buf, MAXLINE)) != 0) {
                what_to_do[0] = '\0'; 
                id = 0; amount = 0; 
                
                printf("server received %d bytes\n", n);
                
                sscanf(buf, "%s %d %d", what_to_do, &id, &amount);
                memset(response_buf, 0, MAXLINE);

                if (strcmp(what_to_do, "exit") == 0) {

                    Close(connfd);
                    FD_CLR(connfd, &p->read_set);
                    
                    p->clientfd[i] = -1;
                    printf("Connection closed.\n");
                    
                    
                    continue; 
                }

                else if (strcmp(what_to_do, "show") == 0) for_show(root, response_buf);
                
                    
                else if (strcmp(what_to_do, "sell") == 0) {
                        struct item *node = BYsearch(root, id);
                        node->left_stock += amount;
                        strcpy(response_buf, "[sell] success\n");
                }


                else if (strcmp(what_to_do, "buy") == 0) {
                        struct item *node = BYsearch(root, id);
                        
                        
                        if (node->left_stock >= amount) {
                            node->left_stock -= amount;
                            strcpy(response_buf, "[buy] success\n");
                        } 

                        else  strcpy(response_buf, "Not enough left stocks\n");
                        
                    } 

                
                

                Rio_writen(connfd, response_buf, MAXLINE);
            }
            
            else {
                Close(connfd);
                FD_CLR(connfd, &p->read_set);
                p->clientfd[i] = -1;

                printf("Connection closed by client.\n");
            }
        }
    }
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
	
    //sem_init(&i->mutex, 0, 1);
	//sem_init(&i->w, 0, 1);
	
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
    
    if(!fp) fprintf(stderr, "\n fail to backup\n");
    

    if (fp) {
        inorder_search(root, fp);
        fclose(fp);

        printf("\n sigkill is received\n");
    } 
    
    
    
    exit(0); 
}




/////////////////////////////////////////////////


void echo(int connfd);

int main(int argc, char **argv) 
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
    char HostnameOfClient[MAXLINE], PortOfClient[MAXLINE];

    static pool pool;


    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(0);
    }


Signal(SIGINT, sigint_handler);

    FILE *fp = fopen("stock.txt", "r");

    int id, stock, price;

    while ( fscanf( fp, "%d %d %d", &id, &stock, &price) != EOF) {
        
        struct item* temp = new_item(id, stock, price);
        root = insert_item(root, temp);
    
    }
    
    fclose(fp);    

    listenfd = Open_listenfd(argv[1]);
   
   
   
   
   init_pool(listenfd, &pool);

    while (1) {
        pool.ready_set = pool.read_set;
        pool.nready = Select(pool.maxfd + 1, &pool.ready_set, NULL, NULL, NULL);

        if (FD_ISSET(listenfd, &pool.ready_set)) {
            clientlen = sizeof(struct sockaddr_storage);
            connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
            Getnameinfo((SA *) &clientaddr, clientlen, HostnameOfClient, MAXLINE, PortOfClient, MAXLINE, 0);
            printf("Connected to (%s, %s)\n", HostnameOfClient, PortOfClient);
            add_client(connfd, &pool);
        }
        check_clients(&pool);
    }
    exit(0);

}

/* $end echoserverimain */
