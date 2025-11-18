#include "widget.h"
#include "ui_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QTime>
#include <algorithm>
#include <random>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , firstSelectedIndex(-1)
    , secondSelectedIndex(-1)
    , matchedPairs(0)
    , totalPairs(0)
    , calculator(new MahjongCalculator())
{
    ui->setupUi(this);
    
    // 設置窗口標題
    setWindowTitle("麻將配對遊戲");
    
    // 創建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(ui->centralWidget);
    
    // 狀態標籤
    statusLabel = new QLabel("歡迎來到麻將配對遊戲！點擊兩張相同的牌進行配對。", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("font-size: 14px; padding: 10px;");
    mainLayout->addWidget(statusLabel);
    
    // 分數標籤
    scoreLabel = new QLabel("已配對: 0/0", this);
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLabel->setStyleSheet("font-size: 16px; font-weight: bold; padding: 5px;");
    mainLayout->addWidget(scoreLabel);
    
    // 遊戲板布局
    gameLayout = new QGridLayout();
    gameLayout->setSpacing(5);
    mainLayout->addLayout(gameLayout);
    
    // 重置按鈕
    resetButton = new QPushButton("重新開始", this);
    resetButton->setStyleSheet("font-size: 14px; padding: 10px; background-color: #4CAF50; color: white; border-radius: 5px;");
    connect(resetButton, &QPushButton::clicked, this, &Widget::resetGame);
    mainLayout->addWidget(resetButton);
    
    // 添加分隔線
    QFrame* separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setStyleSheet("background-color: #CCC; margin: 10px 0;");
    mainLayout->addWidget(separator);
    
    // 聽牌計算器區域
    QLabel* calculatorTitle = new QLabel("麻將聽牌計算器", this);
    calculatorTitle->setAlignment(Qt::AlignCenter);
    calculatorTitle->setStyleSheet("font-size: 16px; font-weight: bold; padding: 10px;");
    mainLayout->addWidget(calculatorTitle);
    
    QLabel* instructionLabel = new QLabel("輸入手牌（例如：111萬、123456789萬、1234567東南西）：", this);
    instructionLabel->setStyleSheet("font-size: 12px; padding: 5px;");
    mainLayout->addWidget(instructionLabel);
    
    // 手牌輸入框
    handInput = new QLineEdit(this);
    handInput->setPlaceholderText("例如：111萬 或 123456789萬");
    handInput->setStyleSheet("font-size: 14px; padding: 8px; border: 2px solid #CCC; border-radius: 5px;");
    mainLayout->addWidget(handInput);
    
    // 計算按鈕
    calculateButton = new QPushButton("計算聽牌", this);
    calculateButton->setStyleSheet("font-size: 14px; padding: 10px; background-color: #2196F3; color: white; border-radius: 5px;");
    connect(calculateButton, &QPushButton::clicked, this, &Widget::calculateTenpai);
    mainLayout->addWidget(calculateButton);
    
    // 結果顯示區域
    QLabel* resultLabel = new QLabel("計算結果：", this);
    resultLabel->setStyleSheet("font-size: 12px; padding: 5px;");
    mainLayout->addWidget(resultLabel);
    
    resultDisplay = new QTextEdit(this);
    resultDisplay->setReadOnly(true);
    resultDisplay->setMaximumHeight(100);
    resultDisplay->setStyleSheet("font-size: 14px; padding: 8px; border: 2px solid #CCC; border-radius: 5px; background-color: #F5F5F5;");
    mainLayout->addWidget(resultDisplay);
    
    // 初始化遊戲
    initializeGame();
}

Widget::~Widget()
{
    delete calculator;
    delete ui;
}

void Widget::initializeGame()
{
    // 清空現有的遊戲板
    QLayoutItem* item;
    while ((item = gameLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    tiles.clear();
    
    // 創建16對牌（32張牌）
    totalPairs = 16;
    matchedPairs = 0;
    firstSelectedIndex = -1;
    secondSelectedIndex = -1;
    
    // 生成16種不同的牌，每種2張
    for (int i = 0; i < totalPairs; i++) {
        MahjongTile tile1;
        tile1.type = i;
        tile1.matched = false;
        tile1.button = nullptr;
        
        MahjongTile tile2;
        tile2.type = i;
        tile2.matched = false;
        tile2.button = nullptr;
        
        tiles.append(tile1);
        tiles.append(tile2);
    }
    
    // 隨機打亂牌的順序
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(tiles.begin(), tiles.end(), g);
    
    // 創建遊戲板
    createGameBoard();
    
    // 更新分數顯示
    scoreLabel->setText(QString("已配對: %1/%2").arg(matchedPairs).arg(totalPairs));
}

void Widget::createGameBoard()
{
    // 創建8x4的遊戲板
    int rows = 4;
    int cols = 8;
    
    for (int i = 0; i < tiles.size(); i++) {
        QPushButton* btn = new QPushButton(getTileName(tiles[i].type), this);
        btn->setMinimumSize(80, 60);
        btn->setStyleSheet(
            "QPushButton {"
            "   background-color: #FFE5B4;"
            "   border: 2px solid #8B4513;"
            "   border-radius: 5px;"
            "   font-size: 16px;"
            "   font-weight: bold;"
            "   color: #8B0000;"
            "}"
            "QPushButton:hover {"
            "   background-color: #FFD700;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #FFA500;"
            "}"
        );
        btn->setProperty("tileIndex", i);
        connect(btn, &QPushButton::clicked, this, &Widget::onTileClicked);
        
        tiles[i].button = btn;
        
        int row = i / cols;
        int col = i % cols;
        gameLayout->addWidget(btn, row, col);
    }
}

QString Widget::getTileName(int type)
{
    // 根據類型返回牌的名稱
    if (type < 9) {
        // 萬子 (1萬 - 9萬)
        return QString("%1萬").arg(type + 1);
    } else if (type < 18) {
        // 筒子 (1筒 - 9筒)
        return QString("%1筒").arg(type - 8);
    } else if (type < 27) {
        // 索子 (1索 - 9索)
        return QString("%1索").arg(type - 17);
    } else {
        // 字牌 (東南西北中發白)
        QStringList chars = {"東", "南", "西", "北", "中", "發", "白"};
        return chars[type - 27];
    }
}

void Widget::onTileClicked()
{
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;
    
    int index = clickedButton->property("tileIndex").toInt();
    
    // 如果牌已經配對，忽略點擊
    if (tiles[index].matched) return;
    
    // 第一次點擊
    if (firstSelectedIndex == -1) {
        firstSelectedIndex = index;
        clickedButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #87CEEB;"
            "   border: 3px solid #4169E1;"
            "   border-radius: 5px;"
            "   font-size: 16px;"
            "   font-weight: bold;"
            "   color: #8B0000;"
            "}"
        );
        statusLabel->setText("選擇第二張牌...");
    }
    // 第二次點擊
    else if (secondSelectedIndex == -1 && index != firstSelectedIndex) {
        secondSelectedIndex = index;
        clickedButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #87CEEB;"
            "   border: 3px solid #4169E1;"
            "   border-radius: 5px;"
            "   font-size: 16px;"
            "   font-weight: bold;"
            "   color: #8B0000;"
            "}"
        );
        checkMatch();
    }
}

void Widget::checkMatch()
{
    if (firstSelectedIndex == -1 || secondSelectedIndex == -1) return;
    
    // 檢查兩張牌是否相同
    if (tiles[firstSelectedIndex].type == tiles[secondSelectedIndex].type) {
        // 配對成功
        tiles[firstSelectedIndex].matched = true;
        tiles[secondSelectedIndex].matched = true;
        
        tiles[firstSelectedIndex].button->setStyleSheet(
            "QPushButton {"
            "   background-color: #90EE90;"
            "   border: 2px solid #228B22;"
            "   border-radius: 5px;"
            "   font-size: 16px;"
            "   font-weight: bold;"
            "   color: #006400;"
            "}"
        );
        tiles[secondSelectedIndex].button->setStyleSheet(
            "QPushButton {"
            "   background-color: #90EE90;"
            "   border: 2px solid #228B22;"
            "   border-radius: 5px;"
            "   font-size: 16px;"
            "   font-weight: bold;"
            "   color: #006400;"
            "}"
        );
        
        tiles[firstSelectedIndex].button->setEnabled(false);
        tiles[secondSelectedIndex].button->setEnabled(false);
        
        matchedPairs++;
        statusLabel->setText("配對成功！繼續選擇下一對牌。");
        scoreLabel->setText(QString("已配對: %1/%2").arg(matchedPairs).arg(totalPairs));
        
        checkWinCondition();
    } else {
        // 配對失敗
        tiles[firstSelectedIndex].button->setStyleSheet(
            "QPushButton {"
            "   background-color: #FFE5B4;"
            "   border: 2px solid #8B4513;"
            "   border-radius: 5px;"
            "   font-size: 16px;"
            "   font-weight: bold;"
            "   color: #8B0000;"
            "}"
        );
        tiles[secondSelectedIndex].button->setStyleSheet(
            "QPushButton {"
            "   background-color: #FFE5B4;"
            "   border: 2px solid #8B4513;"
            "   border-radius: 5px;"
            "   font-size: 16px;"
            "   font-weight: bold;"
            "   color: #8B0000;"
            "}"
        );
        
        statusLabel->setText("配對失敗！請重新選擇。");
    }
    
    firstSelectedIndex = -1;
    secondSelectedIndex = -1;
}

void Widget::checkWinCondition()
{
    if (matchedPairs == totalPairs) {
        statusLabel->setText("恭喜！你完成了所有配對！");
        QMessageBox::information(this, "遊戲結束", 
                                "恭喜你完成了所有配對！\n點擊「重新開始」按鈕開始新遊戲。");
    }
}

void Widget::resetGame()
{
    initializeGame();
    statusLabel->setText("遊戲已重置。開始新的遊戲吧！");
}

void Widget::calculateTenpai()
{
    QString handTiles = handInput->text().trimmed();
    
    if (handTiles.isEmpty()) {
        resultDisplay->setText("請輸入手牌！");
        return;
    }
    
    // 計算聽牌
    QVector<QString> waitingTiles = calculator->calculateWaitingTiles(handTiles);
    
    // 顯示結果
    if (waitingTiles.isEmpty()) {
        resultDisplay->setText("此手牌無法聽牌，或輸入格式錯誤。\n\n請確保輸入格式正確，例如：\n- 111萬（三張1萬）\n- 123456789萬（1-9萬各一張）\n- 東東東南南南西西西（字牌組合）");
    } else {
        QString result = QString("聽牌結果：\n\n聽 %1 張牌：\n").arg(waitingTiles.size());
        for (const QString& tile : waitingTiles) {
            result += tile + " ";
        }
        resultDisplay->setText(result);
    }
}
