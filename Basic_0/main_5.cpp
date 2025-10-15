#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raymath.h" // For Vector2 operations like Vector2Add, Vector2Subtract
#include <algorithm> // For std::min/max if needed, or std::clamp in C++17

// --- Shape Configuration ---
// Combines your original eye config with the new color controls
struct ShapeConfig {
    float OffsetX;
    float OffsetY;
    float Height;
    float Width;
    float Slope_Top;
    float Slope_Bottom;
    float Radius_Top;
    float Radius_Bottom;
    // Color components (0-255)
    float R;
    float G;
    float B;
    float A;
    // Flags for potential inversions (though not directly used in this drawing logic)
    bool Inverse_Radius_Top;
    bool Inverse_Radius_Bottom;
    bool Inverse_Offset_Top;
    bool Inverse_Offset_Bottom;
};

// Preset for a neutral, slightly rounded, blue shape
static const ShapeConfig Preset_NeutralShape = {
    0, 0, // OffsetX, OffsetY
    80, 120, // Height, Width
    0.0f, 0.0f, // Slope_Top, Slope_Bottom (no slope initially)
    20.0f, 20.0f, // Radius_Top, Radius_Bottom (rounded corners)
    102, 191, 255, 255, // R, G, B, A (Sky Blue)
    false, false, false, false // Inverse flags
};

