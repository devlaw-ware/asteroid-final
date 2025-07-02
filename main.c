// main.c
#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum {
    MENU,
    JOGO,
    SAIR
} EstadoJogo;

EstadoJogo estadoAtual = MENU;


#define MAX_ASTEROIDES 8
#define MAX_TIROS 10

// ESTRUTURAS DE DADOS PRINCIPAIS

/* Representa os inimigos/obstáculos do jogo com:
- Posição e velocidade
- Tipo (define tamanho/comportamento)
- Hitbox para detecção de colisão
- Flag de estado ativo/inativo */

typedef struct {
    Vector2 posicao;
    Vector2 velocidade; 
    int tamanho;
    float raio;
    bool ativo; 
} Asteroide;

/* Controla a nave do jogador com:
- Posição, velocidade e aceleração
- Rotação para direcionamento
- Estado de atividade (vivo/morto) */

typedef struct {
    Vector2 posicao;
    Vector2 velocidade;
    float rotacao;
    Vector2 aceleracao;
    bool ativo;
} Nave;

/* Gerencia os projéteis disparados com:
- Posição e velocidade constante
- Flag para controle de projéteis ativos */

typedef struct {
    Vector2 posicao; // Posição atual do tiro
    Vector2 velocidade; // Velocidade constante do tiro
    bool ativo; // Indica se o tiro está ativo e deve ser renderizado
} Tiro;

// VARIÁVEIS GLOBAIS

/* Contém todos os elementos do jogo:
- Instância do jogador
- Array de inimigos com limite máximo
- Array de projéteis ativos
- Pontuação e vidas do jogador */

Nave nave;
Asteroide asteroides[MAX_ASTEROIDES];
Tiro tiros[MAX_TIROS];
int pontuacao = 0;
int vidas = 3;

// PROTÓTIPOS DE FUNÇÕES

/* Funções principais do game loop:
- IniciarJogo: Inicializa/reseta o estado do jogo
- AtualizarJogo: Lógica principal por frame
- DesenharJogo: Renderização gráfica */

void IniciarJogo();
void AtualizarJogo();
void DesenharJogo(); 

/* Funções de gerenciamento específico:
- CriarAsteroide: Cria inimigos em posições específicas
- AtualizarAsteroides/AtualizarTiros: Atualiza estados
- VerificarColisoes: Detecção de colisões entre entidades */

void CriarAsteroide(int index, Vector2 pos, int tamanho);
void AtualizarAsteroides(); 
void AtualizarTiros();
void VerificarColisoes();

/* Programa principal que controla o ciclo de vida do jogo:
- Inicializa a janela e sistemas
- Executa o loop principal de jogo
- Gerencia recursos até o encerramento */

int main() {
    InitWindow(800, 600, "Asteroides Shapes ");
    SetTargetFPS(60);

    IniciarJogo();

    while (!WindowShouldClose() && estadoAtual != SAIR) {
        if (estadoAtual == MENU) {
            // Atualiza o menu
            if (IsKeyPressed(KEY_ONE)) {
                IniciarJogo();
                estadoAtual = JOGO;
            } else if (IsKeyPressed(KEY_TWO)) {
                estadoAtual = SAIR;
            }
            // Desenha o menu
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("ASTEROIDS LAB-AED", 220, 120, 40, WHITE);
            DrawText("1 - Jogar", 320, 250, 30, GREEN);
            DrawText("2 - Sair", 320, 300, 30, RED);
            EndDrawing();
        } else if (estadoAtual == JOGO) {
            AtualizarJogo();
            DesenharJogo();

            // Se a nave morrer, volta ao menu
            if (!nave.ativo && vidas <= 0) {
                estadoAtual = MENU;
            }
        }
    }

    CloseWindow();
    return 0;
}

/* Inicializa todos os sistemas do jogo:
- Configura o jogador com valores padrão
- Prepara sistemas de pontuação e vidas
- Cria pool de inimigos e projéteis */

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
        CriarAsteroide(i, (Vector2){(float)GetRandomValue(0, 800), (float)GetRandomValue(0, 600)}, 3);
    } 

    for (int i = 0; i < MAX_TIROS; i++) {
        tiros[i].ativo = false;
    } 
}

/* Gera inimigos com configurações específicas:
- Posiciona em coordenadas aleatórias
- Define propriedades baseadas no tipo (tamanho)
- Configura sistemas de colisão adequados */

