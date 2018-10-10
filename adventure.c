#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<string.h>

//** Osbaldo Esquivel
//** 01Feb2016

///////////////////////////////////////////////////
// This program                                  //
//  will create files with rooms that connect    //
//  to other rooms. This program names the       //
//  room as a number. In other words, Room 1,    //
//  Room 2, etc. The program will then read      //
//  these randomly generated rooms from the      //
//  files and ask the user to navigate from      //
//  the starting room to the end room.           //
///////////////////////////////////////////////////

#define MAX 7
#define CHARMAX 48

// define a structure to hold data about a room
typedef struct {
   int name;
   int connect[6];
   int connectNum;
   int type;
}Room;

// function prototypes
void makeDir(char *, DIR *);
void createRooms(int *, Room, Room *);
void createConnects(Room, Room, Room *, int *);
void initRooms(int, Room *);
void roomInfo(Room *);
void connectInfo(int, Room *);
void writeFiles(char *, Room, int *, Room *);
void playGame(char *, int *);

int main() {
	// declare variables needed for various functions
	int i, rooms[MAX];
	Room roomList[MAX];
	Room aRoom, tempRoom;
	char path[20] = "esquiveo.rooms.";
	DIR *dir;

	// seed the random number generator
	srand(time(NULL));

	// first, we create the directory
	makeDir(path, dir);

	// create the rooms
	createRooms(rooms, aRoom, roomList);

	// create the connections between rooms
	createConnects(tempRoom, aRoom, roomList, rooms);

	// write the rooms to files
	writeFiles(path, aRoom, rooms, roomList);

	// ask user to navigate through rooms to end
	playGame(path, rooms);

	return 0;
}

// this function will create a directory based on the current process id
void makeDir(char *p, DIR *d) {
	// store process id as int
	int pid = getpid();
	char cpid[10];

	// store process id in char array
	sprintf(cpid, "%d", pid);
	// add this id to the path string
	strcat(p,cpid);

	// make the directory with permissions
	mkdir(p, S_IRUSR | S_IWUSR | S_IXUSR);
	d = opendir(p);

	// if directory failed to be created, exit with 1 status
	if(d == NULL) {
		printf("That directory does not exist.\n");
		exit(1);
	}
}

// funciton to initialize the room structure
void initRooms(int name, Room *r) {
	r->type = 0;
	r->name = name;
	r->connectNum = 0;
}

void createRooms(int *r, Room tempRoom, Room *rList) {
	int i, j;

	// create an array that holds 7 unique values
	// and initialize the array as rooms
	for(i = 0; i < MAX; i++) {
		int temp = 0, t;

		while(!temp) {
			t = (rand() % 10) + 1;
			temp = 1;

			for(j = 0; j < i; j++) {
				if(r[j] == t) {
					temp = 0;
				}
			}
		}

		r[i] = t;

		initRooms(t, &tempRoom);

		rList[i] = tempRoom;
	}
}

// function to create the connections between the rooms
void createConnects(Room t, Room a, Room *r, int *ro) {
	int i, b, temp, tempStart, roomStart, tempEnd, roomEnd;

	//loop through array of room integers and ensure at least 3
	//connections but no more than 6
	for(i = 0; i < MAX; i++) {
		int tempNum = ro[i];
		a = r[i];

		while(a.connectNum < 3) {
			temp = i;
			int finder;

			// make sure connection is unique
			while(temp == i) {
				temp = rand() % MAX;
				finder = ro[temp];

				for(b = 0; b < a.connectNum; b++) {
					if(a.connect[b] == finder) {
						temp = i;
					}
				}
			}
		        // make connection if no connection
			a.connect[a.connectNum] = finder;
			a.connectNum++;
			// store room in array
			r[i] = a;

			// add connection for other room
			t = r[temp];
			t.connect[t.connectNum] = tempNum;
			t.connectNum++;
			r[temp] = t;
		}
	}

	// pick two rooms for start and end
	tempStart = rand() % MAX;
	roomStart = ro[tempStart];
	a = r[tempStart];
	a.type = 1;
	r[tempStart] = a;

	// start and end cannot be the same
	do{
		tempEnd = rand() % MAX;
		roomEnd = ro[tempEnd];
	}while(roomStart == roomEnd);

	// set end room
	a = r[tempEnd];
	a.type = 2;

	// set end room
	r[tempEnd] = a;
}

