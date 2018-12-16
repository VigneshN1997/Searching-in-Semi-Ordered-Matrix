#include <iostream>
#include <omp.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <ctime>
using namespace std;

void traverseTree(int currLevel,int finalLevel,unsigned int** matrix,int lr,int lc,int hr,int hc,int** finalIndicesArr, unsigned int keyToSearch,double st_time,int index);

vector<int> createIndicesVector(int topLeftR, int topLeftC, int bottomRightR, int bottomRightC)
{
	vector<int> indicesVector(4);
	indicesVector[0] = topLeftR;
	indicesVector[1] = topLeftC;
	indicesVector[2] = bottomRightR;
	indicesVector[3] = bottomRightC;
	return indicesVector;
}

pair<int,int> findElement(unsigned int** matrix, unsigned int keyToSearch,int lr, int lc, int hr, int hc,int matrixSize)
{
	bool singleElem = false;
	if(lr == hr && lc == hc)
	{
		singleElem = true;
	}
	pair<int,int> index;
	int mr = (lr + hr)/2;
	int mc = (lc + hc)/2;
	if(matrix[mr][mc] == keyToSearch)
	{
		// printf("found %d %d\n",mr,mc);
		index.first = mr;
		index.second = mc;
		return index;
	}
	else if(matrix[mr][mc] < keyToSearch && !singleElem)
	{
		if((mr + 1 <= hr) && (mc + 1 <= hc))
		{
			index = findElement(matrix,keyToSearch,mr+1,mc+1,hr,hc,matrixSize);
			if(index.first != -1)
			{
				return index;
			}
		}
	}
	else if(!singleElem)
	{
		if((lr <= mr) && (lc <= mc))
		{
			index = findElement(matrix,keyToSearch,lr,lc,mr,mc,matrixSize);
			if(index.first != -1)
			{
				return index;
			}
		}
	}
	if((lr <= mr) && (mc + 1 <= hc) && !singleElem)
	{
		index = findElement(matrix,keyToSearch,lr,mc+1,mr,hc,matrixSize);
		if(index.first != -1)
		{
			return index;
		}
	}
	if((mr + 1 <= hr) && (lc <= mc) && !singleElem)
	{
		index = findElement(matrix,keyToSearch,mr+1,lc,hr,mc,matrixSize);
		if(index.first != -1)
		{
			return index;
		}
	}
	index.first = -1;
	index.second = -1;
	return index;
}


int main(int argc, char* argv[])
{
	int i,j;
	double st_time,end_time;
	int numCores = atoi(argv[1]);
	// FILE* matrixFile = fopen(argv[2],"r");
	long int matrixSize = atol(argv[2]);
	unsigned int** matrix = NULL;
	// fscanf(matrixFile,"%d\n",&matrixSize);
	matrix = (unsigned int**)malloc(matrixSize*sizeof(unsigned int*));
	for(i = 0; i < matrixSize; i++)
	{
		matrix[i] = (unsigned int*)malloc(matrixSize*sizeof(unsigned int));
	}

	unsigned int k = 1;
	if (matrixSize < 50000)
	{
		for(i = 0; i < matrixSize; i++)
		{
			for(j = 0; j < matrixSize; j++)
			{
				matrix[i][j] = k++;
			}
		}
	}
	else
	{
		for(i = 0; i < matrixSize; i++)
		{
			for(j = 0; j < matrixSize;)
			{
				for (int l = 0; l < 10; ++l)
				{
					matrix[i][j] = k;
					j++;
				}
				k++;
			}
		}
	}
	unsigned int keyToSearch = atol(argv[3]);

	// fclose(matrixFile);
	st_time = omp_get_wtime();
	printf("computation starts\n");
	int level = 0;
	int numThreadstoSpawn = 1;
	while((int)pow(3,level) / numCores < 1)
	{
		level++;
	}
	numThreadstoSpawn = (int)pow(3,level);
	// printf("%d\n",numThreadstoSpawn);	
	int numThreads = 0;


	int** finalIndicesArr = (int**)malloc(numThreadstoSpawn*sizeof(int*));
	for(i = 0; i < numThreadstoSpawn; i++)
	{
		finalIndicesArr[i] = (int*)malloc(4*sizeof(int));
	}
	traverseTree(0,level,matrix,0,0,matrixSize - 1, matrixSize - 1,finalIndicesArr,keyToSearch,st_time,0);
	#pragma omp parallel num_threads(numThreadstoSpawn)
	{
		int threadRank = omp_get_thread_num();
		if(finalIndicesArr[threadRank][0] != -1)
		{
			int lr = finalIndicesArr[threadRank][0];
			int lc = finalIndicesArr[threadRank][1];
			int hr = finalIndicesArr[threadRank][2];
			int hc = finalIndicesArr[threadRank][3];
			if(lr <= hr && lc <= hc)
			{
				pair<int,int> index = findElement(matrix,keyToSearch,lr,lc,hr,hc,matrixSize);
				if(index.first != -1)
				{
					printf("[%d,%d]\n",index.first,index.second);
					end_time = omp_get_wtime();
					printf("time_taken:%lf\n",(end_time - st_time));
					exit(0);
				}
			}
		}
	}
	printf("[-1,-1]\n");
	end_time = omp_get_wtime();
	printf("time_taken:%lf\n",(end_time - st_time));
	return 0;
}

