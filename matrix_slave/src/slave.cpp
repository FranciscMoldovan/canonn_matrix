#include "pvm3.h"
#include <stdio.h>

int  main()
{
   int my_tid = pvm_mytid();
   int parent_tid = pvm_parent();
	
   int i,j,k,q,t;

   	int v_dreapta;
	int v_stanga;
	int v_sus;
	int v_jos;
	int blockSize = 0;
	int nrBlocks = 0;
	int** blockFromA;
	int** blockFromB;
	int** resultBlock;
	
   my_tid = pvm_mytid();
   

   pvm_recv(-1,-1);
   pvm_upkint(&v_stanga,1,1);
   pvm_upkint(&v_dreapta,1,1);
   pvm_upkint(&v_sus,1,1);
   pvm_upkint(&v_jos,1,1);





	//receive block size and number of blocks
	pvm_recv(-1,0);
	pvm_upkint(&nrBlocks,1,1);
	pvm_upkint(&blockSize,1,1);
	
	//allocate space for 
	blockFromA = new int* [blockSize];
	blockFromB = new int* [blockSize];
	resultBlock = new int* [blockSize];

	for (i=0; i<blockSize; i++){
		blockFromA[i] = new int[blockSize];
		blockFromB[i] = new int[blockSize];
		resultBlock[i] = new int[blockSize];

	}

	//read block A contents
	for(i=0; i<blockSize; i++)
		pvm_upkint(blockFromA[i],blockSize,1);


	//read block B contents
	pvm_recv(-1,2);
	for(i=0; i<blockSize; i++)
		pvm_upkint(blockFromB[i],blockSize,1);




	//initialize the result
	for(i=0; i<blockSize; i++)
		for(j=0;j<blockSize;j++)
			resultBlock[i][j]=0;


	//compute first resultBlock
	for(i=0; i<blockSize; i++)
		for(j=0;j<blockSize;j++)
			for(k=0;k<blockSize;k++)
				resultBlock[i][j] = resultBlock[i][j] + (blockFromA[i][k]*blockFromB[k][j]);


	//do remaining steps
	for(q=0;q<nrBlocks-1;q++){

		//sending the blocks to neighbors
		pvm_initsend(PvmDataDefault);
		for(t=0; t<blockSize; t++){
			pvm_pkint(blockFromA[t],blockSize,1);			
		}
		pvm_send(v_stanga, 1);


		pvm_initsend(PvmDataDefault);
		for(t=0; t<blockSize; t++){
			pvm_pkint(blockFromB[t],blockSize,1);			
		}
		pvm_send(v_sus, 1);


		//receive blocks from right and lower neighbor
		pvm_recv(v_dreapta,-1);
		for(t=0; t<blockSize; t++)
			pvm_upkint(blockFromA[t],blockSize,1);

		pvm_recv(v_jos,-1);
		for(t=0; t<blockSize; t++)
			pvm_upkint(blockFromB[t],blockSize,1);


		//do computation on new blocks
		for(i=0; i<blockSize; i++)
			for(j=0;j<blockSize;j++)
				for(k=0;k<blockSize;k++)
					resultBlock[i][j]= resultBlock[i][j] + (blockFromA[i][k]*blockFromB[k][j]);


	}


	//send result back to master
	pvm_initsend(PvmDataDefault);
	for(t=0; t<blockSize; t++)
		pvm_pkint(resultBlock[t],blockSize,1);
	pvm_send(parent_tid, 1);


   pvm_exit();
}
