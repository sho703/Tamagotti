#include <M5Unified.h>
#include "M5UnitENV.h"

SHT3X sht3x;

// 初期温度
float initialTemp = -100.0;     // 初期値として不可能な低温を設定
float frame3StartTemp = -100.0; // frame3開始時の温度
float frame5StartTemp = -100.0; // frame5開始時の温度
float frame7StartTemp = -100.0; // frame7開始時の温度

// グローバル変数
bool tempRiseDetected = false;
bool moveToFrame3 = false;
bool moveToFrame5 = false;
bool moveToFrame7 = false;
bool moveToFrame9 = false;
bool heartDisplayed = false;
unsigned long heartStartTime = 0;

// プロトタイプ宣言
void drawFrame(uint8_t frame[12][12]);
void playEffect();
void drawFrameWithOffset(uint8_t frame[12][12], int offset);
void playTransitionSound();
void playFailureSound();
void drawHeart();
void clearHeart();
void resetToInitialState();

// フレームデータ
//たまご
uint8_t frame1[12][12] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0},
    {0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0},
    {0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}
};

uint8_t frame2[12][12] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0},
    {0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1},
    {1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1},
    {0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

//べびっち
uint8_t frame3[12][12] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0}
};

uint8_t frame4[12][12] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0}
};

//まるっち
uint8_t frame5[12][12] = {
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

uint8_t frame6[12][12] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

//たまっち
uint8_t frame7[12][12] = {
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

uint8_t frame8[12][12] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

//死亡時
uint8_t frame9[12][12] = {
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

// 横揺れアニメーション（音量と揺れを調整）
void playEffect() {
    M5.Speaker.setVolume(10);  // 音量を最大に設定
    int amplitude = 8;         // 揺れ幅を拡大
    int cycles = 6;            // 揺れの回数

    for (int i = 0; i < cycles; i++) {
        int offset = (i % 2 == 0) ? amplitude : -amplitude;  // 左右に揺らす
        M5.Lcd.fillScreen(LIGHTGREY);
        drawFrameWithOffset(frame1, offset);  // 横揺れを適用して描画
        M5.Speaker.tone(1000 + (i * 200), 200);  // ピコピコ音
        delay(300);
    }
}

// 横揺れを適用してフレームを描画
void drawFrameWithOffset(uint8_t frame[12][12], int offset) {
    uint16_t size = 10;
    uint16_t centerX = 67 + offset;  // 横揺れのオフセットを適用
    uint16_t centerY = 120;
    uint16_t startX = centerX - (12 * size) / 2;
    uint16_t startY = centerY - (12 * size) / 2;

    for (uint16_t i = 0; i < 12; i++) {
        for (uint16_t j = 0; j < 12; j++) {
            uint16_t color = (frame[i][j] == 1) ? BLACK : LIGHTGREY;
            M5.Lcd.fillRect(startX + j * size, startY + i * size, size, size, color);
        }
    }
}

void drawFrame(uint8_t frame[12][12]) {
    drawFrameWithOffset(frame, 0);  // 横揺れなしの描画
}

// ハートを画面上部に描画
void drawHeart() {
    int centerX = 67;
    int centerY = 20;
    int size = 5;

    // ハートの形を描画
    uint8_t heart[7][7] = {
        {0, 1, 0, 0, 0, 1, 0},
        {1, 1, 1, 0, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 0},
        {0, 0, 1, 1, 1, 0, 0},
        {0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0}
    };

    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            uint16_t color = (heart[i][j] == 1) ? BLACK : LIGHTGREY;
            M5.Lcd.fillRect(centerX - 3 * size + j * size, centerY + i * size, size, size, color);
        }
    }
}

// ハートを消去
void clearHeart() {
    int centerX = 67;
    int centerY = 20;
    int size = 5;

    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            M5.Lcd.fillRect(centerX - 3 * size + j * size, centerY + i * size, size, size, LIGHTGREY);
        }
    }
}

// 切り替え時の音を鳴らす
void playTransitionSound() {
    for (int i = 0; i < 3; i++) {
        M5.Speaker.tone(1500, 200);  // 高い音を鳴らす
        delay(200);
    }
}

// 残念な感じの不協和音
void playFailureSound() {
    int tones[] = {200, 250, 180, 300, 220};
    for (int i = 0; i < 5; i++) {
        M5.Speaker.tone(tones[i], 300);
        delay(300);
    }
}

