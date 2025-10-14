#include "raylib.h"
#include "raymath.h"
#include <cmath>

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

static const EyeConfig Preset_Awe = {
    2, 0, 35, 45,
    -0.1, 0.1,
    12, 12,
    0, 0, 0, 0
};

// Draw a single eye using geometry
void DrawEye(Vector2 center, const EyeConfig& cfg, Color color) {
    float w = cfg.Width;
    float h = cfg.Height;
    float slopeTop = cfg.Slope_Top * h;
    float slopeBottom = cfg.Slope_Bottom * h;

    // Compute polygon points
    Vector2 p1 = { center.x - w/2, center.y - h/2 + slopeTop };   // top-left
    Vector2 p2 = { center.x + w/2, center.y - h/2 };              // top-right
    Vector2 p3 = { center.x + w/2, center.y + h/2 + slopeBottom };// bottom-right
    Vector2 p4 = { center.x - w/2, center.y + h/2 };              // bottom-left

    // Draw main polygon
    DrawTriangle(p1, p2, p3, color);
    DrawTriangle(p1, p3, p4, color);

    // Rounded corners (top & bottom)
    DrawCircleV(p1, cfg.Radius_Top, color);
    DrawCircleV(p2, cfg.Radius_Top, color);
    DrawCircleV(p3, cfg.Radius_Bottom, color);
    DrawCircleV(p4, cfg.Radius_Bottom, color);
}

// Draw both eyes
void DrawEyes(const EyeConfig& cfg, Color color) {
    float cx = GetScreenWidth() / 2.0f;
    float cy = GetScreenHeight() / 2.0f + cfg.OffsetY;

    float spacing = cfg.Width * 1.5f;

    Vector2 leftEye = { cx - spacing/2 - cfg.OffsetX, cy };
    Vector2 rightEye = { cx + spacing/2 + cfg.OffsetX, cy };

    DrawEye(leftEye, cfg, color);
    DrawEye(rightEye, cfg, color);
}

int main() {
    InitWindow(800, 600, "Cozmo Eye Geometry Demo");
    SetTargetFPS(60);

    EyeConfig current = Preset_Awe;
    Color eyeColor = SKYBLUE;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawEyes(current, eyeColor);
        DrawText("Eye shape demo - uses slope, radius, width, height", 10, 10, 20, GRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
