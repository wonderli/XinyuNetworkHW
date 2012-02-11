#include "timer.h"
node* creat_node(int seq, long time)
{
        node *new_node = NULL;
        new_node = (node *) malloc(sizeof(node));
        if(new_node != NULL)
        {
                new_node->time_left = 0;
                new_node->time = time;
                new_node->seq = seq;
                new_node->prev = NULL;
                new_node->next = NULL;
        }
        else
        perror("creat_node malloc error");
        return new_node;
}

linklist* creat_list()
{
        linklist *new_list = NULL;
        new_list = (linklist *) malloc(sizeof(linklist));
        if(new_list != NULL)
        {
                new_list->len = 0;
                new_list->head = NULL;
                new_list->tail = NULL;
        }
        else
        perror("creat list malloc error");
}
int insert_node(linklist *list, node *insert_node)
{
        node *head = list->head;
        node *tail = list->tail;
        node *ptr = head;
        if(head == NULL)
        {
                list->head = insert_node;
                list->tail = insert_node;
                list->len++;
        }else if(insert_node->time <= head->time)
        {
                insert_node->prev = NULL;
                insert_node->next = head;
                head->prev = insert_node;
                list->len++;
        }
        else
        {
                while(ptr != NULL)
                {
                        /* inset node seq equals ptr seq*/
                        if(insert_node->seq == ptr->seq)
                        printf("This %d time node has alread exists.", insert_node->seq);
                        if(insert_node->seq <= ptr->seq)
                        {
                                insert_node->prev = ptr->prev;
                                ptr->prev->next = insert_node;
                                insert_node->next = ptr;
                                ptr->prev = insert_node;
                                ptr->time = ptr->time - insert_node->time;
                                list->len++;
                        }else
                        insert_node->time = insert_node->time - ptr->time;
                        ptr = ptr->next;
                        if(ptr == NULL && insert_node->next == NULL)
                        {
                                tail->next = insert_node;
                                insert_node->prev = tail;
                                insert_node->next = NULL;
                                list->tail = insert_node;
                                list->len++;
                        }

                }
        }

        return TRUE;
}


int cancel_node(linklist *list, int seq)
{
        node* head = list->head;
        node* tail = list->tail;
        node* ptr = list->head;
        if(ptr == NULL)
        {
                printf("Node num %d doesn't exists", seq);
        }
        else if(head->seq == seq)
        {
                ptr = ptr->next;
                if(ptr == NULL)/* list only have one node*/
                {
                        list->head = NULL;
                        list->tail = NULL;
                        list->len--;
                        remove_node(head);
                }
                else
                {
                        ptr->time = ptr->time + head->time;
                        ptr->prev = NULL;
                        list->len--;
                        list->head = ptr;
                        remove_node(head);
                }
        }
        else if(tail->seq == seq)
        {
                ptr = tail->prev;
                list->tail = ptr;
                ptr->next = NULL;
                list->len--;
                remove_node(tail);
        }
        else 
        {
                while(ptr != NULL)
                {
                        if(ptr->seq == seq)
                        {
                                node *tmp_prev = ptr->prev;
                                node *tmp_next = ptr->next;
                                tmp_prev->next = tmp_next;
                                tmp_next->prev = tmp_prev;
                                tmp_next->time = tmp_next->time + ptr->time;
                                remove_node(ptr);
                                list->len--;
                        }
                        ptr = ptr->next;
                }
        }
return TRUE;


}
int remove_node(node *remove_node)
{
        if(remove_node == NULL)
        {
                remove_node->prev = NULL;
                remove_node->next = NULL;
                free(remove_node);    
        }
        else
        perror("remove_node error");
        return TRUE;
}

void print_list(linklist *list)
{
        node *ptr;
        int seq;
        float time;
        printf("list len is %d\n", list->len);
        while(ptr != NULL)
        {
                seq = ptr->seq;
                time = ptr->time;
                printf("(%d, %f\n)->", seq, time);
                ptr = ptr->next;
        }
}
int main()
{
        linklist *List = creat_list();
        node* a = creat_node(1, 100);
        node* b = creat_node(2, 200);
        node* c = creat_node(3, 300);
        insert_node(List, a);
        insert_node(List, b);
        insert_node(List, c);
        print_list(List);
        return 0;
}