// --- ShapeDrawer Class ---
class ShapeDrawer {
public:
    // Pure function to draw the custom shape
    // Takes center coordinates and configuration, draws directly to raylib's drawing buffer
    static void Draw(int centerX, int centerY, const ShapeConfig &cfg) {
        // Convert float color components to raylib Color struct
        Color shapeColor = {
            (unsigned char)cfg.R,
            (unsigned char)cfg.G,
            (unsigned char)cfg.B,
            (unsigned char)cfg.A
        };

        // --- Calculate Core Points and Adjustments ---
        float halfWidth = cfg.Width / 2.0f;
        float halfHeight = cfg.Height / 2.0f;

        // Apply global offset
        float currentCenterX = centerX + cfg.OffsetX;
        float currentCenterY = centerY + cfg.OffsetY;

        // Adjust vertical position for slopes
        float topSlopeOffset = cfg.Height * cfg.Slope_Top;
        float bottomSlopeOffset = cfg.Height * cfg.Slope_Bottom;

        // Calculate corner centers for arcs
        // Note: The image implies radius applies from the "inner" rectangle boundary
        // Top-left arc center
        Vector2 TL_arc_center = {
            currentCenterX - halfWidth + cfg.Radius_Top,
            currentCenterY - halfHeight + cfg.Radius_Top - topSlopeOffset / 2.0f
        };
        // Top-right arc center
        Vector2 TR_arc_center = {
            currentCenterX + halfWidth - cfg.Radius_Top,
            currentCenterY - halfHeight + cfg.Radius_Top + topSlopeOffset / 2.0f
        };
        // Bottom-left arc center
        Vector2 BL_arc_center = {
            currentCenterX - halfWidth + cfg.Radius_Bottom,
            currentCenterY + halfHeight - cfg.Radius_Bottom - bottomSlopeOffset / 2.0f
        };
        // Bottom-right arc center
        Vector2 BR_arc_center = {
            currentCenterX + halfWidth - cfg.Radius_Bottom,
            currentCenterY + halfHeight - cfg.Radius_Bottom + bottomSlopeOffset / 2.0f
        };

        // Ensure radii don't exceed half the width/height
        float actualRadiusTop = std::min(cfg.Radius_Top, halfWidth);
        actualRadiusTop = std::min(actualRadiusTop, halfHeight); // Also clamp against half height
        float actualRadiusBottom = std::min(cfg.Radius_Bottom, halfWidth);
        actualRadiusBottom = std::min(actualRadiusBottom, halfHeight); // Also clamp against half height


        // --- Define Vertices for drawing the outline ---
        // Points on the straight top line (after arc)
        Vector2 p_top_left = {TL_arc_center.x, TL_arc_center.y - actualRadiusTop};
        Vector2 p_top_right = {TR_arc_center.x, TR_arc_center.y - actualRadiusTop};

        // Points on the straight bottom line (after arc)
        Vector2 p_bottom_left = {BL_arc_center.x, BL_arc_center.y + actualRadiusBottom};
        Vector2 p_bottom_right = {BR_arc_center.x, BR_arc_center.y + actualRadiusBottom};
        
        // Points on the straight left line (after arc)
        Vector2 p_left_top = {TL_arc_center.x - actualRadiusTop, TL_arc_center.y};
        Vector2 p_left_bottom = {BL_arc_center.x - actualRadiusBottom, BL_arc_center.y};

        // Points on the straight right line (after arc)
        Vector2 p_right_top = {TR_arc_center.x + actualRadiusTop, TR_arc_center.y};
        Vector2 p_right_bottom = {BR_arc_center.x + actualRadiusBottom, BR_arc_center.y};


        // --- Draw the Filled Shape ---
        // For a filled shape with arbitrary slopes and rounded corners,
        // it's often easiest to use a polygon approach or combine multiple shapes.
        // Given the complexity with slopes AND radii, we'll draw a central rectangle
        // and then overlay rounded corners and slope triangles.

        // Draw the central rectangular part (adjusted for slopes and radii)
        Rectangle innerRect = {
            TL_arc_center.x,
            p_top_left.y, // Adjusted for top radius
            TR_arc_center.x - TL_arc_center.x,
            p_bottom_left.y - p_top_left.y // Adjusted for top/bottom radii
        };
        DrawRectangleRec(innerRect, shapeColor);


        // Draw the top part (trapezoid/rectangle with top slope)
        // This is a simplified approach, a more precise polygon fill might be needed for complex slopes.
        // For now, let's just use the conceptual bounding box with slopes for visualization.

        // Points for the main polygon outline for filling (conceptual, adjusted for slopes and radii)
        Vector2 polygonPoints[8];
        int numPoints = 0;

        // Top-left arc (start from left, go up-right)
        if (actualRadiusTop > 0) {
            float startAngle = 180.0f; // From left
            float endAngle = 270.0f;   // To top
            // Iterate to get points for the arc, and add them to polygonPoints
            for (float angle = startAngle; angle <= endAngle; angle += 10.0f) { // 10 degree steps
                if (numPoints < 8) {
                    polygonPoints[numPoints].x = TL_arc_center.x + actualRadiusTop * cosf(DEG2RAD * angle);
                    polygonPoints[numPoints].y = TL_arc_center.y + actualRadiusTop * sinf(DEG2RAD * angle);
                    numPoints++;
                }
            }
        } else {
            if (numPoints < 8) { polygonPoints[numPoints++] = {currentCenterX - halfWidth, currentCenterY - halfHeight - topSlopeOffset / 2.0f}; }
        }

        // Top-right arc
        if (actualRadiusTop > 0) {
            float startAngle = 270.0f; // From top
            float endAngle = 360.0f;   // To right
            for (float angle = startAngle; angle <= endAngle; angle += 10.0f) {
                if (numPoints < 8) {
                    polygonPoints[numPoints].x = TR_arc_center.x + actualRadiusTop * cosf(DEG2RAD * angle);
                    polygonPoints[numPoints].y = TR_arc_center.y + actualRadiusTop * sinf(DEG2RAD * angle);
                    numPoints++;
                }
            }
        } else {
            if (numPoints < 8) { polygonPoints[numPoints++] = {currentCenterX + halfWidth, currentCenterY - halfHeight + topSlopeOffset / 2.0f}; }
        }
        
        // Bottom-right arc
        if (actualRadiusBottom > 0) {
            float startAngle = 0.0f; // From right
            float endAngle = 90.0f;  // To bottom
            for (float angle = startAngle; angle <= endAngle; angle += 10.0f) {
                 if (numPoints < 8) {
                    polygonPoints[numPoints].x = BR_arc_center.x + actualRadiusBottom * cosf(DEG2RAD * angle);
                    polygonPoints[numPoints].y = BR_arc_center.y + actualRadiusBottom * sinf(DEG2RAD * angle);
                    numPoints++;
                }
            }
        } else {
            if (numPoints < 8) { polygonPoints[numPoints++] = {currentCenterX + halfWidth, currentCenterY + halfHeight + bottomSlopeOffset / 2.0f}; }
        }

        // Bottom-left arc
        if (actualRadiusBottom > 0) {
            float startAngle = 90.0f; // From bottom
            float endAngle = 180.0f;  // To left
            for (float angle = startAngle; angle <= endAngle; angle += 10.0f) {
                if (numPoints < 8) {
                    polygonPoints[numPoints].x = BL_arc_center.x + actualRadiusBottom * cosf(DEG2RAD * angle);
                    polygonPoints[numPoints].y = BL_arc_center.y + actualRadiusBottom * sinf(DEG2RAD * angle);
                    numPoints++;
                }
            }
        } else {
             if (numPoints < 8) { polygonPoints[numPoints++] = {currentCenterX - halfWidth, currentCenterY + halfHeight - bottomSlopeOffset / 2.0f}; }
        }

        // --- Simplified filled drawing (might not perfectly match image for extreme slopes/radii) ---
        // For a perfectly filled complex shape like this, you would typically use DrawPoly or a more advanced
        // triangulation method. However, for a general eye-like shape,
        // drawing a rectangle and then specific arcs and triangles might be visually sufficient.

        // Drawing a filled shape composed of a rectangle and four arc segments.
        // This is a simplified approach to fill the shape. A truly complex shape
        // with arbitrary slopes and radii would need a polygon triangulation or
        // DrawPolyEx with many points.

        // 1. Draw the central rectangle portion (adjusted for slopes at top/bottom and radii at sides)
        // This assumes the straight vertical lines connect the tangent points of the arcs.
        Vector2 startTopLeftLine = {TL_arc_center.x, TL_arc_center.y - actualRadiusTop};
        Vector2 endTopRightLine = {TR_arc_center.x, TR_arc_center.y - actualRadiusTop};
        Vector2 startBottomLeftLine = {BL_arc_center.x, BL_arc_center.y + actualRadiusBottom};
        Vector2 endBottomRightLine = {BR_arc_center.x, BR_arc_center.y + actualRadiusBottom};

        // Define main rectangle vertices for the body of the eye
        Vector2 recPoints[4] = {
            {startTopLeftLine.x, startTopLeftLine.y},
            {endTopRightLine.x, endTopRightLine.y},
            {endBottomRightLine.x, endBottomRightLine.y},
            {startBottomLeftLine.x, startBottomLeftLine.y}
        };

        // Adjust for slopes affecting the top/bottom horizontal lines if not handled by arc centers
        // The image shows the slope affecting the top/bottom horizontal edges of the *bounding box*,
        // then radii are applied.
        // Let's refine based on the image's dashed lines for the inner rectangle.
        float inner_rect_y_top = currentCenterY - halfHeight;
        float inner_rect_y_bottom = currentCenterY + halfHeight;
        float inner_rect_x_left = currentCenterX - halfWidth;
        float inner_rect_x_right = currentCenterX + halfWidth;


        // Calculate the actual points for drawing, considering slopes and radii
        Vector2 P1 = {inner_rect_x_left + actualRadiusTop, inner_rect_y_top - topSlopeOffset / 2.0f}; // Top-left corner start of straight top line
        Vector2 P2 = {inner_rect_x_right - actualRadiusTop, inner_rect_y_top + topSlopeOffset / 2.0f}; // Top-right corner end of straight top line
        Vector2 P3 = {inner_rect_x_right - actualRadiusBottom, inner_rect_y_bottom + bottomSlopeOffset / 2.0f}; // Bottom-right corner end of straight bottom line
        Vector2 P4 = {inner_rect_x_left + actualRadiusBottom, inner_rect_y_bottom - bottomSlopeOffset / 2.0f}; // Bottom-left corner start of straight bottom line

        // For filling, it's simpler to consider it as a series of connected shapes.
        // Let's try drawing a central polygon first, then the rounded corners.

        // This polygon will form the main "body" of the eye, adjusted by slopes and radii
        // Vertices for the main polygon part:
        // Top-left arc start -> P1 -> P2 -> Top-right arc end
        // -> Bottom-right arc start -> P3 -> P4 -> Bottom-left arc end
        // This is complex for a simple DrawPoly.
        // A more robust way to fill: draw a base rectangle, then four rectangles for the extensions
        // due to slopes, and finally the four filled arcs.

        // Let's draw the outline using DrawLineV and DrawCircleSectorLines for precision

        // Calculate tangent points for straight lines
        Vector2 TL_tangent_top = {TL_arc_center.x, TL_arc_center.y - actualRadiusTop};
        Vector2 TR_tangent_top = {TR_arc_center.x, TR_arc_center.y - actualRadiusTop};
        Vector2 BR_tangent_bottom = {BR_arc_center.x, BR_arc_center.y + actualRadiusBottom};
        Vector2 BL_tangent_bottom = {BL_arc_center.x, BL_arc_center.y + actualRadiusBottom};

        Vector2 TL_tangent_left = {TL_arc_center.x - actualRadiusTop, TL_arc_center.y};
        Vector2 TR_tangent_right = {TR_arc_center.x + actualRadiusTop, TR_arc_center.y};
        Vector2 BL_tangent_left = {BL_arc_center.x - actualRadiusBottom, BL_arc_center.y};
        Vector2 BR_tangent_right = {BR_arc_center.x + actualRadiusBottom, BR_arc_center.y};


        // --- Draw Filled Core ---
        // A simpler way to fill is to draw a base rectangle that connects the innermost points,
        // then fill the parts where arcs and slopes extend.
        // This is a common simplification for drawing these types of shapes when not using full polygon rendering.

        // Determine the actual straight line endpoints adjusted for slopes and radii
        Vector2 current_P1 = {currentCenterX - halfWidth + actualRadiusTop, currentCenterY - halfHeight - topSlopeOffset / 2.0f};
        Vector2 current_P2 = {currentCenterX + halfWidth - actualRadiusTop, currentCenterY - halfHeight + topSlopeOffset / 2.0f};
        Vector2 current_P3 = {currentCenterX + halfWidth - actualRadiusBottom, currentCenterY + halfHeight + bottomSlopeOffset / 2.0f};
        Vector2 current_P4 = {currentCenterX - halfWidth + actualRadiusBottom, currentCenterY + halfHeight - bottomSlopeOffset / 2.0f};

        // Draw the main trapezoidal body (adjusted by slopes)
        // This uses a polygon with 4 vertices that connect the 'start' points of the curves.
        Vector2 bodyVertices[4] = {
            {current_P1.x, current_P1.y}, // top-left straight part start
            {current_P2.x, current_P2.y}, // top-right straight part end
            {current_P3.x, current_P3.y}, // bottom-right straight part end
            {current_P4.x, current_P4.y}  // bottom-left straight part start
        };
        DrawTriangle(bodyVertices[0], bodyVertices[1], bodyVertices[3], shapeColor);
        DrawTriangle(bodyVertices[1], bodyVertices[2], bodyVertices[3], shapeColor);

        // Fill the rounded corners (solid sectors)
        if (actualRadiusTop > 0) {
            DrawCircleSector(TL_arc_center, actualRadiusTop, 180, 270, 20, shapeColor); // Top-left
            DrawCircleSector(TR_arc_center, actualRadiusTop, 270, 360, 20, shapeColor); // Top-right
        }
        if (actualRadiusBottom > 0) {
            DrawCircleSector(BL_arc_center, actualRadiusBottom, 90, 180, 20, shapeColor); // Bottom-left
            DrawCircleSector(BR_arc_center, actualRadiusBottom, 0, 90, 20, shapeColor); // Bottom-right
        }

        // Fill the rectangles connecting arcs to the main body, if any
        // Vertical rectangles extending from the main body
        DrawRectangle(current_P1.x - actualRadiusTop, current_P1.y, actualRadiusTop, current_P4.y - current_P1.y, shapeColor); // Left extension
        DrawRectangle(current_P2.x, current_P2.y, actualRadiusTop, current_P3.y - current_P2.y, shapeColor); // Right extension


        // --- Draw Outline (Wireframe) ---
        // Top line
        DrawLineEx(current_P1, current_P2, 2, BLACK); // Draw a thicker line for visibility

        // Bottom line
        DrawLineEx(current_P4, current_P3, 2, BLACK);

        // Left line
        DrawLineEx({currentCenterX - halfWidth, current_P1.y}, {currentCenterX - halfWidth, current_P4.y}, 2, BLACK);
        // Right line
        DrawLineEx({currentCenterX + halfWidth, current_P2.y}, {currentCenterX + halfWidth, current_P3.y}, 2, BLACK);


        // Draw the rounded corners wireframe on top of the fill
        if (actualRadiusTop > 0) {
            DrawCircleSectorLines(TL_arc_center, actualRadiusTop, 180, 270, 20, BLACK); // Top-left
            DrawCircleSectorLines(TR_arc_center, actualRadiusTop, 270, 360, 20, BLACK); // Top-right
        }
        if (actualRadiusBottom > 0) {
            DrawCircleSectorLines(BL_arc_center, actualRadiusBottom, 90, 180, 20, BLACK); // Bottom-left
            DrawCircleSectorLines(BR_arc_center, actualRadiusBottom, 0, 90, 20, BLACK); // Bottom-right
        }

        // Draw the conceptual inner rectangle boundaries (dotted lines from image)
        // Draw the horizontal dotted lines
        DrawLineDashed(currentCenterX - halfWidth, currentCenterY - halfHeight, currentCenterX + halfWidth, currentCenterY - halfHeight, 1, 1, GRAY);
        DrawLineDashed(currentCenterX - halfWidth, currentCenterY + halfHeight, currentCenterX + halfWidth, currentCenterY + halfHeight, 1, 1, GRAY);
        // Draw the vertical dotted lines
        DrawLineDashed(currentCenterX - halfWidth, currentCenterY - halfHeight, currentCenterX - halfWidth, currentCenterY + halfHeight, 1, 1, GRAY);
        DrawLineDashed(currentCenterX + halfWidth, currentCenterY - halfHeight, currentCenterX + halfWidth, currentCenterY + halfHeight, 1, 1, GRAY);

        // Draw the actual computed tangent points and arc centers for debugging
        DrawCircleV(TL_arc_center, 3, RED); // Top-left arc center
        DrawCircleV(TR_arc_center, 3, RED); // Top-right arc center
        DrawCircleV(BL_arc_center, 3, RED); // Bottom-left arc center
        DrawCircleV(BR_arc_center, 3, RED); // Bottom-right arc center
        DrawCircleV(current_P1, 3, GREEN); // P1
        DrawCircleV(current_P2, 3, GREEN); // P2
        DrawCircleV(current_P3, 3, GREEN); // P3
        DrawCircleV(current_P4, 3, GREEN); // P4

        // Draw slope visualizers (from your image)
        // Top slope visualization
        DrawLine(currentCenterX - halfWidth, currentCenterY - halfHeight - topSlopeOffset / 2.0f,
                 currentCenterX - halfWidth + 10, currentCenterY - halfHeight - topSlopeOffset / 2.0f, DARKBLUE);
        DrawLine(currentCenterX + halfWidth, currentCenterY - halfHeight + topSlopeOffset / 2.0f,
                 currentCenterX + halfWidth - 10, currentCenterY - halfHeight + topSlopeOffset / 2.0f, DARKBLUE);
        DrawLine(currentCenterX - halfWidth, currentCenterY - halfHeight - topSlopeOffset / 2.0f,
                 currentCenterX + halfWidth, currentCenterY - halfHeight + topSlopeOffset / 2.0f, DARKBLUE);
        
        // Bottom slope visualization
        DrawLine(currentCenterX - halfWidth, currentCenterY + halfHeight - bottomSlopeOffset / 2.0f,
                 currentCenterX - halfWidth + 10, currentCenterY + halfHeight - bottomSlopeOffset / 2.0f, DARKBLUE);
        DrawLine(currentCenterX + halfWidth, currentCenterY + halfHeight + bottomSlopeOffset / 2.0f,
                 currentCenterX + halfWidth - 10, currentCenterY + halfHeight + bottomSlopeOffset / 2.0f, DARKBLUE);
        DrawLine(currentCenterX - halfWidth, currentCenterY + halfHeight - bottomSlopeOffset / 2.0f,
                 currentCenterX + halfWidth, currentCenterY + halfHeight + bottomSlopeOffset / 2.0f, DARKBLUE);
    }

