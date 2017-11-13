#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/*global variables:*/

/* the sudoku grid*/
	int grid[9][9] = {  
		{6,2,4,5,3,9,1,8,7},  
		{5,1,9,7,2,8,6,3,4},   
		{8,3,7,6,1,4,2,9,5},   
		{1,4,3,8,6,5,7,2,9},   
		{9,5,8,2,4,7,3,6,1},   
		{7,6,2,3,9,1,4,5,8},   
		{3,7,1,9,5,6,8,4,2},   
		{4,9,6,1,8,2,5,7,3},   
		{2,8,5,4,7,3,9,1,6}};

/*struct for passing parameters*/
	typedef struct{
		int row;
		int column;
	}parameters;

/*array to keep track of the results returned by workers*/
int isValid[11];
int validIndex=0;

/*worker method to check that each row contains digits 1-9*/
void *workerRow(){
	int valid=1;
	int i;
	for(i=0; i<9; i++){
		int check[9]={0,0,0,0,0,0,0,0,0}; /*keep track of digits found*/
		int j;
		for(j=0; j<9 && valid==1; j++){
			if(grid[i][j]<=9 && grid[i][j]>=1){ /*valid*/
				int x=grid[i][j]; 
				if(check[x-1]==0){ /*digit not found before within same row*/
					check[x-1]=1;}
				else{valid=0;} /*repetition*/
					
			}else{valid=0;}
		}
	}
	 isValid[validIndex++]=valid;
	pthread_exit(0);
}

/*worker method to check that each column has digits 1-9*/
void *workerCol(){
	int valid=1;
	int i;
	for(i=0; i<9; i++){
		int check[9]={0,0,0,0,0,0,0,0,0};  /*keep track of digits found*/
		int j;
		for(j=0; j<9 && valid==1; j++){
			if(grid[j][i]<=9 && grid[j][i]>=1){ /*valid*/ 
				int x=grid[j][i];
				if(check[x-1]==0){ /*digit not found before within same row*/
					check[x-1]=1;}
				else{valid=0;} /*repetition*/
					
			}else{valid=0;}
		}
	}
	 isValid[validIndex++]=valid;
	pthread_exit(0);
}

/*worker method to check that each subgridhas digits 1-9*/
void *workerSubGrid(void *param){
	int row=((parameters *)param)->row;
	int col=((parameters *)param)->column;
	int valid=1;
	int check[9]={0,0,0,0,0,0,0,0,0}; /*keep track of digits found*/
	int i;
	for(i=row; i<row+3; i++){
		int j;
		for(j=col; j<col+3 && valid==1; j++){
			if(grid[i][j]<=9 && grid[i][j]>=1){ /*valid*/ 
				int x=grid[i][j];
				if(check[x-1]==0){ /*digit not found before*/
					check[x-1]=1;}
				else{valid=0;} /*repetition*/
					
			}else{valid=0;}
		}
	}
	isValid[validIndex++]=valid;
	pthread_exit(0);
}

int main(){
	
	pthread_t tid[11]; //ids of each thread
	pthread_attr_t attr; //set of thread attributes
	 
	pthread_attr_init(&attr);
/*workerRow checks if each row contains the digits 1-9*/
	pthread_create(&tid[9], &attr, workerRow, NULL);
	

/*workerCol checks if each column contains the digits 1-9*/
	pthread_create(&tid[10], &attr, workerCol, NULL);
	

/*creating 9 threads to check that each of the 3*3 subgrids contains the digits 1-9*/
	parameters arrayData[9];
	int index=0;
	int a;
	int b;
	for(a=0; a<=6; a=a+3){
		for(b=0; b<=6; b=b+3){
			arrayData[index].row=a;
			arrayData[index++].column=b;

		}
	}
	

/*workerSubGrid checks if each subGrid contains the digits 1-9*/
	int i;
	for(i=0; i<9; i++){
		pthread_create(&tid[i], &attr, workerSubGrid, &arrayData[i]);
	}

/*wait for workerCol and Row and subgrid thread to terminate*/
	int k;
	for(k=0; k<11; k++){
		pthread_join(tid[k], NULL);
	}


/*loop through isValid array to check is suduko grid is solved*/
	int valid=1;
	for(i=0; i<11 && valid==1; i++){
		if(!isValid[i]){
			valid=0;
		}
	}
	if(valid==1){
		printf("The sudoku grid is valid");
	}else{
		printf("The sudoku grid is invalid");
	}
}


