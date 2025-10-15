#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <algorithm>
using namespace std;

// --- Combined Configuration Struct ---
struct RectangleControl {
    float OffsetX;
    float OffsetY;
    float Width;
    float Height;
    float R; // Red component (0-255)
    float G; // Green component (0-255)
    float B; // Blue component (0-255)
    float A; // Alpha component (0-255)
};

// Preset
// Starts with: Offset(0, 0), Size(50, 40), Color (SKYBLUE: 102, 191, 255, 255)
static const RectangleControl Preset_Initial = {0, 0, 50, 40, 102, 191, 255, 255};

// --- Rectangle Drawer Class ---
class RectangleDrawer {
public:
    static void Draw(int centerX, int centerY, const RectangleControl &ctrl) {
        Color color = {
            (unsigned char)ctrl.R,
            (unsigned char)ctrl.G,
            (unsigned char)ctrl.B,
            (unsigned char)ctrl.A
        };
    
        int rectX = (int)(centerX + ctrl.OffsetX - ctrl.Width / 2);
        int rectY = (int)(centerY + ctrl.OffsetY - ctrl.Height / 2);
        int rectWidth = (int)ctrl.Width;
        int rectHeight = (int)ctrl.Height;
    
        float radius = min(rectWidth, rectHeight) * 0.25f;  // corner roundness (25%)
    
        // --- Draw filled rounded rectangle (oblong shape) ---
        DrawRectangleRounded(
            { (float)rectX, (float)rectY, (float)rectWidth, (float)rectHeight },
            radius / (float)rectWidth * 2.0f,   // roundness factor (0.0–1.0)
            16,                                 // segments (smoothness)
            color
        );
    
        // Outline
        DrawRectangleRoundedLines(
            { (float)rectX, (float)rectY, (float)rectWidth, (float)rectHeight },
            radius / (float)rectWidth * 2.0f,
            16,
            WHITE
        );
    
        // Label dimensions
        DrawText(TextFormat("W: %.0f  H: %.0f", ctrl.Width, ctrl.Height),
                 rectX + rectWidth + 5,
                 rectY + rectHeight / 2 - 10,
                 16, RAYWHITE);
    }

};

// --- Main ---
int main() {
    InitWindow(1000, 600, "Rectangle Controller");
    SetTargetFPS(60);

    RectangleControl ctrl = Preset_Initial;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        // Calculate center for drawing
        float centerX = GetScreenWidth() / 2.0f;
        float centerY = GetScreenHeight() / 2.0f;
        
        // Draw the controlled rectangle
        RectangleDrawer::Draw(centerX, centerY, ctrl);

        // --- GUI controls ---
        GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
        float panelX = 700, panelY = 30;

        DrawText("Rectangle Config Controls", panelX, 10, 20, RAYWHITE);
        
        // 1. Position and Size Controls (4 bars)
        GuiSliderBar({panelX, panelY, 250, 20}, "OffsetX", TextFormat("%.0f", ctrl.OffsetX), &ctrl.OffsetX, -100, 100); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "OffsetY", TextFormat("%.0f", ctrl.OffsetY), &ctrl.OffsetY, -100, 100); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "Width", TextFormat("%.0f", ctrl.Width), &ctrl.Width, 10, 300); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "Height", TextFormat("%.0f", ctrl.Height), &ctrl.Height, 10, 300); panelY += 30;
        
        // Separator
        DrawRectangle(panelX, panelY, 250, 2, GRAY); panelY += 10;
        DrawText("Color Controls (0-255)", panelX, panelY, 16, RAYWHITE); panelY += 20;

        // 2. Color Controls (4 bars)
        GuiSliderBar({panelX, panelY, 250, 20}, "R (Red)", TextFormat("%.0f", ctrl.R), &ctrl.R, 0, 255); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "G (Green)", TextFormat("%.0f", ctrl.G), &ctrl.G, 0, 255); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "B (Blue)", TextFormat("%.0f", ctrl.B), &ctrl.B, 0, 255); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "A (Alpha)", TextFormat("%.0f", ctrl.A), &ctrl.A, 0, 255); panelY += 30;

        EndDrawing();
    }

    CloseWindow();
    return 0;
}