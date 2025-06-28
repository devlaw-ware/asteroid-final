// main.c
#include "raylib.h"
#include <math.h>
#include <stdbool.h>

#define MAX_ASTEROIDES 8
#define MAX_TIROS 10

/*
    Declaramos todos os atributos necessários para representar um asteroide no jogo
    incluido: Posição, movimento, tamanho e estado.
*/

typedef struct {
    Vector2 posicao; // Armazena as coordenadas (x,y) da posição do asteroide
    Vector2 velocidade; // Vetor que define a direção e velocidade do movimento
    int tamanho; // 3=grande, 2=médio, 1=pequeno (define o tamanho visual e de colisão)
    float raio; // Raio de colisão usado para detecção de impactos
    bool ativo; // Indica se o asteroide está atualmente no jogo (true/false)
} Asteroide;

// Definimos a estrutura da nave, que contém propriedades adicionais como rotação e acelaração, utilizados no controle do jogador

typedef struct {
    Vector2 posicao; // Posição atual da nave na tela
    Vector2 velocidade; // Velocidade atual da nave
    float rotacao; // Ângulo de rotação da nave (em graus ou radianos)
    Vector2 aceleracao; // Aceleração aplicada à nave (quando o jogador acelera)
    bool ativo; // Indica se a nave está ativa (o jogador ainda está vivo)
} Nave;

// Estrutura mais simples que representa os tiros disparados pela nave, com apenas posição, velocidade e estado.

typedef struct {
    Vector2 posicao; // Posição atual do tiro
    Vector2 velocidade; // Velocidade constante do tiro
    bool ativo; // Indica se o tiro está ativo e deve ser renderizado
} Tiro;

// Declaração de Variáveis Globais

Nave nave; // Cria uma instância da nave do jogador
Asteroide asteroides[MAX_ASTEROIDES]; // Array com capacidade para MAX_ASTEROIDES asteroides
Tiro tiros[MAX_TIROS]; // Array com capacidade para MAX_TIROS tiros simultâneos
int pontuacao = 0; // Pontuação do jogador, inicializada em 0
int vidas = 3; // Número de vidas do jogador, começa com 3

// Declaração de Funções

void IniciarJogo(); // Inicializa/reseta todos os elementos do jogo para o estado inicial
void AtualizarJogo(); // Contém a lógica principal de atualização do jogo (chamada a cada frame)
void DesenharJogo(); // Responsável por renderizar todos os elementos gráficos do jogo
void CriarAsteroide(int index, Vector2 pos, int tamanho); //  Instancia um novo asteroide na posição especificada
void AtualizarAsteroides(); // Atualiza posição e estado de todos os asteroides ativos
void AtualizarTiros(); // Atualiza posição e estado de todos os tiros ativos
void VerificarColisoes(); // Detecta colisões entre nave/tiros/asteroides e aplica as consequências

int main() {
    InitWindow(800, 600, "Asteroides BR"); // Cria uma janela de 800x600 pixels com o título "Asteroides BR"
    SetTargetFPS(60); // Define a taxa de atualização para 60 quadros por segundo

    IniciarJogo(); // Chama a função de inicialização do jogo

    while (!WindowShouldClose()) { // Loop principal do jogo (executa enquanto a janela não for fechada)
        AtualizarJogo(); // Atualiza a lógica do jogo
        DesenharJogo(); // Renderiza os elementos do jogo
    }

    CloseWindow(); // Fecha a janela quando o loop termina
    return 0;
}

void IniciarJogo() {
    nave = (Nave){ // Inicializa a estrutura da nave do jogador
        .posicao = {400, 300}, // Posição inicial no centro da tela (400,300)
        .velocidade = {0, 0}, // Velocidade inicial zero
        .rotacao = 0, // Sem rotação inicial
        .aceleracao = {0, 0}, // Sem aceleração inicial
        .ativo = true // Nave ativa
    };

    pontuacao = 0; // Zera a pontuação
    vidas = 3; // Define 3 vidas iniciais

    // A estrutura de repetição abaixo cria asteroides iniciais

    for (int i = 0; i < MAX_ASTEROIDES; i++) { 
        CriarAsteroide(i, (Vector2){GetRandomValue(0, 800), GetRandomValue(0, 600)}, 3);
    } // Cria asteroides de tamanho 3 em posições aleatórias na tela

    // A estrutura de repetição abaixo inicializa os tiros

    for (int i = 0; i < MAX_TIROS; i++) {
        tiros[i].ativo = false;
    } // Define todos os tiros como inativos inicialmente
}

