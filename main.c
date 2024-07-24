#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#define MAX_LENGTH 20

void reduce_rating(const char* username, int reduction);
void clear_screen();
int kbhit(void);

void flush_input() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main(void) {
    printf("Hello, this is Fir. You can play Fir with various modifiers.\nThis is Version 0.8.5 Beta, this does not support functions such as blind mode.\n");

    while (1) {
        int command;
        here:
        printf("\n[1] Register a new user\n[2] Play\n[3] See Records\n[4] Exit\n[5] Help\n");
        printf("Enter a command: ");
        if (scanf("%d", &command) != 1) {
            printf("Invalid input. Please enter a number.\n");
            flush_input();
            continue;
        }
        flush_input();

        if (command == 1) {
            char username[MAX_LENGTH];
            printf("Enter a username: ");
            if (scanf("%s", username) != 1) {
                printf("Invalid input.\n");
                flush_input();
                continue;
            }

            FILE *fptr = fopen("user.txt", "r");
            if (!fptr) {
                perror("Error opening user.txt");
                continue;
            }

            char line[MAX_LENGTH];
            int found = 0;

            while (fgets(line, sizeof(line), fptr)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, username) == 0) {
                    found = 1;
                    break;
                }
            }

            fclose(fptr);

            if (found) {
                printf("Username already exists!\n");
            } else {
                printf("Welcome, %s!\n", username);
                fptr = fopen("user.txt", "a");
                if (!fptr) {
                    perror("Error opening user.txt for writing");
                    continue;
                }
                fprintf(fptr, "%s\n", username);
                fclose(fptr);

                fptr = fopen("games.txt", "a");
                if (!fptr) {
                    perror("Error opening games.txt for writing");
                    continue;
                }
                fprintf(fptr, "0\n");
                fclose(fptr);
            }
        } else if (command == 4) {
            printf("Goodbye!\n");
            exit(0);
        } else if (command == 3) {
            char username[MAX_LENGTH];
            printf("Input Username: ");
            if (scanf("%s", username) != 1) {
                printf("Invalid input.\n");
                flush_input();
                continue;
            }

            FILE *fptr = fopen("user.txt", "r");
            if (!fptr) {
                perror("Error opening user.txt");
                continue;
            }

            char line[MAX_LENGTH];
            int found = 0;
            int line_number = 0;
            int user_line = -1;

            while (fgets(line, sizeof(line), fptr)) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, username) == 0) {
                    found = 1;
                    user_line = line_number;
                    break;
                }
                line_number++;
            }

            fclose(fptr);

            if (found) {
                fptr = fopen("games.txt", "r");
                if (!fptr) {
                    perror("Error opening games.txt");
                    continue;
                }

                for (int i = 0; i <= user_line; i++) {
                    fgets(line, sizeof(line), fptr);
                }

                printf("Fir Rating Of %s: %s\n", username, line);
                int fir = atoi(line);
                if (fir <= 4) printf("%s is Unranked\n", username);
                else if (fir >= 5 && fir <= 9) printf("%s is Bronze\n", username);
                else if (fir >= 10 && fir <= 30) printf("%s is Silver\n", username);
                else if (fir >= 31 && fir <= 50) printf("%s is Gold\n", username);
                else if (fir >= 51 && fir <= 100) printf("%s is Platinum\n", username);
                else if (fir >= 101 && fir <= 500) printf("%s is Diamond\n", username);
                else if (fir >= 501 && fir <= 1000) printf("%s is Master\n", username);
                else printf("That guy is a Grand Master\n");
                fclose(fptr);
            } else {
                printf("User not found\n");
            }
        } else if (command == 2) {
            printf("\nYou are trying to play with a human.\nPlease customize your game.\n");

            int size;
            do {
                printf("Select the size of the board (9~40): ");
                if (scanf("%d", &size) != 1) {
                    printf("Invalid input. Please enter a number.\n");
                    flush_input();
                    continue;
                }
                flush_input();
                if (size < 9 || size > 40) {
                    printf("Invalid input. Please try again.\n");
                }
            } while (size < 9 || size > 40);
            size--;

            int player;
            do {
                printf("Select the number of players (2~5): ");
                if (scanf("%d", &player) != 1) {
                    printf("Invalid input. Please enter a number.\n");
                    flush_input();
                    continue;
                }
                flush_input();
                if (player < 2 || player > 5) {
                    printf("Invalid input. Please try again.\n");
                }
            } while (player < 2 || player > 5);

            int time;
            do {
                printf("Choose your time in seconds (300~1200): ");
                if (scanf("%d", &time) != 1) {
                    printf("Invalid input. Please enter a number.\n");
                    flush_input();
                    continue;
                }
                flush_input();
                if (time < 300 || time > 1200) {
                    printf("Invalid input. Please try again.\n");
                }
            } while (time < 300 || time > 1200);

            int player_clocks[player];
            for (int i = 0; i < player; i++) {
                player_clocks[i] = time;
            }

            char player_name[5][20] = { "" };
            for (int i = 0; i < player; i++) {
                printf("Enter the name of player %d: ", i + 1);
                if (scanf("%s", player_name[i]) != 1) {
                    printf("Invalid input.\n");
                    flush_input();
                    continue;
                }
                for (int j = 0; j < i; j++) {
                    if (strcmp(player_name[i], player_name[j]) == 0) {
                        printf("Username already exists! Please try again.\n");
                        i--;
                        break;
                    }
                }
            }

            printf("\nThe game begins!\nThis is a game of\n\n");
            for (int i = 0; i < player; i++) {
                printf("[%s]\n", player_name[i]);
                reduce_rating(player_name[i], 3);
            }
            printf("Remember, make 5 in a row and you will win.\n");
            printf("This game costed 3 Points. And good luck.\n");

            // Waits 3 seconds and clears the screen
            sleep(3);
            clear_screen();

            int board[46][46] = {0};
            int turn = -1;
            int pointer_x = size / 2, pointer_y = size / 2;

            while (1) {
                if(turn!=-1){
                  int win = 0;
                  for (int i = 0; i <= size; i++) {
                      for (int j = 0; j <= size - 4; j++) {
                          if (board[i][j] == turn + 1 && board[i][j + 1] == turn + 1 && board[i][j + 2] == turn + 1 && board[i][j + 3] == turn + 1 && board[i][j + 4] == turn + 1) {
                              win = 1;
                              break;
                          }
                      }
                      if (win) break;
                  }
                  if (win) {
                      printf("%s wins!\n", player_name[turn]);
                    for (int i = 0; i < player; i++) {
                        if (i != turn) {
                            reduce_rating(player_name[i], player);
                        }
                        else{
                          reduce_rating(player_name[i], player * -4);
                        }
                    }
                      break;
                  }
                  for (int j = 0; j <= size; j++) {
                      for (int i = 0; i <= size - 4; i++) {
                          if (board[i][j] == turn + 1 && board[i + 1][j] == turn + 1 && board[i + 2][j] == turn + 1 && board[i + 3][j] == turn + 1 && board[i + 4][j] == turn + 1) {
                              win = 1;
                              break;
                          }
                      }
                      if (win) break;
                  }
                  if (win) {
                      printf("%s wins!\n", player_name[turn]);
                      for (int i = 0; i < player; i++) {
                          if (i != turn) {
                              reduce_rating(player_name[i], player);
                          }
                          else{
                            reduce_rating(player_name[i], player * -4);
                          }
                      }
                      break;
                  }
                  for (int i = 0; i <= size - 4; i++) {
                      for (int j = 0; j <= size - 4; j++) {
                          if (board[i][j] == turn + 1 && board[i + 1][j + 1] == turn + 1 && board[i + 2][j + 2] == turn + 1 && board[i + 3][j + 3] == turn + 1 && board[i + 4][j + 4] == turn + 1) {
                              win = 1;
                              break;
                          }
                      }
                      if (win) break;
                  }
                  if (win) {
                      printf("%s wins!\n", player_name[turn]);
                    for (int i = 0; i < player; i++) {
                        if (i != turn) {
                            reduce_rating(player_name[i], player);
                        }
                        else{
                          reduce_rating(player_name[i], player * -4);
                        }
                    }
                      break;
                  }
                  for (int i = 4; i <= size; i++) {
                      for (int j = 0; j <= size - 4; j++) {
                          if (board[i][j] == turn + 1 && board[i - 1][j + 1] == turn + 1 && board[i - 2][j + 2] == turn + 1 && board[i - 3][j + 3] == turn + 1 && board[i - 4][j + 4] == turn + 1) {
                              win = 1;
                              break;
                          }
                      }
                      if (win) break;
                  }
                  if (win) {
                      printf("%s wins!\n", player_name[turn]);
                    for (int i = 0; i < player; i++) {
                        if (i != turn) {
                            reduce_rating(player_name[i], player);
                        }
                        else{
                          reduce_rating(player_name[i], player * -4);
                        }
                    }
                      break;
                  }
                }
                
                turn++;
                turn %= player;
                printf("It is %s's turn. Remaining time: %d seconds.\n", player_name[turn], player_clocks[turn]);
                sleep(1);

                while (player_clocks[turn] > 0) {
                    // Reduce clock
                    sleep(1);
                    player_clocks[turn]--;
                    // Clear screen
                    clear_screen();

                    for (int i = 0; i <= size; i++) {
                        for (int j = 0; j <= size; j++) {
                            if (pointer_x == i && pointer_y == j) {
                                printf("🔘");
                                continue;
                            }
                            if (board[i][j] == 0) {
                                printf("⚫");
                            } else {
                                if (board[i][j] == 1) printf("🔴");
                                else if (board[i][j] == 2) printf("🔵");
                                else if (board[i][j] == 3) printf("🟡");
                                else if (board[i][j] == 4) printf("🟢");
                                else if (board[i][j] == 5) printf("🟣");
                            }
                        }
                        printf("\n");
                    }

                    // Handle user input for moving the pointer
                    if (kbhit()) {
                        char input = getchar();
                        if (input == 'A' || input == 'a') {
                            if (pointer_y > 0) {
                                pointer_y--;
                            }
                        } else if (input == 'D' || input == 'd') {
                            if (pointer_y < size) {
                                pointer_y++;
                            }
                        } else if (input == 'W' || input == 'w') {
                            if (pointer_x > 0) {
                                pointer_x--;
                            }
                        } else if (input == 'S' || input == 's') {
                            if (pointer_x < size) {
                                pointer_x++;
                            }
                        } else if (input == ' ' || input == '\r') {
                            if (board[pointer_x][pointer_y] == 0) {
                                board[pointer_x][pointer_y] = turn + 1;
                                break;
                            }
                        } else if (input == 'J' || input == 'j') {
                            int a, b;
                            if (scanf("%d %d", &a, &b) != 2) {
                                printf("Invalid input.\n");
                                flush_input();
                                continue;
                            }
                            a--;
                            b--;

                            // If a and b are in the range
                            if (a >= 0 && a <= size && b >= 0 && b <= size) {
                                pointer_x = a;
                                pointer_y = b;
                            } else {
                                printf("Invalid coordinates.\n");
                            }
                        }
                        else if (input == 'x' || input=='X'){
                          //Draw
                          char draw[80]="";
                          printf("Before you type this command, you should have asked other players if they wanted to draw.\n If you want to draw, type 'I would Like to draw and I've already asked other players about this'\n\n");
                          scanf("%s",draw);
                          if(strcmp(draw,"I would Like to draw and I've already asked other player about this")==0){
                            printf("You have drawn. You can't draw again");
                            for(int i=0; i<player; i++){
                              reduce_rating(player_name[i], player * -1);
                            }
                            goto here;
                          }
                        }
                        else if(input=='0'){
                          break;
                        }
                    }
                }

                if (player_clocks[turn] == 0) {
                    printf("Time's up for %s!\n", player_name[turn]);
                    player_clocks[turn] = 50;
                    for (int i = 0; i <= size; i++) {
                        for (int j = 0; j <= size; j++) {
                            if (board[i][j] == turn + 1) {
                                board[i][j] = 0;
                            }
                        }
                    }
                    sleep(2);
                }
            }
        } else if (command == 5) {
            printf("\n\nThis is a help menu.\nTo play a game, just press 2 after registration (to register, press 1).\nThere is a tier system in this game.\nThe higher the tier, the better the player.\n\n0~4 Unranked\n5~9 Bronze\n10~30 Silver\n31~50 Gold\n51~100 Platinum\n101~500 Diamond\n501~1000 Master\n\n If someone beats another player, their ratings could change.\n");
        } else {
            printf("Invalid command. Please try again. (Press 5 For help)\n");
        }
    }
}
void reduce_rating(const char* username, int reduction) {
  FILE *fptr = fopen("user.txt", "r");
  if (!fptr) {
      perror("Error opening user.txt");
      return;
  }

  char line[MAX_LENGTH];
  int found = 0;
  int line_number = 0;
  int user_line = -1;

  while (fgets(line, sizeof(line), fptr)) {
      line[strcspn(line, "\n")] = 0;
      if (strcmp(line, username) == 0) {
          found = 1;
          user_line = line_number;
          break;
      }
      line_number++;
  }

  fclose(fptr);

  if (found) {
      fptr = fopen("games.txt", "r+");
      if (!fptr) {
          perror("Error opening games.txt");
          return;
      }

      // Read all ratings into memory
      char ratings[100][MAX_LENGTH];
      int count = 0;

      while (fgets(ratings[count], sizeof(ratings[count]), fptr)) {
          ratings[count][strcspn(ratings[count], "\n")] = 0;
          count++;
      }

      // Reduce the rating
      int fir = atoi(ratings[user_line]);
      fir -= reduction;

      if (fir < 0) fir = 0; // Ensure rating does not go below 0

      // Update the rating in memory
      snprintf(ratings[user_line], MAX_LENGTH, "%d", fir);

      // Write all ratings back to the file
      freopen("games.txt", "w", fptr);
      for (int i = 0; i < count; i++) {
          fprintf(fptr, "%s\n", ratings[i]);
      }

      fclose(fptr);
  }
}

void clear_screen() {
  #ifdef _WIN32
      system("cls");
  #else
      system("clear");
  #endif
}
int kbhit(void) {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
              
