#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raymath.h"
#include <cmath>

// --- Configuration Struct for the Star ---
struct StarConfig {
    float CenterX;
    float CenterY;
    float OuterRadius;
    float InnerRadius;
    float RotationDeg;
    float R, G, B, A; // Color components
};

static const StarConfig Preset_Star = {400, 300, 100, 40, 0, 255, 200, 0, 255}; // Gold Star

// --- Polygon Drawer Class ---
class PolygonDrawer {
public:
    // PURE FUNCTION: Draws a 5-pointed star using triangulation and basic lines.
    static void DrawStar(const StarConfig &cfg) {
        
        Color starColor = {
            (unsigned char)cfg.R, (unsigned char)cfg.G, (unsigned char)cfg.B, (unsigned char)cfg.A
        };

        // 1. Define Vertices Array (10 vertices for the star points)
        Vector2 vertices[10];
        
        // 2. Calculate Vertex Coordinates (The core math remains the same)
        float angleStep = 360.0f / 10.0f; 
        float currentAngle = cfg.RotationDeg - 90.0f; // Start at the top point

        for (int i = 0; i < 10; i++) {
            float radius = (i % 2 == 0) ? cfg.OuterRadius : cfg.InnerRadius; // Alternating radii
            
            float angleRad = currentAngle * DEG2RAD;

            vertices[i].x = cfg.CenterX + radius * cosf(angleRad);
            vertices[i].y = cfg.CenterY + radius * sinf(angleRad);
            
            currentAngle += angleStep;
        }

        // --- 3. Draw the FILLED Star using Triangulation ---
        // A 5-pointed star is a non-convex polygon. The simplest way to fill it is
        // by drawing 5 large triangles, all originating from the center point.
        Vector2 centerPoint = {cfg.CenterX, cfg.CenterY};
        for (int i = 0; i < 10; i += 2) {
            // Each large triangle connects: Center -> Outer Point (i) -> Next Outer Point (i+2)
            // Note: We use modulo 10 to wrap back to the first point after the last one.
            
            Vector2 p_outer_1 = vertices[i];
            Vector2 p_outer_2 = vertices[(i + 2) % 10]; 

            DrawTriangle(centerPoint, p_outer_1, p_outer_2, starColor);
        }

        // --- 4. Draw the OUTLINE using basic lines ---
        // This replaces the non-working DrawPolyLinesEx
        float thickness = 3.0f;
        for (int i = 0; i < 10; i++) {
            // Draw a line from the current vertex (i) to the next vertex ((i + 1) % 10)
            DrawLineEx(vertices[i], vertices[(i + 1) % 10], thickness, BLACK);
        }
    }
};

// --- Main (Code remains the same) ---
int main() {
    InitWindow(800, 600, "Polygon Drawer: Star Shape");
    SetTargetFPS(60);

    StarConfig cfg = Preset_Star;

    while (!WindowShouldClose()) {
        
        // --- Input Update (GUI) ---
        GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
        float panelX = 550, panelY = 30;

        cfg.CenterX = GetScreenWidth() / 2.0f;
        cfg.CenterY = GetScreenHeight() / 2.0f;
        
        BeginDrawing();
        ClearBackground(DARKGRAY);

        DrawText("Star Config Controls", panelX, 10, 20, RAYWHITE);

        GuiSliderBar({panelX, panelY, 220, 20}, "Outer Radius", TextFormat("%.0f", cfg.OuterRadius), &cfg.OuterRadius, 50, 250); panelY += 30;
        // Clamp InnerRadius to be less than OuterRadius
        float maxInnerRadius = cfg.OuterRadius - 10.0f;
        GuiSliderBar({panelX, panelY, 220, 20}, "Inner Radius", TextFormat("%.0f", cfg.InnerRadius), &cfg.InnerRadius, 10, maxInnerRadius); 
        if (cfg.InnerRadius > maxInnerRadius) cfg.InnerRadius = maxInnerRadius; // Safety clamp
        panelY += 30;

        GuiSliderBar({panelX, panelY, 220, 20}, "Rotation", TextFormat("%.0f", cfg.RotationDeg), &cfg.RotationDeg, 0, 360); panelY += 30;
        
        DrawRectangle(panelX, panelY, 220, 2, GRAY); panelY += 10;

        GuiSliderBar({panelX, panelY, 220, 20}, "R", TextFormat("%.0f", cfg.R), &cfg.R, 0, 255); panelY += 30;
        GuiSliderBar({panelX, panelY, 220, 20}, "G", TextFormat("%.0f", cfg.G), &cfg.G, 0, 255); panelY += 30;
        GuiSliderBar({panelX, panelY, 220, 20}, "B", TextFormat("%.0f", cfg.B), &cfg.B, 0, 255); panelY += 30;
        GuiSliderBar({panelX, panelY, 220, 20}, "A", TextFormat("%.0f", cfg.A), &cfg.A, 0, 255); panelY += 30;
        
        // --- Drawing ---
        PolygonDrawer::DrawStar(cfg);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
