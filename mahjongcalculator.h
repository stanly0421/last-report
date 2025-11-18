#ifndef MAHJONGCALCULATOR_H
#define MAHJONGCALCULATOR_H

#include <QString>
#include <QVector>
#include <QMap>

// 麻將聽牌計算器
class MahjongCalculator
{
public:
    MahjongCalculator();
    
    // 計算聽牌 - 輸入手牌，返回所有可能的聽牌
    QVector<QString> calculateWaitingTiles(const QString& handTiles);
    
    // 解析手牌字符串為牌的列表
    QVector<int> parseHandTiles(const QString& handTiles);
    
    // 將牌的編號轉換為名稱
    QString getTileName(int tileId);
    
    // 將名稱轉換為牌的編號
    int parseTile(const QString& tileName);
    
private:
    // 檢查是否為完整牌型（不包括聽牌）
    bool isCompletedHand(const QVector<int>& tiles);
    
    // 檢查是否可以和牌（包含指定的牌）
    bool canWin(QVector<int> tiles, int tile);
    
    // 檢查是否為有效的組合（順子或刻子）
    bool isValidGroup(const QVector<int>& tiles);
    
    // 計算牌的類型（萬、筒、索、字）
    int getTileType(int tileId);
    
    // 檢查是否為順子
    bool isSequence(int tile1, int tile2, int tile3);
    
    // 檢查是否為刻子（三張相同）
    bool isTriplet(int tile1, int tile2, int tile3);
    
    // 檢查是否為對子（兩張相同）
    bool isPair(int tile1, int tile2);
    
    // 檢查剩餘的牌是否可以組成面子（順子和刻子）
    bool checkMelds(QMap<int, int> tiles);
};

#endif // MAHJONGCALCULATOR_H
