#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define ScreenWidth 1200
#define ScreenHeight 600
#define Gravity 1
#define MAX_LINHAS 10
#define MAX_COLUNAS 200
#define TIROWIDTH 80
#define TIROHEIGHT 6







// Estruturas
typedef struct
{
    float height;
    float width;
    int x;
    int y;
    int vel_x;
    int vel_y;
    int vidas;
    int pontos;
    bool movimento;
    bool pulo;
    bool tiro;
} PERSONAGEM;

typedef struct
{
    float height;
    float width;
    int x;
    int y;
    int vel_x;
    int vel_y;
} ENEMIES;

typedef struct
{
    float height;
    float width;
    int x;
    int y;
} OBSTACULOS;

typedef struct
{
    float height;
    float width;
    int x_esquerda;
    int x_direita;
    int y;
    int vel_x;
    bool atirando;
} TIROS;

typedef struct
{
    char mapa[MAX_LINHAS][MAX_COLUNAS]; // Matriz do mapa
    int largura;
    int altura;
} Mapa;
//--------------------------------------------------------

//Funcoes

//FUNCOES DO MENU ------------------------------------------------------------
void desenhaMenu(int posicao_mouse, Texture2D menu, Texture2D logo)
{
    Rectangle botoes[] =
    {
        {ScreenWidth / 2 - 100, 400, 200, 40},
        {ScreenWidth / 2 - 100, 400 + 50, 200, 40},
        {ScreenWidth / 2 - 100, 400 + 2 * 50, 200, 40}
    };

    const char* textoBotao[] = {"Jogar", "Sair", "Ranking"};

    // Desenhando os botões
    for (int i = 0; i < 3; i++)
    {
        Color corBotao = (posicao_mouse == i + 1) ? DARKGRAY : GRAY;
        DrawRectangleRec(botoes[i], corBotao);
        DrawText(textoBotao[i], botoes[i].x + (botoes[i].width / 2 - MeasureText(textoBotao[i], 20) / 2), botoes[i].y + 10, 20, WHITE);
    }

    // Desenhando as imagens do menu
    DrawTexture(logo, ScreenWidth / 4, 1, WHITE);
    DrawTexture(menu, ScreenWidth / 15 - 100, ScreenHeight / 4, WHITE);
    DrawText("MADE BY KEVIN BENDER AND MIGUEL GOMES", 800, 550, 16, WHITE);
}

//posicao dos botoes
int verificarPosicaoBotao(Vector2 mousePosition, Rectangle botoes[], int numBotoes)
{
    for (int i = 0; i < numBotoes; i++)
    {
        if (CheckCollisionPointRec(mousePosition, botoes[i]))
        {
            return i + 1; // Retorna a posição do botão (1, 2, 3, ...)
        }
    }
    return 0; // Nenhum botão pressionado
}
//---------------------------------------------------------------------------------------

//FUNCOES DO MAPA ------------------------------------------------------------------------
void carregarMapa(Mapa* mapa)
{
    FILE* file = fopen("bin/debug/resources/terreno.txt", "r");
    if (file == NULL)
    {
        printf("Erro ao abrir o arquivo de mapa!\n");
        return;
    }

    int linha = 0;
    while (fgets(mapa->mapa[linha], MAX_COLUNAS, file) != NULL && linha < MAX_LINHAS)
    {
        linha++;
    }

    mapa->altura = linha; // Definindo a altura do mapa
    mapa->largura = strlen(mapa->mapa[0]) - 1; // Definindo a largura do mapa
    fclose(file);
}


void desenharMapa(Mapa* mapa, Texture2D piso, Texture2D espinho, Texture2D inimigo)
{

    for (int i = 0; i < mapa->altura; i++)
    {
        for (int j = 0; j < mapa->largura; j++)
        {
            char tile = mapa->mapa[i][j];

            // Posições dos blocos
            int x = j * 48;  // Tamanho de cada bloco (32x32 pixels)
            int y = i * 48;

            switch (tile)
            {
            case 'S': // Espinho
                DrawTextureEx(espinho, (Vector2)
                {
                    x,y
                }, 0.0f, 3.0f, WHITE);
                break;
            case 'O': // Piso
                DrawTextureEx(piso,(Vector2)
                {
                    x,y
                }, 0.0f, 3.0f, WHITE);

                break;
            case 'P': // Posição de nascimento do jogador
                DrawRectangle(x, y, 48, 48, BLUE);
                break;
            case 'M': // Inimigo
                DrawTextureEx(inimigo, (Vector2)
                {
                    x,y
                }, 0.0f, 3.0f, WHITE);
                //rawRectangle(x, y, 48, 48, GREEN);
                break;
            case ' ': // Espaço vazio
                break; // Não desenha nada
            }
        }
    }
}
//------------------------------------------------------------------------------------

