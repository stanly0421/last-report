#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QVector>
#include <QMessageBox>
#include <QLineEdit>
#include <QTextEdit>
#include "mahjongcalculator.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

// 麻將牌結構
struct MahjongTile {
    int type;        // 牌的類型 (0-8: 萬子, 9-17: 筒子, 18-26: 索子, 27-33: 字牌)
    bool matched;    // 是否已配對
    QPushButton* button; // 對應的按鈕
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void onTileClicked();
    void resetGame();
    void calculateTenpai();  // 新增：計算聽牌

private:
    void initializeGame();
    void createGameBoard();
    QString getTileName(int type);
    void checkMatch();
    void checkWinCondition();

    Ui::Widget *ui;
    QVector<MahjongTile> tiles;
    QLabel* statusLabel;
    QLabel* scoreLabel;
    QPushButton* resetButton;
    QGridLayout* gameLayout;
    
    // 聽牌計算器相關組件
    QLineEdit* handInput;
    QPushButton* calculateButton;
    QTextEdit* resultDisplay;
    MahjongCalculator* calculator;
    
    int firstSelectedIndex;
    int secondSelectedIndex;
    int matchedPairs;
    int totalPairs;
};
#endif // WIDGET_H
