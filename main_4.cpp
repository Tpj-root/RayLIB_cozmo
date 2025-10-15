#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raymath.h"
#include <algorithm>
using namespace std;

// --- Eye configuration ---
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
static const EyeConfig Preset_Neutral = {0, 0, 40, 50, 0, 0, 10, 10, 0, 0, 0, 0};

// --- EyeDrawer class ---
enum CornerType { T_R, T_L, B_L, B_R };

class EyeDrawer {
public:
    static void Draw(int centerX, int centerY, const EyeConfig &cfg, Color color) {
        float delta_y_top = cfg.Height * cfg.Slope_Top / 2.0f;
        float delta_y_bottom = cfg.Height * cfg.Slope_Bottom / 2.0f;
        float totalHeight = cfg.Height + delta_y_top - delta_y_bottom;

        float rTop = cfg.Radius_Top;
        float rBottom = cfg.Radius_Bottom;
        if (rTop + rBottom > totalHeight - 1) {
            float scale = (totalHeight - 1) / (rTop + rBottom);
            rTop *= scale;
            rBottom *= scale;
        }

        Vector2 TL = { centerX + cfg.OffsetX - cfg.Width / 2 + rTop,
                       centerY + cfg.OffsetY - cfg.Height / 2 + rTop - delta_y_top };
        Vector2 TR = { centerX + cfg.OffsetX + cfg.Width / 2 - rTop,
                       centerY + cfg.OffsetY - cfg.Height / 2 + rTop + delta_y_top };
        Vector2 BL = { centerX + cfg.OffsetX - cfg.Width / 2 + rBottom,
                       centerY + cfg.OffsetY + cfg.Height / 2 - rBottom - delta_y_bottom };
        Vector2 BR = { centerX + cfg.OffsetX + cfg.Width / 2 - rBottom,
                       centerY + cfg.OffsetY + cfg.Height / 2 - rBottom + delta_y_bottom };

        // Edges
        DrawLineV(TL, TR, color);
        DrawLineV(BL, BR, color);
        DrawLineV(TL, BL, color);
        DrawLineV(TR, BR, color);

// Rounded corners
// Wire
       if (rTop > 0) { 
           DrawCircleLines(TL.x, TL.y, rTop, color); 
           DrawCircleLines(TR.x, TR.y, rTop, color); }
       if (rBottom > 0) { 
           DrawCircleLines(BL.x, BL.y, rBottom, color); 
           DrawCircleLines(BR.x, BR.y, rBottom, color); }
// Wire
// Top slope triangle
       if (cfg.Slope_Top != 0) {
           Vector2 apex = {(TL.x + TR.x) / 2, TL.y - cfg.Slope_Top * cfg.Height};
           DrawTriangleLines(TL, TR, apex, color);
       }

       // Bottom slope triangle
       if (cfg.Slope_Bottom != 0) {
           Vector2 apex = {(BL.x + BR.x) / 2, BL.y + cfg.Slope_Bottom * cfg.Height};
           DrawTriangleLines(BL, BR, apex, color);

       }

        //     // Solid
        // if (rTop > 0) { 
        //     DrawCircle(TL.x, TL.y, rTop, color); 
        //     DrawCircle(TR.x, TR.y, rTop, color); 
        // }
        // if (rBottom > 0) { 
        //     DrawCircle(BL.x, BL.y, rBottom, color); 
        //     DrawCircle(BR.x, BR.y, rBottom, color); 
        // }

        //     // Solid

        // if (cfg.Slope_Top != 0) {
        //     Vector2 apex = {(TL.x + TR.x) / 2, TL.y - cfg.Slope_Top * cfg.Height};
        //     DrawTriangle(TL, TR, apex, color); // filled
        // }
        // if (cfg.Slope_Bottom != 0) {
        //     Vector2 apex = {(BL.x + BR.x) / 2, BL.y + cfg.Slope_Bottom * cfg.Height};
        //     DrawTriangle(BL, BR, apex, color); // filled
        // }
        


    }
};

// --- Main ---
int main() {
    InitWindow(1000, 600, "Eye Config Controller");
    SetTargetFPS(60);

    EyeConfig cfg = Preset_Neutral;
    Color eyeColor = SKYBLUE;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw eyes
        float centerX = GetScreenWidth() / 2.0f;
        float centerY = GetScreenHeight() / 2.0f;
        EyeDrawer::Draw(centerX - 75, centerY, cfg, eyeColor);
        EyeDrawer::Draw(centerX + 75, centerY, cfg, eyeColor);

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
