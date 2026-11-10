/*
 * echo - read and echo text lines until client closes connection
 */
/* $begin echo */
#include "csapp.h"


// ///////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////

// extern struct item *root; 

// struct item {
//     int ID;             
//     int left_stock;         
//     int price;          
//     int readcnt;        
// //sem_t mutex;            
//     //sem_t w;  
//     struct item *left;
//     struct item *right;
// };

// struct item* BYsearch(struct item *root, int ID); 

// void send_stocks_to_client(struct item *root, int connfd) {
//     if (!root) return;
//     send_stocks_to_client(root->left, connfd);
    
//     char line[MAXLINE];
//     sprintf(line, "%d %d %d\n", root->ID, root->left_stock, root->price); // 포맷 매핑[cite: 14]
//     Rio_writen(connfd, line, strlen(line));
    
//     send_stocks_to_client(root->right, connfd);
// }
// /////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////



// void echo(int connfd) 
// {
//     int n; 
//     char buf[MAXLINE]; 
//     rio_t rio;

//     ///////////////////////////////////////////////////////////////////
//     char cmd[MAXLINE];
//     int id, amount;
//  //////`/////////////////////////////////////////////////////////////////


//     Rio_readinitb(&rio, connfd);
    

//     while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
//         // 초기화 중요!!
//         cmd[0] = '\0';

//         sscanf(buf, "%s %d %d", cmd, &id, &amount); 
        
//         if (strcmp(cmd, "show") == 0) send_stocks_to_client(root, connfd);
         

//         else if (strcmp(cmd, "buy") == 0) {
//             struct item *node = BYsearch(root, id); 
           
            
//             if (node != NULL && (node->left_stock) >= amount) {
//                 node->left_stock -= amount; 

//                 char *str = "[buy] success\n";
//                 Rio_writen(connfd, str, strlen(str));
//             } 
            
//             else {
//                 char *str = "Not enough left stocks\n";
//                 Rio_writen(connfd, str, strlen(str));
//             }
//         } 


//         else if (strcmp(cmd, "sell") == 0) {
//             struct item *node = BYsearch(root, id);

//             if (node != NULL) {
           
//                 node->left_stock += amount; 
           
//                 char *str = "[sell] success\n";
//                 Rio_writen(connfd, str, strlen(str));
           
//             }

//         } 
        
//         else if (strcmp(cmd, "exit") == 0) break; 
        
//     }




// }
// /* $end echo */