//FUNCOES DE MOVIMENTO --------------------------------------------------------------
int checarpiso(Mapa *mapa, PERSONAGEM *personagem)
{

    Rectangle personagemRec =
    {
        personagem->x,
        personagem->y,
        personagem->width,
        personagem->height
    };

    for (int i = 0; i < mapa->altura; i++)
    {
        for (int j = 0; j < mapa->largura; j++)
        {
            char tile = mapa->mapa[i][j];
            Rectangle tileRec =
            {
                j * 48,
                i * 48,
                48,
                48
            };

            if (tile == 'O' && CheckCollisionRecs(personagemRec, tileRec))
            {
                return 1; // Colisão com o piso
            }
            else if ((tile == 'S' || tile == 'M') && CheckCollisionRecs(personagemRec, tileRec))
            {
                return -1; // Colisão com espinho ou inimigo
            }
        }
    }

    return 0; // Sem colisão
}







//-------------------------------------------------------------------------------------


int main(void)
{
    // Inicialização
    InitWindow(ScreenWidth, ScreenHeight, "INF MAN - PRODUCED BY NAO INTENDO BRASIL INC");
    InitAudioDevice();
    SetTargetFPS(60);
    Color backgroundColor = BLACK;
    //

    // LOADS ----------------------------------------------------------------------------
    Texture2D menu = LoadTexture("bin/Debug/resources/imagem_menu.png");
    Texture2D logo = LoadTexture("bin/Debug/resources/logo.png");
    Texture2D background = LoadTexture("bin/debug/resources/background.png");
    Sound menusound = LoadSound("bin/Debug/resources/menusound.wav");
    //girando a imagem do boneco parado ~~~~~~~~~~~~~~~~~
    Image parado = LoadImage ("bin/debug/resources/Megaman.png");
    Texture2D parado_esquerda = LoadTextureFromImage(parado);
    ImageFlipHorizontal (&parado);
    Texture2D parado_direita = LoadTextureFromImage(parado);
    UnloadImage(parado);
    //girando a imagem do boneco parado Tiro ~~~~~~~~~~~~~~~~~
    Image paradoTiro = LoadImage ("bin/debug/resources/MegamanTiro.png");
    Texture2D parado_esquerdaTiro = LoadTextureFromImage(paradoTiro);
    ImageFlipHorizontal (&paradoTiro);
    Texture2D parado_direitaTiro = LoadTextureFromImage(paradoTiro);
    UnloadImage(paradoTiro);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //girando a imagem do boneco correndo ~~~~~~~~~~~~~~~
    Image correndo1 = LoadImage ("bin/debug/resources/MegamanCorre1.png");
    Image correndo2 = LoadImage ("bin/debug/resources/MegamanCorre2.png");
    Image correndo3 = LoadImage ("bin/debug/resources/MegamanCorre3.png");
    Texture2D correndo1_esquerda = LoadTextureFromImage(correndo1);
    Texture2D correndo2_esquerda = LoadTextureFromImage(correndo2);
    Texture2D correndo3_esquerda = LoadTextureFromImage(correndo3);
    ImageFlipHorizontal(&correndo1);
    ImageFlipHorizontal(&correndo2);
    ImageFlipHorizontal(&correndo3);
    Texture2D correndo1_direita = LoadTextureFromImage(correndo1);
    Texture2D correndo2_direita = LoadTextureFromImage(correndo2);
    Texture2D correndo3_direita = LoadTextureFromImage(correndo3);
    UnloadImage(correndo1);
    UnloadImage(correndo2);
    UnloadImage(correndo3);
    //girando a imagem do boneco correndo Tiro ~~~~~~~~~~~~~~~
    Image correndo1Tiro = LoadImage ("bin/debug/resources/MegamanTiroCorre1.png");
    Image correndo2Tiro = LoadImage ("bin/debug/resources/MegamanTiroCorre2.png");
    Image correndo3Tiro = LoadImage ("bin/debug/resources/MegamanTiroCorre3.png");
    Texture2D correndo1_esquerdaTiro = LoadTextureFromImage(correndo1Tiro);
    Texture2D correndo2_esquerdaTiro = LoadTextureFromImage(correndo2Tiro);
    Texture2D correndo3_esquerdaTiro = LoadTextureFromImage(correndo3Tiro);
    ImageFlipHorizontal(&correndo1Tiro);
    ImageFlipHorizontal(&correndo2Tiro);
    ImageFlipHorizontal(&correndo3Tiro);
    Texture2D correndo1_direitaTiro = LoadTextureFromImage(correndo1Tiro);
    Texture2D correndo2_direitaTiro = LoadTextureFromImage(correndo2Tiro);
    Texture2D correndo3_direitaTiro = LoadTextureFromImage(correndo3Tiro);
    UnloadImage(correndo1Tiro);
    UnloadImage(correndo2Tiro);
    UnloadImage(correndo3Tiro);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //girando a imagem do boneco pulando~~~~~~~~~~~~~~~~~~
    Image pulando = LoadImage ("bin/debug/resources/MegamanPulo.png");
    Texture2D pulando_direita = LoadTextureFromImage(pulando);
    ImageFlipHorizontal(&pulando);
    Texture2D pulando_esquerda = LoadTextureFromImage(pulando);
    UnloadImage(pulando);
    //girando a imagem do boneco pulando Tiro~~~~~~~~~~~~~~~~~~
    Image pulandoTiro = LoadImage ("bin/debug/resources/MegamanPuloTiro.png");
    Texture2D pulando_direitaTiro = LoadTextureFromImage(pulandoTiro);
    ImageFlipHorizontal(&pulandoTiro);
    Texture2D pulando_esquerdaTiro = LoadTextureFromImage(pulandoTiro);
    UnloadImage(pulandoTiro);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Texture2D piso = LoadTexture ("bin/debug/resources/Piso.png");
    Texture2D espinho = LoadTexture ("bin/debug/resources/Espinho.png");
    Texture2D inimigo = LoadTexture ("bin/debug/resources/inimigo.png");

    // -------------------------------------------------------------------------------------


    // DECLARANDO VAR -----------------------------------------------------------------------
    //botoes menu~~~~~~~~~~~~~~~
    Rectangle botoes[] =
    {
        {ScreenWidth / 2 - 100, 400, 200, 40},
        {ScreenWidth / 2 - 100, 400 + 50, 200, 40},
        {ScreenWidth / 2 - 100, 400 + 2 * 50, 200, 40}
    };
    //~~~~~~~~~~~~~~~~~~~~~~~~~~

    //personagem~~~~~~~~~~~~~~~~
    PERSONAGEM personagem;
    personagem.x = ScreenWidth / 2;
    personagem.y = ScreenHeight / 2;
    personagem.vel_x = 0;
    personagem.height=72;
    personagem.width=72;
    personagem.vel_y = Gravity;
    personagem.movimento = false;
    personagem.pulo = false;
    personagem.tiro = false;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~
    TIROS tiro;
    tiro.height=TIROHEIGHT;
    tiro.width=TIROWIDTH;
    tiro.vel_x=10;
    tiro.atirando=false;
    Mapa mapa;
    //camera~~~~~~~~~~~~~~~~~~~~
    Camera2D camera = {0};
    camera.offset = (Vector2)
    {
        0, 0
    };
    camera.target = (Vector2)
    {
        0.0f, 0.0f
    };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Animação do personagem~~~~~~~~
    float tempo=0;
    int frameAtual=0; //tem que ser int pra colocar dentro do array
    float frameDuracao=0.1;
    Texture2D correndo_direita[3]= {correndo1_direita, correndo2_direita, correndo3_direita};
    Texture2D correndo_esquerda[3]= {correndo1_esquerda, correndo2_esquerda, correndo3_esquerda};
    Texture2D correndo_direitaTiro[3]= {correndo1_direitaTiro, correndo2_direitaTiro, correndo3_direitaTiro};
    Texture2D correndo_esquerdaTiro[3]= {correndo1_esquerdaTiro, correndo2_esquerdaTiro, correndo3_esquerdaTiro};
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int tela = 0; // 0 = Menu, 1 = Jogo, 2 = Ranking
    int posicao_mouse = 0; // Nenhum botão selecionado inicialmente

    // ------------------------------------------------------------------------------------------------------



    while (!WindowShouldClose())
    {
        tempo += GetFrameTime();

        if (tempo >= frameDuracao)
        {
            frameAtual = (frameAtual + 1) % 3; // vai de 0 a 2
            tempo = 0;
        }

        // Verificar a posição do mouse
        Vector2 mousePosition = GetMousePosition();
        posicao_mouse = verificarPosicaoBotao(mousePosition, botoes, 3);

        if (tela == 0)   // Menu
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (posicao_mouse == 1)
                {
                    tela = 1; // Vai pro jogo
                }
                else if (posicao_mouse == 2)
                {
                    break; // Sai do jogo
                }
                else if (posicao_mouse == 3)
                {
                    tela = 2; // Vai para o ranking
                }
            }

            // Desenha o menu
            BeginDrawing();
            ClearBackground(backgroundColor);
            desenhaMenu(posicao_mouse, menu, logo);
            EndDrawing();
        }
        else if (tela == 1)   // Jogo
        {
            carregarMapa(&mapa);
            BeginDrawing();
            //Mover camera
            BeginMode2D(camera);
            if (personagem.x > ScreenWidth * 0.6)
            {
                camera.offset.x = -(personagem.x - ScreenWidth*0.6);
            }
            else if (personagem.x < ScreenWidth * 0.4)
            {
                camera.offset.x = -(personagem.x - ScreenWidth*0.4);
            }

            int i;
            for (i=0; i<20; i++)
            {
                DrawTextureEx(background,(Vector2)
                {
                    i*672,0             //672= largura da imagem
                }, 0.0f, 3.0f, WHITE); // Desenha o fundo do jogo
            }
            desenharMapa(&mapa, piso, espinho, inimigo);

            //MOVIMENTOS
            //Movimento horizontal
            int direcao;
            if (IsKeyDown(KEY_A))
            {
                personagem.movimento = true;
                personagem.vel_x=7;
                direcao = 1;
            }
            else if (IsKeyDown(KEY_D))
            {
                personagem.movimento=true;
                personagem.vel_x=-7;
                direcao = -1;
            }
            else
            {
                personagem.vel_x=0;
                personagem.movimento=false;
            }
            //
            //Movimento vertical
            if (IsKeyDown(KEY_W) && checarpiso(&mapa, &personagem))  //&& checarpiso(mapa, personagem, recpiso)
            {
                personagem.vel_y=20;
                personagem.pulo = true;
            }
            else if(checarpiso(&mapa,&personagem)) //checarpiso(mapa, personagem, recpiso)
            {
                personagem.vel_y=0;
                personagem.pulo = false;
            }
            else
            {
                personagem.vel_y-=Gravity;
            }
            personagem.x -= personagem.vel_x;
            personagem.y -= personagem.vel_y;
            Vector2 posicao_personagem = {personagem.x, personagem.y};
            //Tiro

            if (IsKeyPressed(KEY_SPACE))
            {
                tiro.x_esquerda = personagem.x + personagem.width;
                tiro.x_direita = personagem.x;
                personagem.tiro = true;
            }
            else
            {
                personagem.tiro = false;
            }
            //TERMINA MOVIMENTO
            //Inicia o tiro
            if (IsKeyDown(KEY_SPACE) && !tiro.atirando)
            {
                tiro.atirando = true;
            }

            if (tiro.atirando)
            {
                switch(direcao)
                {
                case -1:
                    tiro.x_esquerda+=tiro.vel_x;
                    DrawRectangle(tiro.x_esquerda, personagem.y + 27, tiro.width, tiro.height, YELLOW);
                    break;
                case 1:
                    tiro.x_direita-=tiro.vel_x;
                    DrawRectangle(tiro.x_direita, personagem.y + 27, tiro.width, tiro.height, YELLOW);
                    break;
                }

                if(personagem.pulo)
                {
                    if (direcao == -1)
                    {
                        DrawTextureEx(pulando_esquerda, posicao_personagem, 0.0f, 3.0f, WHITE);
                    }
                    else
                    {
                        DrawTextureEx(pulando_direita, posicao_personagem, 0.0f, 3.0f, WHITE);
                    }
                }
                else if(personagem.movimento)
                {
                    if (direcao == -1)
                    {
                        DrawTextureEx(correndo_direita[frameAtual], posicao_personagem, 0.0f, 3.0f, WHITE);
                    }
                    else
                    {
                        DrawTextureEx(correndo_esquerda[frameAtual], posicao_personagem, 0.0f, 3.0f, WHITE);
                    }
                }
                else
                {
                    if (direcao == -1)
                    {
                        DrawTextureEx(parado_direita, posicao_personagem, 0.0f, 3.0f, WHITE);
                    }
                    else
                    {
                        DrawTextureEx(parado_esquerda, posicao_personagem, 0.0f, 3.0f, WHITE);
                    }
                }
            }
            else if(personagem.pulo)
            {
                if (direcao == -1)
                {
                    DrawTextureEx(pulando_esquerda, posicao_personagem, 0.0f, 3.0f, WHITE);
                }
                else
                {
                    DrawTextureEx(pulando_direita, posicao_personagem, 0.0f, 3.0f, WHITE);
                }
            }
            else if(personagem.movimento)
            {
                if (direcao == -1)
                {
                    DrawTextureEx(correndo_direita[frameAtual], posicao_personagem, 0.0f, 3.0f, WHITE);
                }
                else
                {
                    DrawTextureEx(correndo_esquerda[frameAtual], posicao_personagem, 0.0f, 3.0f, WHITE);
                }
            }
            else
            {
                if (direcao == -1)
                {
                    DrawTextureEx(parado_direita, posicao_personagem, 0.0f, 3.0f, WHITE);
                }
                else
                {
                    DrawTextureEx(parado_esquerda, posicao_personagem, 0.0f, 3.0f, WHITE);
                }
            }
            if (tiro.x_esquerda>ScreenWidth)
            {
                tiro.x_esquerda=0;
                tiro.atirando=false;
            }
            EndDrawing();
            EndMode2D();
        }
        else if (tela == 2)   // Ranking
        {
            BeginDrawing();
            ClearBackground(WHITE);
            DrawText("Chamar a função imprime top5", ScreenWidth / 2 - 200, ScreenHeight / 2, 20, BLACK);
            EndDrawing();
        }
    }

