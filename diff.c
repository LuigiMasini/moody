#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int lines_counter(FILE*);
char *read_line (int, FILE*);
bool remove_lf (char*);

#define MAXLS 256 // or other suitable maximum line size
char line[MAXLS];



void main (int argc, char *argv[]) {
	
	//ottieni formato file da confrontare
	
	char files_type[5];
	
	if (argc > 1)
// 		files_type = argv[argc];		//hahahah che stupido, sono stringhe
		strcpy(files_type, argv[argc-1]);	//hahah stupido 2, non avevo messo -1, segfault :D
	else {
		printf ("Inserire formato file da confrontare:\t");
		scanf ("%s", files_type);
	}
#ifdef DEBUG
	printf("\nformato:%s", files_type);
#endif
	
	//ottieni profondità (numero livelli cartelle)
	
	int n=1, d=0;			//n è dimensione stringa contenente path, d è numero di livelli in profondità
	if (argc > 2){
		sscanf(argv[1],"%d",&d);	//estraggo numero da stringa argomenti
		if (d > 0)
			n=d*3;			//ad ogni livello corrisponde un ** /
		else
			n=1;
	}
	char prof[n];
	if (d>0) {
		//aggiungo tanti /** quanti livelli di profondità
		strcpy(prof, "**/");		//il primo lo sovrascrivo, per evitare garbage (ci ho perso 2 giorni... che coglione )
		while (d-1) {			//il resto
			strcat(prof, "**/");
			d--;
		}
		strcat(prof, "*");		//la fine
	}
	else
		strcpy(prof, "*");		//se profondità 0 (o <0, forse dovrei dare errore?)
#ifdef DEBUG
	printf ("\nprofondità:%s\n", prof);
#endif
	
	//ottieni lista dei file da confrontare(list.txt)
	
	char *lscomand = (char*) malloc(sizeof(files_type)+sizeof(prof)+22);	//stringa contenitore comando
	sprintf(lscomand, "ls -1 %s%s | cat >list.txt", prof, files_type);		//assemblo comando
	system(lscomand);
	
	//ok, we finally have our list of files to compare
	
	//QUANTI SONO
	
	FILE *list = fopen("list.txt", "r");
	if (list==NULL){
		printf("Could not open file");
		exit(2);
	}
	int files_number = lines_counter(list);
	fclose(list);
	
	bool battagliaNavale[files_number][files_number];	//registro combinazioni
	/*
	due cicli for annidati, per confrontare ogni file con tutti gli altri
	ogni coppia viene però controllata due volte, ulteriore ottimizzazione è dimezzare controlli,
	ma per farlo dovrei prima inizializzare ogni cella della matrice allo stesso numero per evitare garbage (memset)
	*/
	
	printf ("\nRESULTS:\n\n");		//intestazione tabella
	for (int i = 0; i<files_number; i++)
		printf("%d ", i+1);

	printf("\tn°\tfile name\n");
	
	int counter=0;
	for(int j=0; j<files_number; j++){
		
		list = fopen("list.txt", "r");
		
		
		//copio contenuto di line dentro a un'altra stringa, 
		//sennò se tengo puntatore, alla lettura successiva cambia
		char *primo = strcpy( malloc(MAXLS), read_line(j, list));	
		
		printf("\n");
		char *dicomand = malloc (MAXLS*2 + 20); //diff + primo +secondo+ >/dev/null
		
		for (int k=0; k<files_number; k++){
			bool r;		//risultato: false se uguali, true se diversi
			if (j==k)	//sono lo stesso file, elimino un po' di combinazioni
				r=false;		//è ovvio che sono uguali
			else {
				list = fopen("list.txt", "r");
				char *secondo = read_line(k, list);
				
				sprintf (dicomand, "diff %s %s >/dev/null", primo, secondo);		//compongo comando
				
#ifdef DEBUG
	printf("\ncomando : %s", dicomand);
#endif
				int aux = system(dicomand);		//lo eseguo e salvo l'exit status di diff (0 if inputs are the same, 1 if different, 2 if trouble)
				if (aux==2){
					fprintf(stderr, "diff comand failed!!");
					exit(2);
				}
				else
					r = aux;
				fclose(list);
				counter++;
			}
			
			battagliaNavale[j][k] = r;		//ottengo tabella doppia entrata con risultati combinazioni
// 			battagliaNavale[k][j] = r;
			
			
			if(argc>3/* && ( argv[2] == "l" || argv[2] == "-l")*/){		//non so perchè non va, non ho voglia di perderci altro tempo, rimane come funzione nascosta
				if(!battagliaNavale[j][k])
					printf("* ");
				else
					printf("  ");
			}
			else
				printf("%d ", battagliaNavale[j][k]);
		}
		printf("\t%d\t%s", j+1, primo);
	}
	
#ifdef DEBUG
	printf("\n%d\n", counter);
#endif
	printf("\n\n");
}


char *read_line (int lineNumber, FILE *file) {
	int count=0;
	while ( fgets(line, MAXLS, file) != NULL ) /* read a line */
	{
		if (count == lineNumber){
			//use line or in a function return it
			//in case of a return first close the file with "fclose(file);"
			remove_lf(line);
			return line;
		}
		else{
			count++;
		}
	}
	//source: https://stackoverflow.com/questions/21114591/c-programming-read-specific-line-from-text-file/21114671
}

bool remove_lf (char *string) {
	
	int i;
	for (i = 0; string[i] != '\n' && string[i] != '\0'; i++);
	if(string[i] == '\n'){
		string[i]='\0';
		return true;
	}
	else
		return false;
}


/*
 * OTTIMIZAZIONI POSSIBILI:
 * 
 * -dimezzare confronti (vedi commento riga 78)
 * -opzioni di visualizzazione(switch case?)
 * -eliminare lista.txt se richiesto
 * -creare stringa con nome file e passare quella (riscritta 5 volte!)
 * -visualizzare colonne allineate (come si fa?)
 */