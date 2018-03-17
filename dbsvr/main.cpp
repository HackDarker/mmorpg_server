#include "gameServer.h"
#include "dbModule.h"

int main(int argc, char const *argv[])
{
	GameServer* game = GameServer::Instance();
	game->RegisterModule(DB_MODULE,new DbModule);
	game->Run();

	return 0;
}