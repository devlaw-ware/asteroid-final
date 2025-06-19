// main.c
#include "raylib.h"
#include <math.h>
#include <stdbool.h>

#define MAX_ASTEROIDES 8 //TESTE TESTE
#define MAX_TIROS 10

typedef struct {
    Vector2 posicao;
    Vector2 velocidade;
    int tamanho; // 3=grande, 2=médio, 1=pequeno
    float raio;
    bool ativo;
} Asteroide;

typedef struct {
    Vector2 posicao;
    Vector2 velocidade;
    float rotacao;
    Vector2 aceleracao;
    bool ativo;
} Nave;

typedef struct {
    Vector2 posicao;
    Vector2 velocidade;
    bool ativo;
} Tiro;

Nave nave;
Asteroide asteroides[MAX_ASTEROIDES];
Tiro tiros[MAX_TIROS];
int pontuacao = 0;
int vidas = 3;

void IniciarJogo();
void AtualizarJogo();
void DesenharJogo();
void CriarAsteroide(int index, Vector2 pos, int tamanho);
void AtualizarAsteroides();
void AtualizarTiros();
void VerificarColisoes();

int main() {
    InitWindow(800, 600, "Asteroides BR");
    SetTargetFPS(60);

    IniciarJogo();

    while (!WindowShouldClose()) {
        AtualizarJogo();
        DesenharJogo();
    }

    CloseWindow();
    return 0;
}

void IniciarJogo() {
    nave = (Nave){
        .posicao = {400, 300},
        .velocidade = {0, 0},
        .rotacao = 0,
        .aceleracao = {0, 0},
        .ativo = true
    };

    pontuacao = 0;
    vidas = 3;

    for (int i = 0; i < MAX_ASTEROIDES; i++) {
        CriarAsteroide(i, (Vector2){GetRandomValue(0, 800), GetRandomValue(0, 600)}, 3);
    }

    for (int i = 0; i < MAX_TIROS; i++) {
        tiros[i].ativo = false;
    }
}

void CriarAsteroide(int index, Vector2 pos, int tamanho) {
    asteroides[index].posicao = pos;
    asteroides[index].velocidade = (Vector2){
        (float)GetRandomValue(-15, 15) / 10.0f,
        (float)GetRandomValue(-15, 15) / 10.0f
    };
    asteroides[index].tamanho = tamanho;
    asteroides[index].ativo = true;

    switch (tamanho) {
        case 3: asteroides[index].raio = 40; break;
        case 2: asteroides[index].raio = 25; break;
        case 1: asteroides[index].raio = 15; break;
        default: asteroides[index].raio = 20; break;
    }
}

void AtualizarJogo() {
    // Controle WASD corrigido: W diminui Y (sobe), S aumenta Y (desce)
    Vector2 direcao = {0, 0};
    if (IsKeyDown(KEY_W)) direcao.y -= 1; // sobe
    if (IsKeyDown(KEY_S)) direcao.y += 1; // desce
    if (IsKeyDown(KEY_A)) direcao.x -= 1;
    if (IsKeyDown(KEY_D)) direcao.x += 1;

    if (direcao.x != 0 || direcao.y != 0) {
        float len = sqrtf(direcao.x * direcao.x + direcao.y * direcao.y);
        direcao.x /= len;
        direcao.y /= len;

        nave.aceleracao.x = direcao.x * 0.3f;
        nave.aceleracao.y = direcao.y * 0.3f;

        // Atualiza rotação para apontar na direção do movimento
        nave.rotacao = atan2f(-direcao.y, direcao.x) * (180.0f / PI) + 90.0f;
    } else {
        nave.aceleracao.x = 0;
        nave.aceleracao.y = 0;
        nave.velocidade.x *= 0.95f;
        nave.velocidade.y *= 0.95f;
    }

    nave.velocidade.x += nave.aceleracao.x;
    nave.velocidade.y += nave.aceleracao.y;

    float maxVel = 5.0f;
    if (nave.velocidade.x > maxVel) nave.velocidade.x = maxVel;
    if (nave.velocidade.x < -maxVel) nave.velocidade.x = -maxVel;
    if (nave.velocidade.y > maxVel) nave.velocidade.y = maxVel;
    if (nave.velocidade.y < -maxVel) nave.velocidade.y = -maxVel;

    nave.posicao.x += nave.velocidade.x;
    nave.posicao.y += nave.velocidade.y;

    // Bater nas bordas sem ultrapassar
    if (nave.posicao.x < 0) {
        nave.posicao.x = 0;
        nave.velocidade.x = 0;
    }
    if (nave.posicao.x > 800) {
        nave.posicao.x = 800;
        nave.velocidade.x = 0;
    }
    if (nave.posicao.y < 0) {
        nave.posicao.y = 0;
        nave.velocidade.y = 0;
    }
    if (nave.posicao.y > 600) {
        nave.posicao.y = 600;
        nave.velocidade.y = 0;
    }

    // Tiro com clique esquerdo do mouse
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i = 0; i < MAX_TIROS; i++) {
            if (!tiros[i].ativo) {
                tiros[i].ativo = true;
                tiros[i].posicao = nave.posicao;
                tiros[i].velocidade.x = (float)(sin(nave.rotacao * DEG2RAD) * 10);
                tiros[i].velocidade.y = (float)(-cos(nave.rotacao * DEG2RAD) * 10);
                break;
            }
        }
    }

    AtualizarAsteroides();
    AtualizarTiros();
    VerificarColisoes();

    // Reaparecer asteroides se a maioria destruída
    int ativos = 0;
    for (int i = 0; i < MAX_ASTEROIDES; i++) {
        if (asteroides[i].ativo) ativos++;
    }
    if (ativos <= MAX_ASTEROIDES / 3) {
        for (int i = 0; i < MAX_ASTEROIDES; i++) {
            if (!asteroides[i].ativo) {
                Vector2 pos = {GetRandomValue(0, 800), GetRandomValue(0, 600)};
                CriarAsteroide(i, pos, 3);
            }
        }
    }
}

