#include "globalModule.h"

int main(int argc, char const *argv[])
{
	GameServer* game = GameServer::Instance();
	game->RegisterModule(GLOBAL_MODULE,new globalModule);
	game->Run();

	return 0;
}