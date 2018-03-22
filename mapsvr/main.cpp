#include "gameServer.h"
#include "mapModule.h"

int main(int argc, char const *argv[])
{
	GameServer* game = GameServer::Instance();
	game->RegisterModule(MAP_MODULE,new MapModule);
	game->Run();

	return 0;
}
