// ECE 209 - Program 2
// simplified Scrabble game
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "random.h"
#include "scrabble.h"

// helper functions -- used in main only
void flushInput();
void printBoard();
void printRack(const char rack[]);

const char * dictionaries[3] = {"twl06.txt", "colors.txt", "ece.txt"};

int main(void) {
   char color[21];
   int d;  // used to choose dictionary
   printf("Welcome to Scrabble Solitaire!\n");
   printf("What is your favorite color? "); fflush(stdout);
   scanf("%20s", color);
   flushInput();
   seedRandom(color);   // seed the random number generator
   printf("Available dictionaries:\n");
   for (d=0; d<3; d++) {
      printf("%4d: %s\n", d, dictionaries[d]);
   }
   printf("Which do you want to use (0, 1, 2)? "); fflush(stdout);
   scanf("%d", &d);
   flushInput();

   char tiles[RACK_SIZE] = {};  // fill with null characters (zero)
   int init = scrabbleInit(dictionaries[d]);
   if (init == -1) {
      printf("Cannot open dictionary file: %s\n", dictionaries[d]);
      return 0;
   }
   chooseTiles(tiles);  // get first set of tiles from the bag

   // user interface -- ask the user what to do next
   char move = 0;
   char selection[RACK_SIZE+1];
   int score;
   int totalScore = 0;
   unsigned int row, col, dir;
   char *nl;
   while (move != 'q') {
      printBoard();
      printRack(tiles);
      printf("\nWhat would you like to do (w, d, q) ? ");
      fflush(stdout);
      scanf("%c", &move);
      flushInput();  // only read the first character

      switch(move) {
         case 'q': break;   // exit loop

         case 'w':  // place a selection from player's tiles
            printf("Tiles to place: "); fflush(stdout);
            // using fgets to limit the size of the string being read
            fgets(selection, RACK_SIZE+1, stdin);
            nl = strchr(selection, '\n');
            if (nl) *nl = '\0';  // remove newline if necessary
            else flushInput();
            if (!checkTiles(selection, tiles)) {
               printf("ILLEGAL: Using tile(s) not in your tile rack\n");
               break;
            }
            // get starting square and direction
            printf("row (0-%d): ", BOARD_SIZE-1); fflush(stdout);
            scanf("%u", &row);
            printf("column (0-%d): ", BOARD_SIZE-1); fflush(stdout);
            scanf("%u", &col);
            printf("horizontal (0) or vertical (1): "); fflush(stdout);
            scanf("%u", &dir);
            flushInput();
            if ((row >= BOARD_SIZE) || (col >= BOARD_SIZE)) {
               printf("ILLEGAL starting square (%u,%u)\n", row, col);
               break;
            }
            if (dir > 1) {
               printf("ILLEGAL direction %u\n", dir);
               break;
            }
            printf("Trying tiles %s at square (%u,%u) in %s direction\n",
                     selection, row, col, dir ? "vertical" : "horizontal");
            score = scoreMove(selection, row, col, dir);
            if (score == -1) {
               printf("ILLEGAL word/position\n");
               score = 0;
               break;
            }
            if (score == -2) {
               printf("ILLEGAL: word not found in dictionary\n");
               score = 0;
               break;
            }
            totalScore += score;
            placeWord(selection, row, col, dir, tiles);
            printf("Score = %d, TOTAL Score = %d\n", score, totalScore);
            score = 0;
            chooseTiles(tiles);
            break;

         case 'd':  // discard tiles and re-mix the bag
            printf("Tiles to discard: "); fflush(stdout);
            // using fgets to limit the size of the string being read
            fgets(selection, RACK_SIZE+1, stdin);
            nl = strchr(selection, '\n');
            if (nl) *nl = '\0';  // remove newline if necessary
            else flushInput();
            if (!checkTiles(selection, tiles)) {
               printf("ILLEGAL: Specified tile(s) not in your tile rack\n");
               break;
            }
            discardTiles(selection, tiles);
            break;

         default:
            printf("Unknown move -- try again.\n");
      }

   }

   printf("Thanks for playing. Your total score = %d\n", totalScore);
   return 0;
}

// print board as a 2D grid
void printBoard() {
   unsigned int row = 0, col = 0;
   printf("\n    ");   // room for row numbers
   // print column numbers
   for (col=0; col<BOARD_SIZE; col++) {
      printf(" %X ", col);
   }
   printf("\n");
   // print rows
   for (row=0; row<BOARD_SIZE; row++) {
      printf("%3X ", row);
      for (col=0; col<BOARD_SIZE; col++) {
         char square = board[row][col];
         if (!square) printf(" - ");
         else printf(" %c ", square);
      }
      printf("\n");
   }
   printf("\n");
   printf("Tiles in the bag: %d\n", tilesInBag); //Debugging purposes
}

// print the letters in a player's rack, along with the score of each letter
void printRack(const char rack[]) {
   printf("Tiles in your rack: ");
   int count = 0;
   for (int i = 0; i<RACK_SIZE; i++) {
      char letter = rack[i];
      if (letter) {
         if (count > 0) printf(", ");
         printf("%c - %d", letter, letterPoints[letter - 'a']);
         count++;
      }
   }
   printf("\n");
}

// USER INPUT FUNCTION
// read characters until linefeed is removed from stdin
void flushInput() {
   int c;
   while ((c = getchar()) != '\n');
}