// function to write rooms to files
void writeFiles(char *p, Room a, int *r, Room *rList) {
	// declare variables
	char file[50];
	FILE *fptr;
	int i, ii;

	// write each room name, connections, and type to files
	for(i = 0; i < MAX; i++) {
		int cons, rType, *connect;
		// set room name and number
		int name;
		a = rList[i];

		name = r[i];

		// set file path
		snprintf(file, sizeof(file), "%s/room%d.txt", p, name);

		// open file
		fptr = fopen(file, "w");

		// check for error opening file and exit with 1 status if error
		if(fptr < 0) {
			perror("There was an error opening the file.\n");
			exit(1);
		}

		// set connection number, type, and connections
		cons = a.connectNum;
		connect = a.connect;
		rType = a.type;


		// write name of room to file
		fprintf(fptr, "ROOM NAME: %d\n", name);

		// write connection name line by line
		for(ii = 0; ii < cons; ii++){
			fprintf(fptr, "CONNECTION %d: %d\n", ii+1, connect[ii]);
		}

		// write appropriate room type
		switch(rType) {
			case 0: {fprintf(fptr, "ROOM TYPE: MID_ROOM\n\n"); break;}
			case 1: {fprintf(fptr, "ROOM TYPE: START_ROOM\n\n"); break;}
			case 2: {fprintf(fptr, "ROOM TYPE: END_ROOM\n\n"); break;}
		}

		fclose(fptr);
	}
}

// function to output room info
void roomInfo(Room *r) {
	int j, temp;

	// output current room
	printf("CURRENT LOCATION: %d\n", r->name);

	// loop through file and output connections
	printf("POSSIBLE CONNECTIONS: ");
	for(j = 0; j < r->connectNum; j++) {
		if(j != (r->connectNum - 1)) {
			printf("%d, ", r->connect[j]);
		}
		else {
			printf("%d.\n", r->connect[j]);
		}
	}
}

// functio adjust the connecitons as needed
void connectInfo(int c, Room *r) {
	int j;

	// if a room has been connected to return
	if(r->name == c) {
		return;
	}
	// if 6 connections have already been made, return
	else if(r->connectNum >= 6) {
		return;
	}
	// allow the connection otherwise
	else {
		for(j = 0; j < r->connectNum; j++) {
			if(r->connect[j] == c) {
				return;
			}
		}
	// update connections
	r->connect[r->connectNum] = c;
	r->connectNum++;
	}
}

// function to allow user to navigate through rooms and exit when at end room
void playGame(char *p, int *rList) {
	// declare variables
	int i, j, s = 0, fullPath[80];
	FILE *fptr;
	char file[50], aType;
	Room temp;
	Room start;
	Room end;
	Room roomArr[MAX];

	// loop through each file
	for(i = 0; i < MAX; i++) {
		int rName = rList[i];

		// set path to file name
		snprintf(file, sizeof(file), "%s/room%d.txt", p, rName);

		// set file pointer to read
		fptr = fopen(file, "r");

		// variables for line information
		char *row;
		size_t size = 0;

		// loop through file and read each line and
		// get connections to output
		while(!feof(fptr)) {
			getline(&row, &size, fptr);
			// name line will create a room from the file
			if(row[5] == 'N') {
				char val = row[11] - CHARMAX;

				if(val == 1) {
					if(row[12] == CHARMAX) {
						val = 10;
					}
				}
				initRooms(val, &temp);
			}
			// get connections and number
			else if(row[0] == 'C') {
				char val = row[14] - CHARMAX;

				if(val == 1) {
					if(row[15] == CHARMAX) {
						val = 10;
					}
				}
				// add connection if able
				connectInfo(val, &temp);
			}
			// if type line, set room type
			else if(row[5] == 'T') {
				char aType = row[11];

				if(aType == 'E') {
					temp.type = 2;
					end = temp;
				}
				else if(aType == 'S') {
					temp.type = 1;
					start = temp;
				}
			}
		}
		//store room and close file
		roomArr[i] = temp;

		fclose(fptr);
	}

	// start at current location
	Room *c = &start;

	// loop until user reaches end room
	while(c->type != 2) {
		int isRoom, num;

		do{
			// ouput current room
			roomInfo(c);

			// get user data for next room
			printf("WHERE TO?>");
			char userVal[4];
			fgets(userVal, 4, stdin);
			num = atoi(userVal);
			printf("\n");

			isRoom = 0;

			// check if room name is valid
			for(j = 0; j < c->connectNum; j++) {
				if(c->connect[j] == num) {
					isRoom = 1;
				}
			}

			// output error message if not a valid room
			if(isRoom == 0) {
				printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
			}

		}while(isRoom == 0);

		// store user path
		fullPath[s] = num;

		// locate room in array and increase steps
		for(j = 0; j < MAX; j++) {
			if(roomArr[j].name == num) {
				c = &roomArr[j];
			}
		}

		s++;
	}

	// output ending message
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", s);

	// output user path taken
	for(i = 0; i < s; i++) {
		printf("%d\n", fullPath[i]);
	}
}
