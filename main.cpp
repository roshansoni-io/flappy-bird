#include <raylib.h>
#include <vector>
#include <algorithm>
#include "include/constants.hpp"
#include "include/ui.hpp"

#if defined(__EMSCRIPTEN__)
    #include <emscripten/emscripten.h>
#endif

class Bird {
public:
    Vector2 position{};
    float velocityY = 0.0f;
    Texture2D texture{};

    void Reset() {
        position = { SCREEN_WIDTH / 3.0f, SCREEN_HEIGHT / 2.0f };
        velocityY = 0.0f;
    }

    void Update(float deltaTime) {
        velocityY += GRAVITY * deltaTime;
        position.y += velocityY * deltaTime;
    }

    void Draw() const {
        float width = texture.width * SCALE;
        float height = texture.height * SCALE;
        float rotation = std::clamp(velocityY * 0.08f, -25.0f, 30.0f);

        Rectangle sourceRect = { 0, 0, (float)texture.width, (float)texture.height };
        Rectangle destRect = { position.x, position.y, width, height };
        Vector2 origin = { width / 2.0f, height / 2.0f };

        DrawTexturePro(texture, sourceRect, destRect, origin, rotation, WHITE);
    }

    Rectangle GetBounds() const {
        float width = texture.width * SCALE * BIRD_COLLISION_SCALE;
        float height = texture.height * SCALE * BIRD_COLLISION_SCALE;

        return {
            position.x - width / 2.0f,
            position.y - height / 2.0f,
            width,
            height
        };
    }
};

class Pipe {
public:
    float position = 0.0f;
    float gapCenterY = 0.0f;
    bool scoreGiven = false;

    static void Spawn(std::vector<Pipe>& pipes, int pipeWidth) {
        Pipe pipe;
        pipe.position = SCREEN_WIDTH + pipeWidth * SCALE;
        pipe.gapCenterY = (float)GetRandomValue((int)(150 * SCALE), (int)(SCREEN_HEIGHT - 150 * SCALE));
        pipe.scoreGiven = false;
        pipes.push_back(pipe);
    }

    void Update(float deltaTime) {
        position -= PIPE_SPEED * deltaTime;
    }

    void Draw(Texture2D texture) const {
        Vector2 origin = { 0, 0 };

        Rectangle sourceRect = { 0, 0, (float)texture.width, (float)texture.height };
        
        Rectangle sourceRectFlipped = { 0, 0, (float)texture.width, -(float)texture.height };

        float width = texture.width * SCALE;
        float height = texture.height * SCALE;

        Rectangle destRectBottom = { position, gapCenterY + PIPE_GAP_SIZE / 2, width, height };
        DrawTexturePro(texture, sourceRect, destRectBottom, origin, 0, WHITE);

        Rectangle destRectTop = {
            position,
            gapCenterY - PIPE_GAP_SIZE / 2 - height,
            width,
            height
        };
        DrawTexturePro(texture, sourceRectFlipped, destRectTop, origin, 0, WHITE);
    }

    Rectangle GetBottomBounds(Texture2D texture) const {
        return {
            position,
            gapCenterY + PIPE_GAP_SIZE / 2,
            (float)texture.width * SCALE,
            (float)texture.height * SCALE
        };
    }

    Rectangle GetTopBounds(Texture2D texture) const {
        return {
            position,
            gapCenterY - PIPE_GAP_SIZE / 2 - (float)texture.height * SCALE,
            (float)texture.width * SCALE,
            (float)texture.height * SCALE
        };
    }
};

static Texture2D birdTexture{};
static Texture2D backgroundTexture{};
static Texture2D pipeTexture{};

static Bird bird;
static std::vector<Pipe> pipes;
static float pipeSpawnTimer = 0.0f;
static float bgOffset = 0.0f;
static float bgWidth = 0.0f;

static GameState gameState = MENU;
static int score = 0;
static int highScore = 0;

static void ResetGame(bool startPlaying) {
    score = 0;
    pipes.clear();
    pipeSpawnTimer = 0.0f;
    bird.Reset();
    if (startPlaying) {
        bird.velocityY = BIRD_JUMP_FORCE;
        gameState = PLAYING;
    }
}

static void UpdateDrawFrame() {
    float deltaTime = GetFrameTime();

    bgOffset -= BG_SCROLL_SPEED * deltaTime;
    if (bgOffset <= -bgWidth) bgOffset += bgWidth;

    if (gameState == MENU) {
        if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            ResetGame(true);
        }
    }
    else if (gameState == PLAYING) {
        if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            bird.velocityY = BIRD_JUMP_FORCE;
        }

        bird.Update(deltaTime);

        pipeSpawnTimer += deltaTime;
        if (pipeSpawnTimer >= PIPE_SPAWN_DELAY) {
            pipeSpawnTimer = 0.0f;
            Pipe::Spawn(pipes, pipeTexture.width);
        }

        Rectangle birdBounds = bird.GetBounds();

        for (Pipe& pipe : pipes) {
            pipe.Update(deltaTime);

            if (!pipe.scoreGiven && bird.position.x > pipe.position + pipeTexture.width * SCALE) {
                pipe.scoreGiven = true;
                score++;
            }

            if (CheckCollisionRecs(birdBounds, pipe.GetBottomBounds(pipeTexture)) ||
                CheckCollisionRecs(birdBounds, pipe.GetTopBounds(pipeTexture))) {
                gameState = GAME_OVER;
                if (score > highScore) highScore = score;
            }
        }

        pipes.erase(
            std::remove_if(pipes.begin(), pipes.end(), [&](const Pipe& pipe) {
                return pipe.position < -pipeTexture.width * SCALE;
            }),
            pipes.end()
        );

        if (birdBounds.y < 0 || birdBounds.y + birdBounds.height > SCREEN_HEIGHT) {
            gameState = GAME_OVER;
            if (score > highScore) highScore = score;
        }
    }
    else if (gameState == GAME_OVER) {
        if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            ResetGame(true);
        }
    }

    BeginDrawing();
    ClearBackground(BLACK);

    DrawTextureEx(backgroundTexture, { bgOffset, 0 }, 0.0f, SCALE, WHITE);
    DrawTextureEx(backgroundTexture, { bgOffset + bgWidth, 0 }, 0.0f, SCALE, WHITE);

    for (const Pipe& pipe : pipes) {
        pipe.Draw(pipeTexture);
    }

    bird.Draw();

    if (gameState == MENU) {
        if (UI::Menu(SCREEN_WIDTH, SCREEN_HEIGHT)) {
            ResetGame(true);
        }
    }
    else if (gameState == PLAYING) {
        UI::ScorePanel(score, SCREEN_WIDTH / 2.0f - 30 * SCALE, 20 * SCALE, 60 * SCALE, 60 * SCALE);
    }
    else if (gameState == GAME_OVER) {
        if (UI::GameOver(SCREEN_WIDTH, SCREEN_HEIGHT, score, highScore)) {
            ResetGame(true);
        }
    }

    EndDrawing();
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Flappy Bird");
    SetTargetFPS(60);

    birdTexture = LoadTexture("assets/bird.png");
    backgroundTexture = LoadTexture("assets/background.png");
    pipeTexture = LoadTexture("assets/pipe.png");

    bird.texture = birdTexture;
    bird.Reset();

    bgWidth = backgroundTexture.width * SCALE;

#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!WindowShouldClose()) {
        UpdateDrawFrame();
    }
#endif

    UnloadTexture(birdTexture);
    UnloadTexture(backgroundTexture);
    UnloadTexture(pipeTexture);
    CloseWindow();
    return 0;
}