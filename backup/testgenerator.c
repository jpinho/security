#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIMPLE_PAIR 1
#define CROSSED_MULT2	2
#define RANDOM_EDGES4 3

char* get_test_description(int);

int main(int argc, char* argv[])
{
	int testcases [12][3] = {
		{	10000, 10000, SIMPLE_PAIR },
		{	10000, 10000, CROSSED_MULT2 },
		{	10000, 10000, RANDOM_EDGES4 },
		
		{	10000, 80000, SIMPLE_PAIR },
		{	10000, 80000, CROSSED_MULT2 },
		{	10000, 80000, RANDOM_EDGES4 },
		
		{	10000, 20000, SIMPLE_PAIR },
		{	10000, 20000, CROSSED_MULT2 },
		{	10000, 20000, RANDOM_EDGES4 },				
		
		{	100000, 100000, SIMPLE_PAIR },
		{	100000, 100000, CROSSED_MULT2 },
		{	100000, 100000, RANDOM_EDGES4 }
	};

	int i;
	srand(time(NULL));
	
	for(i=0; i<12; i++)
	{	
		int s, j;
		char filepath[100];
		
		int students = testcases[i][0],
	 		  jobs = testcases[i][1],
	 		  test_type = testcases[i][2];
		
		sprintf(filepath, "tests/test-k%02d.txt", i+1);
		printf("writting test %02d\n", i+1);
		
		FILE* testout;
		testout = fopen(filepath, "a+w");
		
		fprintf(testout, "%d %d\n", students, jobs);
		
		for(s=1; s<=students;s++) {
			switch(test_type){
				case SIMPLE_PAIR: 
					fprintf(testout, "1 %d\n", (s-1) % jobs);
					break;
				case CROSSED_MULT2:
					fprintf(testout, "2 %d %d\n", (s==1 ? s-1 : s-2) % jobs , (s==1 ? s+2: s+1) % jobs);
					break;
				case RANDOM_EDGES4: {
					int ntests = 4;
					fprintf(testout, "%d", ntests);
					
					int* jobs_choosed = (int*)calloc(jobs, sizeof(int));
					while(ntests-- > 0)
					{
						int testid = 0; 
						while(jobs_choosed[(testid = rand() % jobs)] == 1);
						jobs_choosed[testid] = 1;
						
						fprintf(testout, " %d", testid);
					}
						
					fprintf(testout, "\n");
					break;
				}
			}
		}

		fprintf(testout, "\n\n");
		fprintf(testout, "[Teste: K%02d]\n", i+1);
		fprintf(testout, "[# Estudantes: %d]\n", students);
		fprintf(testout, "[# Empregos: %d]\n", jobs);
		fprintf(testout, "[Descrição: %s]", get_test_description(test_type));
				
		fclose(testout); 		  
	}
	
	return 0;
}

char* get_test_description(int test_type)
{
	switch(test_type){
		case SIMPLE_PAIR: return "Teste SIMLE_PAIR, consiste num teste onde cada estudante quer apenas o emprego correspondente ao seu número, exemplo: s1-e1; s2-e2, ...\n"
							 			 		 "Este teste produz caminhos de aumento de nível 2\n";
		
		case CROSSED_MULT2: return "Teste CROSSED_MULT2, consiste num teste cruzado de multiplicidade 2, ou seja, cada estudante quer o emprego correspondente ao seu número,\n"
														"ou o emprego do colega do lado, e o colega do lado quer o seu, ou então quer o que corresponde ao seu número, exemplo: s1-e1,e2; s2-e1,e2, ...\n"
														"Este teste obriga a que todos os estudantes sejam empurrados para a sua segunda escolha, por forma a darem oportunidade ao estudante seguinte.\n"
														"Este teste produz caminhos de aumento de nivel 2.\n";
		
		case RANDOM_EDGES4: return "Teste RANDOM_EDGES4, consiste num teste onde todos os estudantes escolhem pelo menos quatro empregos aleatórios";
		
		default: return "N/A";
	}
	
	return NULL;
}
