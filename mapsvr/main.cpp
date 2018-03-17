#include "gameServer.h"
#include "gatewayModule.h"

int main(int argc, char const *argv[])
{
	GameServer* game = GameServer::Instance();
	game->RegisterModule(GATEWAY_MODULE,new GatewayModule);
	game->Run();

	return 0;
}