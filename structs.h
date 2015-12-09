typedef struct code_struct{
	unsigned int chapter;
	unsigned int paragraph;
	unsigned int line;
	unsigned int letter;

	char alpha;

	struct code_struct *next;
} CODE;
