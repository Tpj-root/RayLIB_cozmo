#include "raylib.h"
#include <math.h> // For fminf

// MyDrawSlopedRoundedRectangle: Draws a custom shape with a sloped top edge and rounded corners
// rec: The bounding box for the general shape (x, y, width, height)
// radiusBottom: Radius for the bottom-left and bottom-right corners
// radiusTop: Radius for the top-left and top-right corners (these will interact with the slope)
// slopeFactor: A value that determines the horizontal shift of the top-right corner relative to the top-left.
//              0 means no slope (top-left and top-right are vertically aligned).
//              Positive values shift the top-right corner to the left relative to rec.x + rec.width.
//              Negative values shift the top-right corner to the right relative to rec.x + rec.width.
//              The image implies a positive slope (top-right is "inward" from the right edge of the main rectangle).
// segments: Number of segments to use for drawing the arcs (more segments = smoother curve)
// color: Color of the shape
void MyDrawSlopedRoundedRectangle(Rectangle rec, float radiusBottom, float radiusTop, float slopeFactor, int segments, Color color)
{
    // Ensure radii are not negative
    radiusBottom = fmaxf(0.0f, radiusBottom);
    radiusTop = fmaxf(0.0f, radiusTop);

    // Limit radii to half of the respective side to prevent self-intersection
    radiusBottom = fminf(radiusBottom, rec.height / 2.0f);
    radiusTop = fminf(radiusTop, rec.height / 2.0f);
    radiusBottom = fminf(radiusBottom, rec.width / 2.0f);
    radiusTop = fminf(radiusTop, rec.width / 2.0f);

    // If both radii are zero, draw a simple rectangle or a sloped rectangle (if slopeFactor is used).
    if (radiusBottom <= 0.0f && radiusTop <= 0.0f && fabsf(slopeFactor) < 0.001f) {
        DrawRectangleRec(rec, color);
        return;
    }

    // Adjust slopeFactor relative to height as in the image "Slope x Height"
    float actualSlopeShift = slopeFactor * rec.height;

    // Define main points of the shape
    Vector2 pTL = { rec.x, rec.y };
    Vector2 pTR = { rec.x + rec.width, rec.y };
    Vector2 pBL = { rec.x, rec.y + rec.height };
    Vector2 pBR = { rec.x + rec.width, rec.y + rec.height };

    // Apply the slope to the top-right corner's X coordinate
    pTR.x += actualSlopeShift;
    // (Note: In your image, the slope is relative to the right edge of the *main* rectangle's bounding box.
    // So, pTR.x moves *inward* from rec.x + rec.width for a positive slope as depicted.)
    // If you want it to behave strictly like "Slope x Height" is a *horizontal offset from the right edge*,
    // then pTR.x = rec.x + rec.width - actualSlopeShift;
    // For the image's depiction, it seems the slope shifts the actual endpoint from the standard top-right.
    // Let's assume actualSlopeShift is directly added/subtracted from the default top-right x.
    // The image shows the top-right corner shifted *left*, so a positive slopeFactor should mean a
    // negative shift to pTR.x, or pTR.x = (rec.x + rec.width) - actualSlopeShift.
    // I'll implement it as: `pTR.x -= actualSlopeShift;` if slopeFactor is positive.
    // If you want `slopeFactor` to be directly what is shown as `Slope x Height` in the image,
    // then the top-right x coordinate is `rec.x + rec.width - (slopeFactor * rec.height)`
    // Let's adjust `pTR.x` based on the image's visual representation:
    pTR.x = rec.x + rec.width - actualSlopeShift; // This makes the top-right corner move left for positive slopeFactor.

    // Calculate effective centers for the bottom arcs
    Vector2 centerBL_arc = { pBL.x + radiusBottom, pBL.y - radiusBottom };
    Vector2 centerBR_arc = { pBR.x - radiusBottom, pBR.y - radiusBottom };

    // Draw the main body as a polygon first to handle the slope
    // The main polygon will be:
    // top-left corner without radius, top-right corner without radius (adjusted for slope),
    // bottom-right arc start, bottom-left arc end.

    // A. Main central rectangle (vertical strip)
    // This is the easiest part. A rectangle from x to x+width, and from y + radiusTop to y + height - radiusBottom
    DrawRectangle(rec.x, rec.y + radiusTop, (int)rec.width, (int)(rec.height - radiusTop - radiusBottom), color);

    // B. Bottom horizontal rectangle (between bottom arcs)
    DrawRectangle(rec.x + radiusBottom, (int)(rec.y + rec.height - radiusBottom), (int)(rec.width - 2 * radiusBottom), (int)radiusBottom, color);


    // C. Draw the bottom-left arc (quarter circle)
    DrawCircleSector(centerBL_arc, radiusBottom, 90.0f, 180.0f, segments, color);
    // D. Draw the bottom-right arc (quarter circle)
    DrawCircleSector(centerBR_arc, radiusBottom, 0.0f, 90.0f, segments, color);


    // E. Draw the top section, which involves the slope and top radii.
    // This is more complex because the top edges are not straight horizontal.

    // First, draw a trapezoidal or polygonal fill for the top section between the top arcs
    // We need the start and end points of the straight parts of the top-left and top-right edges after rounding.
    // Top-left arc starts at (rec.x, rec.y + radiusTop) and ends at (rec.x + radiusTop, rec.y)
    // Top-right arc starts at (pTR.x - radiusTop, pTR.y) and ends at (pTR.x, pTR.y + radiusTop)

    // A simpler way for the top section is to fill the area between the top-left and top-right arcs
    // and the center rectangle's top.
    // We can draw a polygon that connects:
    // (rec.x + radiusTop, rec.y) -> (pTR.x - radiusTop, pTR.y) -> (pTR.x - radiusTop, rec.y + radiusTop) -> (rec.x + radiusTop, rec.y + radiusTop)

    // Coordinates for the inner top rectangle/trapezoid fill (between the curved parts)
    Vector2 innerTopLeft_rect = { rec.x + radiusTop, rec.y };
    Vector2 innerTopRight_rect = { pTR.x - radiusTop, rec.y };
    Vector2 innerTopLeft_rect_bottom = { rec.x + radiusTop, rec.y + radiusTop };
    Vector2 innerTopRight_rect_bottom = { pTR.x - radiusTop, rec.y + radiusTop };

    // Ensure the top-right x doesn't go left of top-left x after radii
    if (innerTopRight_rect.x < innerTopLeft_rect.x)
    {
        // This means the slope and radii are too large. Adjust or handle gracefully.
        // For simplicity, we'll just draw a narrow polygon or nothing if it collapses.
        // A more robust solution might cap the slope or radius.
    }

    // Draw the trapezoidal top fill (between the top arcs and the central rectangle)
    // Points:
    // 1: (rec.x + radiusTop, rec.y) -- Top-left point where straight top line begins
    // 2: (pTR.x - radiusTop, rec.y) -- Top-right point where straight top line begins (adjusted for slope)
    // 3: (pTR.x - radiusTop, rec.y + radiusTop) -- Bottom-right point of this section
    // 4: (rec.x + radiusTop, rec.y + radiusTop) -- Bottom-left point of this section
    Vector2 top_fill_points[4] = {
        innerTopLeft_rect,
        innerTopRight_rect,
        innerTopRight_rect_bottom,
        innerTopLeft_rect_bottom
    };
    DrawTriangleFan(top_fill_points, 4, color);


    // F. Draw the top-left arc
    Vector2 centerTL_arc = { rec.x + radiusTop, rec.y + radiusTop };
    DrawCircleSector(centerTL_arc, radiusTop, 180.0f, 270.0f, segments, color);

    // G. Draw the top-right arc (adjusted for slope)
    Vector2 centerTR_arc = { pTR.x - radiusTop, rec.y + radiusTop };
    DrawCircleSector(centerTR_arc, radiusTop, 270.0f, 360.0f, segments, color);

    // H. Fill the remaining left and right vertical strips (if not covered by main rect and arcs)
    // Left strip
    DrawRectangle(rec.x, (int)(rec.y + radiusTop), (int)radiusBottom, (int)(rec.height - radiusTop - radiusBottom), color);
    // Right strip - this needs to be a polygon due to the slope
    // Points for the right strip polygon:
    // 1: pTR.x - radiusTop, rec.y + radiusTop
    // 2: pTR.x, rec.y + radiusTop (this is the actual top-right corner of the rounded part, not the slope point)
    // 3: pBR.x, rec.y + rec.height - radiusBottom
    // 4: pBR.x - radiusBottom, rec.y + rec.height - radiusBottom
    // 5: pTR.x - radiusTop (bottom-right of inner top trapezoid), rec.y + radiusTop (top of inner vertical)
    // (This is getting complex, a simpler approach might be to just draw more filling rectangles)

    // Let's refine the fill. Instead of `DrawRectangle`, we'll draw two vertical strips between the top and bottom arcs.
    // These strips run from `rec.y + radiusTop` to `rec.y + rec.height - radiusBottom`.
    // The width of these strips is `radiusTop` on the left and `radiusBottom` on the right.

    // Left vertical strip (between TL arc and BL arc)
    DrawRectangle(rec.x, (int)(rec.y + radiusTop), (int)radiusTop, (int)(rec.height - radiusTop - radiusBottom), color);

    // Right vertical strip (between TR arc and BR arc)
    // The right edge is sloped, so this needs to be a polygon
    Vector2 right_strip_points[4] = {
        { pTR.x - radiusTop, rec.y + radiusTop }, // Top-left of right strip
        { pTR.x, rec.y + radiusTop },             // Top-right (corner of arc)
        { pBR.x, rec.y + rec.height - radiusBottom }, // Bottom-right
        { pBR.x - radiusBottom, rec.y + rec.height - radiusBottom } // Bottom-left of right strip
    };
    DrawTriangleFan(right_strip_points, 4, color);
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