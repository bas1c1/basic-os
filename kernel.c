#define WHITE_TXT 0x07
#define NULL 0
#define new(t) malloc(sizeof(t))

char *vidmem = (char *) 0xb8000;
int cursor = 0;
int cursory = 0;

void kmain(void) {
	cls();
	putc('A');
	putc('B');
	putc('\n');
	printf("Hello! This is new line!");
	putc('\n');
	printf("OMFG printf is working!!!!!!!");
	char *output;
	int digit = 1;
	itoa(digit, output);
	putc('\n');
	printf(output);
}

//STD:

static unsigned char buffmem[1024 * 1024];
static int next_index = 0;
void *malloc(int sz) {
    void *mem;
    if(sizeof buffmem - next_index < sz)
        return NULL;
    mem = &buffmem[next_index];
    next_index += sz;
    return mem;
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
