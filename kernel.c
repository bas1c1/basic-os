#define NULL 0
#define WHITE_TXT 0x07
#define new(t) malloc(sizeof(t))
#define bool int
#define true 1
#define false 0

unsigned char keyboard_map[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

#define LINES 25
#define COLUMNS_IN_LINE 80
#define BYTES_FOR_EACH_ELEMENT 2
#define SCREENSIZE BYTES_FOR_EACH_ELEMENT * COLUMNS_IN_LINE * LINES

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64
#define IDT_SIZE 256
#define INTERRUPT_GATE 0x8e
#define KERNEL_CODE_SEGMENT_OFFSET 0x08

#define ENTER_KEY_CODE 0x1C

void keyboard_handler(void);
char read_port(unsigned short port);
void write_port(unsigned short port, unsigned char data);
void load_idt(unsigned long *idt_ptr);

//const int fkbb = SCREENSIZE;
char *kbbuffer = "";
char *vidmem = (char *) 0xb8000;
int cursor = 0;
int cursory = 0;

int kbbcur = 0;

void kmain(void) {
	cls();
	
	idt_init();
	char* str = "";
	scanf(str);
	newline();
	printf(str);
	newline();
	
	scanf(str);
	newline();
	printf(str);
	newline();
}

//KB:

bool endinp = false;
struct IDT_entry {
	unsigned short int offset_lowerbits;
	unsigned short int selector;
	unsigned char zero;
	unsigned char type_attr;
	unsigned short int offset_higherbits;
};

struct IDT_entry IDT[IDT_SIZE];

void idt_init(void) {
	unsigned long keyboard_address;
	unsigned long idt_address;
	unsigned long idt_ptr[2];
	keyboard_address = (unsigned long)keyboard_handler;
	IDT[0x21].offset_lowerbits = keyboard_address & 0xffff;
	IDT[0x21].selector = KERNEL_CODE_SEGMENT_OFFSET;
	IDT[0x21].zero = 0;
	IDT[0x21].type_attr = INTERRUPT_GATE;
	IDT[0x21].offset_higherbits = (keyboard_address & 0xffff0000) >> 16;
	write_port(0x20 , 0x11);
	write_port(0xA0 , 0x11);
	write_port(0x21 , 0x20);
	write_port(0xA1 , 0x28);
	write_port(0x21 , 0x00);
	write_port(0xA1 , 0x00);
	write_port(0x21 , 0x01);
	write_port(0xA1 , 0x01);
	write_port(0x21 , 0xff);
	write_port(0xA1 , 0xff);
	idt_address = (unsigned long)IDT ;
	idt_ptr[0] = (sizeof (struct IDT_entry) * IDT_SIZE) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16 ;
	load_idt(idt_ptr);
}

void kb_init(void) {
	write_port(0x21 , 0xFD);
}

void kb_stop(void) {
	write_port(0x21 , 0xFF);
}

void keyboard_handler_main(void) {
	unsigned char status;
	char keycode;

	write_port(0x20, 0x20);

	status = read_port(KEYBOARD_STATUS_PORT);
	if (status & 0x01) {
		keycode = read_port(KEYBOARD_DATA_PORT);
		if(keycode < 0)
			return;

		if(keycode == ENTER_KEY_CODE) {
			kbbcur++;
			kbbuffer[kbbcur] = '\0';
			endinp = true;
			kb_stop();
			return;
		}

		kbbuffer[kbbcur] = keyboard_map[(unsigned char) keycode];
		kbbcur++;

		putc(keyboard_map[(unsigned char) keycode]);
	}
}

void kbbuffer_init(int size) {
	kbbuffer = (char*)malloc(size);
}

//STD:

static unsigned char buffmem[1024 * 1024];
static int next_index = 0;
static void *malloc(int sz) {
    void *mem;
    if(sizeof buffmem - next_index < sz)
        return NULL;
    mem = &buffmem[next_index];
    next_index += sz;
    return mem;
}

char* strcpy(char* destination, const char* source) {
    if (destination == NULL) {
        return NULL;
    }
    char *ptr = destination;
    while (*source != '\0') {
        *destination = *source;
        destination++;
        source++;
    }
    *destination = '\0';
    return ptr;
}

char *scanf(char *st) {
	kbbuffer_init(2048);
	kb_init();
	while(endinp == false) {}
	endinp = false;
	char *fret = strcpy(st, kbbuffer);
	kbbuffer_init(2048);
	kbbcur = 0;
	return fret;
}

int strlen(const char* str) {
	int length = 0;
	while(str[length])
		length++;
	return length;
}

int digit_count(int num) {
	int count = 0;
	if(num == 0)
		return 1;
	while(num > 0) {
		count++;
		num = num/10;
	}
	return count;
}

void itoa(int num, char *number) {
	int dgcount = digit_count(num);
	int index = dgcount - 1;
	char x;
	if(num == 0 && dgcount == 1) {
		number[0] = '0';
		number[1] = '\0';
	} else {
		while(num != 0) {
			x = num % 10;
			number[index] = x + '0';
		    index--;
			num = num / 10;
		}
		number[dgcount] = '\0';
	}
}

void symcol(char color) {
	unsigned int i=(cursory*80*2)+cursor;
	vidmem[i]=color;
	i++;
}

void newline() {
	cursory++;
	cursor = 0;
}

void putc(char ch) {
	unsigned int i=(cursory*80*2)+cursor;
	if (ch == '\n') {
		cursory++;
		return;
	}
	vidmem[i]=ch;
	i++;
	vidmem[i]=WHITE_TXT;
	cursor += 2;
}

void clsym(int index) {
	unsigned int i=(cursory*80*2)+index;
	vidmem[i]=' ';
	i++;
	vidmem[i]=WHITE_TXT;
}

void cls() {
	unsigned int i=0;
	while(i < (80*25*2)) {
		vidmem[i]=' ';
		i++;
		vidmem[i]=WHITE_TXT;
		i++;
	}
	vidmem[0]=' ';
	vidmem[1]=WHITE_TXT;
}

void printf(char *message) {
	unsigned int i=(cursory*80*2);

	while(*message!=0) {
		if(*message=='\n') {
			cursory++;
			i=(cursory*80*2);
			*message++;
			cursory++;
		} else {
			vidmem[i]=*message;
			*message++;
			i++;
			vidmem[i]=WHITE_TXT;
			i++;
			cursor += 2;
		}
	}
}
