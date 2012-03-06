#include "deltalist.h"
/* Create new node */
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

/* Create new deltalist */
linklist* create_list()
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
        return new_list;
}
/* Insert new node and update list */
int insert_node(linklist *list, node *n)
{
        node *head = list->head;
        node *tail = list->tail;
        node *ptr = head;

	long dtime = n->time;
        list->len++;

        //printf("Insert node :%d, %d\n", n->seq, n->time);

	if (head == NULL) {
		list->head = n;
		list->tail = n;
		return TRUE;
	}

	for (ptr = head; ptr != NULL; ptr = ptr->next) {
		if (ptr->time > dtime) {
			break;
		}
		dtime -= ptr->time;
	}

	n->next = ptr;
	n->time = dtime;
	if (ptr == NULL) {
		n->prev = tail;
		list->tail = n;
                tail->next = n;

		return TRUE;
	}
        n->prev = ptr->prev;
        if (ptr->prev != NULL) {
                ptr->prev->next = n;
        }
        ptr->prev = n;
        if (ptr->next != NULL) {
                ptr->next->prev = n;
        }

	for (; ptr != NULL; ptr = ptr->next) {
		list->tail = ptr;
		ptr->time -= dtime;
		dtime = 0;
	}

	/*
        if(head == NULL)
        {
                list->head = insert_node;
                list->tail = insert_node;
                list->len++;
        } else if(insert_node->time <= head->time) {
                if(insert_node->seq == ptr->seq)
                {
                        printf("This seq %d node has already exits.", insert_node->seq);
                        return FALSE;
                }
                insert_node->prev = NULL;
                insert_node->next = head;
                head->prev = insert_node;
                list->head = insert_node;
                head->time = head->time - insert_node->time;
                list->len++;
        }
        else
        {
                while(ptr != NULL)
                {
                        if(insert_node->seq == ptr->seq)
                        {
                        
                                printf("This seq %d node has alread exists.", insert_node->seq);
                                return FALSE;
                        }

                        if(insert_node->time <= ptr->time)
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
	*/

        return TRUE;
}

/* cancel node action */
int cancel_node(linklist *list, int seq)
{
        node* head = list->head;
        node* tail = list->tail;
        node* ptr = list->head;

        node *found = NULL;
        long dtime = 0;

        for (ptr = list->head; ptr != NULL; ptr = ptr->next) {
                if (ptr->seq == seq) {
                        found = ptr;
                        break;
                }
        }

        if (found == NULL) {
                return FALSE;
        }

        list->len--;

        dtime = found->time;

        if (found->prev != NULL) {
                found->prev->next = found->next;
        }
        if (found->next != NULL) {
                found->next->prev = found->prev;
        }

        if (found == head) {
                list->head = NULL;
                list->tail = NULL;
                return TRUE;
        }

        list->tail = found->next;

        for (ptr = found->next; ptr != NULL; ptr = ptr->next) {
                ptr->time += dtime;
                dtime = ptr->time;
                list->tail = ptr;
        }

        if (list->tail == NULL) {
                list->tail = found->prev;
        }

        /*
        if(ptr == NULL)
        {
                printf("Node num %d doesn't exists", seq);
        }
        else if(head->seq == seq)
        {
                ptr = ptr->next;
                if(ptr == NULL)
                {
                        list->head = NULL;
                        list->tail = NULL;
                        list->len--;
                        remove_node(head);
                }
                else
                {
                        list->head = ptr;
                        ptr->time = ptr->time + head->time;
                        ptr->prev = NULL;
                        list->len--;
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
				ptr = tmp_next;
                                list->len--;
                        } else {
				ptr = ptr->next;
			}
                }
        }
        */
return TRUE;


}

/* Remove node from list */
int remove_node(node *remove_node)
{
        if(remove_node != NULL)
        {
                remove_node->prev = NULL;
                remove_node->next = NULL;
                
                free(remove_node);    
        }
        else
		perror("remove_node error");
        return TRUE;
}

/* Print list */
int print_list(linklist *list)
{
        node *ptr;
        int seq;
        float time;
        int i = 0;
        printf("list len is %d\n", list->len);
        ptr = list->head;
        while(ptr != NULL)
        {
                seq = ptr->seq;
                time = ptr->time;
                printf("(%d, %f)->", seq, time);
                ptr = ptr->next;
                i++;
        }
        printf("\n");
        if (i != list->len) {
                printf("ERROR: len should be %d\n", i);
                return -1;
        }
        return 0;
}

/* Test the head of this list whether expired or not*/
int expire(linklist *list)
{
	if(list == NULL)
	{
		perror("list doesn't exits!");
	}
	else
	{
		if(list->head->time <= 0)
                {
			return TRUE;
                }
	}
	return FALSE;
}

int insert_and_print(linklist *List, int seq, int time) {
        node* a = creat_node(seq, time);
        printf("INSERT: %d %d\n", seq, time);
        insert_node(List, a);
        print_list(List);
        printf("-----------------\n");
	return 0;
}

int cancel_and_print(linklist *List, int seq) {
        printf("CANCEL: %d\n", seq);
        cancel_node(List, seq);
        print_list(List);
        printf("-----------------\n");
	return 0;
}

int main()
{
        linklist *List = create_list();

        insert_and_print(List, 1, 359);
        insert_and_print(List, 2, 9403);
        insert_and_print(List, 3, 9403);
        insert_and_print(List, 4, 9403);
        insert_and_print(List, 5, 9403);
        cancel_and_print(List, 5);
        insert_and_print(List, 6, 9270);
        insert_and_print(List, 7, 9270);
        insert_and_print(List, 8, 9270);
        insert_and_print(List, 9, 9270);
        insert_and_print(List, 10, 9270);
        cancel_and_print(List, 10);

        return 0;
}

