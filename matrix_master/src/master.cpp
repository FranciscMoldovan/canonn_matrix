#include "pvm3.h"
#include <stdio.h>
#define N 4

int  main()
{
   int *task_ids;
   int my_tid,i,j;
   int nr_proc = 0;
   
  

   //matrix size
   int n=0;
   //block size
   int blockS = 0;
   //number of blocks
   int nrBlocks =0;

   //matrices to be multiplied
	int** A;
	int** B;
   //result matrix
	int** result;

   //read and initialize data
	FILE * f;
	f = fopen("testfile2.txt","r");
	//read matrix dimension
	fscanf(f,"%d",&n);
	printf("\nMatrix dimension is %d\n",n);
	//read block size
	fscanf(f,"%d",&blockS);
	printf("\nBlock size is %d \n", blockS);

	A = new int*[n];
	B = new int*[n];
    result = new int* [n];


	for (i=0; i<n; i++){
		result[i] = new int[n];
		A[i] = new int[n];
		B[i] = new int[n];
	}

	//initialize result
	for (i=0; i<n; i++)
		for (j=0; j<n; j++)
			result[i][j] = 0;

	
	//read matrix A
	for(i=0; i<n; i++)
		for(j=0;j<n;j++)
			fscanf(f,"%d",&A[i][j]);

	//read matrix B
	for(i=0; i<n; i++)
		for(j=0;j<n;j++)
			fscanf(f,"%d",&B[i][j]);

	
		for(i=0; i<n; i++){
			for(j=0;j<n;j++)
				printf("%d ",A[i][j]);
			printf("\n");
		}
		printf("\n");
		for(i=0; i<n; i++){
			for(j=0;j<n;j++)
				 printf("%d ",B[i][j]);
			printf("\n");
		}

	fclose(f);

	my_tid = pvm_mytid();

	nrBlocks = n/blockS;
	printf("\nNr of blocks is %d \n",nrBlocks);
	nr_proc = nrBlocks*nrBlocks;
	printf("\nNr of processes is %d \n",nr_proc);
	task_ids = new int[nr_proc];

    pvm_spawn("lab6_slave", (char **)0, PvmTaskDefault, "", nr_proc, task_ids);

	int k=0;
	//matrix of mesh
	int** a = new int* [nrBlocks];
	for (i = 0; i < nrBlocks; i++)
		a[i] = new int[nrBlocks];


	for(i=0;i<nrBlocks;i++)
		for(j=0;j<nrBlocks;j++)
			a[i][j] = task_ids[k++];

//      print mesh
		for(i=0;i<nrBlocks;i++){
				for(j=0;j<nrBlocks;j++)
					printf("%d ",a[i][j]);
			printf("\n");
		}

	// send neighbors to all processes
	int v_dreapta;
	int v_stanga;
	int v_sus;
	int v_jos;


	for(i=0;i<nrBlocks;i++)
		for(j=0;j<nrBlocks;j++){
		
			 pvm_initsend(PvmDataDefault);

			 // vecin stanga 
			 if(j > 0 )  v_stanga = a[i][j-1];
			 else		 v_stanga = a[i][nrBlocks-1];
  			 pvm_pkint(&v_stanga, 1, 1);


			// vecin dreapta
			 if(j < nrBlocks-1 )  v_dreapta = a[i][j+1];
			 else		   v_dreapta = a[i][0];
  			 pvm_pkint(&v_dreapta, 1, 1);

			 // vecin sus
			 if(i > 0 )  v_sus = a[i-1][j];
			 else		 v_sus = a[nrBlocks-1][j];
  			 pvm_pkint(&v_sus, 1, 1);

			  // vecin jos 
			 if(i < nrBlocks-1)  v_jos = a[i+1][j];
			 else		  v_jos = a[0][j];
  			 pvm_pkint(&v_jos, 1, 1);

			 pvm_send(a[i][j], 0);
		}


				


	int q,t,counti,countj;
 

	//alloc space for blocks (blockS*blockS)
	int** blockFromA = new int* [blockS];
	int** blockFromB = new int* [blockS];

	for (i=0; i<blockS; i++){
		blockFromA[i] = new int[blockS];
		blockFromB[i] = new int[blockS];
	}

  
	int z,v;
	//sending the blocks to processes	
	for (i=0; i<nrBlocks; i++)
		for (j=0; j<nrBlocks; j++){

			//build small block
			counti = 0; countj = 0;
			for(q=i*blockS; q<(i+1)*blockS; q++){
				for (t=j*blockS; t<(j+1)*blockS; t++){
					blockFromA[counti][countj] = A[q][t];
					blockFromB[counti][countj] = B[q][t];
					countj++;
				}
				counti++;
				countj = 0;
			}


		printf("\n");
			//send block
			pvm_initsend(PvmDataDefault);	
			pvm_pkint(&nrBlocks,1,1);
			pvm_pkint(&blockS,1,1);

			for(t=0; t<blockS; t++)
				pvm_pkint(blockFromA[t],blockS,1);
			printf("sending block to :%d \n", a[i][(j+i)%nrBlocks]);
			pvm_send(a[i][(j+i)%nrBlocks],0);


			pvm_initsend(PvmDataDefault);	
			for(t=0; t<blockS; t++)
				pvm_pkint(blockFromB[t],blockS,1);
			printf("sending block to :%d \n", a[(j+i)%nrBlocks][j]);
			pvm_send(a[(j+i)%nrBlocks][j],2);


		}






	//compute the matrix multiplication sequentially
		for (i=0; i<n; i++)
			for (j=0; j<n; j++)
				for(z=0; z<n; z++)
					result[i][j]+=(A[i][z]*B[z][j]);
		
		printf("\nComputed result: \n");
		for (i=0; i<n; i++){
			for (j=0; j<n; j++)
				printf("%d ",result[i][j]);
			printf("\n");
		}



	for (i=0; i<n; i++)
		for (j=0; j<n; j++)
			result[i][j] = 0;

	//receive results from processes and display it
		
	int** resultBlock = new int* [blockS];
	for (i=0; i<blockS; i++){
		resultBlock[i] = new int[blockS];
	}

	for (i=0; i<nrBlocks; i++)
		for (j=0; j<nrBlocks; j++){
			pvm_recv(a[i][j], -1);
			for(t=0; t<blockS; t++)
				pvm_upkint(resultBlock[t],blockS,1);

			for(q=0; q<blockS;q++)
				for(v=0;v<blockS;v++)
					result[(i*blockS)+q][(j*blockS)+v] = resultBlock[q][v];
		}
	
	printf("\nFinal received result:\n");
	for (i=0; i<n; i++){
		for (j=0; j<n; j++)
			printf("%d ",result[i][j]);
		printf("\n");
	}


   pvm_exit();
}