void CriarAsteroide(int index, Vector2 pos, int tamanho) { //  Define a posição inicial do asteroide
    asteroides[index].posicao = pos; 
    asteroides[index].velocidade = (Vector2){ // Atribui uma velocidade aleatória entre -1.5 e 1.5 em ambos os eixos
        (float)GetRandomValue(-15, 15) / 10.0f, // Velocidade X aleatória
        (float)GetRandomValue(-15, 15) / 10.0f // Velocidade Y aleatória
    }; 
    asteroides[index].tamanho = tamanho; // Configura o tamanho do asteroide (3-grande, 2-médio, 1-pequeno)
    asteroides[index].ativo = true; // Ativa o asteroide para que ele apareça no jogo

    switch (tamanho) { // Define o raio de colisão baseado no tamanho do asteroide
        case 3: asteroides[index].raio = 40; 
                break; // Raio para asteroides grandes
        case 2: asteroides[index].raio = 25; 
                break; // Raio para asteroides médios
        case 1: asteroides[index].raio = 15; 
                break; // Raio para asteroides pequenos
        default: asteroides[index].raio = 20; 
                break; // Raio padrão para tamanhos inválidos
    } 
}

void AtualizarJogo() {
    //  Controle WASD corrigido: W diminui Y (sobe), S aumenta Y (desce)
    Vector2 direcao = {0, 0};
    if (IsKeyDown(KEY_W)) direcao.y += 1; // sobe
    if (IsKeyDown(KEY_S)) direcao.y -= 1; // desce
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
/*Percorre todos os asteroides do jogo.

Para cada asteroide ativo:

    - Atualiza sua posição, somando a velocidade atual nas direções X e Y.
    - Se o asteroide sair para fora da tela (direita, esquerda, em cima ou embaixo), ele reaparece do lado oposto (wrap-around), mantendo o movimento contínuo pelo espaço do 
    jogo.

*/

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

/*
    Função AtualizarTiros:
    - Percorre todos os tiros do jogo.
    - Para cada tiro ativo:
        - Atualiza sua posição somando a velocidade atual em X e Y.
        - Se o tiro sair fora da tela (fora dos limites 0-800 em X ou 0-600 em Y),
          desativa o tiro (tiros[i].ativo = false).
    - Isso garante que apenas tiros visíveis permaneçam ativos e se movam.
*/
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

/*
    Função VerificarColisoes:
    - Verifica colisões entre tiros ativos e asteroides ativos.
    - Para cada tiro:
        - Calcula a distância entre o tiro e o asteroide.
        - Se a distância for menor que o raio do asteroide (colisão):
            1. Desativa o tiro
            2. Se o asteroide for grande (tamanho > 1):
                - Divide em 2 asteroides menores (tamanho reduzido)
                - Cria novos asteroides em posições próximas com deslocamento aleatório
            3. Desativa o asteroide original
            4. Adiciona 100 pontos à pontuação
            5. Interrompe a verificação para este tiro (break)
*/
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

    /*
    Verificação de colisões entre a nave e os asteroides:
    - Percorre todos os asteroides.
    - Para cada asteroide ativo e enquanto a nave estiver ativa:
        - Calcula a distância entre o centro da nave e o centro do asteroide.
        - Se a distância for menor que o raio do asteroide mais 10 (tolerância para o tamanho da nave):
            - Diminui uma vida.
            - Desativa a nave (pode indicar fim de jogo ou respawn).
*/
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
// Rafael: comentar daqui pra baixo o resto do codigo e sua parte
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