void AtualizarAsteroides() {
    for (int i = 0; i < MAX_ASTEROIDES; i++) {
        if (!asteroides[i].ativo) continue;

        asteroides[i].posicao.x += asteroides[i].velocidade.x;
        asteroides[i].posicao.y += asteroides[i].velocidade.y;

        // Wrap-around dos asteroides na tela
        if (asteroides[i].posicao.x > 800) asteroides[i].posicao.x = 0;
        if (asteroides[i].posicao.x < 0) asteroides[i].posicao.x = 800;
        if (asteroides[i].posicao.y > 600) asteroides[i].posicao.y = 0;
        if (asteroides[i].posicao.y < 0) asteroides[i].posicao.y = 600;
    }
}

void AtualizarTiros() {
    for (int i = 0; i < MAX_TIROS; i++) {
        if (!tiros[i].ativo) continue;

        tiros[i].posicao.x += tiros[i].velocidade.x;
        tiros[i].posicao.y += tiros[i].velocidade.y;

        if (tiros[i].posicao.x < 0 || tiros[i].posicao.x > 800 || tiros[i].posicao.y < 0 || tiros[i].posicao.y > 600) {
            tiros[i].ativo = false;
        }
    }
}

void VerificarColisoes() {
    for (int i = 0; i < MAX_TIROS; i++) {
        if (!tiros[i].ativo) continue;

        for (int j = 0; j < MAX_ASTEROIDES; j++) {
            if (!asteroides[j].ativo) continue;

            float dx = tiros[i].posicao.x - asteroides[j].posicao.x;
            float dy = tiros[i].posicao.y - asteroides[j].posicao.y;
            float distancia = sqrtf(dx*dx + dy*dy);

            if (distancia < asteroides[j].raio) {
                tiros[i].ativo = false;

                if (asteroides[j].tamanho > 1) {
                    int tamanhoNovo = asteroides[j].tamanho - 1;

                    for (int k = 0; k < 2; k++) {
                        for (int l = 0; l < MAX_ASTEROIDES; l++) {
                            if (!asteroides[l].ativo) {
                                Vector2 posNovo = {
                                    asteroides[j].posicao.x + GetRandomValue(-10, 10),
                                    asteroides[j].posicao.y + GetRandomValue(-10, 10)
                                };
                                CriarAsteroide(l, posNovo, tamanhoNovo);
                                break;
                            }
                        }
                    }
                }

                asteroides[j].ativo = false;
                pontuacao += 100;
                break;
            }
        }
    }

    for (int i = 0; i < MAX_ASTEROIDES; i++) {
        if (!asteroides[i].ativo || !nave.ativo) continue;

        float dx = nave.posicao.x - asteroides[i].posicao.x;
        float dy = nave.posicao.y - asteroides[i].posicao.y;
        float distancia = sqrtf(dx*dx + dy*dy);

        if (distancia < asteroides[i].raio + 10) {
            vidas--;
            nave.ativo = false;
        }
    }
}

void DesenharJogo() {
    BeginDrawing();
    ClearBackground(BLACK);

    if (nave.ativo) {
        Vector2 vertices[] = {
            {(float)(sin(nave.rotacao * DEG2RAD) * 20), (float)(-cos(nave.rotacao * DEG2RAD) * 20)},
            {(float)(sin((nave.rotacao + 150) * DEG2RAD) * 20), (float)(-cos((nave.rotacao + 150) * DEG2RAD) * 20)},
            {(float)(sin((nave.rotacao - 150) * DEG2RAD) * 20), (float)(-cos((nave.rotacao - 150) * DEG2RAD) * 20)}
        };

        DrawTriangleLines(
            (Vector2){nave.posicao.x + vertices[0].x, nave.posicao.y + vertices[0].y},
            (Vector2){nave.posicao.x + vertices[1].x, nave.posicao.y + vertices[1].y},
            (Vector2){nave.posicao.x + vertices[2].x, nave.posicao.y + vertices[2].y},
            WHITE
        );
    }

    for (int i = 0; i < MAX_ASTEROIDES; i++) {
        if (!asteroides[i].ativo) continue;

        DrawCircleLines((int)asteroides[i].posicao.x, (int)asteroides[i].posicao.y, asteroides[i].raio, WHITE);
    }

    for (int i = 0; i < MAX_TIROS; i++) {
        if (!tiros[i].ativo) continue;

        DrawCircle((int)tiros[i].posicao.x, (int)tiros[i].posicao.y, 3, RED);
    }

    DrawText(TextFormat("Pontuação: %d", pontuacao), 10, 10, 20, WHITE);
    DrawText(TextFormat("Vidas: %d", vidas), 10, 40, 20, WHITE);

    if (!nave.ativo) {
        DrawText("Nave destruída! Pressione R para reiniciar.", 200, 300, 20, RED);
        if (IsKeyPressed(KEY_R)) {
            IniciarJogo();
        }
    }

    EndDrawing();
}
