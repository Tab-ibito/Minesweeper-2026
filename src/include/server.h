#ifndef SERVER_H
#define SERVER_H

#include <cstdlib>
#include <iostream>
#include <vector>

/*
 * You may need to define some global variables for the information of the game map here.
 * Although we don't encourage to use global variables in real cpp projects, you may have to use them because the use of
 * class is not taught yet. However, if you are member of A-class or have learnt the use of cpp class, member functions,
 * etc., you're free to modify this structure.
 */

int rows;         // The count of rows of the game map. You MUST NOT modify its name.
int columns;      // The count of columns of the game map. You MUST NOT modify its name.
int total_mines;  // The count of mines of the game map. You MUST NOT modify its name. You should initialize this
                  // variable in function InitMap. It will be used in the advanced task.
int game_state;  // The state of the game, 0 for continuing, 1 for winning, -1 for losing. You MUST NOT modify its name.
std::vector<std::vector<bool>> map;          // 实际排布地图，1是踩到雷
std::vector<std::vector<char>> map_visible;  // 玩家可见地图
int explored = 0;                            // 已经探明的数量
int target = 0;                              // 需要探明的数量
int marked_mine_count = 0;                   // 标记的数量

/**
 * @brief The definition of function InitMap()
 *
 * @details This function is designed to read the initial map from stdin. For example, if there is a 3 * 3 map in which
 * mines are located at (0, 1) and (2, 2) (0-based), the stdin would be
 *     3 3
 *     .X.
 *     ...
 *     ..X
 * where X stands for a mine block and . stands for a normal block. After executing this function, your game map
 * would be initialized, with all the blocks unvisited.
 */
void InitMap() {
    std::cin >> rows >> columns;
    // TODO (student): Implement me!
    map = std::vector(rows, std::vector<bool>(columns, false));
    map_visible = std::vector(rows, std::vector<char>(columns));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            char c;
            std::cin >> c;
            map_visible[i][j] = '?';
            if (c == 'X') {
                map[i][j] = true;
                total_mines++;
            }
        }
    }
    target = rows * columns - total_mines;
}

/**
 * @brief The definition of function VisitBlock(int, int)
 *
 * @details This function is designed to visit a block in the game map. We take the 3 * 3 game map above as an example.
 * At the beginning, if you call VisitBlock(0, 0), game_state would be 0 (game continues), and the game map would
 * be
 *     1??
 *     ???
 *     ???
 * If you call VisitBlock(0, 1) after that, game_state would be -1 (game ends and the player loses), and the
 * game map would be
 *     1X?
 *     ???
 *     ???
 * If you call VisitBlock(0, 2), VisitBlock(2, 0), VisitBlock(1, 2) instead, game_state after the last operation
 * would be 1 (game ends and the player wins), and the game map would be
 *     1@1
 *     122
 *     01@
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 *
 * @note You should edit the value of game_state in this function. Precisely, edit it to
 *    0  if the game continues after visit that block, or that block has already been visited before.
 *    1  if the game ends and the player wins.
 *    -1 if the game ends and the player loses.
 *
 * @note For invalid operation, you should not do anything.
 */
bool is_valid_position(int r, int c) {
    // 检查合法坐标
    return r >= 0 && r < rows && c >= 0 && c < columns;
}

int get_mine_nums(int r, int c, bool get_flagged = false) {
    // 检查3*3区域的雷数量
    int cnt = 0;
    for (int i = r - 1; i <= r + 1; i++) {
        for (int j = c - 1; j <= c + 1; j++) {
            // 去除不合法位置
            if (!is_valid_position(i, j)) continue;
            if (i == r && j == c) continue;
            if (map[i][j] && !get_flagged) cnt++;
            if (map_visible[i][j] == '@' && get_flagged) cnt++;
        }
    }
    return cnt;
}

void VisitBlock(int r, int c) {
    // TODO (student): Implement me!
    // 已经被探索过或者做过标记，跳过
    if (map_visible[r][c] != '?') return;
    if (map[r][c]) {
        // 踩到雷，直接退出
        map_visible[r][c] = 'X';
        game_state = -1;
        return;
    }
    int cnt = get_mine_nums(r, c);
    map_visible[r][c] = cnt + 48;
    explored++;
    if (cnt == 0) {
        // 周边没有雷，排查外围一圈
        for (int i = r - 1; i <= r + 1; i++) {
            for (int j = c - 1; j <= c + 1; j++) {
                if (!is_valid_position(i, j)) continue;
                if (map_visible[i][j] == '?') VisitBlock(i, j);
            }
        }
    }
    if (explored == target) game_state = 1;
}

