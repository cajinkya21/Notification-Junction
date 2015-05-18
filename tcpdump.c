#include "tcpdump.h"

char retstring[1024];
static int count = 1;   

char* extractVal(char *key_val) {

	char *ptr, *val;
	char temp[128] ;
	strcpy(temp, key_val);
	ptr = strtok(temp, "::");
	ptr = (key_val + strlen(ptr) + 2);
	if(!ptr) printf("> %s %d extractVal() : Returned NULL\n", __FILE__, __LINE__);
	val = (char *)malloc(sizeof(char) * (strlen(ptr) + 1));
	strcpy(val, ptr);

	return val;
}

char* getValFromArgs(char *usage, char* key) {   
	char *occ, keycopy[128], *retstr, localkeyval[256];
	int i = 0;
	strcpy(keycopy, key);
	strcat(keycopy, "::");
	occ = strstr(usage, keycopy);
	while(*occ != '#') {
		localkeyval[i++] = *occ;
		occ++;    
	}
	localkeyval[i] = '\0';

	retstr = (char *)malloc(sizeof(char) * (i+1));

	strcpy(retstr, localkeyval);
	return retstr;
}


void getnotify(struct getnotify_thread_args *args) {
	char *dev = NULL;
	
	char device[256];
	int num;
	char expt2[256], expt3[256];
    char argscopy[256];
    strcpy(argscopy, args->argssend);

    printf("> %s %d getnotify() : Args received in getnotify - %s.\n", __FILE__, __LINE__, argscopy);
    
	strcpy(expt2, getValFromArgs(argscopy, "devicename"));
	strcpy(expt3, getValFromArgs(argscopy, "num_packets"));


	strcpy(device, extractVal(expt2));
	num = atoi(extractVal(expt3));
	
	
    dev = extractVal(getValFromArgs(args->argssend,"devicename"));
	char errbuf[PCAP_ERRBUF_SIZE];		/* error buffer */
	int num_packets = atoi(extractVal(getValFromArgs(args->argssend,"num_packets")));	
	struct bpf_program fp;			/* compiled filter program (expression) */
    char filter_exp[] = "ip";		/* filter expression [3] */
	bpf_u_int32 mask;			/* subnet mask it is unsigned int*/
	bpf_u_int32 net;			/* ip */
	pcap_t *handle;				/* packet capture handle */
	
	/* get network number and mask associated with capture device */

	if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
		fprintf(stderr, "Couldn't get netmask for device %s: %s\n",
		    dev, errbuf);
		net = 0;
		mask = 0;
	}
	
	/* open capture device */ 
	handle = pcap_open_live(dev, SNAP_LEN, 1, 1000, errbuf);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
		return;
	}
	/* make sure we're capturing on an Ethernet device [2] */
	if (pcap_datalink(handle) != DLT_EN10MB) {
		fprintf(stderr, "%s is not an Ethernet\n", dev);
		return;
	}
	
	/* compile the filter expression */
	if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
		fprintf(stderr, "Couldn't parse filter %s: %s\n",
		    filter_exp, pcap_geterr(handle));
		return;
	}

	/* apply the compiled filter */
	if (pcap_setfilter(handle, &fp) == -1) {
		fprintf(stderr, "Couldn't install filter %s: %s\n",
		    filter_exp, pcap_geterr(handle));
		return;
	}

	/* now we can set our callback function */
	pcap_loop(handle, num_packets, got_packet, NULL);
	
	/* cleanup */
	pcap_freecode(&fp);
	pcap_close(handle);

	printf("\nCapture complete. \n");
	count = 1;
	
	strcpy(args->argsrecv, retstring);
	*retstring = '\0';
	return;

}
void
got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{

	                /* packet counter */
	
	/* declare pointers to packet headers */
	const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
	const struct sniff_ip *ip;              /* The IP header */
	const struct sniff_tcp *tcp;            /* The TCP header */
	const char *payload;                    /* Packet payload */

    char string[64];
	int size_ip;
	int size_tcp;
	int size_payload;
	
	//retstring[0] = ":";
	
	
	sprintf(string, "\nPacket number %d:\n", count);
	strcat(retstring, string);
	printf("\nPacket number %d:\n", count);
	count++;
	
	/* define ethernet header */
	ethernet = (struct sniff_ethernet*)(packet);
	
	/* define/compute ip header offset */
	ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
	size_ip = IP_HL(ip)*4;
	if (size_ip < 20) {
	    sprintf(string, "   * Invalid IP header length: %u bytes\n", size_ip);
	    strcat(retstring, string);
		printf("   * Invalid IP header length: %u bytes\n", size_ip);
		return;
	}

	/* print source and destination IP addresses */
	sprintf(string, "       From: %s\n", inet_ntoa(ip->ip_src));
    strcat(retstring, string);
	printf("       From: %s\n", inet_ntoa(ip->ip_src));
	sprintf(string, "       To: %s\n", inet_ntoa(ip->ip_dst));
    strcat(retstring, string);
	printf("         To: %s\n", inet_ntoa(ip->ip_dst));
	
	/* determine protocol */	
	switch(ip->ip_p) {
		case IPPROTO_TCP:
		    sprintf(string, "   Protocol: TCP\n");
            strcat(retstring, string);
			printf("   Protocol: TCP\n");
			break;
		case IPPROTO_UDP:
			printf("   Protocol: UDP\n");
			return;
		case IPPROTO_ICMP:
			printf("   Protocol: ICMP\n");
			return;
		case IPPROTO_IP:
			printf("   Protocol: IP\n");
			return;
		default:
			printf("   Protocol: unknown\n");
			return;
	}
	
	/*
	 *  OK, this packet is TCP.
	 */
	
	/* define/compute tcp header offset */
	tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
	size_tcp = TH_OFF(tcp)*4;
	if (size_tcp < 20) {
		printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
		return;
	}
	
	sprintf(string, "   Src port: %d\n", ntohs(tcp->th_sport));
    strcat(retstring, string);
	printf("   Src port: %d\n", ntohs(tcp->th_sport));
	sprintf(string, "   Dst port: %d\n", ntohs(tcp->th_dport));
    strcat(retstring, string);
	printf("   Dst port: %d\n", ntohs(tcp->th_dport));
	
	/* define/compute tcp payload (segment) offset */
	payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);
	
	/* compute tcp payload (segment) size */
	size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);
	
	/*
	 * Print payload data; it might be binary, so don't just
	 * treat it as a string.
	 */
	/*I have skipped printing of payload*/
return;
}
/*
int main(int argc, char *argv[]) {
	getnotify_thread_args* args ;
	args = (getnotify_thread_args *) malloc (sizeof(getnotify_thread_args) );
	if(argc < 2) {
		printf("Kindly enter the string of argumetns ");
	}
	printf("%s is string of arguments ", argv[1]);
	strcpy(args->argssend,argv[1]);
	getnotify(args);
	printf("===========================\n");
	printf("retstring - \n%s", retstring);
    strcpy(args->argsrecv, retstring);
    printf("\n%s\nRETURNED.", args->argsrecv);
	return 0;
}*/
