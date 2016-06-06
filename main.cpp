#include <mpi.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <random>

#define ROOT 0

#define PARTICIPATION_TAG 100
#define LEADER_TAG 200

void send_participation_info(int rank, int size) {
  int participates = rand() % 2 == 0;

  for (int i = 0; i < size; i++) {
    MPI_Send(&participates, 1, MPI_INT, i, PARTICIPATION_TAG, MPI_COMM_WORLD);
  }
}

void send_leader_vote(int rank, int size) {
  int leader_vote = rand() % size;

  for (int recipient = 0; recipient < size; recipient++) {
    MPI_Send(&leader_vote, 1, MPI_INT, recipient, LEADER_TAG, MPI_COMM_WORLD);
  }
}

void receive_leaders_votes(int rank, int size) {
  MPI_Status status;

  int *leaders_votes = new int[size];

  for (int sender = 0; sender < size; sender++) {
    MPI_Recv(
      &(leaders_votes[sender]), 1, MPI_INT,
      sender, LEADER_TAG, MPI_COMM_WORLD, &status
    );

    printf("Process: %d, received leader vote: %d\n",
      rank, leaders_votes[sender]);
  }

  delete[] leaders_votes;
  leaders_votes = NULL;
}

int main(int argc, char **argv)
{
  int size,rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  srand(rank * time(NULL));

  send_participation_info(rank, size);

  send_leader_vote(rank, size);
  receive_leaders_votes(rank, size);

  MPI_Finalize();
}
