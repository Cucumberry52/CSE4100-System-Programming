/*
 * echo - read and echo text lines until client closes connection
 */
/* $begin echo */
#include "csapp.h"

extern struct item *root;

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

struct item* BYsearch(struct item *root, int ID);

void stock_show(struct item *node, char *buf) {
    if (node == NULL) return;
    
    stock_show(node->left, buf);

    
    P(&node->mutex);
    node->readcnt++;
    if (node->readcnt == 1)   P(&node->w);
    V(&node->mutex);

sprintf(buf + strlen(buf), "%d %d %d\n", node->ID, node->left_stock, node->price);
    
    P(&node->mutex);
    node->readcnt--;
    if (node->readcnt == 0) V(&node->w);
    V(&node->mutex);


    stock_show(node->right, buf);
}





void echo(int connfd) 
{
    int n; 
    char buf[MAXLINE]; 
    rio_t rio;
    
    char what_to_do[MAXLINE];
    int id, amounts;
    char response_buf[MAXLINE];

    Rio_readinitb(&rio, connfd);
    
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        printf("server received %d bytes\n", n);
        
        //초기화
        what_to_do[0] = '\0';
        id = 0;
        amounts = 0;
        
        sscanf(buf, "%s %d %d", what_to_do, &id, &amounts); 
        memset(response_buf, 0, MAXLINE);
        

        if (strcmp(what_to_do, "exit") == 0)    break; 


        else if (strcmp(what_to_do, "show") == 0)  stock_show(root, response_buf);
         
        
         else if (strcmp(what_to_do, "sell") == 0) {
            struct item *node = BYsearch(root, id);
            
            if (node != NULL){
  
                P(&node->w);
                
                node->left_stock += amounts;
                strcpy(response_buf, "[sell] success\n");
                
                V(&node->w);

            }
            
        } 
        

        else if (strcmp(what_to_do, "buy") == 0) {
            struct item *node = BYsearch(root, id); 
                
                if (node != NULL){

                P(&node->w);
                
                if (node->left_stock >= amounts) {
                    node->left_stock -= amounts;
                    strcpy(response_buf, "[buy] success\n");
                } 
                
                else strcpy(response_buf, "Not enough left stocks\n");
                
                V(&node->w);
            }
            
            
            else strcpy(response_buf, "Stock not found\n");
            
        } 
        
       
            Rio_writen(connfd, response_buf, MAXLINE);
            
    }

}



