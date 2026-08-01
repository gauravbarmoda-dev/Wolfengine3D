#include "game.h"

int main(){
    Game game;

    if(game.Initialize(640, 480, "Doom")){
        game.Run();
    }
    return 0;
}
