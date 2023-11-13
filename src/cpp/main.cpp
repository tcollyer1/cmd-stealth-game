#include <iostream>
#include <io.h>
#include <fcntl.h>

#include "..\h\game.h"

using namespace std;

int main()
{
    _setmode(_fileno(stdout), _O_U16TEXT); // To allow for unicode characters
    srand(time((time_t*)NULL)); // To improve the "true" randomness of rand()

    wcout << "Loading game...\n";
    Game game;

    game.Run();
}