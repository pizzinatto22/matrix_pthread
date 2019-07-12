#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct MatrixPart{
    int iteration;
    int thread;
    int rowstart;
    int rowend;
    int size;
    int **A;
    int **B;
    int **result;
};


int ** matrixCreateAndFill(const int size, const int defaultValue);
void matrixPrint(const int ** matrix, const int size);
void matrixDispose(int ** matrix, const int size);
void *matrixMultiply (void *param);

int main (int argc, char* argv[]){
    //getting & converting  program parameters
    const int MATRIX_SIZE = atoi(argv[1]);
    const int ITERATIONS = atoi(argv[2]);
    const int THREADS_MAX = atoi(argv[3]);

    if (THREADS_MAX > MATRIX_SIZE){
        printf("Number of threads must be less or equal than matrix size. Aborting.\n");
        exit(1);
    }

    int **A      = matrixCreateAndFill(MATRIX_SIZE,1);
    int **B      = matrixCreateAndFill(MATRIX_SIZE,2);

    time_t start,end;

    double avg = 0;

    //repeat multiplication ITERATIONS times to improve the execution time average
    for (int i = 0; i < ITERATIONS; i++){

        //creating an array of threads, just to handle this guys
        pthread_t threads[THREADS_MAX];
        struct MatrixPart partition[THREADS_MAX];

        const int LINES_PER_THREAD = MATRIX_SIZE / THREADS_MAX;

        int residual = MATRIX_SIZE % THREADS_MAX; //this amount must be amortized (load balancing threads)
        int rowstart = 0;
        int rowend;

        int **result = matrixCreateAndFill(MATRIX_SIZE,0);

        start = clock();

        //create independent threads, where each one will handle a couple of lines
        for (int j = 0; j < THREADS_MAX; j++) {

            partition[j].iteration= i; //just for logging
            partition[j].thread   = j; //same

            partition[j].rowstart = rowstart;
            partition[j].size     = MATRIX_SIZE ;
            partition[j].A        = A;
            partition[j].B        = B;
            partition[j].result   = result;

            rowend = rowstart + LINES_PER_THREAD - 1;

            if (residual > 0){
                rowend ++;
                residual --;
            }

            partition[j].rowend   = rowend;

            rowstart = rowend + 1;


            int ret = pthread_create(&threads[j],NULL,matrixMultiply,(void *)&partition[j]);
            if (ret){
                printf("Iteration %d: Can't create thread %d. Aborting.\n",i,j);
                exit(1);
            }
        }

        //create a barrier, waiting each thread
        for (int j = 0; j< THREADS_MAX; j++)
            pthread_join(threads[j],NULL);

        end = clock();

        //elapsed clock cycles
        avg += difftime(end,start);

        //matrixPrint((const int **)result,MATRIX_SIZE);

        matrixDispose(result,MATRIX_SIZE);
    }

    matrixDispose(A, MATRIX_SIZE);
    matrixDispose(B, MATRIX_SIZE);


    avg = (avg/CLOCKS_PER_SEC) / ITERATIONS;


    printf("\n Avg time for [%d x %d] matrix multiplication in %d iterations (each one with %d threads) is: %.20f seconds \n\n",
            MATRIX_SIZE,
            MATRIX_SIZE,
            ITERATIONS,
            THREADS_MAX,
            avg
          );

    return 0;
}


int ** matrixCreateAndFill(const int size, const int defaultValue){
    int ** matrix = new int*[size];

    for (int i = 0; i < size; i++){
        matrix[i] = new int[size];

        for (int j = 0; j < size; j++)
            matrix[i][j] = defaultValue;
    }

    return matrix;
}

void matrixPrint(const int ** matrix, const int size){
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++)
            printf("%d ",matrix[i][j]);
        printf("\n");
    }
}

void matrixDispose(int ** matrix, const int size){
    for (int i = 0; i < size; i++)
        delete[] matrix[i];

    delete[] matrix;
}

void *matrixMultiply (void *param){
    MatrixPart *matrixPart = (MatrixPart *) param;

    //avoiding multiple derreferences in some fields
    int size      = matrixPart->size;
    int ** A      = matrixPart->A;
    int ** B      = matrixPart->B;
    int ** result = matrixPart->result;


    for (int i = matrixPart->rowstart; i <= matrixPart->rowend; i++){
        //printf("Iteration %d: Thread %d dealing with row %d\n",matrixPart->iteration,matrixPart->thread,i);
        for (int j = 0; j < size; j++)
            for (int k = 0; k < size; k++)
                result[i][j] += A[i][k] * B[k][j];
    }

    return NULL;
}
