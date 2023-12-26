
#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

 void upper(char* s){
  for (int i = 0; s[i]!='\0'; i++) {
      if(s[i] >= 'a' && s[i] <= 'z') {
         s[i] = s[i] -32;
      }
   }
 }

 char* encryption(char* msg , char* key){
    int msgLen = strlen(msg), keylen = strlen(key), i, j;
    char* encryptedMsg = (char*)malloc(sizeof(char)*msgLen+1);
    for(i = 0, j=0; i < msgLen; ++i,++j){

        if(!isalpha(msg[i])){
        encryptedMsg[i] = msg[i];
        continue;
        }

        if(isupper(msg[i]))
        encryptedMsg[i] = ((msg[i] + key[i%keylen]) % 26) + 'A';

        else if(islower(msg[i]))
        encryptedMsg[i] = tolower(((toupper(msg[i]) + key[i%keylen]) % 26) + 'A');

    }

    encryptedMsg[i] = '\0';
    return encryptedMsg;

 }

char* decryption(char * encryptedMsg, char* key){
      int msgLen = strlen(encryptedMsg), keylen = strlen(key), i, j;
      char* decryptedMsg = (char*)malloc(sizeof(char)*msgLen+1);

      for(i = 0; i < msgLen; ++i){

        if(!isalpha(encryptedMsg[i])){
        decryptedMsg[i] = encryptedMsg[i];
        continue;
        }

        if(isupper(encryptedMsg[i]))
        decryptedMsg[i] = (((encryptedMsg[i] - key[i%keylen]) + 26) % 26) + 'A';
        
        else if(islower(encryptedMsg[i]))
        decryptedMsg[i] = tolower((((toupper(encryptedMsg[i]) - key[i%keylen]) + 26) % 26) + 'A');
      }

    decryptedMsg[i] = '\0';
    return decryptedMsg;
}





//structure used to define a node
typedef struct node_t {
	struct node_t *left, *right;
	int freq;
	char c;
} *node;

//global variables
int n_nodes = 0, qend = 1; 		//global variables for keep track of no.of nodes and end of the que
struct node_t pool[256] = {{0}};        //pool of nodes
node qqq[255], *q = qqq-1;      	//the priority que
char *code[128] = {0}, buf[1024];	//a string array of the codes for each letter
int input_data=0,output_data=0;

//function used to create a new node
node new_node(int freq, char c, node a, node b)
{
	node n = &pool[n_nodes++];
	if (freq != 0){
		n->c = c;			//assign the character 'c' to the character of the node (eventually a leaf)
		n->freq = freq;			//assign frequency
	}
	else {
		n->left = a, n->right = b;	//if there is no frequency provided with the invoking
		n->freq = a->freq + b->freq;	//the removed nodes at the end of the que will be added to left and right
	}
	return n;
}

//function ued to insert a node into the priority que
void qinsert(node n)
{
	int j, i = qend++;
	while ((j = i / 2)) {
		if (q[j]->freq <= n->freq) break;
		q[i] = q[j], i = j;
	}
	q[i] = n;
}

node qremove()
{
	int i=1, l;
	node n = q[i];

	if (qend < 2) return 0;
	qend--;
	while ((l = i * 2) < qend) {
		if (l + 1 < qend && q[l + 1]->freq < q[l]->freq) l++;
		q[i] = q[l], i = l;
	}
	q[i] = q[qend];
	return n;	//return the node
}

//go along the builded huffman tree and assign the code for each character
void build_code(node n, char *s, int len)
{
	static char *out = buf;
	if (n->c) {
		s[len] = 0;		//if the provided node is a leaf (end node)
		strcpy(out, s);		//it contains a character
		code[(int)n->c] = out;	//therefore the code is copied in to the relevant character.
		out += len + 1;		//out pointer is incremented
		return;
	}

	s[len] = '0'; build_code(n->left,  s, len + 1);	//recurring is used to write out the code
	s[len] = '1'; build_code(n->right, s, len + 1);	//if right add a 1 and if right add a 0
}

void import_file(FILE *fp_in, unsigned int *freq){
	char c,s[16]={0};		//temporary variables
	int i = 0;
	printf("File Read:\n");
	while((c=fgetc(fp_in))!=EOF){
                freq[(int)c]++;		//read the file character by character and increment the particular frequency
		putchar(c);
	}
	for (i = 0; i < 128; i++)
		if (freq[i]) qinsert(new_node(freq[i], i, 0, 0));//insert new nodes into the que if there is a frequency
	while (qend > 2)
		qinsert(new_node(0, 0, qremove(), qremove()));	//build the tree
	build_code(q[1], s, 0);		//build the code for the characters
}

