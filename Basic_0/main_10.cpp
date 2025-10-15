#include "raylib.h"
#include <math.h> // For fminf and fmaxf

void MyDrawSlopedRoundedRectangle(Rectangle rec, float radiusBottom, float radiusTop, float slopeFactor, int segments, Color color)
{
    // 1. Calculate and constrain radii
    radiusBottom = fmaxf(0.0f, radiusBottom);
    radiusTop = fmaxf(0.0f, radiusTop);

    // Ensure radii don't exceed half the side length
    float maxRadius = fminf(rec.width, rec.height) / 2.0f;
    radiusBottom = fminf(radiusBottom, maxRadius);
    radiusTop = fminf(radiusTop, maxRadius);

    // 2. Calculate coordinates and slope offset
    float actualSlopeShift = slopeFactor * rec.height;
    
    // Y-coordinate of the center of the top arcs
    float arcCenterY = rec.y + radiusTop;
    // Y-coordinate of the center of the bottom arcs
    float arcBottomCenterY = rec.y + rec.height - radiusBottom;

    // Center coordinates for the four arcs
    Vector2 centers[4] = {
        // Top-Left (TL)
        { rec.x + radiusTop, arcCenterY },
        // Top-Right (TR) - Center is relative to the sloped edge
        // The sloped edge starts at rec.x + rec.width - actualSlopeShift
        { rec.x + rec.width - actualSlopeShift - radiusTop, arcCenterY },
        // Bottom-Right (BR)
        { rec.x + rec.width - radiusBottom, arcBottomCenterY },
        // Bottom-Left (BL)
        { rec.x + radiusBottom, arcBottomCenterY }
    };
    
    // --- 3. Draw the main rectangular body (Full fill, then clip corners with arcs) ---

    // A. Vertical Center Section (Covers full width, but skips top and bottom arc heights)
    // This is the largest, most reliable fill.
    DrawRectangle(rec.x, (int)arcCenterY, (int)rec.width, (int)(rec.height - radiusTop - radiusBottom), color);

    // B. Top Sloped Trapezoid (Fills the area between the top arcs)
    // Since the top edge is sloped, this needs to be a polygon (trapezoid).
    Vector2 top_fill_points[4] = {
        // P1: Start of top straight line (after TL arc)
        { centers[0].x, rec.y },
        // P2: End of top straight line (before TR arc)
        { centers[1].x, rec.y }, 
        // P3: Bottom-right of this trapezoid (at the vertical center line)
        { centers[1].x, arcCenterY },
        // P4: Bottom-left of this trapezoid (at the vertical center line)
        { centers[0].x, arcCenterY }
    };
    DrawTriangleFan(top_fill_points, 4, color);

    // C. Bottom Horizontal Rectangle (Fills the area between the bottom arcs)
    DrawRectangle((int)centers[3].x, (int)arcBottomCenterY, (int)(centers[2].x - centers[3].x), (int)radiusBottom, color);

    // D. Left Vertical Strip (Fills the area between the left arcs)
    DrawRectangle(rec.x, (int)centers[0].y, (int)radiusTop, (int)(centers[3].y - centers[0].y), color);

    // E. Right Vertical Strip (Fills the area between the right arcs - Polygon due to slope)
    // The right edge is complex due to the slope. We use a polygon connecting:
    Vector2 right_strip_points[4] = {
        // P1: Top-left (Right edge, at TL arc center Y)
        { centers[1].x, arcCenterY },
        // P2: Top-right (Right edge, at TR arc end X, which is the sloped point)
        { centers[1].x + radiusTop, arcCenterY },
        // P3: Bottom-right (Corner of the overall bounding box, at BR arc center Y)
        { rec.x + rec.width, arcBottomCenterY },
        // P4: Bottom-left (At BR arc center X, where the main vertical strip starts)
        { centers[2].x, arcBottomCenterY }
    };
    DrawTriangleFan(right_strip_points, 4, color);

    // --- 4. Draw the Four Corner Arcs (Quarter Circles) ---
    // These must be drawn last to cover any minor seams or misalignments.
    
    // TL (180-270) - radiusTop
    DrawCircleSector(centers[0], radiusTop, 180.0f, 270.0f, segments, color);
    // TR (270-360) - radiusTop
    DrawCircleSector(centers[1], radiusTop, 270.0f, 360.0f, segments, color);
    // BR (0-90) - radiusBottom
    DrawCircleSector(centers[2], radiusBottom, 0.0f, 90.0f, segments, color);
    // BL (90-180) - radiusBottom
    DrawCircleSector(centers[3], radiusBottom, 90.0f, 180.0f, segments, color);
}

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Custom Sloped Rounded Rectangle");

    SetTargetFPS(60);

    float rectX = 150;
    float rectY = 100;
    float rectWidth = 500;
    float rectHeight = 250;
    float radiusBottom = 40.0f; // Example bottom radius
    float radiusTop = 30.0f;    // Example top radius
    float slopeFactor = 0.2f;   // Example slope factor (positive makes top-right shift left)
    int segments = 16;          // Arc segments

    while (!WindowShouldClose())
    {
        // Update values with mouse or keys if needed for testing
        if (IsKeyDown(KEY_LEFT)) rectX -= 1;
        if (IsKeyDown(KEY_RIGHT)) rectX += 1;
        if (IsKeyDown(KEY_UP)) rectY -= 1;
        if (IsKeyDown(KEY_DOWN)) rectY += 1;
        if (IsKeyDown(KEY_A)) rectWidth -= 1;
        if (IsKeyDown(KEY_S)) rectWidth += 1;
        if (IsKeyDown(KEY_Z)) rectHeight -= 1;
        if (IsKeyDown(KEY_X)) rectHeight += 1;

        if (IsKeyDown(KEY_Q)) radiusBottom -= 1;
        if (IsKeyDown(KEY_W)) radiusBottom += 1;
        if (IsKeyDown(KEY_E)) radiusTop -= 1;
        if (IsKeyDown(KEY_R)) radiusTop += 1;

        if (IsKeyDown(KEY_T)) slopeFactor -= 0.005f;
        if (IsKeyDown(KEY_Y)) slopeFactor += 0.005f;

        if (IsKeyDown(KEY_F)) segments = fmaxf(3, segments - 1);
        if (IsKeyDown(KEY_G)) segments = fminf(360, segments + 1);


        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Draw the custom shape
        MyDrawSlopedRoundedRectangle(
            (Rectangle){ rectX, rectY, rectWidth, rectHeight },
            radiusBottom,
            radiusTop,
            slopeFactor,
            segments,
            BLUE
        );

        // Optional: Draw bounding box and guides for reference
        DrawRectangleLinesEx((Rectangle){ rectX, rectY, rectWidth, rectHeight }, 1, LIGHTGRAY);
        DrawText(TextFormat("Radius Bottom: %.1f (Q/W)", radiusBottom), 10, 10, 20, BLACK);
        DrawText(TextFormat("Radius Top: %.1f (E/R)", radiusTop), 10, 40, 20, BLACK);
        DrawText(TextFormat("Slope Factor: %.2f (T/Y)", slopeFactor), 10, 70, 20, BLACK);
        DrawText(TextFormat("Segments: %d (F/G)", segments), 10, 100, 20, BLACK);


        EndDrawing();
    }

    CloseWindow();

    return 0;
}