#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STUDENT_BASE 11000
#define oo 2000000000

//data types
typedef struct node	   { int value; int capacity; struct node* prev; struct node* next; struct vertex* dual; int is_back; } node;

typedef struct vertex  { int value; struct list* edges;			} vertex;
typedef struct vnode   { vertex* value; struct vnode* next;		} vnode;

typedef struct list	   { node* head; node* tail; int length;	} list;
typedef struct vlist   { vnode* head; int length;				} vlist;
typedef struct graph   { vertex** vertices;	int length;			} graph;

//prototypes
node* node_new();
list* list_new();

vnode* vnode_new();
vlist* vlist_new();
vertex* stack_pop(vlist*);
void stack_push(vlist*, vertex*);
void stack_free(vlist*);

vertex* vertex_new(); 
graph* graph_new(int);

void list_add(list*, int); 
node* list_remove(list*, node*);
void list_add_node(list*, node*);

void graph_add(graph*, int, int);
void reverse_edge(graph*, vertex*, node*);

int quick_maxflow(graph*, int, int);
node** dfs(graph*, int, int, int*);

int main(int argc, char* argv[])
{
	graph* jobs = NULL;
	int nstud, noffers, stud_i, njobs, jobid;
	int s,t,n;
	int maxflow;

	fscanf(stdin, "%d %d\n", &nstud, &noffers);

	n = noffers + nstud + 2;
	s = n-2;
	t = n-1;
	
	jobs = graph_new(n);

	for(stud_i=0; stud_i<nstud; stud_i++) 
	{
		graph_add(jobs, s, noffers+stud_i);
		fscanf(stdin, "%d", &njobs);

		while(njobs > 0) {
			fscanf(stdin,"%d", &jobid);
			graph_add(jobs, noffers+stud_i, jobid);

			if(jobs->vertices[jobid]==NULL)
				graph_add(jobs, jobid, t);

			njobs--;
		}
	}

	jobs->vertices[t] = vertex_new();
	jobs->vertices[t]->value = t;
	jobs->vertices[t]->edges = list_new();

	maxflow = quick_maxflow(jobs, s, t);
	printf("%d\n", maxflow);

	return 0;
}


/******************* :: LIBRARY :: **************************/

//functions - queues
void stack_push(vlist* stack, vertex* vx){
	vnode* vn = vnode_new();
	vn->value = vx;

	if(stack->head==NULL) {
		stack->head = vn;
	}
	else { 
		vn->next = stack->head;
		stack->head = vn;
	}
	
	stack->length++;	
}

vertex* stack_pop(vlist* stack){
	vnode* root;
	vertex* vx;

	if(!stack || !stack->head) return NULL;
	
	root = stack->head;
	stack->head = stack->head->next;

	stack->length--;
	vx = root->value;
	free(root);
	return vx;
}

void stack_free(vlist* stack){
	vnode* n = stack->head;

	while(n!=NULL){
		vnode* n_ptr = n;
		n = n->next;
		free(n_ptr);
	}

	free(stack);
}

//functions - max flows
int quick_maxflow(graph* G, int s, int t){
	int f = 0, v;

	while(1){
		int m=0;
		node** P = dfs(G, s, t, &m);

		if(m==0) break;

		f += 1;
		
		v = t;
		while(v!=s)
		{
			int next_v = P[v]->dual->value;

			if(v!=t && P[v]->dual->value != s)
			{
				vertex* vx_in_setA = P[v]->dual;
				node* to_front_edge = P[v];
				reverse_edge(G, vx_in_setA, to_front_edge);
			}
			else P[v]->capacity = 0;
				
			v = next_v;
		}
	}
	
	return f;
}

node** dfs(graph* G, int s, int t, int* m)
{
	vertex* u;
	node* v;
	node** P = (node**)calloc(G->length, sizeof(node*));
	vlist* S = vlist_new();

	stack_push(S, G->vertices[s]);

	while(S->length > 0)
	{
		u = stack_pop(S);	
		v = u->edges->head;

		while(v!=NULL)
		{
			if(P[v->value] == NULL && v->capacity > 0)
			{
				if(v->is_back && G->vertices[v->value]->edges->length == 0){
					list_remove(v->dual->edges, v);
					v = v->next;
					continue; //to next edge
				}

				P[v->value] = v;

				if(v->value == t){
					*m = 1;
					return P;
				}

				stack_push(S, G->vertices[v->value]);
			}

			v = v->next;
		}
	}

	*m=0;
	return P;
}

//functions - node
node* node_new(){
	node* n = (node*)malloc(sizeof(node));
	n->next = n->prev = NULL;
	n->dual = NULL;
	n->value=0;
	n->capacity=1;
	n->is_back=0;
	return n;
}
vnode* vnode_new(){
	vnode* n = (vnode*)malloc(sizeof(vnode));
	n->next = NULL;
	n->value = NULL;
	return n;
}

//functions - vertex
vertex* vertex_new(){
	vertex* v = (vertex*)malloc(sizeof(vertex));
	v->edges = NULL;
	v->value = 0;
	return v;
}

//functions - list
list* list_new(){
	list* lst = (list*)malloc(sizeof(list));
	lst->head = lst->tail = NULL;
	lst->length=0;
	return lst;
}

vlist* vlist_new(){
	vlist* lst = (vlist*)malloc(sizeof(vlist));
	lst->head = NULL;
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

void list_add_node(list* lst, node* no){
	node* n = no;
	
	if(lst->head == NULL)
		lst->head = n;
	else {
		lst->tail->next = n;
		n->prev = lst->tail;
	}

	lst->tail = n;
	lst->length++;
}

node* list_remove(list* lst, node* no){
	if(lst->head == no)
		lst->head = no->next;
	
	if(lst->tail == no)
		lst->tail = no->prev;

	if(no->prev != NULL)
		no->prev->next = no->next;

	if(no->next != NULL)
		no->next->prev = no->prev;
	
	lst->length--;
	return no;
}

//functions - graph
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
	grp->vertices[vertice]->edges->tail->dual = grp->vertices[vertice];
}

void reverse_edge(graph* G, vertex* vx_in_setA, node* to_front_edge){
	vertex* new_dual = G->vertices[to_front_edge->value];

	node* back_edge = list_remove(vx_in_setA->edges, to_front_edge);
	back_edge->value = to_front_edge->dual->value;
	back_edge->dual = new_dual;
	back_edge->is_back = 1;

	list_add_node(back_edge->dual->edges, back_edge);
}