#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
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
    bool Inverse_Radius_Top;
    bool Inverse_Radius_Bottom;
    bool Inverse_Offset_Top;
    bool Inverse_Offset_Bottom;
};

// Preset
static const EyeConfig Preset_Neutral = {
    0, 0, 40, 50,
    0.0, 0.0,
    10, 10,
    0, 0, 0, 0
};

// Draw eyes
void DrawEyes(const EyeConfig& cfg, Color color) {
    float centerX = GetScreenWidth() / 2.0f;
    float centerY = GetScreenHeight() / 2.0f;

    Vector2 leftEye = { centerX - (cfg.Width * 1.5f) - cfg.OffsetX, centerY + cfg.OffsetY };
    Vector2 rightEye = { centerX + (cfg.Width * 0.5f) + cfg.OffsetX, centerY + cfg.OffsetY };

    // Calculate corner radius considering inverse
    float radiusTopLeft = cfg.Inverse_Radius_Top ? cfg.Radius_Top * -1 : cfg.Radius_Top;
    float radiusTopRight = cfg.Inverse_Radius_Top ? cfg.Radius_Top * -1 : cfg.Radius_Top;
    float radiusBottomLeft = cfg.Inverse_Radius_Bottom ? cfg.Radius_Bottom * -1 : cfg.Radius_Bottom;
    float radiusBottomRight = cfg.Inverse_Radius_Bottom ? cfg.Radius_Bottom * -1 : cfg.Radius_Bottom;

    // Slope example: just move top/bottom edges
    float topOffset = cfg.Slope_Top * cfg.Height;
    float bottomOffset = cfg.Slope_Bottom * cfg.Height;

    // Left eye
    DrawRectangleRounded(
        Rectangle{ leftEye.x, leftEye.y + topOffset, cfg.Width, cfg.Height + bottomOffset - topOffset },
        0.6f, 10, color
    );

    // Right eye
    DrawRectangleRounded(
        Rectangle{ rightEye.x, rightEye.y + topOffset, cfg.Width, cfg.Height + bottomOffset - topOffset },
        0.6f, 10, color
    );
}


int main() {
    InitWindow(1000, 600, "Eye Config Controller");
    SetTargetFPS(60);

    EyeConfig cfg = Preset_Neutral;
    Color eyeColor = SKYBLUE;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        // --- Draw eyes ---
        DrawEyes(cfg, eyeColor);

        // --- GUI controls ---
        GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
        float panelX = 700, panelY = 30;

        DrawText("Eye Config Controls", panelX, 10, 20, RAYWHITE);

        GuiSliderBar({panelX, panelY, 250, 20}, "OffsetX", TextFormat("%.2f", cfg.OffsetX), &cfg.OffsetX, -50, 50); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "OffsetY", TextFormat("%.2f", cfg.OffsetY), &cfg.OffsetY, -50, 50); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "Width", TextFormat("%.2f", cfg.Width), &cfg.Width, 10, 100); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "Height", TextFormat("%.2f", cfg.Height), &cfg.Height, 10, 100); panelY += 30;

        GuiSliderBar({panelX, panelY, 250, 20}, "Slope_Top", TextFormat("%.2f", cfg.Slope_Top), &cfg.Slope_Top, -1.0f, 1.0f); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "Slope_Bottom", TextFormat("%.2f", cfg.Slope_Bottom), &cfg.Slope_Bottom, -1.0f, 1.0f); panelY += 30;

        GuiSliderBar({panelX, panelY, 250, 20}, "Radius_Top", TextFormat("%.2f", cfg.Radius_Top), &cfg.Radius_Top, 0, 50); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "Radius_Bottom", TextFormat("%.2f", cfg.Radius_Bottom), &cfg.Radius_Bottom, 0, 50); panelY += 30;

        GuiCheckBox({panelX, panelY, 20, 20}, "Inverse_Radius_Top", &cfg.Inverse_Radius_Top); panelY += 25;
        GuiCheckBox({panelX, panelY, 20, 20}, "Inverse_Radius_Bottom", &cfg.Inverse_Radius_Bottom); panelY += 25;
        GuiCheckBox({panelX, panelY, 20, 20}, "Inverse_Offset_Top", &cfg.Inverse_Offset_Top); panelY += 25;
        GuiCheckBox({panelX, panelY, 20, 20}, "Inverse_Offset_Bottom", &cfg.Inverse_Offset_Bottom); panelY += 25;

        DrawText("Use sliders and checkboxes to control eye shape", 10, 10, 20, GRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
