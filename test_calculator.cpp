#include "mahjongcalculator.h"
#include <QCoreApplication>
#include <QDebug>
#include <iostream>

// Simple test program for MahjongCalculator
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    
    MahjongCalculator calculator;
    
    // Test cases
    struct TestCase {
        QString input;
        QString description;
    };
    
    QVector<TestCase> testCases = {
        {"111萬", "Example from problem statement: 三張1萬"},
        {"123456789萬", "Example from problem statement: 1-9萬各一張"},
        {"1112345678999萬", "13張萬子（缺一張成和）"},
        {"111222333萬東東", "三個刻子加一個對子（缺一張）"},
        {"東東東南南南西西西", "三組字牌刻子（缺一張）"},
        {"11123456789萬", "有雀頭和順子"},
        {"1234567萬東南西", "混合數牌和字牌"}
    };
    
    std::cout << "=== 麻將聽牌計算器測試 ===" << std::endl << std::endl;
    
    for (const auto& test : testCases) {
        std::cout << "輸入: " << test.input.toStdString() << std::endl;
        std::cout << "說明: " << test.description.toStdString() << std::endl;
        
        // 解析手牌
        QVector<int> tiles = calculator.parseHandTiles(test.input);
        std::cout << "解析到 " << tiles.size() << " 張牌" << std::endl;
        
        // 計算聽牌
        QVector<QString> waitingTiles = calculator.calculateWaitingTiles(test.input);
        
        if (waitingTiles.isEmpty()) {
            std::cout << "結果: 無法聽牌或格式錯誤" << std::endl;
        } else {
            std::cout << "聽 " << waitingTiles.size() << " 張牌: ";
            for (const QString& tile : waitingTiles) {
                std::cout << tile.toStdString() << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::string(50, '-') << std::endl << std::endl;
    }
    
    // Interactive mode
    std::cout << "\n進入互動模式（輸入 'quit' 退出）：" << std::endl;
    while (true) {
        std::cout << "\n請輸入手牌: ";
        std::string input;
        std::getline(std::cin, input);
        
        if (input == "quit" || input == "exit" || input == "q") {
            break;
        }
        
        QString handTiles = QString::fromStdString(input);
        if (handTiles.trimmed().isEmpty()) {
            continue;
        }
        
        QVector<int> tiles = calculator.parseHandTiles(handTiles);
        std::cout << "解析到 " << tiles.size() << " 張牌" << std::endl;
        
        QVector<QString> waitingTiles = calculator.calculateWaitingTiles(handTiles);
        
        if (waitingTiles.isEmpty()) {
            std::cout << "結果: 無法聽牌或格式錯誤" << std::endl;
        } else {
            std::cout << "聽 " << waitingTiles.size() << " 張牌: ";
            for (const QString& tile : waitingTiles) {
                std::cout << tile.toStdString() << " ";
            }
            std::cout << std::endl;
        }
    }
    
    return 0;
}
