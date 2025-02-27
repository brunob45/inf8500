#include "packet_gen_adapt_master.h"
#include "globals.h"
#include <string.h>

void packet_gen_adapt_master::pkt_dispatch(void)
{
	unsigned int addr;
	simple_bus_status status;

	while (true)
	{
		//Recup�ration du paquet
		next_packet = true; 
		cout << "GEN_ADAPT : Attente paquet pret" << endl;
		wait(packet_ready.posedge_event()); // Attendre ready == true
		cout << "GEN_ADAPT : paquet pret" << endl;
		cout << "GEN_ADAPT : Recuperation du paquet" << endl;
		pkt = *packet_in;
		cout << "GEN_ADAPT : Acquittement" << endl;
		next_packet = false;

		//Lecture de l'adresse du paquet
		addr = pkt.getAddress();

		//Utiliser la bonne m�thode d'envoi selon l'adresse
		status = bus_port->burst_write(8, (int*)pkt.getPacket(), addr, 6);
		
		if (status == SIMPLE_BUS_ERROR)
		{
			sb_fprintf(stdout, "%s %s : blocking-write failed at address %x\n",
				sc_time_stamp().to_string().c_str(), name(), addr);
		}
		
		wait(50, SC_NS);
	}
}

