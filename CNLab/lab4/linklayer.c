#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timers.h"
#include "phy.h"
#include "net.h"
#include <errno.h>
#include <unistd.h>

const int NR_BUFS = (MAX_SEQ + 1) / 2;
bool no_nak = true;

bool between(SeqNo a, SeqNo b, SeqNo c) {
	bool ok = false;
	if (((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a))) {
		ok = true;
		// printf("Kabhi to true hua\n");
	}
	return ok;
}

char *getBinary(unsigned char c) {
	static char bin[9];
	bin[8] = '\0';
	for (int i = 7; i >= 0; i--) {
		if (((int)c & (1 << i))) {
			bin[7 - i] = '1';
		} else {
			bin[7 - i] = '0';
		}
	}
	// printf("Bin string %s\n", bin);
	return bin;
}

void addc(char *str, char c) {
	int len = strlen(str);
	str[len] = c;
	str[len + 1] = '\0';
}

char xxor (char a, char b) {
	if ((a == '0' && b == '1') || (a == '1' && b == '0')) {
    return '1';
  } else return '0';
}

void crcxor(char *rem, char *gen, int atrem) {
	for (int i = 0; i < strlen(gen); i++) {
		rem[atrem + i] = xxor(rem[atrem + i], gen[i]);
	}
}

void calculateCRCRem(Frame *frame, char *finalRem) {
	unsigned char *fch = (unsigned char *)frame;
	// printf("Length of frame is %d\n", sizeof(Frame));
	char binString[8 * 1044 + 1];
	binString[0] = '\0';
	// strcat(binString, getBinary(at));
	// printf("Should work bin: %s\n", binString);
	
	int i;
	for (i = 0; i < sizeof(Frame); i++) {
		strcat(binString, getBinary(fch[i]));
	}
	// strcpy(binString, "110101010101001111010101010101010101011110101010000000000000000000000000000000000\0");
	// printf("Bin string: %s\n", binString);
	// printf("Last four characters of frame: %c %c %c %c\n", fch[sizeof(Frame) - 6], fch[sizeof(Frame) - 5], fch[sizeof(Frame) - 4], fch[sizeof(Frame) - 3]);
	char gen[34] = "100000100110000010001110110110111\0";
	int till = strlen(gen);
	// printf("size of generator: %d\n", strlen(gen));
	char rem[8 * 1044 + 1];
	for (i = 0; i < strlen(gen); i++) {
		rem[i] = binString[i];
	}
	rem[strlen(gen)] = '\0';
	int atrem = 0;
	while (till < strlen(binString)) {
		if (rem[atrem] == '1') {
			crcxor(rem, gen, atrem);
		}
		atrem++;
		addc(rem, binString[till]);
		till++;
	}
	if (rem[atrem] == '1') {
		crcxor(rem, gen, atrem);
	}
	
	for (i = atrem; i < strlen(rem); i++) {
		if (rem[i] == '1') break;
	}
	for (int j = 0; j < 32 - (strlen(rem) - i); j++) {
		addc(finalRem, '0');
	}
	for (; i < strlen(rem); i++) {
		addc(finalRem, rem[i]);
	}
	
}

bool computeCRC(Frame *frame) {
	frame->crc[0] = frame->crc[1] = frame->crc[2] = frame->crc[3] = 0;
	// printf("CRC Character: %s %s %s %s", getBinary(frame->crc[0]), getBinary(frame->crc[1]), getBinary(frame->crc[2]), getBinary(frame->crc[3]));
	// memset(frame->crc, 0, sizeof(frame->crc));
	char finalRem[33];
	finalRem[0] = '\0';
	calculateCRCRem(frame, finalRem);
	// printf("Final rem: %s\n", finalRem);
	for (int j = 0; j < 4; j++) {
		int c = 0;
		for (int i = 0; i < 8; i++) {
			if (finalRem[j * 8 + i] == '1') {
				c |= (1 << (7 - i));
			}
		}
		// printf("Expected char %c\n", (unsigned char)c);
		frame->crc[j] = (unsigned char)c;
		// printf("Frame char %c\n", frame->crc[j]);
		
	}
	//CRC-32 to be used which has ones in the following powers of x
	//32, 26, 23, 22, 16, 12, 11, 10, 8, 7, 5, 4, 2, 1, 0
	return true;
}

bool checkCRC(Frame *frame) {
	char finalRem[33];
	finalRem[0] = '\0';
	calculateCRCRem(frame, finalRem);
	// printf("CRC received: %s\n", finalRem);
	bool ok = true;
	for (int i = 0; i < strlen(finalRem); i++) {
		if (finalRem[i] == '1') {
			ok = false;
			break;
		}
	}
	return ok;
}

