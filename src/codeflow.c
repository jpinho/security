#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define oo 1 << 28

/* data types */
typedef struct node	   { int value; struct node* prev; struct node* next; } node;
typedef struct vertex  { int value; struct list* edges;					  } vertex;
typedef struct vnode   { vertex* value; struct vnode* next;				  } vnode;
typedef struct list	   { node* head; node* tail; int length;			  } list;
typedef struct vlist   { vnode* head; vnode* tail; int length;			  } vlist;
typedef struct graph   { vertex** vertices;	int length; int partition[2]; } graph;

/* constants */
static vertex* NIL;
static int* Dist = NULL;
static vertex** Pair = NULL;
static vlist* Q = NULL;

/* prototypes */
node* node_new();
list* list_new();
vnode* vnode_new();
vlist* vlist_new();
vertex* vertex_new(); 
graph* graph_new(int);

vertex* dequeue(vlist*);
void enqueue(vlist*, vertex*);
void list_add(list*, int); 
void graph_add(graph*, int, int);

int hopcroft_karp(graph*);
int bfs(graph*);
int dfs(graph*, vertex*);

/* functions - queues */
void enqueue(vlist* queue, vertex* vx){
	vnode* vn = vnode_new();
	vn->value = vx;
    
	if(queue->head==NULL)
		queue->head = vn;
	else
		queue->tail->next = vn;
	
	queue->tail = vn;
	queue->length++;
}

vertex* dequeue(vlist* queue){
	vnode* root;
	vertex* vx;
    
	if(!queue || !queue->head) return NULL;
	
	root = queue->head;
	queue->head = queue->head->next;
    
	queue->length--;
	vx = root->value;
	return vx;
}

/* functions - node */
node* node_new(){
	node* n = (node*)malloc(sizeof(node));
	n->prev = NULL;
	n->next = NULL;
	n->value=0;
	return n;
}

vnode* vnode_new(){
	vnode* n = (vnode*)malloc(sizeof(vnode));
	n->next = NULL;
	n->value = NULL;
	return n;
}

/* functions - vertex */
vertex* vertex_new(){
	vertex* v = (vertex*)malloc(sizeof(vertex));
	v->edges = NULL;
	v->value = 0;
	return v;
}

/* functions - list*/
list* list_new(){
	list* lst = (list*)malloc(sizeof(list));
	lst->head = lst->tail = NULL;
	lst->length=0;
	return lst;
}

vlist* vlist_new(){
	vlist* lst = (vlist*)malloc(sizeof(vlist));
	lst->head = lst->tail = NULL;
	lst->length=0;
	return lst;
}

void list_add(list* lst, int value){
	node* n = node_new();
	n->value = value;
	
	if(lst->head == NULL)
		lst->head = n;
	else {
		lst->tail->next = n;
		n->prev = lst->tail;
	}
    
	lst->tail = n;
	lst->length++;
}

/* functions - max flows */
int hopcroft_karp(graph* G){
	int f=0, i;
	vertex* v;
    
    /* {ptr shift, so that [-1] is at first position} */
	Dist = ((int*)malloc((1 + G->length)*sizeof(int))) + 1;
	Pair = (vertex**)malloc(G->length*sizeof(vertex*));
	Q = vlist_new();

	/* init Dist={oo} and Pair={NIL} */
	for(i=0; i < (G->length + 1); Dist[i] = oo, Pair[i] = NIL, i++) ;

	while(bfs(G))
		for(i = G->partition[0]; i < G->length; i++){
			v = G->vertices[i];
			if(Pair[v->value] == NIL && dfs(G, v)) 
				f++;
		}

	return f;
}

int bfs(graph* G) {
	int i=0;
	vertex* v;
	node* u;

	for(i = G->partition[0]; i < G->length; i++){
		v = G->vertices[i];
		if(Pair[v->value] == NIL){
			Dist[v->value] = 0;
			enqueue(Q, v);
		}
		else Dist[v->value] = oo;
	}

    Dist[NIL->value] = oo;

    while(Q->length > 0) {
        v = dequeue(Q);

        if(Dist[v->value] < Dist[NIL->value]){
			u = v->edges->head;
			
			while(u != NULL){
                if(Dist[ Pair[u->value]->value ] == oo) {
                    Dist[ Pair[u->value]->value ] = Dist[v->value] + 1;
                    enqueue(Q, Pair[u->value]);
				}
				u = u->next;
			}
		}
	}

    return Dist[NIL->value] != oo;
}

int dfs(graph* G, vertex* v){
	node* u;

	if(v != NIL) {
		u = v->edges->head;

		while(u!=NULL) {
            if(Dist[ Pair[u->value]->value ] == Dist[v->value] + 1)
                if(dfs(G, Pair[u->value]) == 1) {
					Pair[u->value] = v;
					Pair[v->value] = G->vertices[u->value];
                    return 1;
				}

			u=u->next;
		}
        
		Dist[v->value] = oo;
        return 0;
   }
   return 1;
}

/* functions - graph */
graph* graph_new(int vertices){
	graph* grp = (graph*)malloc(sizeof(graph));
	grp->vertices = (vertex**)calloc(vertices, sizeof(vertex*));
	grp->length = vertices;
	return grp;
}

void graph_add(graph* grp, int vertice, int edge){
	if(grp->vertices[vertice] == NULL){
		grp->vertices[vertice] = vertex_new();
		grp->vertices[vertice]->value = vertice;
		grp->vertices[vertice]->edges = list_new();
	}
	list_add(grp->vertices[vertice]->edges, edge);
}


/**
 * problem of jobs max allocation
 * implemented based on the hopcroft-karp algorithm.
 */
int main(int argc, char* argv[])
{
	graph* jobs = NULL;
	int nstud, noffers, stud_i, njobs, jobid, n;
	float seconds;
	clock_t start, end;
    
	start = clock();
	
	/* init special null vertex */
	NIL = vertex_new();
	NIL->edges = NULL;
	NIL->value = -1;
    
	fscanf(stdin, "%d %d\n", &nstud, &noffers);
    
	n = noffers + nstud;
	jobs = graph_new(n);
    
	for(stud_i=0; stud_i<nstud; stud_i++) {
		fscanf(stdin, "%d", &njobs);
        
		while(njobs-- > 0) {
			fscanf(stdin,"%d", &jobid);
			graph_add(jobs, noffers+stud_i, jobid);
		}
	}
	
	jobs->partition[0] = noffers; /* {students} */
	jobs->partition[1] = 0;		  /* {jobs} */
    
	printf("%d\n", hopcroft_karp(jobs));
	
	end = clock();
	seconds = (float)(end - start) / (float)CLOCKS_PER_SEC;
	
	printf ("Your calculations took %.2lf seconds to run.\n", seconds );
	free(jobs);
    
	return 0;
}
