#include "raylib.h"
#include <math.h>

// --- Function Prototypes ---
void MyDrawSlopedRoundedRectangle(Rectangle rec, float radiusBottom, float radiusTop, float slopeFactor, int segments, Color color);

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
#include "raylib.h"
#include <math.h>

// MyDrawSlopedRoundedRectangle: Draws the filled shape with a sloped top edge and rounded corners.
void MyDrawSlopedRoundedRectangle(Rectangle rec, float radiusBottom, float radiusTop, float slopeFactor, int segments, Color color)
{
    // 1. Calculate and constrain radii
    radiusBottom = fmaxf(0.0f, radiusBottom);
    radiusTop = fmaxf(0.0f, radiusTop);

    float maxRadius = fminf(rec.width, rec.height) / 2.0f;
    radiusBottom = fminf(radiusBottom, maxRadius);
    radiusTop = fminf(radiusTop, maxRadius);

    // 2. Calculate coordinates and slope offset
    float actualSlopeShift = slopeFactor * rec.height; // Horizontal shift
    
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
    
    // Calculate the horizontal distance between the top arc centers for overlap check
    float top_straight_width = centers[1].x - centers[0].x;

    // --- 3. Draw the main rectangular body and polygon fills (Overlap strategy) ---

    // A. Central Fill (Covers the full width between the vertical straight segments)
    // Draw from the top of the TL/TR centers down to the bottom of the BL/BR centers.
    DrawRectangle(rec.x, (int)y_top_straight, (int)rec.width, (int)(y_bottom_straight - y_top_straight), color);

    // B. Bottom Horizontal Fill (Fills the area between the bottom arcs)
    // Draw the rectangle spanning the straight section of the bottom edge.
    DrawRectangle((int)centers[3].x, (int)y_bottom_straight, (int)(centers[2].x - centers[3].x), (int)radiusBottom, color);

    // C. Top Fill: Conditional Trapezoid or Simple Rectangle
    if (top_straight_width > 0.0f)
    {
        // Draw the trapezoid (when the arcs do not overlap)
        Vector2 top_fill_points[4] = {
            // P1: Start of top straight line (after TL arc)
            { centers[0].x, rec.y },
            // P2: End of top straight line (before TR arc)
            { centers[1].x, rec.y }, 
            // P3: Bottom-right of this trapezoid (at the vertical center line)
            { centers[1].x, y_top_straight },
            // P4: Bottom-left of this trapezoid (at the vertical center line)
            { centers[0].x, y_top_straight }
        };
        DrawTriangleFan(top_fill_points, 4, color);
    }
    else
    {
        // Fallback: If arcs overlap/meet (like when radius is very large), draw a simple rectangle
        DrawRectangle(rec.x, (int)rec.y, (int)rec.width, (int)radiusTop, color);
    }

    // D. Left Vertical Strip (Fills the area between the left arcs)
    DrawRectangle(rec.x, (int)centers[0].y, (int)radiusTop, (int)(centers[3].y - centers[0].y), color);

    // E. Right Vertical Fill (Polygon due to slope and potential extension beyond rec.width)
    // Fills the area between the right side arcs.
    Vector2 right_strip_points[4] = {
        // P1: Top-Left (Horizontal end of TR arc)
        { centers[1].x + radiusTop, y_top_straight },
        // P2: Top-Right (The sloped corner point X, at y_top_straight)
        { x_tr_corner_sloped, y_top_straight }, 
        // P3: Bottom-Right (Standard corner X, at bottom straight Y)
        { rec.x + rec.width, y_bottom_straight },
        // P4: Bottom-Left (Horizontal start of BR arc)
        { centers[2].x + radiusBottom, y_bottom_straight }
    };
    DrawTriangleFan(right_strip_points, 4, color);


    // --- 4. Draw the Four Corner Arcs (Quarter Circles) ---
    // These must be drawn last to cover any seams and complete the rounding.
    
    // TL (180-270)
    DrawCircleSector(centers[0], radiusTop, 180.0f, 270.0f, segments, color);
    // TR (270-360)
    DrawCircleSector(centers[1], radiusTop, 270.0f, 360.0f, segments, color);
    // BR (0-90)
    DrawCircleSector(centers[2], radiusBottom, 0.0f, 90.0f, segments, color);
    // BL (90-180)
    DrawCircleSector(centers[3], radiusBottom, 90.0f, 180.0f, segments, color);
}