void send_frame(FrameKind fk, SeqNo frame_nr, SeqNo frame_expected, Packet buffer[]) {
	Frame s;
	s.frameKind = fk;
	if (fk == DATA) {
		s.packet = buffer[frame_nr % NR_BUFS];
	}
	s.seq = frame_nr;
	s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);
	if (fk == NAK) {
		no_nak = false;
	}
	int len = 2;
	s.length[0] = len & 0xF;
	s.length[1] = (len >> 8) & 0xF;
	computeCRC(&s);
	sendFrame(&s);
	if (fk == DATA) {
		startTimer(frame_nr % NR_BUFS);
	}
	stopAckTimer();
}	

int main(int argc, char** argv) {
	// printf("NR_BUFS = %d\n", NR_BUFS);
	SeqNo ack_expected, next_frame_to_send, frame_expected, too_far;
	Packet out_buf[NR_BUFS], in_buf[NR_BUFS];
	bool arrived[NR_BUFS];
	SeqNo nbuffered;
	Event event;
	initEvents();
	initTimers();
	initPhy(strtol(argv[2], NULL, 10), argv[1], strtol(argv[3], NULL, 10));
	initNet();
	ack_expected = next_frame_to_send = frame_expected = 0;
	too_far = NR_BUFS;
	nbuffered = 0;
	for (int i = 0; i < NR_BUFS; i++) arrived[i] = false;
	while (1) {
		waitForEvent(&event);
		
		switch(event.eventType) {
			case TIMEOUT:
				debugPrint("Timeout event occured");
				// printf("Timeout event occured\n");
				Timer *timer = (Timer*) event.eventData;
				// printf("Timeout for SEQ no %d\n", timer->seqNo);
				send_frame(DATA, timer->seqNo, frame_expected, out_buf);
				break;
			case ACK_TIMEOUT:
				debugPrint("ACK timeout event occured");
				// printf("ACK timeout event occured");
				send_frame(ACK, 0, frame_expected, out_buf);
				break;	
			
			case FRAME_ARRIVAL:
				debugPrint("Frame arrival event occured");
				// printf("Frame arrived\n");
				Frame *f = (Frame *)event.eventData;
				bool crcCorrect = checkCRC(f);
				if (crcCorrect == false) {
					// printf("CRC Not Correct\n");
					if (no_nak == true) {
						// printf("Sending NAK\n");
						send_frame(NAK, 0, frame_expected, out_buf);
					}
				} else {
					// printf("Crc is correct!\n");
					if (f->frameKind == DATA) {
						// An undamaged frame has arrived
						if ((f->seq != frame_expected) && no_nak == true) {
							send_frame(NAK, 0, frame_expected, out_buf);
						} else startAckTimer();
						if ((between(frame_expected, f->seq, too_far) == true) && (arrived[(f->seq % NR_BUFS)] == false)) {
							arrived[f->seq % NR_BUFS] = true;
							in_buf[f->seq % NR_BUFS] = f->packet;
							while (arrived[frame_expected % NR_BUFS] == true) {
								// printf("Inside it\n");
								Frame tmp;
								tmp.packet = in_buf[frame_expected % NR_BUFS];
								int len = 2;
								if (tmp.packet.data[1] == 255) {
									len = 1;
								}
								tmp.length[0] = len & 0xF;
								tmp.length[1] = (len >> 8) & 0xF;
								toNetworkLayer(&tmp);
								// printf("Sent to network layer\n");
								no_nak = true;
								arrived[frame_expected % NR_BUFS] = false;
								inc(frame_expected);
								inc(too_far);
								startAckTimer();
								
							}
						}
					}
					if ((f->frameKind == NAK) && (between(ack_expected, (f->ack + 1) % (MAX_SEQ + 1), next_frame_to_send) == true)) {
						// printf("Responding to NAK\n");
						send_frame(DATA, (f->ack + 1) % (MAX_SEQ + 1), frame_expected, out_buf);
					}
					while (between(ack_expected, f->ack, next_frame_to_send) == true) {
						nbuffered = nbuffered - 1;
						stopTimer(ack_expected % NR_BUFS);
						inc(ack_expected);
					}
					// toNetworkLayer((Frame*) event.eventData);
				}
				
				break;
			// Below should be perfect
			case NETWORK_LAYER_READY:
				debugPrint("Network layer is ready");
				Frame tmp;
				nbuffered = nbuffered + 1;
				// fromNetworkLayer(&out_buf[])
				fromNetworkLayer(&tmp);
				out_buf[next_frame_to_send % NR_BUFS] = tmp.packet;
				send_frame(DATA, next_frame_to_send, frame_expected, out_buf);
				inc(next_frame_to_send);
				// printf("Frame sent, %s\n", tmp.packet.data);
				break;	
			default:
				debugPrint("Unknown event occured");
				break;
		}
		eventCleanup(&event);
		// if (nbuffered >= NR_BUFS) {
		// 	sleep(15);
		// }
	}
	return 0;
}