// 状態を初期化してframe1に戻る
void resetToInitialState() {
    initialTemp = -100.0;
    frame3StartTemp = -100.0;
    frame5StartTemp = -100.0;
    frame7StartTemp = -100.0;
    tempRiseDetected = false;
    moveToFrame3 = false;
    moveToFrame5 = false;
    moveToFrame7 = false;
    moveToFrame9 = false;
    heartDisplayed = false;
    heartStartTime = 0;
    M5.Lcd.fillScreen(LIGHTGREY);
    drawFrame(frame1);
    delay(100); // リセットの際に画面遷移を防ぐ短い遅延
}

void setup() {
    Serial.begin(9600);
    M5.begin();
    M5.Lcd.fillScreen(LIGHTGREY);
    if (!sht3x.begin(&Wire, SHT3X_I2C_ADDR, 32, 33, 400000U)) {
        Serial.println("SHT3X initialization failed!");
        while (1) delay(1);
    }
    M5.Speaker.begin();  // スピーカーの初期化
}

void loop() {
    Serial.println(sht3x.cTemp);
    if (sht3x.update()) {
        if (initialTemp == -100.0) {
            initialTemp = sht3x.cTemp;  // 初回の温度を記録
        }

        if (sht3x.cTemp >= initialTemp + 0.5 && !tempRiseDetected) {
            tempRiseDetected = true;  // 温度が0.5℃上昇したことを検知
        }

        if (moveToFrame3 && frame3StartTemp == -100.0) {
            frame3StartTemp = sht3x.cTemp;  // frame3開始時の温度を記録
        }

        if (moveToFrame5 && frame5StartTemp == -100.0) {
            frame5StartTemp = sht3x.cTemp;  // frame5開始時の温度を記録
        }

        if (frame3StartTemp != -100.0 && sht3x.cTemp >= frame3StartTemp + 0.5 && !moveToFrame5) {
            playTransitionSound();  // 切り替え時の音を鳴らす
            moveToFrame5 = true;    // frame3から0.5℃上昇したらframe5,6に移行
        }

        if (frame5StartTemp != -100.0 && sht3x.cTemp >= frame5StartTemp + 0.5 && !moveToFrame7) {
            playTransitionSound();  // 切り替え時の音を鳴らす
            moveToFrame7 = true;    // frame5から0.5℃上昇したらframe7,8に移行
        }

        if ((sht3x.cTemp <= initialTemp - 0.2 && !moveToFrame9) ||
            (frame3StartTemp != -100.0 && sht3x.cTemp <= frame3StartTemp - 0.2 && !moveToFrame9) ||
            (frame5StartTemp != -100.0 && sht3x.cTemp <= frame5StartTemp - 0.2 && !moveToFrame9) ||
            (frame7StartTemp != -100.0 && sht3x.cTemp <= frame7StartTemp - 0.2 && !moveToFrame9)) {
            playFailureSound();  // 残念な感じの不協和音
            moveToFrame9 = true;  // -0.2℃下がったらframe9に移行
        }
    }

    if (tempRiseDetected && !moveToFrame3) {
        playEffect();  // 横揺れアニメーションとピコピコ音
        if (M5.BtnA.wasPressed()) {
            moveToFrame3 = true;  // ボタンAを押したらframe3,4へ移行
        }
    }

    if (moveToFrame9) {
        drawFrame(frame9);
        delay(500);

        if (M5.BtnA.wasPressed()) {
            resetToInitialState();  // 初期状態にリセット
        }

    } else if (moveToFrame7) {
        drawFrame(frame7);
        delay(500);
        drawFrame(frame8);
        delay(500);

        if (M5.BtnA.wasPressed() && !heartDisplayed) {
            drawHeart();  // ハートを画面上部に追加表示
            heartDisplayed = true;
            heartStartTime = millis();
        }

        if (heartDisplayed && millis() - heartStartTime >= 1000) {
            clearHeart();  // 1秒後にハートを消去
            heartDisplayed = false;  // ハート表示状態をリセット
        }
    } else if (moveToFrame5) {
        drawFrame(frame5);
        delay(500);
        drawFrame(frame6);
        delay(500);
    } else if (moveToFrame3) {
        drawFrame(frame3);
        delay(500);
        drawFrame(frame4);
        delay(500);
    } else if (!tempRiseDetected) {
        drawFrame(frame1);
        delay(500);
        drawFrame(frame2);
        delay(500);
    }

    M5.update();  // ボタンの状態を更新
}
