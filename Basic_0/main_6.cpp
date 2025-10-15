#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raymath.h" // For Vector2 and geometric functions
#include <algorithm>
#include <cmath>
using namespace std;

// --- Combined Configuration Struct (from user code) ---
struct RectangleControl {
    float OffsetX;
    float OffsetY;
    float Width;
    float Height;
    float R; // Red component (0-255)
    float G; // Green component (0-255)
    float B; // Blue component (0-255)
    float A; // Alpha component (0-255)
    float Roundness; // New control for roundness factor (0.0 to 1.0)
    float LineThickness; // New control for outline thickness
};

// Preset
static const RectangleControl Preset_Initial = {0, 0, 150, 100, 102, 191, 255, 255, 0.4f, 2.0f};

// --- Custom Drawing Class to expose the internal math logic ---
class CustomRaylibDrawer {
public:
    // PURE FUNCTION: Replicates DrawRectangleRoundedLines logic using basic primitives.
    static void DrawRectangleRoundedLinesCustom(Rectangle rec, float roundness, int segments, float lineThick, Color color) {
        
        float maxRadius = (std::min(rec.width, rec.height) / 2.0f);
        float radius = maxRadius * roundness;

        if (radius <= 0.0f) {
            DrawRectangleLinesEx(rec, lineThick, color);
            return;
        }

        // --- 1. Define the Corner Arc Centers (4 points) ---
        Vector2 centerTL = {rec.x + radius, rec.y + radius};                         
        Vector2 centerTR = {rec.x + rec.width - radius, rec.y + radius};             
        Vector2 centerBL = {rec.x + radius, rec.y + rec.height - radius};            
        Vector2 centerBR = {rec.x + rec.width - radius, rec.y + rec.height - radius};

        // --- 2. Define Tangent Points (Start/End of Straight Lines) ---
        Vector2 p1_top_start = {centerTR.x, centerTR.y - radius}; 
        Vector2 p2_top_end = {centerTL.x, centerTL.y - radius};   
        Vector2 p3_right_start = {centerTR.x + radius, centerTR.y};
        Vector2 p4_right_end = {centerBR.x + radius, centerBR.y};  
        Vector2 p5_bottom_start = {centerBR.x, centerBR.y + radius};
        Vector2 p6_bottom_end = {centerBL.x, centerBL.y + radius}; 
        Vector2 p7_left_start = {centerBL.x - radius, centerBL.y}; 
        Vector2 p8_left_end = {centerTL.x - radius, centerTL.y};   

        // --- 3. Draw the Four Straight Lines (Tangent Segments) ---
        DrawLineEx(p1_top_start, p2_top_end, lineThick, color);      
        DrawLineEx(p3_right_start, p4_right_end, lineThick, color);  
        DrawLineEx(p5_bottom_start, p6_bottom_end, lineThick, color);
        DrawLineEx(p7_left_start, p8_left_end, lineThick, color);    

        // --- 4. Draw the Four Corner Arcs (Quarter Circles) ---
        DrawCircleSectorLines(centerTL, radius, 180, 270, segments, color); // Top-Left
        DrawCircleSectorLines(centerTR, radius, 270, 360, segments, color); // Top-Right
        DrawCircleSectorLines(centerBR, radius, 0, 90, segments, color);    // Bottom-Right
        DrawCircleSectorLines(centerBL, radius, 90, 180, segments, color);  // Bottom-Left
    }
};

// --- Rectangle Drawer Class (FIXED) ---
class RectangleDrawer {
public:
    static void Draw(int centerX, int centerY, const RectangleControl &ctrl) {
        
        Color color = {
            (unsigned char)ctrl.R, (unsigned char)ctrl.G, (unsigned char)ctrl.B, (unsigned char)ctrl.A
        };
        
        Rectangle rec = {
            (float)(centerX + ctrl.OffsetX - ctrl.Width / 2),
            (float)(centerY + ctrl.OffsetY - ctrl.Height / 2),
            ctrl.Width,
            ctrl.Height
        };
        
        // Draw the outline using the custom function
        CustomRaylibDrawer::DrawRectangleRoundedLinesCustom(
            rec,
            ctrl.Roundness,
            16,
            ctrl.LineThickness,
            color
        );
        
        // --- FIX: Recalculate the center points for debug visualization ---
        float maxRadius = (std::min(rec.width, rec.height) / 2.0f);
        float radius = maxRadius * ctrl.Roundness;
        
        // Only draw debug points if there is a radius
        if (radius > 0.0f) {
             Vector2 centerTL = {rec.x + radius, rec.y + radius};                        
             Vector2 centerTR = {rec.x + rec.width - radius, rec.y + radius};            
             Vector2 centerBL = {rec.x + radius, rec.y + rec.height - radius};           
             Vector2 centerBR = {rec.x + rec.width - radius, rec.y + rec.height - radius};

             // Debug visualization (these lines caused the error)
             DrawCircleV(centerTL, 3, GREEN);
             DrawCircleV(centerTR, 3, GREEN);
             DrawCircleV(centerBL, 3, GREEN);
             DrawCircleV(centerBR, 3, GREEN);
        }

        // Label dimensions
        DrawText(TextFormat("W: %.0f H: %.0f", ctrl.Width, ctrl.Height),
                 (int)(rec.x + rec.width + 10),
                 (int)(rec.y + rec.height / 2 - 10),
                 16, RAYWHITE);
    }
};

// --- Main (Unchanged) ---
int main() {
    InitWindow(1000, 600, "Rectangle Controller (Custom Rounded Outline)");
    SetTargetFPS(60);

    RectangleControl ctrl = Preset_Initial;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGRAY);

        float centerX = GetScreenWidth() / 2.0f;
        float centerY = GetScreenHeight() / 2.0f;
        
        RectangleDrawer::Draw(centerX, centerY, ctrl);

        // --- GUI controls ---
        GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
        float panelX = 700, panelY = 30;

        DrawText("Rectangle Config Controls", panelX, 10, 20, RAYWHITE);
        
        GuiSliderBar({panelX, panelY, 250, 20}, "OffsetX", TextFormat("%.0f", ctrl.OffsetX), &ctrl.OffsetX, -100, 100); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "OffsetY", TextFormat("%.0f", ctrl.OffsetY), &ctrl.OffsetY, -100, 100); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "Width", TextFormat("%.0f", ctrl.Width), &ctrl.Width, 20, 300); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "Height", TextFormat("%.0f", ctrl.Height), &ctrl.Height, 20, 300); panelY += 30;
        
        GuiSliderBar({panelX, panelY, 250, 20}, "Roundness (0-1)", TextFormat("%.2f", ctrl.Roundness), &ctrl.Roundness, 0.0f, 1.0f); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "Thickness", TextFormat("%.1f", ctrl.LineThickness), &ctrl.LineThickness, 1.0f, 10.0f); panelY += 30;

        DrawRectangle(panelX, panelY, 250, 2, GRAY); panelY += 10;
        DrawText("Color Controls (0-255)", panelX, panelY, 16, RAYWHITE); panelY += 20;

        GuiSliderBar({panelX, panelY, 250, 20}, "R (Red)", TextFormat("%.0f", ctrl.R), &ctrl.R, 0, 255); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "G (Green)", TextFormat("%.0f", ctrl.G), &ctrl.G, 0, 255); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "B (Blue)", TextFormat("%.0f", ctrl.B), &ctrl.B, 0, 255); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "A (Alpha)", TextFormat("%.0f", ctrl.A), &ctrl.A, 0, 255); panelY += 30;

        EndDrawing();
    }

    CloseWindow();
    return 0;
}