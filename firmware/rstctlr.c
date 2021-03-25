// Reset/Power switch controller.

#include <avr/interrupt.h>
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

typedef unsigned char byte;
typedef unsigned int size_t;

uint16_t pinstate = 0;

const char DIGITS[] = "0123456789ABCDEF";
const char digits[] = "0123456789acdefg";

enum {
	SCLK  = 0,	// TLC59281 shift clock is PB0.
	SIN = 1,	// ...serial in is PB1.
	LAT = 2,	// ...latch is PB2.
	BLANK = 3,	// ...blank is PB3 (pulled high).
};

inline static void
delayn(int n)
{
	for (int k = 0; k < n; ++k)
		_delay_us(1);
}

inline static void
delay()
{
	delayn(1);
}

void
setoutput(int pin)
{
	DDRB |= (1<<pin);
}

void
pinhi(int pin)
{
	PORTB |= (1<<pin);
}

void
pinlo(int pin)
{
	PORTB &= ~(1<<pin);
}

void
sclock()
{
	pinhi(SCLK);
	delay();
	pinlo(SCLK);
}

void
shift(int bit)
{
	if (bit != 0) {
		pinhi(SIN);
		delay();
	}
	sclock();
	delay();
	pinlo(SIN);
	delay();
}

void
latch()
{
	pinhi(LAT);
	delay();
	pinlo(LAT);
	delay();
}

void
emitstate(uint16_t pinstate)
{
	for (int k = 0; k < 16; ++k)
		shift((pinstate >> k) & 1);
	latch();
}

void
blank()
{
	pinstate = 0U;
	pinhi(BLANK);
	delayn(10);
	emitstate(pinstate);
	pinlo(BLANK);
	delayn(10);
}

void
inituart()
{
	const unsigned long BAUD_RATE = 9600UL;

	const unsigned long prescale = ((F_CPU / 16UL + BAUD_RATE / 2) / BAUD_RATE) - 1UL;
	UBRRH = prescale >> 8;
	UBRRL = prescale;
	UCSRB = /*(1 << RXCIE) |*/ (1 << RXEN) | (1 << TXEN);
	UCSRC = (1 << UCSZ0) | (1 << UCSZ1);
}

void
initmcu()
{
	setoutput(SCLK);
	setoutput(SIN);
	setoutput(LAT);
	setoutput(BLANK);

	blank();
	inituart();
}

enum CMDState {
	CmdREADING,
	CmdERROR,
};

enum {
	RbOK = 0,
	RbOVERFLOW = 1,
	RbUNDERFLOW = 2,

	CAPACITY = 16,
};

typedef struct RingBuffer RingBuffer;
struct RingBuffer {
	volatile byte buffer[CAPACITY];
	volatile size_t readidx, writeidx;
};

inline size_t
rblen(volatile RingBuffer *buffer)
{
	return buffer->writeidx - buffer->readidx;
}

inline int
rbempty(volatile RingBuffer *buffer)
{
	return rblen(buffer) == 0;
}

inline int
rbfull(volatile RingBuffer *buffer)
{
	return rblen(buffer) == CAPACITY;
}

int
rbinsert(volatile RingBuffer *buffer, byte b)
{
	if (rbfull(buffer))
		return RbOVERFLOW;
	buffer->buffer[buffer->writeidx % CAPACITY] = b;
	buffer->writeidx++;
	return RbOK;
}

int
rbremove(volatile RingBuffer *buffer, byte *b)
{
	if (rbempty(buffer))
		return RbUNDERFLOW;
	*b = buffer->buffer[buffer->readidx % CAPACITY];
	buffer->readidx++;
	return RbOK;
}

volatile enum CMDState cmdstate = CmdREADING;
volatile RingBuffer readbuf = { { 0 }, 0, 0 };

byte
uartgetc()
{
	while ((UCSRA & (1 << RXC)) == 0)
		;
	byte b = UDR;
	return b;
}

void
fakeisr()
{
	int b = uartgetc();
	if (cmdstate == CmdERROR) {
		if (b == '\n')
			cmdstate = CmdREADING;
		return;
	}
	if (rbinsert(&readbuf, b) != RbOK)
		cmdstate = CmdERROR;
}

