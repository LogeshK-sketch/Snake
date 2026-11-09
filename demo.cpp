#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

Color color = {100, 155, 100, 255};

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque){
    for(unsigned int i = 0; i<deque.size(); i++){
        if(Vector2Equals(deque[i], element)){
            return true;
        }
    }
    return false;
}

bool eventTriggered(double interval){
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval){
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Food{
    public:
        Vector2 position = {5,6};
        Texture2D texture;

        Food(deque<Vector2> snakeBody){
            Image image = LoadImage("images/football.png");
            texture = LoadTextureFromImage(image);
            UnloadImage(image);
            position = GenerateRandomPos(snakeBody);
        }        


        ~Food(){
            UnloadTexture(texture);
        }
        void Draw(){
            Rectangle source = {0, 0, (float)texture.width, (float)texture.height};

            Rectangle dest = {offset + position.x * cellSize, offset + position.y * cellSize, (float)cellSize, (float)cellSize};
            DrawTexturePro(texture, source, dest, Vector2{0, 0}, 0, WHITE);
        }

        Vector2 GenerateRandomCell(){
            float x = GetRandomValue(0, cellCount-1);
            float y = GetRandomValue(0, cellCount-1);
            return Vector2{x,y};
        }

        Vector2 GenerateRandomPos(deque<Vector2> snakeBody){

            Vector2 Position = GenerateRandomCell();
            while(ElementInDeque(position,  snakeBody)){
                position = GenerateRandomCell();
            }
            return position;
        }
};

class Snake{
    public:
        deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        Vector2 direction = {1,0};
        bool addSegment = false;

        void Draw(){
            for(unsigned int i = 0; i<body.size(); i++){
                float x = body[i].x;
                float y = body[i].y;
                Rectangle segment = Rectangle(offset+x*cellSize, offset+y*cellSize, cellSize, cellSize);
                DrawRectangleRounded(segment, 0.5, 6, DARKGREEN);
            }
        }

        void Update(){
            body.push_front(Vector2Add(body[0], direction));
            if(addSegment == true){

                addSegment = false;
            }else{
                body.pop_back();
            }
        }
};

class Game{
    public:
        Snake snake = Snake();
        Food food = Food(snake.body);
        bool running = true;
        int score = 0;
        Sound Eating;
        Sound Wallhit;

        Game(){
            InitAudioDevice();
            Eating = LoadSound("Sounds/eat.mp3");
            Wallhit = LoadSound("Sounds/wall.mp3");
        }

        ~Game(){
            UnloadSound(Eating);
            UnloadSound(Wallhit);
            CloseAudioDevice();
        }

        void Draw(){
            food.Draw();
            snake.Draw();
        }

        void Update(){
            if(running){
                snake.Update();
                CheckCollisionWithFood();
                CheckCollisionWithEdges();
                CheckCollisionWithTail();
            }
        }

        void Reset(){
            snake.body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
            snake.direction = {1,0};
        }

        void CheckCollisionWithFood(){
            if(Vector2Equals(snake.body[0], food.position)){
                food.position = food.GenerateRandomPos(snake.body);
                snake.addSegment = true;
                score++;
                PlaySound(Eating);
            }
        }

        void CheckCollisionWithEdges(){
            if(snake.body[0].x == cellCount || snake.body[0].x == -1){
                GameOver();
            }
            if(snake.body[0].y == cellCount || snake.body[0].y == -1){
                GameOver();
            }
        }

        void GameOver(){
            Reset();
            food.position = food.GenerateRandomPos(snake.body);
            running = false;
            score = 0;
            PlaySound(Wallhit);
        }

        void CheckCollisionWithTail(){
            deque<Vector2> headlessBody = snake.body;
            headlessBody.pop_front();
            if(ElementInDeque(snake.body[0], headlessBody)){
                GameOver();
            }
        }
};

int main(){
    const int screenWidth = cellSize*cellCount;
    const int screenHeight = cellSize*cellCount;

    InitWindow(2*offset + cellSize*cellCount , 2*offset + cellSize*cellCount, "demo");
    int currentFPS = 60;
    SetTargetFPS(currentFPS);

    Game game = Game();
    
    while(!WindowShouldClose()){

        BeginDrawing();

            if(eventTriggered(0.2)){
                game.Update();
            }

            if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
                game.snake.direction = {0,-1};
                game.running = true;
            }
            if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1){
                game.snake.direction = {0,1};
                game.running = true;
            }
            if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1){
                game.snake.direction = {-1,0};
                game.running = true;
            }
            if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
                game.snake.direction = {1,0};
                game.running = true;
            }

            ClearBackground(color);
            DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount}, 5, DARKGREEN);
            game.Draw();
            DrawText(TextFormat("Score: %d", game.score), offset-5, offset+cellSize*cellCount+10, 40, DARKGREEN);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}