void encode(FILE* fp_in, unsigned int *freq ){

	char in,c,temp[20] = {0};
	int i,j=0,k=0,lim=0;
	rewind(fp_in);
	for(i=0; i<128; i++){
		if(freq[i])	lim += (freq[i]*strlen(code[i]));
	}
	output_data = lim;			//The output data is equal to the limit
	printf("\nEncoded:\n");
	for(i=0; i<lim; i++){
		if(temp[j] == '\0'){
			in = fgetc(fp_in);
			strcpy(temp,code[in]);
			printf("%s",code[in]);
			j = 0;
		}
		if(temp[j] == '1')
                        c = c|(1<<(7-k));       //shifts 1 to relevant position and OR with the temporary char
                else if(temp[j] == '0')
                        c = c|(0<<(7-k));       //shifts 0 to relevant position and OR with the temporary char
                else
                        printf("ERROR: Wrong input!\n");
                k++;                            // k is used to divide the string into 8 bit chunks and save
		j++;
		
	}
}

void print_code(unsigned int *freq){
	int i;
	printf("\n---------CODE TABLE---------\n----------------------------\nCHAR  FREQ  CODE\n----------------------------\n");
	for(i=0; i<128; i++){
		if(isprint((char)i)&&code[i]!=NULL&&i!=' ')
			printf("%-4c  %-4d  %16s\n",i,freq[i],code[i]);
		else if(code[i]!=NULL){
			switch(i){
				case '\n':
					printf("\\n  ");
					break;
				case ' ':
					printf("\' \' ");
					break;
				case '\t':
					printf("\\t  ");
					break;
				default:
					printf("%0X  ",(char)i);
					break;
			}
			printf("  %-4d  %16s\n",freq[i],code[i]);
		}
	}
	printf("----------------------------\n");

}

void huff(int argc, char* argv[]){
	FILE *fp_in, *fp_out;				//FIFO pointers
	char file_name[50]={0};				//file name
	unsigned int freq[128] = {0},i;			//frequency of the letters


	printf("**********************************************************************\n");
	printf("   COMPRESSION USING HUFFMAN    \n");

	printf("**********************************************************************\n\n");


	if( argc == 2 ) {
		strcpy(file_name,argv[1]);		//commandline argument directly allows to compress the file
	}
   	else if( argc > 2 ) {
      		printf("Too many arguments supplied.\n");
   	}
   	else {
		printf("Please enter the file to be compressed\t: "); //else a prompt comes to enter the file name
		scanf("%s",file_name);
   	}

	if(strlen(file_name)>=50){ 
		printf("ERROR: Enter a file name less than 50 chars");
		exit(0);
	}

	//import the file into the program and update the huffman tree
	if((fp_in = fopen(file_name,"r"))==NULL){	//open the file stream
		printf("\nERROR: No such file\n");
		exit(0);
	}
	import_file(fp_in,freq);			//import the file and fills frequency array


	print_code(freq);				//print the code table

	//Encode and save the encoded file
	encode(fp_in,freq);

	fclose(fp_in);
	fclose(fp_out);


	for(i=0; i<128; i++)	input_data += freq[i];	//calculate input byte
	printf("\nInput bytes:\t\t%d\n",input_data);
	output_data = (output_data%8)? (output_data/8)+1 : (output_data/8);
	printf("Output bytes:\t\t%d\n",output_data);

	printf("\nCompression ratio:\t%.2f%%\n\n\n",((double)(input_data-output_data)/input_data)*100);

}

int main(int argc, char *argv[]){
  FILE * f , * fp;
  char dir[] = "files";
  char back[] = "\\";
  char* enc , * dec ,* buff = NULL;
  char de[] = "decrypt.txt";
  char en[] = "encrypt.txt";
  char temp[100] , key[100];
  int c = mkdir(dir);
  int choice=0 , k;
  char filename[100];

  while(1){
    printf("\nWHAT DO YOU WANT TO DO ?  \n 1.ENCRYPTION \n 2.DECRYPTION \n 3.COMPRESSION \n 4.EXIT\n ENTER THE CHOICE :");
    scanf("%d",&choice);
    switch(choice){
      case 1 :
      printf("Enter the file name : ");
      scanf("%s",filename);
      printf("\nEnter the key : ");
      scanf("%s",key);
      upper(key);
      f = fopen(filename,"r");
      if(f==NULL) printf("file is not opened.");
      fseek(f,0,SEEK_END);
      k = ftell(f);
      rewind(f);
      strcpy(temp,dir);
      strcat(temp,back);
      strcat(temp,en);
      fp = fopen(temp,"w");
      getdelim( &buff, &k, '\0', f); 
      enc = encryption(buff,key);
      fputs(enc,fp);
      fclose(f);
      fclose(fp);
      break;

      case 2 :
      printf("Enter the file name : ");
      scanf("%s",filename);
      printf("\nEnter the key : ");
      scanf("%s",key);
      upper(key);
      f = fopen(filename,"r");
      if(f==NULL) printf("file is not opened.");
      fseek(f,0,SEEK_END);
      k = ftell(f);
      rewind(f);
      strcpy(temp,dir);
      strcat(temp,back);
      strcat(temp,de);
      fp = fopen(temp,"w");
      getdelim( &buff, &k, '\0', f); 
      dec = decryption(buff,key);
      fputs(dec,fp);
      fclose(f);
      fclose(fp);
      break;

      case 3:
      huff(argc,argv);
      break;

      case 4:
      exit(0);

      default:
      printf("\n Enter a valid choice :");
      break;

    }
    strcpy(temp,"");

  }

  return 0;
}