#include "raylib.h"

// --- Eye configuration (simplified for this example) ---
struct EyeConfig {
    float OffsetX;
    float OffsetY;
    float Height;
    float Width;
};

// Preset
static const EyeConfig Preset_Neutral = {0, 0, 40, 50};

// --- EyeDrawer class ---
class EyeDrawer {
public:
    static void Draw(int centerX, int centerY, const EyeConfig &cfg, Color color) {
        
        // Calculate the top-left corner of the rectangle
        int rectX = (int)(centerX + cfg.OffsetX - cfg.Width / 2);
        int rectY = (int)(centerY + cfg.OffsetY - cfg.Height / 2);
        
        int rectWidth = (int)cfg.Width;
        int rectHeight = (int)cfg.Height;
        
        // --- WIREFRAME RECTANGLE (DrawRectangleLines) ---
        // Draws the outline of the rectangle.
        DrawRectangleLines(rectX, rectY, rectWidth, rectHeight, color);
        
        // Note: DrawRectangle() would draw a solid filled rectangle.
        // DrawRectangleLines() draws the wireframe/outline.
    }
};

// --- Main ---
int main() {
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Simple Eye Rectangle Wireframe");
    SetTargetFPS(60);

    EyeConfig cfg = Preset_Neutral;
    Color eyeColor = SKYBLUE;
    
    // Main game loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw a single eye in the center of the screen
        float centerX = screenWidth / 2.0f;
        float centerY = screenHeight / 2.0f;
        
        EyeDrawer::Draw(centerX, centerY, cfg, eyeColor);

        DrawText("This is the basic rectangle wireframe.", 10, 10, 20, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}