/**
 * @brief The definition of function MarkMine(int, int)
 *
 * @details This function is designed to mark a mine in the game map.
 * If the block being marked is a mine, show it as "@".
 * If the block being marked isn't a mine, END THE GAME immediately. (NOTE: This is not the same rule as the real
 * game.)
 *
 * For example, if we use the same map as before, and the current state is:
 *     1?1
 *     ???
 *     ???
 * If you call MarkMine(0, 1), you marked the right mine. Then the resulting game map is:
 *     1@1
 *     ???
 *     ???
 * If you call MarkMine(1, 0), you marked the wrong mine(There's no mine in grid (1, 0)).
 * The game_state would be -1 and game ends immediately. The game map would be:
 *     1?1
 *     X??
 *     ???
 * This is different from the Minesweeper you've played. You should beware of that.
 *
 * @param r The row coordinate (0-based) of the block to be marked.
 * @param c The column coordinate (0-based) of the block to be marked.
 *
 * @note You should edit the value of game_state in this function. Precisely, edit it to
 *    0  if the game continues after visit that block, or that block has already been visited before.
 *    1  if the game ends and the player wins.
 *    -1 if the game ends and the player loses.
 *
 * @note For invalid operation, you should not do anything.
 */
void MarkMine(int r, int c) {
    // TODO (student): Implement me!
    // 已经被探索过或者做过标记，跳过
    if (map_visible[r][c] != '?') return;
    // 不是地雷，失败
    if (!map[r][c]) {
        map_visible[r][c] = 'X';
        game_state = -1;
        return;
    }
    map_visible[r][c] = '@';
    marked_mine_count++;
}

/**
 * @brief The definition of function AutoExplore(int, int)
 *
 * @details This function is designed to auto-visit adjacent blocks of a certain block.
 * See README.md for more information
 *
 * For example, if we use the same map as before, and the current map is:
 *     ?@?
 *     ?2?
 *     ??@
 * Then auto explore is available only for block (1, 1). If you call AutoExplore(1, 1), the resulting map will be:
 *     1@1
 *     122
 *     01@
 * And the game ends (and player wins).
 */

void AutoExplore(int r, int c) {
    // TODO (student): Implement me!
    if (map_visible[r][c] == '?' || map_visible[r][c] == '@') return;
    int cnt = get_mine_nums(r, c);
    int cnt_flagged = get_mine_nums(r, c, true);
    if (cnt != cnt_flagged) return;

    // 自动点开
    for (int i = r - 1; i <= r + 1; i++) {
        for (int j = c - 1; j <= c + 1; j++) {
            if (!is_valid_position(i, j)) continue;
            if (map_visible[i][j] == '?') VisitBlock(i, j);
        }
    }
}

/**
 * @brief The definition of function ExitGame()
 *
 * @details This function is designed to exit the game.
 * It outputs a line according to the result, and a line of two integers, visit_count and marked_mine_count,
 * representing the number of blocks visited and the number of marked mines taken respectively.
 *
 * @note If the player wins, we consider that ALL mines are correctly marked.
 */
void ExitGame() {
    // TODO (student): Implement me!
    if (game_state == 1) {
        std::cout << "YOU WIN!" << std::endl;
        std::cout << explored << ' ' << total_mines << std::endl;
    }
    if (game_state == -1) {
        std::cout << "GAME OVER!" << std::endl;
        std::cout << explored << ' ' << marked_mine_count << std::endl;
    }
    exit(0);  // Exit the game immediately
}

/**
 * @brief The definition of function PrintMap()
 *
 * @details This function is designed to print the game map to stdout. We take the 3 * 3 game map above as an example.
 * At the beginning, if you call PrintMap(), the stdout would be
 *    ???
 *    ???
 *    ???
 * If you call VisitBlock(2, 0) and PrintMap() after that, the stdout would be
 *    ???
 *    12?
 *    01?
 * If you call VisitBlock(0, 1) and PrintMap() after that, the stdout would be
 *    ?X?
 *    12?
 *    01?
 * If the player visits all blocks without mine and call PrintMap() after that, the stdout would be
 *    1@1
 *    122
 *    01@
 * (You may find the global variable game_state useful when implementing this function.)
 *
 * @note Use std::cout to print the game map, especially when you want to try the advanced task!!!
 */
void PrintMap() {
    // TODO (student): Implement me!
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            if (!is_valid_position(i, j)) continue;
            if (game_state == 1 && map_visible[i][j] == '?') {
                std::cout << '@';
                continue;
            }
            std::cout << map_visible[i][j];
        }
        std::cout << std::endl;
    }
}

#endif
