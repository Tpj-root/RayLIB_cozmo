#include "raylib.h"
#include <math.h>

// --- Function Prototypes ---
void MyDrawSlopedRoundedRectangle(Rectangle rec, float radiusBottom, float radiusTop, float slopeFactor, int segments, Color color);
void MyDrawSlopedRoundedRectangleWires(Rectangle rec, float radiusBottom, float radiusTop, float slopeFactor, int segments, Color color, float thickness);

// --- MAIN FUNCTION ---
int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Raylib Custom Sloped Rounded Shape");

    SetTargetFPS(60);

    // Initial parameters (can be changed with keyboard)
    Rectangle shapeRec = { 150, 100, 500, 250 };
    float radiusBottom = 40.0f;
    float radiusTop = 30.0f;
    float slopeFactor = 0.2f;
    int segments = 16;

    while (!WindowShouldClose())
    {
        // --- Input Handling for parameters ---
        if (IsKeyDown(KEY_Q)) radiusBottom = fmaxf(0, radiusBottom - 1);
        if (IsKeyDown(KEY_W)) radiusBottom += 1;
        if (IsKeyDown(KEY_E)) radiusTop = fmaxf(0, radiusTop - 1);
        if (IsKeyDown(KEY_R)) radiusTop += 1;

        if (IsKeyDown(KEY_T)) slopeFactor -= 0.005f;
        if (IsKeyDown(KEY_Y)) slopeFactor += 0.005f;

        if (IsKeyDown(KEY_F)) segments = fmaxf(3, segments - 1);
        if (IsKeyDown(KEY_G)) segments = fminf(360, segments + 1);

        BeginDrawing();

        ClearBackground(RAYWHITE);

        // 1. Draw the Filled Shape
        MyDrawSlopedRoundedRectangle(
            shapeRec,
            radiusBottom,
            radiusTop,
            slopeFactor,
            segments,
            (Color){ 0, 121, 241, 200 } // Semi-transparent Blue fill
        );

        // 2. Draw the Wireframe (Outline)
        MyDrawSlopedRoundedRectangleWires(
            shapeRec,
            radiusBottom,
            radiusTop,
            slopeFactor,
            segments,
            BLACK, // Black outline
            2.0f   // 2-pixel thickness
        );
        
        // Optional: Draw bounding box and guides
        DrawRectangleLinesEx(shapeRec, 1, LIGHTGRAY);
        DrawText(TextFormat("Radius Bottom: %.1f (Q/W)", radiusBottom), 10, 10, 20, BLACK);
        DrawText(TextFormat("Radius Top: %.1f (E/R)", radiusTop), 10, 40, 20, BLACK);
        DrawText(TextFormat("Slope Factor: %.2f (T/Y)", slopeFactor), 10, 70, 20, BLACK);
        DrawText(TextFormat("Segments: %d (F/G)", segments), 10, 100, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

// ----------------------------------------------------------------------
// --- DRAWING IMPLEMENTATIONS ---
// ----------------------------------------------------------------------

// Function to draw the filled shape (The final, robust version)
void MyDrawSlopedRoundedRectangle(Rectangle rec, float radiusBottom, float radiusTop, float slopeFactor, int segments, Color color)
{
    // ... (Implementation from the previous answer - omitted for brevity, assume it's here) ...
    // Note: Use the full, robust implementation provided in the last response.
    // Due to the complexity of this function, it's best to keep the full implementation separate.
}

// --- NEW WIREFRAME DRAWING FUNCTION ---
void MyDrawSlopedRoundedRectangleWires(Rectangle rec, float radiusBottom, float radiusTop, float slopeFactor, int segments, Color color, float thickness)
{
    // 1. Calculate parameters and centers (Same as the fill function)
    radiusBottom = fmaxf(0.0f, radiusBottom);
    radiusTop = fmaxf(0.0f, radiusTop);

    float maxRadius = fminf(rec.width, rec.height) / 2.0f;
    radiusBottom = fminf(radiusBottom, maxRadius);
    radiusTop = fminf(radiusTop, maxRadius);

    float actualSlopeShift = slopeFactor * rec.height; 
    
    float y_top_straight = rec.y + radiusTop;
    float y_bottom_straight = rec.y + rec.height - radiusBottom;

    float x_tr_corner_sloped = rec.x + rec.width - actualSlopeShift;
    
    // Center coordinates for the four arcs
    Vector2 centers[4] = {
        // Top-Left (TL)
        { rec.x + radiusTop, y_top_straight },
        // Top-Right (TR) - Adjusted for slope
        { x_tr_corner_sloped - radiusTop, y_top_straight },
        // Bottom-Right (BR)
        { rec.x + rec.width - radiusBottom, y_bottom_straight },
        // Bottom-Left (BL)
        { rec.x + radiusBottom, y_bottom_straight }
    };

    // 2. Define the 8 corner points of the straight-line segments
    // These points connect the arcs.
    Vector2 p[8] = {
        // P0: Top-Left Vertical Start (End of BL arc)
        { centers[3].x - radiusBottom, centers[3].y },
        // P1: Top-Left Horizontal Start (Start of TL arc)
        { centers[0].x - radiusTop, centers[0].y },
        // P2: Top-Left Horizontal End (End of TL arc)
        { centers[0].x, centers[0].y - radiusTop },
        // P3: Top-Right Horizontal Start (Start of TR arc)
        { centers[1].x, centers[1].y - radiusTop },
        // P4: Top-Right Vertical End (End of TR arc)
        { centers[1].x + radiusTop, centers[1].y },
        // P5: Bottom-Right Vertical Start (Start of BR arc)
        { centers[2].x + radiusBottom, centers[2].y },
        // P6: Bottom-Right Horizontal End (End of BR arc)
        { centers[2].x, centers[2].y + radiusBottom },
        // P7: Bottom-Left Horizontal Start (Start of BL arc)
        { centers[3].x, centers[3].y + radiusBottom }
    };
    
    // 3. Draw the 4 straight-line segments (Point to Point Draw Line)

    // Left Vertical: P0 -> P1
    DrawLineEx(p[0], p[1], thickness, color);
    
    // Top Sloped: P2 -> P3
    // THIS IS THE SLOPED LINE
    DrawLineEx(p[2], p[3], thickness, color);
    
    // Right Vertical (This line is slightly tricky as it involves P4 and P5):
    // P4 is the horizontal end of the TR arc. P5 is the vertical start of the BR arc.
    // We draw the line from P4's x-coordinate to P5's x-coordinate, at their respective y-coordinates.
    // The visual right edge is between the corner points of the rectangle's bounding box that fall on the right.
    // A simpler way: Draw a line from the top-right corner of the inner straight section to the bottom-right corner.
    Vector2 right_top = { centers[1].x + radiusTop, centers[1].y };
    Vector2 right_bottom = { centers[2].x + radiusBottom, centers[2].y };
    DrawLineEx(right_top, right_bottom, thickness, color);


    // Bottom Horizontal: P7 -> P6
    DrawLineEx(p[7], p[6], thickness, color);


    // 4. Draw the 4 corner arcs (Quarter Circles)
    
    // TL (180-270)
    DrawCircleSectorLines(centers[0], radiusTop, 180.0f, 270.0f, segments, color);
    // TR (270-360)
    DrawCircleSectorLines(centers[1], radiusTop, 270.0f, 360.0f, segments, color);
    // BR (0-90)
    DrawCircleSectorLines(centers[2], radiusBottom, 0.0f, 90.0f, segments, color);
    // BL (90-180)
    DrawCircleSectorLines(centers[3], radiusBottom, 90.0f, 180.0f, segments, color);
}