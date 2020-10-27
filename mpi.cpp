#include "mpi.h"

#include<locale.h>
#include <iostream>
#include <vector>
using namespace std;
/*void Random(vector<vector<int>>& matr, int n, int m)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            matr[i][j] = 1;
}*/

int min_loc(vector<int> vec, int num_elem)
{
    int local_min = INT_MAX;
    for (int i = 0; i < num_elem; i++)
    {
        local_min = min(local_min, vec[i]);
    }
    return local_min;
}

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "Russian");
    // Почитать
    const size_t row = 3;
    const size_t col = 4;
    vector<int> Matr = { -9,-8,-7,-6,-5,-12,-4,-3,-2,-1,1,2 };

    int RANK;
    int SIZE;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &SIZE);
    MPI_Comm_rank(MPI_COMM_WORLD, &RANK);

    if (SIZE > 1 + row * col)
        SIZE = row * col + 1;

    int min_res = INT_MAX;
    int min_res1 = INT_MAX;
    int buf;

    MPI_Status stat;



    int div = (row * col) / (SIZE - 1);
    int mod = (row * col) % (SIZE - 1);

    if (RANK == 0) {
        cout << "d" << div << endl;
        cout << "m" << mod << endl;

        // Распределили данные по процессам
        for (int i = 1; i < SIZE; i++)
        {
            if (i <= mod)
            {
                MPI_Send(&Matr[0] + div * (i - 1), div + 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
            else
            {
                MPI_Send(&Matr[0] + div * (i - 1) + mod, div, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
        }
    }
    vector<int> rows(div);
    if (RANK == 0)
    {
        // Посчитали минимум из локальных минимумов
        for (int i = 1; i < SIZE; i++)
        {
            MPI_Recv(&buf, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &stat);
            min_res1 = min(min_res1, buf);
        }
        //Вывели общий минимум
        cout << min_res1;
    }
    else
    {
        if (RANK < SIZE)
        {
            //min_res = INT_MAX;
            if (RANK <= mod) {
                rows.resize(div + 1);
                MPI_Recv(&rows[0], rows.size(), MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);
            }
            else
            {
                MPI_Recv(&rows[0], rows.size(), MPI_INT, 0, 0, MPI_COMM_WORLD, &stat);

            }
            min_res = min_loc(rows, rows.size());

            // Отправили в рут
            MPI_Send(&min_res, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }


    MPI_Finalize();
    return 0;
}