    // Helper function for dashed lines (not natively in raylib 4.2, requires custom implementation)
    // You'd typically add this to your utility functions or use a texture/shader.
    // For this example, a simple manual drawing is provided.
    static void DrawLineDashed(float x1, float y1, float x2, float y2, float dashLength, float spaceLength, Color color) {
        Vector2 start = {x1, y1};
        Vector2 end = {x2, y2};
        Vector2 delta = Vector2Subtract(end, start);
        float lineLength = Vector2Length(delta);
        Vector2 direction = Vector2Normalize(delta);

        float currentLength = 0;
        while (currentLength < lineLength) {
            Vector2 dashStart = Vector2Add(start, Vector2Scale(direction, currentLength));
            currentLength += dashLength;
            if (currentLength > lineLength) currentLength = lineLength; // Cap at end
            Vector2 dashEnd = Vector2Add(start, Vector2Scale(direction, currentLength));
            DrawLineV(dashStart, dashEnd, color);
            currentLength += spaceLength;
        }
    }
};

// --- Main ---
int main() {
    InitWindow(1200, 800, "Custom Shape Controller"); // Increased window size
    SetTargetFPS(60);

    ShapeConfig cfg = Preset_NeutralShape;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(DARKGRAY); // Changed background for better contrast

        // Draw the shape in the center
        float centerX = GetScreenWidth() / 2.0f;
        float centerY = GetScreenHeight() / 2.0f;
        ShapeDrawer::Draw(centerX, centerY, cfg);

        // --- GUI controls ---
        GuiSetStyle(DEFAULT, TEXT_SIZE, 16);
        float panelX = 900, panelY = 30; // Adjusted panel position

        DrawText("Shape Config Controls", panelX, 10, 20, RAYWHITE);
        
        // Position and Size Controls (4 bars)
        GuiSliderBar({panelX, panelY, 250, 20}, "OffsetX", TextFormat("%.0f", cfg.OffsetX), &cfg.OffsetX, -200, 200); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "OffsetY", TextFormat("%.0f", cfg.OffsetY), &cfg.OffsetY, -200, 200); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "Width", TextFormat("%.0f", cfg.Width), &cfg.Width, 20, 400); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "Height", TextFormat("%.0f", cfg.Height), &cfg.Height, 20, 400); panelY += 30;
        
        // Slopes
        GuiSliderBar({panelX, panelY, 250, 20}, "Slope_Top", TextFormat("%.2f", cfg.Slope_Top), &cfg.Slope_Top, -0.5f, 0.5f); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "Slope_Bottom", TextFormat("%.2f", cfg.Slope_Bottom), &cfg.Slope_Bottom, -0.5f, 0.5f); panelY += 30;

        // Radii
        GuiSliderBar({panelX, panelY, 250, 20}, "Radius_Top", TextFormat("%.0f", cfg.Radius_Top), &cfg.Radius_Top, 0, 100); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "Radius_Bottom", TextFormat("%.0f", cfg.Radius_Bottom), &cfg.Radius_Bottom, 0, 100); panelY += 30;
        
        // Separator
        DrawRectangle(panelX, panelY, 250, 2, GRAY); panelY += 10;
        DrawText("Color Controls (0-255)", panelX, panelY, 16, RAYWHITE); panelY += 20;

        // Color Controls (4 bars)
        GuiSliderBar({panelX, panelY, 250, 20}, "R (Red)", TextFormat("%.0f", cfg.R), &cfg.R, 0, 255); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "G (Green)", TextFormat("%.0f", cfg.G), &cfg.G, 0, 255); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "B (Blue)", TextFormat("%.0f", cfg.B), &cfg.B, 0, 255); panelY += 30;
        GuiSliderBar({panelX, panelY, 250, 20}, "A (Alpha)", TextFormat("%.0f", cfg.A), &cfg.A, 0, 255); panelY += 30;

        // Inverse Checkboxes (from your original config)
        GuiCheckBox({panelX, panelY, 20, 20}, "Inverse_Radius_Top", &cfg.Inverse_Radius_Top); panelY += 25;
        GuiCheckBox({panelX, panelY, 20, 20}, "Inverse_Radius_Bottom", &cfg.Inverse_Radius_Bottom); panelY += 25;
        GuiCheckBox({panelX, panelY, 20, 20}, "Inverse_Offset_Top", &cfg.Inverse_Offset_Top); panelY += 25;
        GuiCheckBox({panelX, panelY, 20, 20}, "Inverse_Offset_Bottom", &cfg.Inverse_Offset_Bottom); panelY += 25;

        EndDrawing();
    }

    CloseWindow();
    return 0;
}