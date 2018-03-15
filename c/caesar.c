#define constant "During a few days halt near Vesontio for the provision of corn and other supplies a panic arose from inquiries made by our troops and remarks uttered by Gauls and traders who affirmed that the Germans were men of a mighty frame and an incredible valour and skill at arms Julius Caesar\0"


int ceasar_encrypt(unsigned int key, unsigned char *input, unsigned char *output){
	int i;
	unsigned char ch;
	int finish;

	finish = 0;
	for (i = 0; input[i] != 0; i++){
		ch = input[i];
		if(ch >= 'a' && ch <= 'z'){
			ch = ch + key;
			if (ch > 'z') {
				ch = ch - 'z' + 'a' - 1;
			}
		}
		else if (ch >= 'A' && ch <= 'Z') {
			ch = ch + key;
			if (ch > 'Z'){
				ch = ch - 'Z' + 'A' - 1;
			}
		}
		output[i] = ch;
	}

	finish = 1;
	return finish;
}

void ceasar_decrypt(unsigned int key, unsigned char *input, unsigned char *output){
	int i;
	unsigned char ch;

	for (i = 0; input[i] != 0; i++){
		ch = input[i];
		if (ch >= 'a' && ch <= 'z'){
			ch = ch - key;
			if (ch < 'a') {
				ch = ch + 'z' - 'a' + 1;
			}
		}
		else if (ch >= 'A' && ch <= 'Z'){
			ch = ch - key;
			if (ch < 'A'){
				ch = ch + 'Z' - 'A' + 1;
			}
		}
		output[i] = ch;
	}
}


int compare_string(unsigned char *string1, unsigned char *string2){
	int match;
	int i;

	match = 1;
	i = 0;

	while (string1[i] != 0){
		if (string1[i] != string2[i])
		{
		    match = 0;
		}
		i++;
	}
	if (string2[i] != 0)
	{
	    match = 0;
	}

	return match;
}


int main(void){
    unsigned char* stringptr = "During a few days halt near Vesontio for the provision of corn and other supplies a panic arose from inquiries made by our troops and remarks uttered by Gauls and traders who affirmed that the Germans were men of a mighty frame and an incredible valour and skill at arms Julius Caesar";
    unsigned char outputstring[300];
    unsigned char decryptstring[300];
	unsigned int key;
	int i;
	int match;

	i = 0;

	key = 15;

	ceasar_encrypt(key, stringptr, &outputstring[0]);

	key = 15;

	ceasar_decrypt(key, &outputstring[0], &decryptstring[0]);

	match = compare_string (stringptr, &decryptstring[0]);

	if (match == 0) match = -1;
	else match = 1;

	return match;
}
