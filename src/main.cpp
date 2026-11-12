//顔だけhappyが変

#include <M5Unified.h>
#include <math.h>

M5Canvas canvas(&M5.Display);
String currentExpression = "NEUTRAL";

// 画面中心 (160, 120) に対する公式基準座標のオフセット計算
// _eye_pos = (-70, -16) -> 左目(90, 104), 右目(230, 104)
static const int EYE_LEFT_X  = 90;
static const int EYE_RIGHT_X = 230;
static const int EYE_Y       = 104;
static const int EYE_SIZE    = 20; // _eye_size_limit (8 ~ 32) の標準サイズ

// _mouth_pos = (0, 26) -> 口(160, 146)
static const int MOUTH_X     = 160;
static const int MOUTH_Y     = 146;

// 回転する四角形を描画する直線図形ヘルパー関数
void fillRotatedRect(int cx, int cy, int w, int h, float angleDeg, uint16_t color) {
    if (angleDeg == 0.0f) {
        canvas.fillRect(cx - w / 2, cy - h / 2, w, h, color);
        return;
    }

    float rad = angleDeg * 0.0174532925f; // deg to rad
    float cosA = cosf(rad);
    float sinA = sinf(rad);

    float hw = w / 2.0f;
    float hh = h / 2.0f;

    // ローカル座標から回転後の4頂点を計算
    int x0 = cx + (int)(-hw * cosA - -hh * sinA);
    int y0 = cy + (int)(-hw * sinA + -hh * cosA);
    int x1 = cx + (int)( hw * cosA - -hh * sinA);
    int y1 = cy + (int)( hw * sinA + -hh * cosA);
    int x2 = cx + (int)( hw * cosA -  hh * sinA);
    int y2 = cy + (int)( hw * sinA +  hh * cosA);
    int x3 = cx + (int)(-hw * cosA -  hh * sinA);
    int y3 = cy + (int)(-hw * sinA +  hh * cosA);

    // 2つの三角形で回転四角形を合成
    canvas.fillTriangle(x0, y0, x1, y1, x2, y2, color);
    canvas.fillTriangle(x0, y0, x2, y2, x3, y3, color);
}

// 直線図形（四角形・多角形）のみでアバターを描画
void drawAvatar(String expr) {
    canvas.fillScreen(TFT_BLACK);

    // -------------------------------------------------------------
    // 1. 目（Eyes）の描画（円を使わず、角型・回転四角形・直線で表現）
    // -------------------------------------------------------------
    if (expr == "HAPPY") {
        // Happy: 傾き 15.5度 (1550 / 100) + 薄型目（まぶた被せ表現）
        fillRotatedRect(EYE_LEFT_X, EYE_Y, EYE_SIZE + 4, 6, 15.5f, TFT_WHITE);
        fillRotatedRect(EYE_RIGHT_X, EYE_Y, EYE_SIZE + 4, 6, -15.5f, TFT_WHITE);
    } 
    else if (expr == "ANGRY") {
        // Angry: 内側傾き 4.5度 (450 / 100)
        fillRotatedRect(EYE_LEFT_X, EYE_Y, EYE_SIZE, EYE_SIZE, 4.5f, TFT_WHITE);
        fillRotatedRect(EYE_RIGHT_X, EYE_Y, EYE_SIZE, EYE_SIZE, -4.5f, TFT_WHITE);
        // 上まぶたの直線カット（目頭を下げるつり眉効果）
        canvas.fillTriangle(EYE_LEFT_X - 12, EYE_Y - 12, EYE_LEFT_X + 12, EYE_Y - 12, EYE_LEFT_X + 12, EYE_Y - 2, TFT_BLACK);
        canvas.fillTriangle(EYE_RIGHT_X - 12, EYE_Y - 12, EYE_RIGHT_X + 12, EYE_Y - 12, EYE_RIGHT_X - 12, EYE_Y - 2, TFT_BLACK);
    } 
    else if (expr == "SAD") {
        // Sad: 外側傾き -4.0度 (-400 / 100)
        fillRotatedRect(EYE_LEFT_X, EYE_Y, EYE_SIZE, EYE_SIZE, -4.0f, TFT_WHITE);
        fillRotatedRect(EYE_RIGHT_X, EYE_Y, EYE_SIZE, EYE_SIZE, 4.0f, TFT_WHITE);
        // 上まぶたの直線カット（八の字効果）
        canvas.fillTriangle(EYE_LEFT_X - 12, EYE_Y - 12, EYE_LEFT_X + 12, EYE_Y - 12, EYE_LEFT_X - 12, EYE_Y - 2, TFT_BLACK);
        canvas.fillTriangle(EYE_RIGHT_X - 12, EYE_Y - 12, EYE_RIGHT_X + 12, EYE_Y - 12, EYE_RIGHT_X + 12, EYE_Y - 2, TFT_BLACK);
    } 
    else { 
        // Neutral: 正方形目 (回転角 0度)
        fillRotatedRect(EYE_LEFT_X, EYE_Y, EYE_SIZE, EYE_SIZE, 0.0f, TFT_WHITE);
        fillRotatedRect(EYE_RIGHT_X, EYE_Y, EYE_SIZE, EYE_SIZE, 0.0f, TFT_WHITE);
    }

    // -------------------------------------------------------------
    // 2. 口（Mouth）の描画（円弧・楕円を使わず四角形・直線で表現）
    // -------------------------------------------------------------
    if (expr == "HAPPY") {
        // 開口（_mouth_max_size 60x50 に準拠した長方形口）
        canvas.fillRect(MOUTH_X - 30, MOUTH_Y - 15, 60, 30, TFT_WHITE);
        canvas.fillRect(MOUTH_X - 26, MOUTH_Y - 11, 52, 22, TFT_BLACK); // 中抜きで枠線表現
    } 
    else if (expr == "ANGRY") {
        // 斜め直線口
        fillRotatedRect(MOUTH_X, MOUTH_Y, 70, 6, -5.0f, TFT_WHITE);
    } 
    else if (expr == "SAD") {
        // への字（2本の太い直線でV字）
        fillRotatedRect(MOUTH_X - 18, MOUTH_Y + 4, 36, 6, 12.0f, TFT_WHITE);
        fillRotatedRect(MOUTH_X + 18, MOUTH_Y + 4, 36, 6, -12.0f, TFT_WHITE);
    } 
    else {
        // Neutral: 公式コードの _mouth_min_size (90x6) を正確に再現した直線口
        canvas.fillRect(MOUTH_X - 45, MOUTH_Y - 3, 90, 6, TFT_WHITE);
    }

    // メモリから画面へ一括反映
    canvas.pushSprite(0, 0);
}

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);

    Serial.begin(115200);
    Serial.setTimeout(50);

    canvas.createSprite(320, 240);
    drawAvatar(currentExpression);

    Serial.println("\n--- Linear Geometry Avatar Ready ---");
}

void loop() {
    M5.update();

    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.replace("\r", "");
        cmd.trim();
        cmd.toUpperCase();

        if (cmd == "HAPPY" || cmd == "ANGRY" || cmd == "SAD" || cmd == "NEUTRAL") {
            currentExpression = cmd;
            drawAvatar(currentExpression);
            Serial.printf("Raw Received: [%s] -> Expression Changed!\n", cmd.c_str());
        }
    }

    delay(20);
}