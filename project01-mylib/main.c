#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hash.h"
#include "list.h"
#include "bitmap.h"

#define MAXARGS 100
#define MAXLINE 100



struct list Lists[10];       
char list_name[10][20];         
int list_cnt = 0;               

struct hash Hashs[10];       
char hash_name[10][20];
int hash_cnt = 0;

struct bitmap *Bitmaps[10];      
char bm_name[10][20];
int bm_cnt = 0;

// 객체배열 속 index 찾는 법
int find_list_index(char *name) {
    for (int i = 0; i < list_cnt; i++) if (!strcmp(list_name[i], name)) return i;
    return -1;
}

int find_hash_index(char *name) {
    for (int i = 0; i < hash_cnt; i++) if (!strcmp(hash_name[i], name)) return i;
    return -1;
}

int find_bitmap_index(char *name) {
    for (int i = 0; i < bm_cnt; i++) if (!strcmp(bm_name[i], name)) return i;
    return -1;
}


// 해쉬 안정성을 위해 작성함
unsigned my_hash(const struct hash_elem *e, void *aux) {
    return hash_int(e->data); 
}

bool my_less(const struct hash_elem *a, const struct hash_elem *b, void *aux) {
    return a->data < b->data;
}

bool list_less (const struct list_elem *a, const struct list_elem *b, void *aux) {

    return (list_entry(a, struct list_item, elem)->data) < (list_entry(b, struct list_item, elem)->data);
}

// 해쉬에 적용할 함수들. hash_apply에서 사용
void hash_square(struct hash_elem *e, void *aux) {
    e->data = (e->data) * (e->data);
}

void hash_triple(struct hash_elem *e, void *aux) {
    e->data = (e->data)*(e->data) * (e->data);
}


