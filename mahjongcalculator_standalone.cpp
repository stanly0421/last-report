#include "mahjongcalculator_standalone.h"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <codecvt>
#include <locale>

MahjongCalculatorStandalone::MahjongCalculatorStandalone()
{
}

std::vector<std::string> MahjongCalculatorStandalone::calculateWaitingTiles(const std::string& handTiles)
{
    std::vector<std::string> waitingTiles;
    
    // 解析手牌
    std::vector<int> tiles = parseHandTiles(handTiles);
    
    // 如果解析失敗，返回空結果
    if (tiles.empty()) {
        return waitingTiles;
    }
    
    // 檢查每一種可能的牌（1-34）
    // 1-9: 萬子, 10-18: 筒子, 19-27: 索子, 28-34: 字牌
    for (int testTile = 1; testTile <= 34; testTile++) {
        // 創建一個包含測試牌的手牌副本
        std::vector<int> testHand = tiles;
        testHand.push_back(testTile);
        
        // 檢查是否可以和牌
        if (canWin(testHand)) {
            waitingTiles.push_back(getTileName(testTile));
        }
    }
    
    return waitingTiles;
}

std::vector<int> MahjongCalculatorStandalone::parseHandTiles(const std::string& handTiles)
{
    std::vector<int> tiles;
    
    // 將UTF-8字符串轉換為寬字符串以便處理中文
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring whandTiles;
    try {
        whandTiles = converter.from_bytes(handTiles);
    } catch (...) {
        return tiles;  // 轉換失敗
    }
    
    size_t pos = 0;
    while (pos < whandTiles.length()) {
        wchar_t ch = whandTiles[pos];
        
        // 匹配數字
        if (ch >= L'1' && ch <= L'9') {
            // 收集所有連續的數字
            std::vector<int> numbers;
            while (pos < whandTiles.length() && whandTiles[pos] >= L'1' && whandTiles[pos] <= L'9') {
                numbers.push_back(whandTiles[pos] - L'0');
                pos++;
            }
            
            // 檢查後面的類型
            if (pos < whandTiles.length()) {
                wchar_t type = whandTiles[pos];
                if (type == L'萬') {
                    for (int num : numbers) {
                        tiles.push_back(num);  // 1-9
                    }
                    pos++;
                } else if (type == L'筒') {
                    for (int num : numbers) {
                        tiles.push_back(num + 9);  // 10-18
                    }
                    pos++;
                } else if (type == L'索') {
                    for (int num : numbers) {
                        tiles.push_back(num + 18);  // 19-27
                    }
                    pos++;
                }
            }
        }
        // 匹配字牌
        else if (ch == L'東') {
            tiles.push_back(28);
            pos++;
        } else if (ch == L'南') {
            tiles.push_back(29);
            pos++;
        } else if (ch == L'西') {
            tiles.push_back(30);
            pos++;
        } else if (ch == L'北') {
            tiles.push_back(31);
            pos++;
        } else if (ch == L'中') {
            tiles.push_back(32);
            pos++;
        } else if (ch == L'發') {
            tiles.push_back(33);
            pos++;
        } else if (ch == L'白') {
            tiles.push_back(34);
            pos++;
        } else {
            pos++;  // 跳過未知字符
        }
    }
    
    return tiles;
}

std::string MahjongCalculatorStandalone::getTileName(int tileId)
{
    if (tileId >= 1 && tileId <= 9) {
        return std::to_string(tileId) + "萬";
    } else if (tileId >= 10 && tileId <= 18) {
        return std::to_string(tileId - 9) + "筒";
    } else if (tileId >= 19 && tileId <= 27) {
        return std::to_string(tileId - 18) + "索";
    } else if (tileId == 28) {
        return "東";
    } else if (tileId == 29) {
        return "南";
    } else if (tileId == 30) {
        return "西";
    } else if (tileId == 31) {
        return "北";
    } else if (tileId == 32) {
        return "中";
    } else if (tileId == 33) {
        return "發";
    } else if (tileId == 34) {
        return "白";
    }
    return "";
}

bool MahjongCalculatorStandalone::canWin(std::vector<int> tiles)
{
    // 標準麻將需要14張牌和牌
    if (tiles.size() != 14) {
        return false;
    }
    
    // 排序牌
    std::sort(tiles.begin(), tiles.end());
    
    // 統計每張牌的數量
    std::map<int, int> tileCount;
    for (int t : tiles) {
        tileCount[t]++;
    }
    
    // 嘗試每一張牌作為雀頭（對子）
    for (auto it = tileCount.begin(); it != tileCount.end(); ++it) {
        int pairTile = it->first;
        if (it->second >= 2) {
            // 移除雀頭
            std::map<int, int> remaining = tileCount;
            remaining[pairTile] -= 2;
            if (remaining[pairTile] == 0) {
                remaining.erase(pairTile);
            }
            
            // 檢查剩餘的牌是否可以組成順子和刻子
            if (checkMelds(remaining)) {
                return true;
            }
        }
    }
    
    return false;
}

bool MahjongCalculatorStandalone::checkMelds(std::map<int, int> tiles)
{
    // 如果沒有剩餘的牌，表示成功
    if (tiles.empty()) {
        return true;
    }
    
    // 取得最小的牌
    int minTile = tiles.begin()->first;
    int count = tiles[minTile];
    
    // 嘗試組成刻子
    if (count >= 3) {
        std::map<int, int> remaining = tiles;
        remaining[minTile] -= 3;
        if (remaining[minTile] == 0) {
            remaining.erase(minTile);
        }
        if (checkMelds(remaining)) {
            return true;
        }
    }
    
    // 嘗試組成順子（只有數牌可以組成順子）
    int tileType = getTileType(minTile);
    if (tileType >= 0 && tileType <= 2) {  // 萬、筒、索
        int baseValue = (minTile - 1) % 9 + 1;
        int nextTile1 = minTile + 1;
        int nextTile2 = minTile + 2;
        
        // 確保順子在同一類型內且不超過9
        if (baseValue <= 7 && tiles.count(nextTile1) && tiles.count(nextTile2)) {
            std::map<int, int> remaining = tiles;
            remaining[minTile]--;
            if (remaining[minTile] == 0) {
                remaining.erase(minTile);
            }
            remaining[nextTile1]--;
            if (remaining[nextTile1] == 0) {
                remaining.erase(nextTile1);
            }
            remaining[nextTile2]--;
            if (remaining[nextTile2] == 0) {
                remaining.erase(nextTile2);
            }
            if (checkMelds(remaining)) {
                return true;
            }
        }
    }
    
    return false;
}

int MahjongCalculatorStandalone::getTileType(int tileId)
{
    if (tileId >= 1 && tileId <= 9) {
        return 0;  // 萬
    } else if (tileId >= 10 && tileId <= 18) {
        return 1;  // 筒
    } else if (tileId >= 19 && tileId <= 27) {
        return 2;  // 索
    } else if (tileId >= 28 && tileId <= 34) {
        return 3;  // 字牌
    }
    return -1;
}
