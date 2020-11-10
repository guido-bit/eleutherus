#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<tbpgp.h>

void cleanup(int signum)
{
	printf("IN CLEANUP\n");
	tbpgp_quit();
	printf("OUT FROM CLEANUP\n");
}
int main(int argc, char **argv)
{
	TbpGpContext context;


	context = tbpgp_context_new("0ef977f5af3b3ac74757d9360531d01f7c3e1b8f27a2a72b151fa5ecf111945e");
	tbpgp_context_dispatch(context);

	tbpgp_signal_connect(SIGINT, cleanup);
	tbpgp_main();

	printf("WTF IM HERE!?");

	return 0;
}
