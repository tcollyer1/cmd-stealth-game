#include <iostream>
#include <io.h>
#include <fcntl.h>

#include "..\h\game.h"

using namespace std;

int main()
{
    _setmode(_fileno(stdout), _O_U16TEXT); // To allow for unicode characters
    srand(time((time_t*)NULL)); // seed rand() with the time to improve the "true" randomness

    wcout << "Loading game...\n";
    Game game;

    game.Run();
}