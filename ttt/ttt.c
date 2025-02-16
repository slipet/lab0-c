#include "ttt.h"

static int move_record[N_GRIDS];
static int move_count = 0;

static int (*move_predic)(char *, void *);

static void record_move(int move)
{
    move_record[move_count++] = move;
}

static void print_moves()
{
    printf("Moves: ");
    for (int i = 0; i < move_count; i++) {
        printf("%c%d", 'A' + GET_COL(move_record[i]),
               1 + GET_ROW(move_record[i]));
        if (i < move_count - 1) {
            printf(" -> ");
        }
    }
    printf("\n");
}

static int get_input(char player)
{
    char *line = NULL;
    size_t line_length = 0;
    int parseX = 1;

    int x = -1, y = -1;
    while (x < 0 || x > (BOARD_SIZE - 1) || y < 0 || y > (BOARD_SIZE - 1)) {
        printf("%c> ", player);
        int r = getline(&line, &line_length, stdin);
        if (r == -1)
            exit(1);
        if (r < 2)
            continue;
        x = 0;
        y = 0;
        parseX = 1;
        if (strcmp(line, "quit\n") == 0) {
            // temporarily code for terminate the game
            return -1;
        }
        for (int i = 0; i < (r - 1); i++) {
            if (isalpha(line[i]) && parseX) {
                x = x * 26 + (tolower(line[i]) - 'a' + 1);
                if (x > BOARD_SIZE) {
                    // could be any value in [BOARD_SIZE + 1, INT_MAX]
                    x = BOARD_SIZE + 1;
                    printf("Invalid operation: index exceeds board size\n");
                    break;
                }
                continue;
            }
            // input does not have leading alphabets
            if (x == 0) {
                printf("Invalid operation: No leading alphabet\n");
                y = 0;
                break;
            }
            parseX = 0;
            if (isdigit(line[i])) {
                y = y * 10 + line[i] - '0';
                if (y > BOARD_SIZE) {
                    // could be any value in [BOARD_SIZE + 1, INT_MAX]
                    y = BOARD_SIZE + 1;
                    printf("Invalid operation: index exceeds board size\n");
                    break;
                }
                continue;
            }
            // any other character is invalid
            // any non-digit char during digit parsing is invalid
            // TODO: Error message could be better by separating these two cases
            printf("Invalid operation\n");
            x = y = 0;
            break;
        }
        x -= 1;
        y -= 1;
    }
    free(line);
    return GET_INDEX(y, x);
}

// void init_ttt(int opt, int argc,void *argv[])
// {
//     if(opt == 0) {//USE_RL
//         rl_agent_t *agent = argv[0];
//     }
//     else if(opt == 1) {//USE_MCTS
//     }else if(opt == 2){

//     }else {//default

//     }
// }

int tic_tac_toe(int algo, int para)
{
    srand(time(NULL));
    char table[N_GRIDS];
    memset(table, ' ', N_GRIDS);
    char turn = 'X';
    char ai = 'O';
    void *arg = NULL;
    unsigned int state_num = 1;
    move_predic = NULL;

    if (algo == 0) {  // RL
        arg = (void *) malloc(sizeof(rl_agent_t));
        CALC_STATE_NUM(state_num);
        init_rl_agent((rl_agent_t *) arg, state_num, 'O');
        load_model((rl_agent_t *) arg, state_num, MODEL_NAME);

        move_predic = (int (*)(char *, void *)) play_rl;
    } else if (algo == 1) {  // MCTS
        arg = &ai;
        if (para == 0) {  // float
            move_predic = (int (*)(char *, void *)) mcts;
        } else if (para == 1) {  // fixed32
        } else if (para == 2) {  // fixed64
        } else {                 // default
            move_predic = (int (*)(char *, void *)) mcts;
        }
    } else {  // default MINMAX
        negamax_init();
        arg = &ai;
        move_predic = (int (*)(char *, void *)) negamax_predict;
    }
    while (1) {
        char win = check_win(table);
        if (win == 'D') {
            draw_board(table);
            printf("It is a draw!\n");
            break;
        } else if (win != ' ') {
            draw_board(table);
            printf("%c won!\n", win);
            break;
        }

        if (turn == ai) {
            int move = move_predic(table, arg);
            if (algo > 0) {
                if (move != -1) {
                    table[move] = ai;
                    record_move(move);
                } else {
                    record_move(move);
                }
            }
        } else {
            draw_board(table);
            int move;
            while (1) {
                move = get_input(turn);
                if (move == -1)
                    goto exit;
                if (table[move] == ' ') {
                    break;
                }
                printf("Invalid operation: the position has been marked\n");
            }
            table[move] = turn;
            record_move(move);
        }
        turn = turn == 'X' ? 'O' : 'X';
    }
exit:
    print_moves();
    move_count = 0;
    return 0;
}