void CriarAsteroide(int index, Vector2 pos, int tamanho) {
    asteroides[index].posicao = pos; 
    asteroides[index].velocidade = (Vector2){ 
        (float)GetRandomValue(-15, 15) / 10.0f,
        (float)GetRandomValue(-15, 15) / 10.0f
    }; 
    asteroides[index].tamanho = tamanho;
    asteroides[index].ativo = true; 

    switch (tamanho) {
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
    // Controle WASD corrigido: W = frente (sobe), S = ré (desce)
    Vector2 direcao = {0, 0};
    if (IsKeyDown(KEY_W)) direcao.y -= 1; // W = frente (sobe)
    if (IsKeyDown(KEY_S)) direcao.y += 1; // S = ré (desce)
    if (IsKeyDown(KEY_A)) direcao.x -= 1; // esquerda
    if (IsKeyDown(KEY_D)) direcao.x += 1; // direita

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
                Vector2 pos = {(float)GetRandomValue(0, 800), (float)GetRandomValue(0, 600)};
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
                                    asteroides[j].posicao.x + (float)GetRandomValue(-10, 10),
                                    asteroides[j].posicao.y + (float)GetRandomValue(-10, 10)
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
            - Se ainda houver vidas, reposiciona a nave.
            - Se vidas chegar a zero, desativa a nave.
    */
    for (int i = 0; i < MAX_ASTEROIDES; i++) {
        if (!asteroides[i].ativo || !nave.ativo) continue;

        float dx = nave.posicao.x - asteroides[i].posicao.x;
        float dy = nave.posicao.y - asteroides[i].posicao.y;
        float distancia = sqrtf(dx*dx + dy*dy);

        if (distancia < asteroides[i].raio + 10) {
            vidas--;
            if (vidas > 0) {
                // Reposiciona e reseta a nave para continuar jogando
                nave.posicao = (Vector2){400, 300};
                nave.velocidade = (Vector2){0, 0};
                nave.aceleracao = (Vector2){0, 0};
                nave.rotacao = 0;
            } else {
                nave.ativo = false; // Só desativa quando vidas acabam
            }
            break; // Importante para evitar múltiplas colisões no mesmo frame
        }
    }
}

/*
Esta função chamada DesenharJogo é responsável por desenhar a tela do jogo a cada frame.

1. BeginDrawing(): Inicia o processo de desenho na tela, preparando para atualizar o que será mostrado.

2. ClearBackground(BLACK): Limpa a tela preenchendo com a cor preta, para apagar o que foi desenhado antes.

3. if (nave.ativo): Verifica se a nave está ativa (ou seja, se deve ser desenhada). Se não estiver ativa, nada é desenhado.

4. Vector2 vertices[] = {...}: Calcula os três vértices de um triângulo que representa a nave.
   - Cada vértice é calculado usando funções trigonométricas (seno e cosseno) para determinar a posição dos pontos considerando a rotação da nave.
   - A nave é desenhada como um triângulo com tamanho fixo (20 unidades) apontando na direção da rotação atual.
   - Os vértices são:
     * A ponta da nave (frente), alinhada com a rotação.
     * Dois vértices traseiros, posicionados a 150 graus para a direita e para a esquerda da rotação.

5. DrawTriangleLines(...): Desenha as linhas do triângulo na tela, ligando os três vértices calculados.
   - Os vértices são deslocados pela posição atual da nave (nave.posicao.x, nave.posicao.y).
   - A cor usada para desenhar o triângulo é branca (WHITE).

Resumindo: esta função limpa a tela e, se a nave estiver ativa, calcula e desenha um triângulo na posição e rotação atuais da nave, simulando sua forma e orientação no jogo.
*/

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

    // Desenha os asteroides
    for (int i = 0; i < MAX_ASTEROIDES; i++) {
        if (!asteroides[i].ativo) continue;
        DrawCircleLines(
            (int)asteroides[i].posicao.x,
            (int)asteroides[i].posicao.y,
            asteroides[i].raio,
            WHITE
        );
    }

    // Desenha os tiros
    for (int i = 0; i < MAX_TIROS; i++) {
        if (!tiros[i].ativo) continue;
        DrawCircle(
            (int)tiros[i].posicao.x,
            (int)tiros[i].posicao.y,
            3,
            RED
        );
    }

    // Exibe a pontuação e vidas
    DrawText(TextFormat("Pontuacao: %d", pontuacao), 10, 10, 20, YELLOW);
    DrawText(TextFormat("Vidas: %d", vidas), 10, 40, 20, GREEN);

    // Mensagem de reinício se perdeu todas as vidas
    if (!nave.ativo && vidas <= 0) {
        DrawText("Nave destruida! Pressione R para reiniciar.", 200, 300, 20, RED);
        if (IsKeyPressed(KEY_R)) {
            IniciarJogo();
        }
    }

    EndDrawing();
}