//UNLOAD ----------------------------------
    UnloadSound(menusound);
    UnloadTexture(menu);
    UnloadTexture(logo);
    UnloadTexture(background);
    UnloadTexture (parado_esquerda);
    UnloadTexture (parado_direita);
    UnloadTexture (correndo1_direita);
    UnloadTexture (correndo2_direita);
    UnloadTexture (correndo3_direita);
    UnloadTexture (correndo1_esquerda);
    UnloadTexture (correndo2_esquerda);
    UnloadTexture (correndo3_esquerda);
    UnloadTexture (pulando_esquerda);
    UnloadTexture (pulando_direita);
    UnloadTexture (parado_esquerdaTiro);
    UnloadTexture (parado_direitaTiro);
    UnloadTexture (correndo1_direitaTiro);
    UnloadTexture (correndo2_direitaTiro);
    UnloadTexture (correndo3_direitaTiro);
    UnloadTexture (correndo1_esquerdaTiro);
    UnloadTexture (correndo2_esquerdaTiro);
    UnloadTexture (correndo3_esquerdaTiro);
    UnloadTexture (pulando_esquerdaTiro);
    UnloadTexture (pulando_direitaTiro);
    UnloadTexture (piso);
    UnloadTexture (espinho);
//--------------------------------------------



    CloseAudioDevice();
    CloseWindow();

    return 0;
}
