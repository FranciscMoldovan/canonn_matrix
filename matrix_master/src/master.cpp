#include <pvm3.h>
#include <stdio.h>
#include <math.h>
//#include <QApplication>
//#include <QTextEdit>
#include <qapplication.h>
#include <qpushbutton.h>

int main(int argc, char **argv)
{



  int *tids;
  int my_tid, i, j;
  int nproc = 0;

  // matrix size
  int size_mat = 0;

  // block size
  int size_block = 0;

  // number of blocks on each dimension
  int nb_blocks = 0;

  // matrices to be multiplied
  int **mat1;
  int **mat2;

  // result matrix
  int **result;

  // reading and initializing data
  FILE *f = fopen(argv[1], "r");

  // reading matrix dimensions
  fscanf(f, "%d", &size_mat);
  printf("Matrix Dimension:%d\n", size_mat);

  // reading block size
  fscanf(f, "%d", &size_block);
  printf("Block Dimension:%d\n", size_block);

  mat1 = new int*[size_mat];
  mat2 = new int*[size_mat];
  result = new int*[size_mat];

  // by adding (), all values are initialized to zero
  for(i = 0; i < size_mat; i++)
  {
    result[i] = new int[size_mat]();
    mat1[i] = new int[size_mat];
    mat2[i] = new int[size_mat];
  }

  // reading first matrix from text file
  for(i = 0; i < size_mat; i++)
    for(j = 0; j < size_mat; j++)
    {
      fscanf(f, "%d", &mat1[i][j]);
    }

  printf("First Matrix Contents:\n");
  for(i = 0; i < size_mat; i++)
  {
    for(j = 0; j < size_mat; j++)
    {
      printf("%d ", mat1[i][j]);
    }
    printf("\n");
  }

  // reading second matrix from text file
  for(i = 0; i < size_mat; i++)
    for(j = 0; j < size_mat; j++)
    {
      fscanf(f, "%d", &mat2[i][j]);
    }


  printf("Second Matrix Contents:\n");
  for(i = 0; i < size_mat; i++)
  {
    for(j = 0; j < size_mat; j++)
    {
      printf("%d ", mat2[i][j]);
    }
    printf("\n");
  }

  fclose(f);

  // enroll in pvm
  my_tid = pvm_mytid();

  nb_blocks = size_mat / size_block;
  printf("Total Numer of Blocks:%d\n", nb_blocks * nb_blocks);
  nproc = pow(nb_blocks, 2);
  printf("Number of Workers:%d\n", nproc);

  tids = new int[nproc];

  pvm_spawn("mat_slave", (char **)0, PvmTaskDefault, "", nproc, tids);

  int k = 0;

  // matrix of mesh
  int **a = new int*[nb_blocks];
  for(i = 0; i < nb_blocks; i++)
  {
    a[i] = new int[nb_blocks];
  }

  for(i = 0; i < nb_blocks; i++)
    for(j = 0; j < nb_blocks; j++)
    {
      a[i][j] = tids[k++];
    }

  // printing the mesh of processes:
  for(i = 0; i < nb_blocks; i++)
  {
    for(j = 0; j < nb_blocks; j++)
    {
      printf("%d ", a[i][j]);
    }
    printf("\n");
  }



  // neighbours:
  int n_right;
  int n_left;
  int n_up;
  int n_down;




  for(i = 0; i < nb_blocks; i++)
    for(j = 0; j < nb_blocks; j++)
    {
      pvm_initsend(PvmDataDefault);
      // left neighbour
      if(j > 0)
      {
        n_left = a[i][j - 1];
      }
      else
      {
        n_left = a[i][nb_blocks - 1];
      }
      pvm_pkint(&n_left, 1, 1);

      // right neighbour
      if(j < nb_blocks - 1)
      {
        n_right = a[i][j + 1];
      }
      else
      {
        n_right = a[i][0];
      }
      pvm_pkint(&n_right, 1, 1);

      // up neighbour
      if(i > 0)
      {
        n_up = a[i - 1][j];
      }
      else
      {
        n_up = a[nb_blocks - 1][j];
      }
      pvm_pkint(&n_up, 1, 1);

      // neighbour down
      if(i < nb_blocks - 1)
      {
        n_down = a[i + 1][j];
      }
      else
      {
        n_down = a[0][j];
      }
      pvm_pkint(&n_down, 1, 1);

      pvm_send(a[i][j], 0);
    }

  printf("( 1 ) \n");


  int q, t, count_i, count_j;

  // allocating space for blocks
  int **block_mat1 = new int*[size_block];
  int **block_mat2 = new int*[size_block];

  for(i = 0; i < size_block; i++)
  {
    block_mat1[i] = new int[size_block];
    block_mat2[i] = new int[size_block];
  }

  printf("( 2 )\n");

  int z, v;

  // sending the blocks to the processes
  for(i = 0; i < nb_blocks; i++)
    for(j = 0; j < nb_blocks; j++)
    {
      // building a block
      count_i = 0;
      count_j = 0;

      for(q = i * size_block; q < (i + 1)*size_block; q++)
      {
        for(t = j * size_block; t < (j + 1)*size_block; t++)
        {
          block_mat1[count_i][count_j] = mat1[q][t];
          block_mat2[count_i][count_j] = mat2[q][t];
          count_j++;
        }
        count_i++;
        count_j = 0;
      }

        printf("\n");

        // send block
        pvm_initsend(PvmDataDefault);
        pvm_pkint(&nb_blocks, 1, 1);
        pvm_pkint(&size_block, 1, 1);

        for(t = 0; t < size_block; t++)
        {
          pvm_pkint(block_mat1[t], size_block, 1);
        }
        printf("sending block to %d\n", a[i][(j + 1) % nb_blocks]);
        pvm_send(a[i][(j + 1) % nb_blocks], 0);

        pvm_initsend(PvmDataDefault);
        for(t = 0; t < size_block; t++)
        {
          pvm_pkint(block_mat2[t], size_block, 1);
        }
        printf("sending block to %d\n", a[(j + 1) % nb_blocks][j]);
        pvm_send(a[(j + i) % nb_blocks][j], 2);

      }

    printf("( 3 )\n");

    // compute the matrix multiplication sequentially
    for(i = 0; i < size_mat; i++)
      for(j = 0; j < size_mat; j++)
        for(z = 0; z < size_mat; z++)
        {
          result[i][j] += mat1[i][z] * mat2[z][j];
        }

    printf("\nComputed result: \n");
    for(i = 0; i < size_mat; i++)
    {
      for(j = 0; j < size_mat; j++)
      {
        printf("%d ", result[i][j]);
      }
      printf("\n");
    }


    for(i = 0; i < size_mat; i++)
    {
      for(j = 0; j < size_mat; j++)
      {
        result[i][j] = 0;
      }
    }

    printf("( 3 )\n");

  //receive result from slaves and display it
  //  int **result_block = new int*[size_block];
  //  for(i = 0; i < size_block; i++)
  //  {
  //    result_block[i] = new  int[size_block];
  //  }

  //  printf("( 4 patru)\n");

  //  for(i = 0; i < size_block; i++)
  //    for(j = 0; j < size_block; j++)
  //    {
  //      pvm_recv(a[i][j], -1);
  //      for(t = 0; t < size_block; t++)
  //      {
  //        pvm_upkint(result_block[t], size_block, 1);
  //      }

  //      for(q = 0; q < size_block; q++)
  //      {
  //        for(v = 0; v < size_block; v++)
  //        {
  //          result[(i * size_block) + q][(j * size_block) + v] = result_block[q][v];
  //        }
  //      }
  //    }

  //   printf("( 5 )\n");

  //  printf("\n Final Received Result:\n");
  //  for(i = 0; i < size_mat; i++)
  //  {
  //    for(j = 0; j < size_mat; j++)
  //    {
  //      printf("%d", result[i][j]);
  //    }
  //    printf("\n");
  //  }

  pvm_exit();
  return 0;
}











