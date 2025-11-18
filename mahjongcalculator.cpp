#include "mahjongcalculator.h"
#include <QRegularExpression>
#include <algorithm>

MahjongCalculator::MahjongCalculator()
{
}

QVector<QString> MahjongCalculator::calculateWaitingTiles(const QString& handTiles)
{
    QVector<QString> waitingTiles;
    
    // 解析手牌
    QVector<int> tiles = parseHandTiles(handTiles);
    
    // 如果解析失敗，返回空結果
    if (tiles.isEmpty()) {
        return waitingTiles;
    }
    
    // 檢查每一種可能的牌（1-34）
    // 1-9: 萬子, 10-18: 筒子, 19-27: 索子, 28-34: 字牌
    for (int testTile = 1; testTile <= 34; testTile++) {
        // 創建一個包含測試牌的手牌副本
        QVector<int> testHand = tiles;
        testHand.append(testTile);
        
        // 檢查是否可以和牌
        if (canWin(testHand, testTile)) {
            waitingTiles.append(getTileName(testTile));
        }
    }
    
    return waitingTiles;
}

QVector<int> MahjongCalculator::parseHandTiles(const QString& handTiles)
{
    QVector<int> tiles;
    
    // 正則表達式匹配數字+類型（萬/筒/索）或字牌
    QRegularExpression numberPattern("(\\d+)([萬筒索])");
    QRegularExpression charPattern("[東南西北中發白]");
    
    int pos = 0;
    while (pos < handTiles.length()) {
        QChar ch = handTiles[pos];
        
        // 匹配數字+類型
        if (ch.isDigit()) {
            QRegularExpressionMatch match = numberPattern.match(handTiles, pos);
            if (match.hasMatch() && match.capturedStart() == pos) {
                QString numbers = match.captured(1);
                QString type = match.captured(2);
                
                for (QChar numChar : numbers) {
                    int num = numChar.digitValue();
                    if (num >= 1 && num <= 9) {
                        int tileId = 0;
                        if (type == "萬") {
                            tileId = num;  // 1-9
                        } else if (type == "筒") {
                            tileId = num + 9;  // 10-18
                        } else if (type == "索") {
                            tileId = num + 18;  // 19-27
                        }
                        tiles.append(tileId);
                    }
                }
                pos = match.capturedEnd();
                continue;
            }
        }
        
        // 匹配字牌
        QRegularExpressionMatch charMatch = charPattern.match(handTiles, pos);
        if (charMatch.hasMatch() && charMatch.capturedStart() == pos) {
            QString charTile = charMatch.captured(0);
            QStringList chars = {"東", "南", "西", "北", "中", "發", "白"};
            int index = chars.indexOf(charTile);
            if (index >= 0) {
                tiles.append(28 + index);  // 28-34
            }
            pos = charMatch.capturedEnd();
            continue;
        }
        
        pos++;
    }
    
    return tiles;
}

QString MahjongCalculator::getTileName(int tileId)
{
    if (tileId >= 1 && tileId <= 9) {
        return QString("%1萬").arg(tileId);
    } else if (tileId >= 10 && tileId <= 18) {
        return QString("%1筒").arg(tileId - 9);
    } else if (tileId >= 19 && tileId <= 27) {
        return QString("%1索").arg(tileId - 18);
    } else if (tileId >= 28 && tileId <= 34) {
        QStringList chars = {"東", "南", "西", "北", "中", "發", "白"};
        return chars[tileId - 28];
    }
    return "";
}

int MahjongCalculator::parseTile(const QString& tileName)
{
    QVector<int> tiles = parseHandTiles(tileName);
    if (!tiles.isEmpty()) {
        return tiles[0];
    }
    return -1;
}

bool MahjongCalculator::canWin(QVector<int> tiles, int tile)
{
    // 標準麻將需要14張牌和牌
    if (tiles.size() != 14) {
        return false;
    }
    
    // 排序牌
    std::sort(tiles.begin(), tiles.end());
    
    // 統計每張牌的數量
    QMap<int, int> tileCount;
    for (int t : tiles) {
        tileCount[t]++;
    }
    
    // 嘗試每一張牌作為雀頭（對子）
    for (auto it = tileCount.begin(); it != tileCount.end(); ++it) {
        int pairTile = it.key();
        if (it.value() >= 2) {
            // 移除雀頭
            QMap<int, int> remaining = tileCount;
            remaining[pairTile] -= 2;
            if (remaining[pairTile] == 0) {
                remaining.remove(pairTile);
            }
            
            // 檢查剩餘的牌是否可以組成順子和刻子
            if (checkMelds(remaining)) {
                return true;
            }
        }
    }
    
    return false;
}

bool MahjongCalculator::checkMelds(QMap<int, int> tiles)
{
    // 如果沒有剩餘的牌，表示成功
    if (tiles.isEmpty()) {
        return true;
    }
    
    // 取得最小的牌
    int minTile = tiles.begin().key();
    int count = tiles[minTile];
    
    // 嘗試組成刻子
    if (count >= 3) {
        QMap<int, int> remaining = tiles;
        remaining[minTile] -= 3;
        if (remaining[minTile] == 0) {
            remaining.remove(minTile);
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
        if (baseValue <= 7 && tiles.contains(nextTile1) && tiles.contains(nextTile2)) {
            QMap<int, int> remaining = tiles;
            remaining[minTile]--;
            if (remaining[minTile] == 0) {
                remaining.remove(minTile);
            }
            remaining[nextTile1]--;
            if (remaining[nextTile1] == 0) {
                remaining.remove(nextTile1);
            }
            remaining[nextTile2]--;
            if (remaining[nextTile2] == 0) {
                remaining.remove(nextTile2);
            }
            if (checkMelds(remaining)) {
                return true;
            }
        }
    }
    
    return false;
}

int MahjongCalculator::getTileType(int tileId)
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

bool MahjongCalculator::isSequence(int tile1, int tile2, int tile3)
{
    QVector<int> tiles = {tile1, tile2, tile3};
    std::sort(tiles.begin(), tiles.end());
    
    // 檢查是否為同一類型
    int type1 = getTileType(tiles[0]);
    int type2 = getTileType(tiles[1]);
    int type3 = getTileType(tiles[2]);
    
    if (type1 != type2 || type2 != type3 || type1 >= 3) {
        return false;  // 字牌不能組成順子
    }
    
    // 檢查是否為連續
    return (tiles[1] == tiles[0] + 1) && (tiles[2] == tiles[1] + 1);
}

bool MahjongCalculator::isTriplet(int tile1, int tile2, int tile3)
{
    return tile1 == tile2 && tile2 == tile3;
}

bool MahjongCalculator::isPair(int tile1, int tile2)
{
    return tile1 == tile2;
}

bool MahjongCalculator::isCompletedHand(const QVector<int>& tiles)
{
    return tiles.size() == 14 && canWin(const_cast<QVector<int>&>(tiles), tiles.last());
}
