#include "raylib.h"
#include <vector>
#include <algorithm>
#include <raymath.h>
#include <string>

struct Bullet { Vector2 p; float r; float vy; bool alive; };
struct Asteroid { Vector2 p; Vector2 v; float r; bool alive; };
struct Particle { Vector2 p; Vector2 v; float life; float maxLife; Color c; bool alive; };
struct Star {
    Vector2 pos;
    float speed;
    int size;
};

Color col = { static_cast<unsigned char>(GetRandomValue(200, 255)),
              static_cast<unsigned char>(GetRandomValue(120, 200)),
              static_cast<unsigned char>(GetRandomValue(0, 60)),
              255 };

enum class Scene { LOADING, MENU, PLAY, PAUSE, GAMEOVER };

int main() {
    const int W = 960, H = 540;
    InitWindow(W, H, "Space Shooter");
    SetTargetFPS(60);

    Scene scene = Scene::LOADING;
    float loadT = 0.0f, loadDur = 1.6f;

    Texture2D txShip = LoadTexture("ship.png");
    Texture2D txAst = LoadTexture("asteroid.png");
    Texture2D txLogo = LoadTexture("logo.png");
    Texture2D txBg = LoadTexture("space_bg.png");
    Texture2D txBtn = LoadTexture("button.png");

    Vector2 ship = { static_cast<float>(W) / 2, static_cast<float>(H) - 80 };
    float shipSpeed = 380.0f;
    float shipW = txShip.id ? static_cast<float>(txShip.width) * 0.75f : 48.0f;
    float shipH = txShip.id ? static_cast<float>(txShip.height) * 0.75f : 48.0f;
    float fireCD = 0.18f, fireTimer = 0.0f;
    int lives = 3, score = 0, highScore = 0;
    bool invuln = false; float invulnT = 0.0f, invulnDur = 1.2f, blinkT = 0.0f;

    std::vector<Bullet> bullets;
    std::vector<Asteroid> asts;
    std::vector<Particle> parts;
    std::vector<Star> stars;
    for (int i = 0; i < 120; i++) {
        stars.push_back({ { static_cast<float>(GetRandomValue(0, W)), static_cast<float>(GetRandomValue(0, H)) },
                         static_cast<float>(GetRandomValue(30, 120)), GetRandomValue(1, 2) });
    }

    float spawnT = 0.0f, spawnCD = 0.9f, diffT = 0.0f;
    int menuIndex = 0;

    // Define lambdas before use
    auto ClampShip = [&]() {
        if (ship.x - shipW / 2 < 16) ship.x = 16 + shipW / 2;
        if (ship.x + shipW / 2 > W - 16) ship.x = W - 16 - shipW / 2;
        if (ship.y - shipH / 2 < 16) ship.y = 16 + shipH / 2;
        if (ship.y + shipH / 2 > H - 16) ship.y = H - 16 - shipH / 2;
        };

    auto SpawnAsteroid = [&]() {
        float r = static_cast<float>(GetRandomValue(18, 34));
        float x = static_cast<float>(GetRandomValue(static_cast<int>(r), W - static_cast<int>(r)));
        float y = -r - 10;
        float vy = static_cast<float>(GetRandomValue(120, 220)) + score * 0.05f;
        float vx = static_cast<float>(GetRandomValue(-50, 50));
        asts.push_back({ {x, y}, {vx, vy}, r, true });
        };

    auto Explode = [&](Vector2 at, int count) {
        for (int i = 0; i < count; i++) {
            float a = GetRandomValue(0, 628) / 100.0f;
            float s = static_cast<float>(GetRandomValue(60, 180));
            Vector2 v = { cosf(a) * s, sinf(a) * s };
            float life = GetRandomValue(20, 45) / 60.0f;
            Color c = { static_cast<unsigned char>(GetRandomValue(200, 255)),
                        static_cast<unsigned char>(GetRandomValue(120, 200)),
                        static_cast<unsigned char>(GetRandomValue(0, 60)), 255 };
            parts.push_back({ at, v, life, life, c, true });
        }
        };

    auto ResetGame = [&]() {
        bullets.clear(); asts.clear(); parts.clear();
        ship = { static_cast<float>(W) / 2, static_cast<float>(H) - 80 };
        lives = 3; score = 0;
        invuln = false; invulnT = 0.0f; blinkT = 0.0f;
        spawnT = 0.0f; spawnCD = 0.9f; diffT = 0.0f;
        fireTimer = 0.0f;
        };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        for (auto& s : stars) {
            s.pos.y += s.speed * dt;
            if (s.pos.y > H) s.pos = { static_cast<float>(GetRandomValue(0, W)), -5.0f };
        }

        if (scene == Scene::LOADING) {
            loadT += dt;
            if (loadT >= loadDur) scene = Scene::MENU;
        }
        else if (scene == Scene::MENU) {
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) menuIndex = (menuIndex + 2) % 3;
            if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) menuIndex = (menuIndex + 1) % 3;
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                if (menuIndex == 0) { ResetGame(); scene = Scene::PLAY; }
                else if (menuIndex == 1) { OpenURL("https://www.raylib.com/"); }
                else if (menuIndex == 2) break;
            }
        }
        else if (scene == Scene::PLAY) {
            fireTimer += dt; diffT += dt; spawnT += dt;
            if (diffT > 6.0f) { diffT = 0.0f; spawnCD = std::max(0.45f, spawnCD - 0.05f); }

            Vector2 dir = { 0.0f, 0.0f };
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) dir.x -= 1.0f;
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) dir.x += 1.0f;
            if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) dir.y -= 1.0f;
            if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) dir.y += 1.0f;
            float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
            if (len > 0) { dir.x /= len; dir.y /= len; }
            ship.x += dir.x * shipSpeed * dt;
            ship.y += dir.y * shipSpeed * dt;
            ClampShip();

            if (IsKeyDown(KEY_SPACE) && fireTimer >= fireCD) {
                fireTimer = 0.0f;
                bullets.push_back({ {ship.x - 10.0f, ship.y - shipH * 0.5f - 6.0f}, 4.0f, -640.0f, true });
                bullets.push_back({ {ship.x + 10.0f, ship.y - shipH * 0.5f - 6.0f}, 4.0f, -640.0f, true });
            }

            if (spawnT >= spawnCD) { spawnT = 0.0f; SpawnAsteroid(); if (GetRandomValue(0, 100) < 35) SpawnAsteroid(); }

            for (auto& b : bullets) if (b.alive) { b.p.y += b.vy * dt; if (b.p.y < -20.0f) b.alive = false; }
            for (auto& a : asts) if (a.alive) {
                a.p.x += a.v.x * dt; a.p.y += a.v.y * dt;
                if (a.p.y > H + a.r) a.alive = false;
            }

            for (auto& a : asts) if (a.alive) {
                for (auto& b : bullets) if (b.alive) {
                    if (CheckCollisionCircles(a.p, a.r, b.p, b.r)) {
                        b.alive = false; a.alive = false; score += 10;
                        Explode(a.p, 22);
                    }
                }
            }

            if (!invuln) {
                for (auto& a : asts) if (a.alive) {
                    Rectangle shipRect = { ship.x - shipW * 0.35f, ship.y - shipH * 0.35f, shipW * 0.7f, shipH * 0.7f };
                    if (CheckCollisionCircleRec(a.p, a.r * 0.85f, shipRect)) {
                        a.alive = false; lives--; invuln = true; invulnT = 0.0f; blinkT = 0.0f; Explode(ship, 28);
                        break;
                    }
                }
            }
            else { invulnT += dt; blinkT += dt; if (invulnT >= invulnDur) { invuln = false; blinkT = 0.0f; } }

            for (auto& p : parts) if (p.alive) { p.life -= dt; p.p.x += p.v.x * dt; p.p.y += p.v.y * dt; if (p.life <= 0.0f) p.alive = false; }
            bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.alive; }), bullets.end());
            asts.erase(std::remove_if(asts.begin(), asts.end(), [](const Asteroid& a) { return !a.alive; }), asts.end());
            parts.erase(std::remove_if(parts.begin(), parts.end(), [](const Particle& p) { return !p.alive; }), parts.end());

            if (IsKeyPressed(KEY_P)) scene = Scene::PAUSE;
            if (lives <= 0) { highScore = std::max(highScore, score); scene = Scene::GAMEOVER; }
        }
        else if (scene == Scene::PAUSE) {
            if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) scene = Scene::PLAY;
            if (IsKeyPressed(KEY_ENTER)) scene = Scene::MENU;
        }
        else if (scene == Scene::GAMEOVER) {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) { ResetGame(); scene = Scene::PLAY; }
            if (IsKeyPressed(KEY_ESCAPE)) scene = Scene::MENU;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (txBg.id) {
            DrawTexturePro(txBg, { 0.0f, 0.0f, static_cast<float>(txBg.width), static_cast<float>(txBg.height) },
                { 0.0f, 0.0f, static_cast<float>(W), static_cast<float>(H) }, { 0.0f, 0.0f }, 0.0f, WHITE);
        }
        else {
            for (auto& s : stars) DrawRectangle(static_cast<int>(s.pos.x), static_cast<int>(s.pos.y), s.size, s.size, WHITE);
        }

        if (scene == Scene::LOADING) {
            float p = Clamp(loadT / loadDur, 0.0f, 1.0f);
            const char* t = "Loading...";
            int tw = MeasureText(t, 40);
            DrawText(t, W / 2 - tw / 2, H / 2 - 80, 40, RAYWHITE);
            DrawRectangle(W / 2 - 220, H / 2 - 10, 440, 24, DARKGRAY);
            DrawRectangle(W / 2 - 218, H / 2 - 8, static_cast<int>(436 * p), 20, WHITE);
            DrawText(TextFormat("%d%%", static_cast<int>(p * 100)), W / 2 - 18, H / 2 + 24, 20, RAYWHITE);
        }
        else if (scene == Scene::MENU) {
            if (txLogo.id) {
                float s = 0.6f;
                DrawTexturePro(txLogo, { 0.0f, 0.0f, static_cast<float>(txLogo.width), static_cast<float>(txLogo.height) },
                    { static_cast<float>(W) / 2, 140.0f, txLogo.width * s, txLogo.height * s },
                    { static_cast<float>(txLogo.width) * s / 2, static_cast<float>(txLogo.height) * s / 2 }, 0.0f, WHITE);
            }
            else {
                const char* t = "SPACE SHOOTER";
                int tw = MeasureText(t, 56);
                DrawText(t, W / 2 - tw / 2, 110, 56, RAYWHITE);
            }
            const char* items[3] = { "Start Game", "Raylib Website", "Quit" };
            for (int i = 0; i < 3; i++) {
                int fw = 360, fh = 54;
                int bx = W / 2 - fw / 2, by = 240 + i * 70;
                Color bg = i == menuIndex ? GREEN : RED;
                if (txBtn.id) DrawTexturePro(txBtn, { 0.0f, 0.0f, static_cast<float>(txBtn.width), static_cast<float>(txBtn.height) },
                    { static_cast<float>(bx), static_cast<float>(by), static_cast<float>(fw), static_cast<float>(fh) },
                    { 0.0f, 0.0f }, 0.0f, WHITE);
                else DrawRectangle(bx, by, fw, fh, bg);
                int tw = MeasureText(items[i], 28);
                DrawText(items[i], W / 2 - tw / 2, by + (fh - 28) / 2, 28, i == menuIndex ? BLACK : RAYWHITE);
            }
            DrawText("UP/DOWN to navigate • ENTER to select", W / 2 - 240, H - 60, 20, WHITE);
        }
        else if (scene == Scene::PLAY || scene == Scene::PAUSE) {
            for (auto& p : parts) {
                float a = static_cast<unsigned char>(255 * (p.life / p.maxLife));
                Color c = p.c; c.a = static_cast<unsigned char>(std::min(255.0f, a));
                DrawCircleV(p.p, 2.0f, c);
            }
            for (auto& b : bullets) if (b.alive) DrawCircleV(b.p, b.r, WHITE);
            for (auto& a : asts) if (a.alive) {
                if (txAst.id) DrawTexturePro(txAst, { 0.0f, 0.0f, static_cast<float>(txAst.width), static_cast<float>(txAst.height) },
                    { a.p.x - a.r, a.p.y - a.r, a.r * 2.0f, a.r * 2.0f }, { 0.0f, 0.0f }, 0.0f, WHITE);
                else DrawCircleV(a.p, a.r, GREEN);
            }
            bool drawShip = true;
            if (invuln) { blinkT += dt; if (fmodf(blinkT, 0.12f) < 0.06f) drawShip = false; }
            if (drawShip) {
                if (txShip.id) DrawTexturePro(txShip, { 0.0f, 0.0f, static_cast<float>(txShip.width), static_cast<float>(txShip.height) },
                    { ship.x - shipW / 2, ship.y - shipH / 2, shipW, shipH }, { 0.0f, 0.0f }, 0.0f, WHITE);
                else DrawTriangle({ ship.x, ship.y - shipH * 0.5f }, { ship.x - shipW * 0.5f, ship.y + shipH * 0.5f }, { ship.x + shipW * 0.5f, ship.y + shipH * 0.5f }, BLUE);
            }
            DrawText(TextFormat("Score: %d", score), 16, 12, 24, RAYWHITE);
            DrawText(TextFormat("Lives: %d", lives), 16, 40, 24, RED);
            if (scene == Scene::PAUSE) {
                const char* t = "PAUSED";
                int tw = MeasureText(t, 48);
                DrawText(t, W / 2 - tw / 2, H / 2 - 40, 48, YELLOW);
                DrawText("Press P to resume • ENTER for menu", W / 2 - 220, H / 2 + 24, 20, RAYWHITE);
            }
        }
        else if (scene == Scene::GAMEOVER) {
            const char* t = "GAME OVER";
            int tw = MeasureText(t, 64);
            DrawText(t, W / 2 - tw / 2, 110, 64, RED);
            DrawText(TextFormat("Score: %d", score), W / 2 - 80, 200, 28, RAYWHITE);
            DrawText(TextFormat("High Score: %d", highScore), W / 2 - 110, 236, 28, YELLOW);
            DrawText("ENTER to play again", W / 2 - 130, 300, 22, WHITE);
            DrawText("ESC to return to menu", W / 2 - 140, 332, 22, WHITE);
        }

        EndDrawing();
    }

    if (txShip.id) UnloadTexture(txShip);
    if (txAst.id) UnloadTexture(txAst);
    if (txLogo.id) UnloadTexture(txLogo);
    if (txBg.id) UnloadTexture(txBg);
    if (txBtn.id) UnloadTexture(txBtn);
    CloseWindow();
    return 0;
}