#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <utility>
#include <vector>

extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.

// You MUST NOT use any other external variables except for rows, columns and total_mines.
/**
 * @brief The definition of function Execute(int, int, int)
 *
 * @details This function is designed to take a step when player the client's (or player's) role, and the implementation
 * of it has been finished by TA. (I hope my comments in code would be easy to understand T_T) If you do not understand
 * the contents, please ask TA for help immediately!!!
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 * @param type The type of operation to a certain block.
 * If type == 0, we'll execute VisitBlock(row, column).
 * If type == 1, we'll execute MarkMine(row, column).
 * If type == 2, we'll execute AutoExplore(row, column).
 * You should not call this function with other type values.
 */
void Execute(int r, int c, int type);

/**
 * @brief The definition of function InitGame()
 *
 * @details This function is designed to initialize the client state. It should be called at the beginning of the game,
 * after InitMap() has read the map scale. It reads and executes the first step provided by the input (see README).
 */
std::vector<std::vector<char>> map_player;  // 可见地图
std::vector<std::vector<bool>> progress;    // 标记哪些块是已解决

// 定义常量
constexpr int undetected = 0;
constexpr int flagged = 1;
constexpr int number = 2;

void InitGame() {
    // TODO (student): Initialize all your global variables!
    int first_row, first_column;
    std::cin >> first_row >> first_column;
    map_player = std::vector(rows, std::vector<char>(columns, '?'));
    progress = std::vector(rows, std::vector<bool>(columns, false));
    Execute(first_row, first_column, 0);
}

/**
 * @brief The definition of function ReadMap()
 *
 * @details This function is designed to read the game map from stdin when playing the client's (or player's) role.
 * Since the client (or player) can only get the limited information of the game map, so if there is a 3 * 3 map as
 * above and only the block (2, 0) has been visited, the stdin would be
 *     ???
 *     12?
 *     01?
 */
void ReadMap() {
    // TODO (student): Implement me!
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            char c;
            std::cin >> c;
            map_player[i][j] = c;
        }
    }
}

/**
 * @brief The definition of function Decide()
 *
 * @details This function is designed to decide the next step when playing the client's (or player's) role. Open up your
 * mind and make your decision here! Caution: you can only execute once in this function.
 */
int status(int r, int c) {
    // 返回三种状态，未探索是0，插旗是1，数字情况是2
    if (map_player[r][c] == '?') return undetected;
    if (map_player[r][c] == '@') return flagged;
    return number;
}

bool is_valid(int r, int c) {
    // 检查合法坐标
    return r >= 0 && r < rows && c >= 0 && c < columns;
}

int get_status(int r, int c, int target) {
    // 检查3*3区域的标记数量
    int cnt = 0;
    for (int i = r - 1; i <= r + 1; i++) {
        for (int j = c - 1; j <= c + 1; j++) {
            // 去除不合法位置
            if (!is_valid(i, j)) continue;
            if (i == r && j == c) continue;
            // 统计
            if (status(i, j) == target) cnt++;
        }
    }
    return cnt;
}

void strategy_1(int r, int c, int &x, int &y) {
    // 策略1，只需要传一个坐标给x, y就可以
    for (int i = r - 1; i <= r + 1; i++) {
        for (int j = c - 1; j <= c + 1; j++) {
            // 去除不合法位置
            if (!is_valid(i, j)) continue;
            if (i == r && j == c) continue;
            // 返回
            if (status(i, j) == undetected) {
                x = i;
                y = j;
                return;
            }
        }
    }
}

