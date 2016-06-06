#include <mpi.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include <random>

#define ROOT 0

#define PARTICIPATION_TAG 100
#define LEADER_TAG 200
#define LOCATION_TAG 300

void send_participation_info(int rank, int size, int &timer) {
  int participates = rand() % 2 == 0;

  for (int i = 0; i < size; i++) {
    MPI_Send(&participates, 1, MPI_INT, i, PARTICIPATION_TAG, MPI_COMM_WORLD);
    timer++;
  }
}

void send_leader_vote(int rank, int size, int &timer) {
  int leader_vote = rand() % size;

  for (int recipient = 0; recipient < size; recipient++) {
    MPI_Send(&leader_vote, 1, MPI_INT, recipient, LEADER_TAG, MPI_COMM_WORLD);
    timer++;
  }
}

int *receive_leaders_votes(int rank, int size, int &timer) {
  MPI_Status status;

  int *leaders_votes = new int[size];

  for (int sender = 0; sender < size; sender++) {
    MPI_Recv(
      &(leaders_votes[sender]), 1, MPI_INT,
      sender, LEADER_TAG, MPI_COMM_WORLD, &status
    );
    timer++;

    // printf("Process: %d, received leader vote: %d\n",
    //   rank, leaders_votes[sender]);
  }

  return leaders_votes;
}


void choose_leaders(int rank, int size, int &timer) {
  int *leaders_votes = receive_leaders_votes(rank, size, timer);

  int leaders[3],
      max[2] = { 0 },
      i;
  int *count_votes = new int[size]();

  for (i = 0; i < size; i++) {
    count_votes[leaders_votes[i]]++;
  }

  for (i = 0; i < 3; i++) {
    for (int j = 0; j < size; j++) {
      if (count_votes[j] > max[1]) {
        max[0] = j;
        max[1] = count_votes[j];
      }
    }
    leaders[i] = max[0];
    count_votes[max[0]] = 0;
    max[0] = 0;
    max[1] = 0;
  }

  for (int i = 0; i < 3; i++) {
    printf("Choose leader - Process - %d, Leader (%d): %d\n",
      rank, i, leaders[i]);
  }

  delete[] count_votes;
  delete[] leaders_votes;
}

void choose_place() {
  int place = 0, max = 0, count[4] = {0}, i;

  for (i = 0; i < 10; i++) { count[places[i]]++; }

  for (i = 0; i < 4; i++) {
    if (count[i] > max) {
      place = i;
      max = count[i];
    }
  }
}


int main(int argc, char **argv)
{
  int size,rank;

  int timer = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  srand(rank * time(NULL));

  send_participation_info(rank, size, timer);

  send_leader_vote(rank, size, timer);
  choose_leaders(rank, size, timer);


  int location_vote = rand() % 4;

  for (int recipient = 0; recipient < size; recipient++) {
    MPI_Send(&location_vote, 1, MPI_INT, recipient,
      LOCATION_TAG, MPI_COMM_WORLD);
  }

  int *location_votes = new int[size];
  MPI_Status status;
  for (int sender = 0; sender < size; sender++) {
    MPI_Recv(
      &(location_votes[sender]), 1, MPI_INT,
      sender, LOCATION_TAG, MPI_COMM_WORLD, &status
    );

    printf("Process: %d, received location vote: %d\n",
      rank, location_votes[sender]);
  }



  MPI_Finalize();
}
