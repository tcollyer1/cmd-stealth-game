#include <iostream>
#include <io.h>
#include <fcntl.h>

#include "..\h\game.h"

using namespace std;

int main()
{
    _setmode(_fileno(stdout), _O_U16TEXT);

    wcout << "Loading game...\n";
    Game game;

    game.Run();
}