///
/// 
void Do(char cmd_line[],char *argv[]) {
    
    if (!strcmp(argv[0], "quit")) exit(0); 
        
    //
    if (!strcmp(argv[0], "create")) {
        
        if (!strcmp(argv[1], "list")) {
            list_init(&Lists[list_cnt]);
            strcpy(list_name[list_cnt], argv[2]);
            list_cnt++;
        } 
        
        if (!strcmp(argv[1], "hashtable")) {
            hash_init(&Hashs[hash_cnt], my_hash, my_less, NULL);
            strcpy(hash_name[hash_cnt], argv[2]);
            hash_cnt++;
        }

        if (!strcmp(argv[1], "bitmap")) {
            int bit_cnt = atoi(argv[3]);

            Bitmaps[bm_cnt] = bitmap_create(bit_cnt);
            strcpy(bm_name[bm_cnt], argv[2]);
            bm_cnt++;
        }
    } 
    

    // cnt를 줄이지는 않음 <- 이거 중요
    if (!strcmp(argv[0], "delete")) {
        int idx;
        // 리스트를 지우는거면
        if ((idx = find_list_index(argv[1])) != -1) {

            while (!list_empty(&Lists[idx])) {
                struct list_elem *e = list_pop_front( &Lists[idx] );
                
                
                free(list_entry(e, struct list_item, elem));
            }
            
            memset(list_name[idx], 0, sizeof(list_name[idx]));
        } 
        // 해쉬를 지우는거면
        if ((idx = find_hash_index(argv[1])) != -1) {
            
            hash_destroy(&Hashs[idx], (hash_action_func*)free); 
            
            memset(hash_name[idx], 0, sizeof(hash_name[idx]));
        }
        
        // 비트맵을 지우는거면
        if ((idx = find_bitmap_index(argv[1])) != -1) {
            
            bitmap_destroy(Bitmaps[idx]);
            
            Bitmaps[idx] = NULL;
            
            memset(bm_name[idx], 0, sizeof(bm_name[idx]));
        }
    }


    // 자료구조간 동일한 이름은 발생하지 않음
    if (!strcmp(argv[0], "dumpdata")) {
        int idx;
        
        if ((idx = find_list_index(argv[1])) != -1) {
            struct list *lst = &Lists[idx];
           
            // temp용
            struct list_elem *current_e;
            
            if (!list_empty(lst)) {
                for (current_e = list_begin(lst); current_e != list_end(lst); current_e = list_next(current_e)) printf("%d ", list_entry(current_e, struct list_item, elem)->data);
                printf("\n");
                    }
        } 
        
        
        if ((idx = find_hash_index(argv[1])) != -1) {
            struct hash *h = &Hashs[idx];
            
            
            // 해쉬는 이터레이터로
            struct hash_iterator i;

            hash_first(&i, h);

            while (hash_next(&i))  printf("%d ", hash_cur(&i)->data);
        

            if (!hash_empty(h)) printf("\n");
        }
        
        
        if ((idx = find_bitmap_index(argv[1])) != -1) {
            struct bitmap *b = Bitmaps[idx];


            for (size_t i = 0; i < bitmap_size(b); i++) printf("%d", bitmap_test(b, i) ? 1 : 0);
            printf("\n");
        }
    } 
    
    ////////////////////////////
    /////////////////////////////////
    //리스트 명령어 처리
    ////////////////////////////////////
    ////////////////////////
    
    // 내가 만든 함수들 빼먹지 말기..
    if (!strcmp(argv[0], "list_swap")) {
        int idx = find_list_index(argv[1]);
     
        if (idx != -1) list_swap(find_Nth_elem(&Lists[idx], atoi(argv[2]))    ,       find_Nth_elem(&Lists[idx], atoi(argv[3])));
     
        
    }

    if (!strcmp(argv[0], "list_shuffle")) {
        int idx = find_list_index(argv[1]);
        
        if (idx != -1)  list_shuffle(&Lists[idx]);
        
    }

     if (!strcmp(argv[0], "list_init")) list_init (&Lists[find_list_index(argv[1])]);

    /* List traversal. 
    struct list_elem *list_begin (struct list *);
    struct list_elem *list_next (struct list_elem *);
    struct list_elem *list_end (struct list *);

    struct list_elem *list_rbegin (struct list *);
    struct list_elem *list_prev (struct list_elem *);
    struct list_elem *list_rend (struct list *);

    struct list_elem *list_head (struct list *);
    struct list_elem *list_tail (struct list *);
    */

    /* List insertion. */
    if (!strcmp(argv[0], "list_insert")){
        int idx = find_list_index(argv[1]);
        
        if (idx != -1) {
            struct list_item *temp = (struct list_item *) malloc(sizeof(struct list_item));
            temp->data = atoi(argv[3]); 
            
            struct list_elem *before = find_Nth_elem(&Lists[idx], atoi(argv[2]));
            


            if (before == NULL)  before = list_end(&Lists[idx]);
            
            // 넘기는 인자 다시 확인
            list_insert(before, &(temp->elem));
        }
    }

   if (!strcmp(argv[0], "list_splice")) {
    int before_idx = find_list_index(argv[1]); 
    int target_idx = find_list_index(argv[3]);  

    int dest_n = atoi(argv[2]);    
    int src_start = atoi(argv[4]);     
    int src_end = atoi(argv[5]); 

        if (before_idx != -1 && target_idx != -1) {

            struct list_elem *dest = find_Nth_elem(&Lists[before_idx], dest_n);
            
            struct list_elem *src_list_start = find_Nth_elem(&Lists[target_idx], src_start);            
            
            struct list_elem *src_list_end = find_Nth_elem(&Lists[target_idx], src_end);
            
            if (dest == NULL) dest = list_end(&Lists[before_idx]); 
            if (src_list_end == NULL) src_list_end = list_end(&Lists[target_idx]); 

            // 이부분이 핵심
            if (src_list_start != NULL) list_splice(dest, src_list_start, src_list_end);
        }
    }
    
    if (!strcmp(argv[0], "list_push_front")) {
        int idx = find_list_index(argv[1]); 
        
        if (idx != -1) {
            struct list_item *temp = (struct list_item *)malloc(sizeof(struct list_item));
            
            
            temp->data = atoi(argv[2]); 
             list_push_front(&Lists[idx], &temp->elem);
        
            }
    
        }

    if (!strcmp(argv[0], "list_push_back")) {
        int idx = find_list_index(argv[1]); 
        
        if (idx != -1) {
            struct list_item *temp = (struct list_item *)malloc(sizeof(struct list_item));
            
            temp->data = atoi(argv[2]); 
                       list_push_back(&Lists[idx], &temp->elem);
        }
    }

    /* List removal. */
    if (!strcmp(argv[0], "list_remove")) {
        int idx = find_list_index(argv[1]); 
        
        if (idx != -1) {
            struct list_elem *elem = find_Nth_elem(&Lists[idx], atoi(argv[2]));
            
            if (elem != NULL) {
                list_remove(elem);
                free(list_entry(elem, struct list_item, elem)); 
            }

        }
    }
  
    if (!strcmp(argv[0], "list_pop_front")) {
        int idx = find_list_index(argv[1]);
        // empty 체크가 중요함. 
        if (idx != -1 && !list_empty(&Lists[idx])) {
            struct list_elem *e = list_pop_front(&Lists[idx]);
            free(list_entry(e, struct list_item, elem));
        }
  
    }

    if (!strcmp(argv[0], "list_pop_back")) {
        int idx = find_list_index(argv[1]);
        // empty 체크가 중요함.
        if (idx != -1 && !list_empty(&Lists[idx])) {
            struct list_elem *e = list_pop_back(&Lists[idx]);
            
            free(list_entry(e, struct list_item, elem));
        }
    }
 
    // empty 체크 계속
 
    if (!strcmp(argv[0], "list_front")) {
        int idx = find_list_index(argv[1]);
        
        if (idx != -1 && !list_empty(&Lists[idx])) {
        
            struct list_item *item = list_entry(list_front(&Lists[idx]), struct list_item, elem);
            printf("%d\n", item->data);
        
        }
    }


    if (!strcmp(argv[0], "list_back")) {
        int idx = find_list_index(argv[1]);
        
        if (idx != -1 && !list_empty(&Lists[idx])) {
        
            struct list_item *item = list_entry(list_back(&Lists[idx]), struct list_item, elem);
            printf("%d\n", item->data);
        
        }
    }


    if (!strcmp(argv[0], "list_size")) {
        int idx = find_list_index(argv[1]);
        // zu출력
        if (idx != -1) printf("%zu\n", list_size(&Lists[idx]));
        
    }

    if (!strcmp(argv[0], "list_empty")) {
        int idx = find_list_index(argv[1]);
        
        if (idx != -1) {
            
            if (list_empty(&Lists[idx])) printf("true\n");
            
            else printf("false\n");
        }
    }

    /* Miscellaneous. */
    if (!strcmp(argv[0], "list_reverse")) {
        int idx = find_list_index(argv[1]);
        if (idx != -1) list_reverse(&Lists[idx]);    
    }
    
    

    /* Operations on lists with ordered elements. */
    if (!strcmp(argv[0], "list_sort")) {
        int idx = find_list_index(argv[1]);
        
        if (idx != -1) list_sort(&Lists[idx], list_less, NULL);
        
    }

    // list_insert_ordered list0 10와 같은 형식
    if (!strcmp(argv[0], "list_insert_ordered")) {
        int idx = find_list_index(argv[1]);
        
        if (idx != -1) {
            struct list_item *temp = (struct list_item *)malloc(sizeof(struct list_item));
            temp->data = atoi(argv[2]);

            list_insert_ordered(&Lists[idx], &temp->elem, list_less, NULL);
        }
    }
    

    // 정렬이 된 상태여야 list_unique가 제대로 작동하니까 주의.
    if (!strcmp(argv[0], "list_unique")) {
   
        int first_idx = find_list_index(argv[1]);      
 
        if (first_idx != -1) {
            if (argv[2] != NULL) { 
            
                int second_idx = find_list_index(argv[2]);
             
                if (second_idx != -1) list_unique(&Lists[first_idx], &Lists[second_idx], list_less, NULL);
            
            } 
            
            // 인자가 한 개인 경우!!!!!!!!!
            else list_unique(&Lists[first_idx], NULL, list_less, NULL);
            
        }
    }

    /* Max and min. */
    
    if (!strcmp(argv[0], "list_max")) {
        int idx = find_list_index(argv[1]);
        // empty 체크가 중요함.
        
        if (idx != -1 && !list_empty(&Lists[idx])) {
            struct list_item *max_i = list_entry(list_max(&Lists[idx], list_less, NULL) , struct list_item, elem);
            
            printf("%d \n", max_i->data);
        
        }
    }

    if (!strcmp(argv[0], "list_min")) {
    
        int idx = find_list_index(argv[1]);
        // empty 체크가 중요함!!

        if (idx != -1 && !list_empty(&Lists[idx])) {
    
            struct list_item *min_i = list_entry(list_min(&Lists[idx], list_less, NULL), struct list_item, elem);
    
            printf("%d\n", min_i->data);
    
        }
    }

    ///////////////////////
    //////////////////////
    // 해시 테이블 명령어 처리
    ///////////////////////
    ////////////////////////


    if (!strcmp(argv[0], "hash_init")) {
        int idx = find_hash_index(argv[1]);
        if (idx != -1) hash_init(&Hashs[idx], my_hash, my_less, NULL);
    }

    if (!strcmp(argv[0], "hash_clear")) {
        int idx = find_hash_index(argv[1]);
       
        if (idx != -1) hash_clear(&Hashs[idx], (hash_action_func *)free);
        
    }

    if (!strcmp(argv[0], "hash_destroy")) {
        int idx = find_hash_index(argv[1]);
        if (idx != -1) {

            hash_destroy(&Hashs[idx], (hash_action_func *)free);
            
            // 이름에서도 지워!
            memset(hash_name[idx], 0, sizeof(hash_name[idx]));
        }
    }

    /* Search, insertion, deletion. */
    if (!strcmp(argv[0], "hash_insert")) {

        int idx = find_hash_index(argv[1]);
     
        if (idx != -1) {
     
            struct hash_elem *new_e = (struct hash_elem *)malloc(sizeof(struct hash_elem)); 
            new_e->data = atoi(argv[2]); 
            if (hash_insert(&Hashs[idx], new_e) != NULL) free(new_e);  
        }
    }

    if (!strcmp(argv[0], "hash_replace")) {
        
        int idx = find_hash_index(argv[1]);
        
        if (idx != -1) {
        
            struct hash_elem *temp = (struct hash_elem *)malloc(sizeof(struct hash_elem));
            temp->data = atoi(argv[2]);
            
            struct hash_elem *old = hash_replace(&Hashs[idx], temp);

            if (old != NULL) free(old);        
        }
    }

    if (!strcmp(argv[0], "hash_find")) {
        
        int idx = find_hash_index(argv[1]);
        
        if (idx != -1) {
        
            struct hash_elem temp; 
            temp.data = atoi(argv[2]);
            
            struct hash_elem *found = hash_find(&Hashs[idx], &temp); 
        
            // 찾았으면 데이터 출력함
            if (found != NULL) printf("%d\n", found->data); 
        
        }
    }

    if (!strcmp(argv[0], "hash_delete")) {
        int idx = find_hash_index(argv[1]);
        
        if (idx != -1) {
        
            struct hash_elem temp;
            temp.data = atoi(argv[2]);
            
            struct hash_elem *deleted = hash_delete(&Hashs[idx], &temp); 
            
            // free 중요
            if (deleted != NULL) free(deleted); 
        }
    }



    // Iteration. 
    if (!strcmp(argv[0], "hash_apply")) {
        int idx = find_hash_index(argv[1]);
        if (idx != -1) {
          
              if (!strcmp(argv[2], "triple")) hash_apply(&Hashs[idx], hash_triple); 
            if (!strcmp(argv[2], "square")) hash_apply(&Hashs[idx], hash_square);              

            
        }
    }
    /*
    void hash_first (struct hash_iterator *, struct hash *);
    struct hash_elem *hash_next (struct hash_iterator *);
    struct hash_elem *hash_cur (struct hash_iterator *);
    */

    /* Information. */
    if (!strcmp(argv[0], "hash_size")) {
        int idx = find_hash_index(argv[1]);
        
        if (idx != -1) printf("%zu\n", hash_size(&Hashs[idx]));
        
    }

    if (!strcmp(argv[0], "hash_empty")) {
        int idx = find_hash_index(argv[1]);
        
        // 삼항문도 가능하기는 한데 가독성 위해서 if문으로 작성
        if (idx != -1) {
            if (hash_empty(&Hashs[idx])) printf("true\n");
            else  printf("false\n");
        }
    }

    /* Sample hash functions. 
    unsigned hash_bytes (const void *, size_t);
    unsigned hash_string (const char *);
    unsigned hash_int (int);
    */


    ///////////////////////
    ////////////////////////
    // 비트맵 명령어 처리
    ////////////////////////
    ////////////////////////



    if (!strcmp(argv[0], "bitmap_expand")) {
        int idx = find_bitmap_index(argv[1]);
        if (idx != -1)   Bitmaps[idx] = bitmap_expand(Bitmaps[idx], atoi(argv[2]));
    }
   /* 
    struct bitmap *bitmap_create (size_t bit_cnt);
    struct bitmap *bitmap_create_in_buf (size_t bit_cnt, void *, size_t byte_cnt);
    size_t bitmap_buf_size (size_t bit_cnt);
    void bitmap_destroy (struct bitmap *);
*/
    

    /* Bitmap size. */
    if (!strcmp(argv[0], "bitmap_size")) {
    
        int idx = find_bitmap_index(argv[1]);

        if (idx != -1)  printf("%zu\n", bitmap_size(Bitmaps[idx]));
    
    }

    /* Setting and testing single bits. */
    if (!strcmp(argv[0], "bitmap_mark")) {
        
        int idx = find_bitmap_index(argv[1]);
        
        if (idx != -1) bitmap_mark(Bitmaps[idx], atoi(argv[2]));
        
    }

    if (!strcmp(argv[0], "bitmap_reset")) {
        
        int idx = find_bitmap_index(argv[1]);
        
        if (idx != -1) bitmap_reset(Bitmaps[idx], atoi(argv[2]));
        
    }

    if (!strcmp(argv[0], "bitmap_set")) {
      
        int idx = find_bitmap_index(argv[1]);
      
        if (idx != -1) {
            
            if(!strcmp(argv[3], "true")) bitmap_set(Bitmaps[idx], atoi(argv[2]), true);
            
            else  bitmap_set(Bitmaps[idx], atoi(argv[2]), false);
        }
    }


    if (!strcmp(argv[0], "bitmap_flip")) {
        int idx = find_bitmap_index(argv[1]);

        if (idx != -1) bitmap_flip(Bitmaps[idx], atoi(argv[2]));
    }


    if (!strcmp(argv[0], "bitmap_test")) {
        int idx = find_bitmap_index(argv[1]);
      
        if (idx != -1) {
           
            if (bitmap_test(Bitmaps[idx], atoi(argv[2]))) printf("true\n");
           
            else printf("false\n");
        }
    }

    
    if (!strcmp(argv[0], "bitmap_set_all")) {
       
        int idx = find_bitmap_index(argv[1]);
       
        if (idx != -1) {
            
            if (!strcmp(argv[2], "true")) bitmap_set_all(Bitmaps[idx], true);
            
            else bitmap_set_all(Bitmaps[idx], false);

            
        }
    }

    if (!strcmp(argv[0], "bitmap_set_multiple")) {
    
        int idx = find_bitmap_index(argv[1]);
       
        if (idx != -1) {
        
            if( !strcmp(argv[4], "true") ) bitmap_set_multiple(Bitmaps[idx], atoi(argv[2]), atoi(argv[3]), true);
        
            else bitmap_set_multiple(Bitmaps[idx], atoi(argv[2]), atoi(argv[3]), false);
           
        }
    }

    
    if (!strcmp(argv[0], "bitmap_count")) {
     
        int idx = find_bitmap_index(argv[1]);
     
        if (idx != -1) {
            
            if( !strcmp(argv[4], "true") ) printf("%zu\n", bitmap_count(Bitmaps[idx], atoi(argv[2]), atoi(argv[3]), true));
            
            else printf("%zu\n", bitmap_count(Bitmaps[idx], atoi(argv[2]), atoi(argv[3]), false));
            
        }
    }

    if (!strcmp(argv[0], "bitmap_contains")) {

        int idx = find_bitmap_index(argv[1]);

        if (idx != -1) {
 
            bool val;

            if (!strcmp(argv[4], "true")) val = true;
            else val = false;

            if (bitmap_contains(Bitmaps[idx], atoi(argv[2]), atoi(argv[3]), val)) printf("true\n");
            else printf("false\n");
        }
    }

    if (!strcmp(argv[0], "bitmap_any")) {
        int idx = find_bitmap_index(argv[1]);
    
        if (idx != -1) {
    
            if (bitmap_any(Bitmaps[idx], atoi(argv[2]), atoi(argv[3]))) printf("true\n");
            else printf("false\n");
    
        }
    }

    if (!strcmp(argv[0], "bitmap_none")) {
        int idx = find_bitmap_index(argv[1]);
    
        if (idx != -1) {
    
            if (bitmap_none(Bitmaps[idx], atoi(argv[2]), atoi(argv[3]))) printf("true\n");
            else printf("false\n");
    
        }
    }

    if (!strcmp(argv[0], "bitmap_all")) {
        int idx = find_bitmap_index(argv[1]);
        
        if (idx != -1) {
            
            if (bitmap_all(Bitmaps[idx], atoi(argv[2]), atoi(argv[3]))) printf("true\n");
            else printf("false\n");
        }
    }

    /* Finding set or unset bits. */
    if (!strcmp(argv[0], "bitmap_scan")) {
    
        int idx = find_bitmap_index(argv[1]);
    
        if (idx != -1) {
    
            bool val;
            if (!strcmp(argv[4], "true")) val = true;
            else val = false;

            printf("%zu\n", bitmap_scan(Bitmaps[idx], atoi(argv[2]), atoi(argv[3]), val));
        }
    
    }

    if (!strcmp(argv[0], "bitmap_scan_and_flip")) {
      
        int idx = find_bitmap_index(argv[1]);
      
        if (idx != -1) {
      
            bool val;
            if (!strcmp(argv[4], "true")) val = true;
            else val = false;
      
            printf("%zu\n", bitmap_scan_and_flip(Bitmaps[idx], atoi(argv[2]), atoi(argv[3]), val));
      
        }

    }

    /* File input and output. */    
    if (!strcmp(argv[0], "bitmap_file_size")) {
    
        int idx = find_bitmap_index(argv[1]);
    
        if (idx != -1) printf("%zu\n", bitmap_file_size(Bitmaps[idx]));
    }

    /* Debugging. */
    
    if (!strcmp(argv[0], "bitmap_dump")) {
        int idx = find_bitmap_index(argv[1]);
    
        if (idx != -1) bitmap_dump(Bitmaps[idx]);
    }

   

}







//
//
//여기가 메인함수입니다.
//
// 
int main() {
char cmd_line[MAXLINE]; 
char *argv[MAXARGS];

srand(time(NULL));

    while (fgets(cmd_line, sizeof(cmd_line), stdin)) { 
        int argc = 0;
        
        char *token = strtok(cmd_line, " \n\r"); // 토큰이라는 유명 명사 그대로 사용했습니다. ㅠㅠ 카피 아닙니다 ㅠㅠ

        while (token != NULL && argc < MAXARGS) {
        
            argv[argc] = token;
            // 다음 토큰으로 이동하려면 NULL로 설정
            token = strtok(NULL, " \n\r");
        
            argc++;
        }
    
        if (argc == 0) continue; 

        //핵심
        Do(cmd_line, argv); 


        // 초기화
        memset(cmd_line, 0, sizeof(cmd_line)); 
        memset(argv, 0, sizeof(argv));
    }
}




