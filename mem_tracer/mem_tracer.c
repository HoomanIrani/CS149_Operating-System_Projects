// Name: Houman Irani
// Class: CS 149
// Professor: William Andreopoulos
// Date: 04/07/2022
// Assignment 4 mem_tracer


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdlib.h>


#define MAXIMUM_LINE_LENGTH 10
#define MAXIMUM_NUM_LINES 1024



struct TRACE_NODE_STRUCT {
    
    char* functionid;
    struct TRACE_NODE_STRUCT* next;
};


struct LINK_NODE {
    char* line;
    int index;
    struct LINK_NODE* next;
};



typedef struct TRACE_NODE_STRUCT TRACE_NODE;
typedef struct LINK_NODE NODE;

static TRACE_NODE* TRACE_TOP = NULL;


NODE *head = NULL;


void PUSH_TRACE(char* p)  {

    TRACE_NODE* tnode;
    static char global[] = "Global";

    if (TRACE_TOP == NULL) {

        TRACE_TOP = (TRACE_NODE*) malloc(sizeof(TRACE_NODE));

        if (TRACE_TOP == NULL) {
            printf("PUSH_TRACE: Memory Allocation Error\n");
            exit(1);
        }

        TRACE_TOP->functionid = global;
        TRACE_TOP->next = NULL;

    }


    tnode = (TRACE_NODE*) malloc(sizeof(TRACE_NODE));

    if (tnode==NULL) {
        printf("PUSH_TRACE: Memory Allocation Error\n");
        exit(1);
    }

    tnode->functionid = p;
    tnode->next = TRACE_TOP;

    TRACE_TOP = tnode;
}



void POP_TRACE() {

    TRACE_NODE* tnode;
    tnode = TRACE_TOP;
    TRACE_TOP = tnode->next;
    free(tnode);
}



//Function Print_Trace
char* PRINT_TRACE() {

    int d = 50;
    int i, l, j;
    TRACE_NODE* tnode;
    static char buf[100];

    if (TRACE_TOP==NULL) {

        strcpy(buf,"Global");
        return buf;
    }
    sprintf(buf,"%s",TRACE_TOP->functionid);

    l = strlen(buf);

    for(i = 1, tnode = TRACE_TOP->next; tnode != NULL && i < d; i++,tnode = tnode->next) {

        j = strlen(tnode->functionid);

        if (l + j + 1 < 100) {

            sprintf(buf + l,":%s",tnode->functionid);
            l += j+1;
        }

        else {

            break;
        }
    }

    return buf;
}




// Function REALLOCATE
void* REALLOCATE(void* p, int t, char* file, int line, const char *function ) {

    printf("File %s, Line %d, Function %s Reallocated the memory at address %p to a new size %d\n",
           file, line, function, p, t);
    p = realloc(p,t);

    printf("FUNCTION STACK TRACE: %s\n", PRINT_TRACE());
    return p;
}



// Function MALLOC
void* MALLOC(int t,char* file,int lines,const char *function) {

    void* p;
    p = malloc(t);
    printf("File %s, line %d, function %s allocated new memory segment at address %p to size %d\n",
           file, lines, function, p, t);
    printf("FUNCTION STACK TRACE: %s\n", PRINT_TRACE());
    return p;
}



// Function FREE_MEM
void FREE_MEM(void* p,char* file,int line, const char *function) {
    printf("File %s, line %d, function %s deallocated the memory segment at address %p\n",
           file, line, function, p);

    free(p);
    printf("FUNCTION STACK TRACE: %s\n", PRINT_TRACE());
}



// Function addToList
void addToList(char *lines, int index) {

    NODE *temp = (NODE *)malloc(sizeof(NODE));
    temp->line = (char *)malloc(strlen(lines)+1);

    memset(temp->line, '\0', strlen(lines)+1);

    strncpy(temp->line, lines, strlen(lines)+1);
    temp->index=index;
    temp->next=NULL;


    if (head == NULL) {

        head=temp;
    }

    else {

        NODE *t=head;

        while(t->next != NULL) {

            t=t->next;
        }

        t->next=temp;
    }

}




// Function printList dedicated to print values in List
void printList(NODE *head) {

    printf("Following are the content of the linked list: \n");

    if(head != NULL) {

        printf("Index: %d Line: %s", head->index, head->line);
        printList(head->next);\

    }

    printf("\n");
}




//Function freeList
void freeList() {

    NODE *t=head;
    while(t) {

        NODE *temp=t;
        free(temp->line);
        t=t->next;
        free(temp);

    }
}



#define reallocate(a,b) REALLOCATE(a,b,__FILE__,__LINE__,__FUNCTION__)
#define malloc(a) MALLOC(a,__FILE__,__LINE__,__FUNCTION__)
#define free_mem(a) FREE_MEM(a,__FILE__,__LINE__,__FUNCTION__)

// Main Function
int main(int argc, char **argv) {

    char **array;
    int i;
    FILE *fptr =fopen(argv[1], "r");
    int index = 0;
    char lineRead[1024];


    if (fptr == NULL) {
        printf("Error! Opening Input File");
        exit(1);
    }

    int fDesc = open("memtrace.out",O_WRONLY|O_CREAT|O_TRUNC);

    dup2(fDesc, 1) ;

    array = malloc(MAXIMUM_NUM_LINES*sizeof(char *));
    for(i = 0; i < MAXIMUM_NUM_LINES; i++) {

        array[i] = malloc(MAXIMUM_LINE_LENGTH * sizeof(char));
    }

    while((fgets (lineRead, 1024, fptr))!= NULL) {

        int len = strlen(lineRead);

        if(index >= MAXIMUM_NUM_LINES) {

            array=reallocate(array, index+1);
            array[index]=malloc(len * sizeof(char));
        }
        else if (len > MAXIMUM_LINE_LENGTH) {

            array[index] = reallocate(array[i], MAXIMUM_LINE_LENGTH+len);
        }

        strncpy(array[index], lineRead, len);
        addToList(lineRead, index);
        index++;
    }


    array = reallocate(array, ((MAXIMUM_NUM_LINES+1)* sizeof(char *)));
    array[MAXIMUM_LINE_LENGTH] = malloc(MAXIMUM_LINE_LENGTH * sizeof(char));


    close(fDesc);
    fclose(fptr);

    printList(head);
    freeList();


    printf("Printing From The Array: \n");


    for (i = 0; i < index; i++) {

        printf("%d: %s", i, array[i]);
    }


    for (i = 0; i < index; i++) {

        free_mem(array[i]);
    }

    free_mem(array);

    return 0;

}