void traverseTree(int currLevel,int finalLevel,unsigned int** matrix,int lr,int lc,int hr,int hc,int** finalIndicesArr, unsigned int keyToSearch,double st_time,int index)
{
	if(currLevel == finalLevel)
	{
		// int threadRank = omp_get_thread_num();
		printf("rank:%d\n",index);
		finalIndicesArr[index][0] = lr;
		finalIndicesArr[index][1] = lc;
		finalIndicesArr[index][2] = hr;
		finalIndicesArr[index][3] = hc;
	}
	else
	{
		if(lr != -1)
		{
			int mr = (lr + hr)/2;
			int mc = (lc + hc)/2;
			if(matrix[mr][mc] == keyToSearch)
			{
				printf("[%d,%d]\n",mr,mc);
				double end_time = omp_get_wtime();
				printf("time_taken:%lf\n",(end_time - st_time));
				exit(0);
			}

			#pragma omp parallel sections num_threads(3)
			{
				#pragma omp section
				{
					if(matrix[mr][mc] < keyToSearch)
					{
						if((mr + 1 <= hr) && (mc + 1 <= hc))
						{
							traverseTree(currLevel+1,finalLevel,matrix,mr+1,mc+1,hr,hc,finalIndicesArr,keyToSearch,st_time,3*index);
						}
						else
						{
							traverseTree(currLevel+1,finalLevel,matrix,-1,-1,-1,-1,finalIndicesArr,keyToSearch,st_time,3*index);
						}
					}
					else
					{
						if((lr <= mr) && (lc <= mc))
						{
							traverseTree(currLevel+1,finalLevel,matrix,lr,lc,mr,mc,finalIndicesArr,keyToSearch,st_time,3*index);
						}
						else
						{
							traverseTree(currLevel+1,finalLevel,matrix,-1,-1,-1,-1,finalIndicesArr,keyToSearch,st_time,3*index);
						}
					}
				}
				#pragma omp section
				{
					if((lr <= mr) && (mc + 1 <= hc))
					{
						traverseTree(currLevel+1,finalLevel,matrix,lr,mc+1,mr,hc,finalIndicesArr,keyToSearch,st_time,3*index+1);	
					}
					else
					{
						traverseTree(currLevel+1,finalLevel,matrix,-1,-1,-1,-1,finalIndicesArr,keyToSearch,st_time,3*index+1);	
					}
				}
				#pragma omp section
				{
					if((mr + 1 <= hr) && (lc <= mc))
					{
						traverseTree(currLevel+1,finalLevel,matrix,mr+1,lc,hr,mc,finalIndicesArr,keyToSearch,st_time,3*index+2);
					}
					else
					{
						traverseTree(currLevel+1,finalLevel,matrix,-1,-1,-1,-1,finalIndicesArr,keyToSearch,st_time,3*index+2);
					}
				}
			}
		}
		else
		{	
			#pragma omp parallel sections num_threads(3)
			{
				#pragma omp section
				{
					traverseTree(currLevel+1,finalLevel,matrix,-1,-1,-1,-1,finalIndicesArr,keyToSearch,st_time,3*index);
				}
				#pragma omp section
				{
					traverseTree(currLevel+1,finalLevel,matrix,-1,-1,-1,-1,finalIndicesArr,keyToSearch,st_time,3*index+1);
				}
				#pragma omp section
				{
					traverseTree(currLevel+1,finalLevel,matrix,-1,-1,-1,-1,finalIndicesArr,keyToSearch,st_time,3*index+2);
				}
			}
		}
	}
}