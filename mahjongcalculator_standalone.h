#ifndef MAHJONGCALCULATOR_STANDALONE_H
#define MAHJONGCALCULATOR_STANDALONE_H

#include <string>
#include <vector>
#include <map>

// 麻將聽牌計算器 - 獨立版本（不依賴Qt）
class MahjongCalculatorStandalone
{
public:
    MahjongCalculatorStandalone();
    
    // 計算聽牌 - 輸入手牌，返回所有可能的聽牌
    std::vector<std::string> calculateWaitingTiles(const std::string& handTiles);
    
    // 解析手牌字符串為牌的列表
    std::vector<int> parseHandTiles(const std::string& handTiles);
    
    // 將牌的編號轉換為名稱
    std::string getTileName(int tileId);
    
private:
    // 檢查是否可以和牌（包含指定的牌）
    bool canWin(std::vector<int> tiles);
    
    // 檢查剩餘的牌是否可以組成面子（順子和刻子）
    bool checkMelds(std::map<int, int> tiles);
    
    // 計算牌的類型（萬、筒、索、字）
    int getTileType(int tileId);
};

#endif // MAHJONGCALCULATOR_STANDALONE_H
