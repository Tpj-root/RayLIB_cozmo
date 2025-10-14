#include "raylib.h"
#include "raymath.h"

// Eye configuration structure
struct EyeConfig {
    float OffsetX;
    float OffsetY;
    float Height;
    float Width;
    float Slope_Top;
    float Slope_Bottom;
    float Radius_Top;
    float Radius_Bottom;
    int Inverse_Radius_Top;
    int Inverse_Radius_Bottom;
    int Inverse_Offset_Top;
    int Inverse_Offset_Bottom;
};

// Example presets
static const EyeConfig Preset_Neutral = {
    0, 0, 40, 50,
    0.0, 0.0,
    10, 10,
    0, 0, 0, 0
};

static const EyeConfig Preset_Awe = {
    2, 0, 35, 45,
    -0.1, 0.1,
    12, 12,
    0, 0, 0, 0
};

static const EyeConfig Preset_Happy = {
    0, -3, 35, 50,
    -0.2, 0.2,
    10, 8,
    0, 0, 0, 0
};

// Draw one pair of eyes using configuration
void DrawEyes(const EyeConfig& cfg, Color color) {
    float centerX = GetScreenWidth() / 2.0f;
    float centerY = GetScreenHeight() / 2.0f;

    // Eye positions
    Vector2 leftEye = { centerX - (cfg.Width * 1.5f) - cfg.OffsetX, centerY + cfg.OffsetY };
    Vector2 rightEye = { centerX + (cfg.Width * 0.5f) + cfg.OffsetX, centerY + cfg.OffsetY };

    // Draw eyes as rounded rectangles
    DrawRectangleRounded(
        Rectangle{ leftEye.x, leftEye.y, cfg.Width, cfg.Height },
        0.6f, 10, color
        );
    DrawRectangleRounded(
        Rectangle{ rightEye.x, rightEye.y, cfg.Width, cfg.Height },
        0.6f, 10, color
        );
}

int main() {
    InitWindow(800, 600, "Cozmo Face Preset Example");
    SetTargetFPS(60);

    EyeConfig current = Preset_Awe;
    Color eyeColor = SKYBLUE;

    while (!WindowShouldClose()) {
        // Press 1, 2, 3 to switch expressions
        if (IsKeyPressed(KEY_ONE)) current = Preset_Neutral;
        if (IsKeyPressed(KEY_TWO)) current = Preset_Happy;
        if (IsKeyPressed(KEY_THREE)) current = Preset_Awe;

        BeginDrawing();
        ClearBackground(BLACK);

        DrawEyes(current, eyeColor);

        DrawText("Press 1=Neutral, 2=Happy, 3=Awe", 10, 10, 20, GRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
