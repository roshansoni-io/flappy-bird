#include <raylib.h>
#include <vector>
#include <algorithm>

//  Constants 
const float SCALE = 1.0f;

const int SCREEN_WIDTH = (int)(288 * SCALE);
const int SCREEN_HEIGHT = (int)(512 * SCALE);

const float GRAVITY = 1000.0f * SCALE;
const float BIRD_JUMP_FORCE = -300.0f * SCALE;
const float BIRD_COLLISION_SCALE = 0.7f;

const float PIPE_SPEED = 150.0f * SCALE;
const float PIPE_SPAWN_DELAY = 1.5f;
const float PIPE_GAP_SIZE = 120.0f * SCALE;

const float BG_SCROLL_SPEED = 30.0f * SCALE;

enum GameState {
    MENU,
    PLAYING,
    GAME_OVER
};

//  Entities 

class Bird {
public:
    Vector2 position;
    float velocityY;
    Texture2D texture;

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

        Rectangle sourceRect = { 0, 0, (float)texture.width, (float)texture.height };
        
        Rectangle destRect = { position.x, position.y, width, height };
        
        Vector2 origin = { width / 2.0f, height / 2.0f };

        DrawTexturePro(texture, sourceRect, destRect, origin, 0, WHITE);
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
    float position;
    float gapCenterY;
    bool scoreGiven;

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

        // Bottom pipe
        Rectangle destRectBottom = { position, gapCenterY + PIPE_GAP_SIZE / 2, width, height };
        
        DrawTexturePro(texture, sourceRect, destRectBottom, origin, 0, WHITE);

        // Top pipe
        Rectangle destRectTop = { position,
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
          (float)texture.height * SCALE };
    }

    Rectangle GetTopBounds(Texture2D texture) const {
        return {
          position,
          gapCenterY - PIPE_GAP_SIZE / 2 - (float)texture.height * SCALE,
          (float)texture.width * SCALE,
          (float)texture.height * SCALE };
    }
};


int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Flappy Bird");
    
    SetTargetFPS(60);

    // Load Resources
    Texture2D birdTexture = LoadTexture("assets/bird.png");
    Texture2D backgroundTexture = LoadTexture("assets/background.png");
    Texture2D pipeTexture = LoadTexture("assets/pipe.png");

    // Initialize Entities
    Bird bird;
    bird.texture = birdTexture;
    bird.Reset();

    std::vector<Pipe> pipes;
    float pipeSpawnTimer = 0.0f;
    float bgOffset = 0.0f;
    float bgWidth = backgroundTexture.width * SCALE;

    GameState gameState = MENU;
    int score = 0;

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        // Background scrolling
        bgOffset -= BG_SCROLL_SPEED * deltaTime;
        if (bgOffset <= -bgWidth) bgOffset += bgWidth;

        // Logic
        if (gameState == MENU) {
            if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                bird.Reset();
                bird.velocityY = BIRD_JUMP_FORCE;
                gameState = PLAYING;
            }
        } 
        else if (gameState == PLAYING) {
            if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                bird.velocityY = BIRD_JUMP_FORCE;
            }

            bird.Update(deltaTime);

            // Pipe spawning
            pipeSpawnTimer += deltaTime;
            if (pipeSpawnTimer >= PIPE_SPAWN_DELAY) {
                pipeSpawnTimer = 0.0f;
                Pipe::Spawn(pipes, pipeTexture.width);
            }

            // Pipe update and collision
            for (Pipe& pipe : pipes) {
                pipe.Update(deltaTime);

                if (!pipe.scoreGiven && bird.position.x > pipe.position + pipeTexture.width * SCALE) {
                    pipe.scoreGiven = true;
                    score++;
                }

                Rectangle birdBounds = bird.GetBounds();
                
                //check collision and if collided game over
                
                if (CheckCollisionRecs(birdBounds, pipe.GetBottomBounds(pipeTexture)) ||
                    
                    CheckCollisionRecs(birdBounds, pipe.GetTopBounds(pipeTexture))) {
                    
                    gameState = GAME_OVER;
                }
            }

            // Remove off screen pipes
            
            pipes.erase(
                std::remove_if(pipes.begin(), pipes.end(), [&](const Pipe& pipe) {
                    return pipe.position < -pipeTexture.width * SCALE;
                }),
                pipes.end()
            );

            // Screen boundaries
            Rectangle bounds = bird.GetBounds();
            if (bounds.y < 0 || bounds.y + bounds.height > SCREEN_HEIGHT) {
                gameState = GAME_OVER;
            }
        } 
        else if (gameState == GAME_OVER) {
            if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                score = 0;
                pipes.clear();
                pipeSpawnTimer = 0.0f;
                bird.Reset();
                bird.velocityY = BIRD_JUMP_FORCE;
                gameState = PLAYING;
            }
        }

        // Rendering
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw two Background image at a time
        DrawTextureEx(
          backgroundTexture,
          { bgOffset, 0 },
          0.0f,
          SCALE,
          WHITE
          );
        DrawTextureEx(
          backgroundTexture,
          { bgOffset + bgWidth, 0 },
          0.0f,
          SCALE,
          WHITE
          );

        // Draw Pipes and Bounding Boxes
        for (const Pipe& pipe : pipes) {
            pipe.Draw(pipeTexture);
            DrawRectangleLinesEx(pipe.GetBottomBounds(pipeTexture), 2, BLUE);
            DrawRectangleLinesEx(pipe.GetTopBounds(pipeTexture), 2, BLUE);
        }

        // Draw Bird and Bounding Box 
        bird.Draw();
        DrawRectangleLinesEx(bird.GetBounds(), 2, RED);

        // UI
        if (gameState == MENU) {
            DrawText("CLICK TO START", SCREEN_WIDTH / 2 - MeasureText("CLICK TO START", (int)(20 * SCALE)) / 2, SCREEN_HEIGHT / 2, (int)(20 * SCALE), WHITE);
        } 
        else if (gameState == PLAYING) {
            DrawText(TextFormat("%d", score), SCREEN_WIDTH / 2 - (int)(10 * SCALE), (int)(50 * SCALE), (int)(40 * SCALE), WHITE);
        } 
        else if (gameState == GAME_OVER) {
            DrawText("GAME OVER", SCREEN_WIDTH / 2 - MeasureText("GAME OVER", (int)(30 * SCALE)) / 2, SCREEN_HEIGHT / 2 - (int)(40 * SCALE), (int)(30 * SCALE), RED);
            DrawText(TextFormat("Score: %d", score), SCREEN_WIDTH / 2 - MeasureText(TextFormat("Score: %d", score), (int)(20 * SCALE)) / 2, SCREEN_HEIGHT / 2 + (int)(10 * SCALE), (int)(20 * SCALE), WHITE);
            DrawText("CLICK TO RESTART", SCREEN_WIDTH / 2 - MeasureText("CLICK TO RESTART", (int)(20 * SCALE)) / 2, SCREEN_HEIGHT / 2 + (int)(50 * SCALE), (int)(20 * SCALE), WHITE);
        }

        EndDrawing();
    }

    // Cleanup
    UnloadTexture(birdTexture);
    UnloadTexture(backgroundTexture);
    UnloadTexture(pipeTexture);
    CloseWindow();

    return 0;
}