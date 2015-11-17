#include <pvm3.h>
#include <stdio.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <QString>
#include <QLabel>
#include <QWidget>
#include <QMessageBox>
#include <QTextEdit>
#include <string>
int main(int argc, char **argv)
{
  int my_tid = pvm_mytid();
  int parent_tid = pvm_parent();

  int i, j, k, q, t;

  // neighbours:
  int n_right;
  int n_left;
  int n_up;
  int n_down;

  int size_block = 0;
  int nb_blocks = 0;

  int **block_mat1;
  int **block_mat2;
  int **result;

  std::string my_message("Message:");

  // enroll in pvm
  my_tid = pvm_mytid();


  QApplication a(argc, argv);
  QTextEdit textEdit;
  //textEdit.setText("MY TID:"+number_string);
  textEdit.setEnabled(false);
  textEdit.show();



  char strr[500];
  sprintf(strr, "MY_TID:%d \n", my_tid);
  my_message.append(strr);


  pvm_recv(-1, -1);
  pvm_upkint(&n_left, 1, 1);
  pvm_upkint(&n_right, 1, 1);
  pvm_upkint(&n_up, 1, 1);
  pvm_upkint(&n_down, 1, 1);



  sprintf(strr, "L:%d \n", n_left);
  my_message.append(strr);

  sprintf(strr, "R:%d \n", n_right);
  my_message.append(strr);

  sprintf(strr, "U:%d \n", n_up);
  my_message.append(strr);

  sprintf(strr, "dD:%d \n", n_down);
  my_message.append(strr);



  //a.exec();


  // receive block size and number of blocks on each dim
  pvm_recv(-1, 0);
  pvm_upkint(&nb_blocks, 1, 1);
  pvm_upkint(&size_block, 1, 1);

  sprintf(strr, "nb_blocks:%d \n", nb_blocks);
  my_message.append(strr);
  sprintf(strr, "size_block:%d \n", size_block);
  my_message.append(strr);




  // allocating space
  block_mat1 = new int*[size_block];
  block_mat2 = new int*[size_block];
  result = new int*[size_block]();

  for(i = 0; i < size_block; i++)
  {
    block_mat1[i] = new int[size_block];
    block_mat2[i] = new int[size_block];
    result[i] = new int[size_block]();
  }



  // reading content of block from first matrix
  for(i = 0; i < size_block; i++)
  {
    pvm_upkint(block_mat1[i], size_block, 1);
  }

  sprintf(strr, "first block's contents:\n");
  my_message.append(strr);
  for(i = 0; i < size_block; i++)
  {
    for(j = 0; j < size_block; j++)
    {
      sprintf(strr, "%d \n", block_mat1[i][j]);
      my_message.append(strr);
    }
  }





  //  // reading content of block from second matrix
  pvm_recv(-1, 2);
  for(i = 0; i < size_block; i++)
  {
    pvm_upkint(block_mat2[i], size_block, 1);
  }


  //  //  ////////////////////////////////
  sprintf(strr, "second block's contents:\n");
  my_message.append(strr);
  for(i = 0; i < size_block; i++)
  {
    for(j = 0; j < size_block; j++)
    {
      sprintf(strr, "%d \n", block_mat2[i][j]);
      my_message.append(strr);
    }
  }



  //  ///////////////////////////////



  // compute the first result Block
  for(i = 0; i < size_block; i++)
    for(j = 0; j < size_block; j++)
      for(k = 0; k < size_block; k++)
      {
        result[i][j] += (block_mat1[i][k] * block_mat2[k][j]);
      }

  sprintf(strr, "res:\n");
  my_message.append(strr);
  for(i = 0; i < size_block; i++)
  {
    for(j = 0; j < size_block; j++)
    {
      sprintf(strr, "%d\n",result[i][j]);
      my_message.append(strr);
    }
  }

  ///
  QString string = QString::fromStdString(my_message);
  textEdit.setText(string);
  a.exec();
  ///

  //  // do the remaining steps
  //  for(q = 0; q < nb_blocks - 1; q++)
  //  {
  //    // sending the blocks to neighbours
  //    pvm_initsend(PvmDataDefault);
  //    for(t = 0; t < size_block; t++)
  //    {
  //      pvm_pkint(block_mat1[t], size_block, 1);
  //    }
  //    pvm_send(n_left, 1);

  //    pvm_initsend(PvmDataDefault);
  //    for(t = 0; t < size_block; t++)
  //    {
  //      pvm_pkint(block_mat2[t], size_block, 1);
  //    }
  //    pvm_send(n_up, 1);

  //    // receive blocks from right and lower neighbour
  //    pvm_recv(n_right, -1);
  //    for(t = 0; t < size_block; t++)
  //    {
  //      pvm_upkint(block_mat1[t], size_block, 1);
  //    }

  //    pvm_recv(n_down, -1);
  //    for(t = 0; t < size_block; t++)
  //    {
  //      pvm_upkint(block_mat2[t], size_block, 1);
  //    }
  //    printf("~~~~xoxoxoxoxooxox~~~~~~~\n");

  //    // perform computations on new blocks
  //    for(i = 0; i < size_block; i++)
  //      for(j = 0; j < size_block; j++)
  //        for(k = 0; k < size_block; k++)
  //        {
  //          result[i][j] += block_mat1[i][k] * block_mat2[k][j];
  //        }
  //  }
  //  // send result back to master
  //  pvm_initsend(PvmDataDefault);
  //  for(t = 0; t < size_block; t++)
  //  {
  //    pvm_pkint(result[t], size_block, 1);
  //  }
  //  pvm_send(parent_tid, 1);




  pvm_exit();
  return 0;
}





















