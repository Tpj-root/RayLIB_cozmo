#include "raylib.h"
#include <algorithm>
#include <cmath>
using namespace std;

// --- Custom rounded rectangle drawer (FIXED) ---
void MyDrawRectangleRounded(Rectangle rec, float roundness, int segments, Color color)
{
    // Ensure float math is used for min
    float minSide = fminf(rec.width, rec.height);

    // 1. Calculate the Corner Radius (Radius must be 0 if roundness is 0)
    // Radius is constrained by 50% of the shorter side, then scaled by roundness [0, 1].
    float radius = (roundness * minSide) / 2.0f; 

    // Handle the case where no roundness is applied
    if (radius <= 0.0f) {
        DrawRectangleRec(rec, color);
        return;
    }

    // --- 2. Draw the Three Rectangular Fill Sections ---

    // A. Vertical Center Section (Full Height - 2 * radius, Full Width)
    // This connects the left and right rounded ends.
    DrawRectangle(rec.x, rec.y + radius, (int)rec.width, (int)(rec.height - 2 * radius), color);

    // B. Top Horizontal Section (Width - 2 * radius, Height = radius)
    // This fills the area between the top-left and top-right arcs.
    DrawRectangle(rec.x + radius, rec.y, (int)(rec.width - 2 * radius), (int)radius, color);

    // C. Bottom Horizontal Section (Width - 2 * radius, Height = radius)
    // This fills the area between the bottom-left and bottom-right arcs.
    DrawRectangle(rec.x + radius, rec.y + rec.height - radius, (int)(rec.width - 2 * radius), (int)radius, color);


    // --- 3. Draw the Four Corner Arcs (Quarter Circles) ---
    Vector2 centers[4] = {
        // Top-Left (TL)
        { rec.x + radius, rec.y + radius },
        // Top-Right (TR)
        { rec.x + rec.width - radius, rec.y + radius },
        // Bottom-Right (BR)
        { rec.x + rec.width - radius, rec.y + rec.height - radius },
        // Bottom-Left (BL)
        { rec.x + radius, rec.y + rec.height - radius }
    };

    // Angles start at 0 (right) and go counter-clockwise: 
    // TR (270-360), BR (0-90), BL (90-180), TL (180-270)
    for (int i = 0; i < 4; i++) {
        float startAngle = 90.0f * i;
        float endAngle = startAngle + 90.0f;
        
        // We need to shift the angles to match the corner sequence defined in centers[]
        // The standard sequence is: TL, TR, BR, BL
        if (i == 0) { startAngle = 180; endAngle = 270; } // TL
        if (i == 1) { startAngle = 270; endAngle = 360; } // TR
        if (i == 2) { startAngle = 0; endAngle = 90; }    // BR
        if (i == 3) { startAngle = 90; endAngle = 180; }  // BL
        
        DrawCircleSector(centers[i], radius, startAngle, endAngle, segments, color);
    }
}

// --- Main Program Setup for Testing ---

// Preset
static float Roundness = 0.5f;

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Fixed Custom Rounded Rectangle");
    SetTargetFPS(60);

    Rectangle testRec = { screenWidth/4.0f, screenHeight/4.0f, screenWidth/2.0f, screenHeight/2.0f };

    while (!WindowShouldClose()) {
        
        // Control roundness with arrow keys
        if (IsKeyDown(KEY_RIGHT)) Roundness += 0.01f;
        if (IsKeyDown(KEY_LEFT)) Roundness -= 0.01f;
        Roundness = fmaxf(0.0f, fminf(1.0f, Roundness)); // Clamp between 0.0 and 1.0

        BeginDrawing();
        ClearBackground(DARKGRAY);

        // Draw the fixed custom rounded rectangle
        MyDrawRectangleRounded(testRec, Roundness, 30, SKYBLUE);
        
        DrawText("My Custom Rounded Rectangle (FIXED)", 10, 10, 20, RAYWHITE);
        DrawText(TextFormat("Roundness: %.2f (use LEFT/RIGHT arrows)", Roundness), 10, screenHeight - 30, 20, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