void strategy_2(int r, int c, int &x, int &y, int &type, bool &is_changed) {
    // 策略2
    if (progress[r][c]) return;
    std::vector<std::vector<bool>> infer = std::vector(rows, std::vector<bool>(columns, false));
    // 标注未探明区域
    for (int i = r - 1; i <= r + 1; i++) {
        for (int j = c - 1; j <= c + 1; j++) {
            // 去除不合法位置
            if (!is_valid(i, j)) continue;
            if (i == r && j == c) continue;
            if (status(i, j) == undetected) {
                infer[i][j] = true;
            }
        }
    }
    // 考虑所有可能重叠情况并检测
    bool is_overlap = false;
    int r2, c2;  // 结合推理起点
    for (int i = r - 2; i <= r + 2; i++) {
        if (is_overlap) break;
        for (int j = c - 2; j <= c + 2; j++) {
            // 去除不合法位置
            if (!is_valid(i, j)) continue;
            if (i == r && j == c) continue;
            if (progress[i][j]) continue;
            if (status(i, j) != number) continue;
            bool is_cross = false;  // 检测到交错直接跳过
            // 遍历扫描
            for (int s = i - 1; s <= i + 1; s++) {
                if (is_cross) break;
                for (int t = j - 1; t <= j + 1; t++) {
                    // 去除不合法位置
                    if (!is_valid(s, t)) continue;
                    if (s == i && t == j) continue;
                    if (status(s, t) != undetected) continue;
                    // 确定不重叠
                    if (!infer[s][t]) {
                        is_cross = true;
                        break;
                    }
                }
            }
            // 无交错情况，符合推理条件
            if (!is_cross) {
                is_overlap = true;
                r2 = i;
                c2 = j;
                break;
            }
        }
    }

    if (!is_overlap) return;  // 没有符合条件的
    int remained_1 = map_player[r][c] - 48 - get_status(r, c, flagged);
    int remained_2 = map_player[r2][c2] - 48 - get_status(r2, c2, flagged);
    int difference = remained_1 - remained_2;  // 检测地雷数显示差别
    std::vector<std::pair<int, int>> pos;
    // 找出差异坐标
    for (int i = r2 - 1; i <= r2 + 1; i++) {
        for (int j = c2 - 1; j <= c2 + 1; j++) {
            if (!is_valid(i, j)) continue;
            if (i == r2 && j == c2) continue;
            if (infer[i][j]) infer[i][j] = false;
        }
    }
    for (int i = r - 1; i <= r + 1; i++) {
        for (int j = c - 1; j <= c + 1; j++) {
            if (!is_valid(i, j)) continue;
            if (i == r && j == c) continue;
            if (infer[i][j]) pos.push_back({i, j});
        }
    }
    if (pos.size() == 0) return;  // 所见区域相等，没有有效信息
    if (difference == 0) {        // 能确定集合之差的部分一定不是雷
        x = pos[0].first;
        y = pos[0].second;
        type = 0;
        is_changed = true;
    }
    if (difference == pos.size()) {  // 能确定集合之差的部分一定是雷
        x = pos[0].first;
        y = pos[0].second;
        type = 1;
        is_changed = true;
    }
}

void Decide() {
    // TODO (student): Implement me!
    int x = -1, y = -1, type = 0;  // 给Evaluate的传参

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            // 策略0 点击一个最前面的
            if (status(i, j) == undetected) {
                x = i;
                y = j;
            }
        }
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            // 策略1 对于单个块的确定性结果标记雷
            if (progress[i][j]) continue;
            if (status(i, j) == number) {
                int cnt = map_player[i][j] - 48;
                int cnt_flagged = get_status(i, j, flagged);
                int cnt_undetected = get_status(i, j, undetected);
                int cnt_number = get_status(i, j, number);
                if (!cnt_undetected) {
                    progress[i][j] = true;
                    continue;
                }
                if (cnt_flagged == cnt) {
                    // 只剩下没开的
                    strategy_1(i, j, x, y);
                    type = 0;
                    Execute(x, y, type);
                    return;
                }
                if (cnt_undetected + cnt_flagged == cnt) {
                    // 只剩下没标的
                    strategy_1(i, j, x, y);
                    type = 1;
                    Execute(x, y, type);
                    return;
                }
            }
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            // 策略2 比较未探明部分是否被周边方块包含
            if (progress[i][j]) continue;
            if (status(i, j) == number) {
                bool is_changed = false;
                strategy_2(i, j, x, y, type, is_changed);
                if (!is_changed) continue;
                Execute(x, y, type);
                return;
            }
        }
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            // 策略3 优先按没开过图比较稀疏的部分
            if (status(i, j) == undetected) {
                int cnt_number = get_status(i, j, number);
                if (cnt_number == 0) {
                    Execute(i, j, type);
                    return;
                }
            }
        }
    }
    Execute(x, y, type);
    return;
}

#endif