int
fakereadc()
{
	byte b;

	fakeisr();
	while (rbremove(&readbuf, &b) != RbOK)
		fakeisr();

	return b;
}

#define readc fakereadc


/*
ISR(USART_RX_vect, ISR_BLOCK) {
	int b = uartgetc();
	if (cmdstate == CmdERROR) {
		if (b == '\n')
			cmdstate = CmdREADING;
		return;
	}
	if (rbinsert(&readbuf, b) != RbOK)
		cmdstate = CmdERROR;
}

void
wfi()
{
	asm volatile("sei;sleep;cli" :::);
}

void
nop()
{
	asm volatile("nop" :::);
}

void
probeintr()
{
	sei();
	nop();
	cli();
}

int
readc()
{
	byte b;

	probeintr();
	while (rbremove(&readbuf, &b) != RbOK)
		wfi();

	return b;
}
*/

size_t
readcmd(char *buffer, size_t size)
{
	size_t k = 0;
	while (k < size) {
		int b = readc();
		if (b == '\r')
			continue;
		buffer[k++] = b;
		if (b == '\n') {
			return k;
		}
	}
	cmdstate = CmdERROR;
	return ~0;
}

void
uartputc(byte b)
{
	while ((UCSRA & (1 << UDRE)) == 0)
		;
	UDR = b;
	while ((UCSRA & (1 << TXC)) == 0)
		;
}

void
reply(const char __flash *s)
{
	for (const char __flash *p = s; *p != '\0'; ++p)
		uartputc(*p);
}


int
hexdigit(char digit)
{
	for (const char *p = DIGITS; *p != '\0'; ++p)
		if (*p == digit)
			return(p - DIGITS);
	for (const char *p = digits; *p != '\0'; ++p)
		if (*p == digit)
			return(p - digits);

	return -1;
}

int
hexbyte(const char hex[2])
{
	int digit0, digit1;

	digit0 = hexdigit(hex[0]);
	if (digit0 < 0)
		return -1;
	digit1 = hexdigit(hex[1]);
	if (digit1 < 0)
		return -1;

	return (digit0 << 4) + digit1;
}

const char __flash EBADMSG[] = "!msg\r\n";
const char __flash EBADCMD[] = "!cmd\r\n";
const char __flash EBADPORT[] = "!port\r\n";
const char __flash EBADCKSUM[] = "!cksum\r\n";

const char __flash OK[] = "+ok\r\n";

void
evalreset(const char *cmd, size_t len)
{
	if (len != 7 || cmd[6] != '\n') {
		reply(EBADMSG);
		return;
	}
	if (memcmp(cmd, "RST#", 4) != 0) {
		reply(EBADCMD);
		return;
	}
	if (memcmp(cmd + 4, "E4", 2) != 0) {
		reply(EBADCKSUM);
		return;
	}
	blank();
	reply(OK);
}

// Commands are simple strings of the form:
// IPP#CS\n
void
evalcmd(const char *cmd, size_t len)
{
	if (len != 7 || cmd[6] != '\n') {
		reply(EBADMSG);
		return;
	}

	if (cmd[0] == 'R') {
		evalreset(cmd, len);
		return;
	}

	if (cmd[0] != 'I' && cmd[0] != 'O') {
		reply(EBADCMD);
		return;
	}

	int hex = hexbyte(cmd + 1);
	if (hex < 0 || hex >= 16) {
		reply(EBADPORT);
		return;
	}
	int port = hex;

	if (cmd[3] != '#') {
		reply(EBADMSG);
		return;
	}
	hex = hexbyte(cmd + 4);
	if (hex < 0) {
		reply(EBADMSG);
		return;
	}
	unsigned int cksum = hex;
	for (int k = 0; k < 4; ++k)
		cksum += cmd[k];
	cksum &= 0xFF;
	if (cksum != 0) {
		reply(EBADCKSUM);
		return;
	}

	if (cmd[0] == 'I')
		pinstate |= (1U << port);
	else
		pinstate &= ~(1U << port);
	emitstate(pinstate);

	reply(OK);
}

int
main()
{
	initmcu();

	for (;;) {
		char cmd[8];
		size_t len = readcmd(cmd, sizeof(cmd));
		if (len >= sizeof(cmd))
			continue;
		cmd[len] = '\0';
		evalcmd(cmd, len);
	}

	return 0;
}
