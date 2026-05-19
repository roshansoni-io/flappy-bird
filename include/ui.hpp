#ifndef ui
#define ui

#include <raylib.h>
#include "constants.hpp"

namespace UI
{
    constexpr float ROUNDNESS = 0.25f;
    constexpr int SEGMENTS = 8;

    constexpr Color PANEL_COLOR = {35, 35, 35, 230};
    constexpr Color OVERLAY_COLOR = {0, 0, 0, 140};

    // BUTTON

    inline bool Button(
      Rectangle rect,
      const char* text,
      Color color,
      Color textColor = WHITE
      )
    {
        Vector2 mouse = GetMousePosition();

        bool hovered = CheckCollisionPointRec(mouse, rect);
        
        bool pressed = hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

        if (pressed)
            rect.y += 2 * SCALE;

        Color drawColor = hovered
            ? ColorBrightness(color, 0.15f)
            : color;

        DrawRectangleRounded(
          rect,
          ROUNDNESS,
          SEGMENTS,
          drawColor
          );
          
        DrawRectangleRoundedLines(
          rect,
          ROUNDNESS,
          SEGMENTS,
          WHITE
          );

        int fontSize = (int)(20 * SCALE);

        int textWidth = MeasureText(text, fontSize);

        int textX = rect.x + rect.width / 2 - textWidth / 2;
        
        int textY = rect.y + rect.height / 2 - fontSize / 2;

        DrawText(text, textX, textY, fontSize, textColor);

        return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

    // SCORE PANEL

    inline void ScorePanel(
      int score,
      float x,
      float y,
      float width,
      float height)
    {
        Rectangle panel = {x, y, width, height};

        DrawRectangleRounded(
          panel,
          ROUNDNESS,
          SEGMENTS,
          OVERLAY_COLOR
          );
        DrawRectangleRoundedLines(
          panel,
          ROUNDNESS,
          SEGMENTS,
          WHITE
          );

        const char* scoreText = TextFormat("%d", score);

        int fontSize = (int)(40 * SCALE);

        int textX = x + width / 2- MeasureText(scoreText, fontSize) / 2;

        int textY = y + height / 2 - fontSize / 2;

        DrawText(scoreText, textX, textY, fontSize, WHITE);
    }

    // MENU

    inline bool Menu(int screenWidth, int screenHeight)
    {
        DrawRectangle(0, 0,
        screenWidth,
        screenHeight,
        OVERLAY_COLOR);

        const char* title = "FLAPPY";

        int titleSize = (int)(42 * SCALE);

        int titleX = screenWidth / 2
                   - MeasureText(title, titleSize) / 2;

        DrawText(title,
                 titleX,
                 screenHeight / 3,
                 titleSize,
                 GOLD);

        Rectangle startBtn =
        {
            screenWidth / 2.0f - 60 * SCALE,
            screenHeight / 2.0f,
            120 * SCALE,
            45 * SCALE
        };

        return Button(startBtn, "START", LIME);
    }

    // GAME OVER

    inline bool GameOver(int screenWidth,
                         int screenHeight,
                         int score,
                         int highScore)
    {
        DrawRectangle(0, 0,
                      screenWidth,
                      screenHeight,
                      OVERLAY_COLOR);

        Rectangle panel =
        {
            screenWidth / 2.0f - 120 * SCALE,
            screenHeight / 2.0f - 90 * SCALE,
            240 * SCALE,
            180 * SCALE
        };

        DrawRectangleRounded(panel,
                             ROUNDNESS,
                             SEGMENTS,
                             PANEL_COLOR);

        DrawRectangleRoundedLines(panel,
                                  ROUNDNESS,
                                  SEGMENTS,
                                  LIGHTGRAY);

        // Title
        const char* title = "GAME OVER";

        int titleSize = (int)(30 * SCALE);

        int titleX = panel.x + panel.width / 2
                   - MeasureText(title, titleSize) / 2;

        DrawText(title,
                 titleX,
                 panel.y + 20 * SCALE,
                 titleSize,
                 RED);

        // Score
        DrawText(TextFormat("SCORE : %d", score),
                 panel.x + 25 * SCALE,
                 panel.y + 75 * SCALE,
                 (int)(20 * SCALE),
                 WHITE);

        DrawText(TextFormat("BEST  : %d", highScore),
                 panel.x + 25 * SCALE,
                 panel.y + 105 * SCALE,
                 (int)(20 * SCALE),
                 YELLOW);

        // Restart button
        Rectangle restartBtn =
        {
            panel.x + panel.width / 2 - 70 * SCALE,
            panel.y + panel.height + 20 * SCALE,
            140 * SCALE,
            45 * SCALE
        };

        return Button(restartBtn, "RESTART", ORANGE);
    }

}

#endif