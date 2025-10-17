#include <stdio.h>
#include <stdlib.h>

typedef struct Memb {
	short int uid;					// ID único
	char valid;						// Indicador de remoção
	short int size;					// Tamanho (em bytes) ocupado no arquivo

	enum {Alun, Prof} tipo;			// ~tipo~ do dado 
	
	char tamn;						// Quantidade de caracteres válidos no nome ( bytes ocupados pelo nome no arquivo)
	char nome[64];					// Nome da pessoa

	union {
		struct {					// Tipo = Aluno
			long int matr;			// Matrícula
			char peri;				// Período
			char discs;				// Quantidade de disciplinas registradas

			int grade[5][18];		// Grade de horários; primerio índice indica dia da semana [seg <-> sex]; segundo índice indica turno [m1 <-> t3 <-> n6]; 
									// elementos são codigos de disciplina
		};

		struct {					// Tipo = Professor
			int id;					// Número de identificação do docente
			float slro;				// Salário
			char turms;				// Quantidade de turmas 

			int aulas[5][18]; 		// Grade das aulas por dia e horario; elementos são as salas em que ocorrerão as aulas
		};
	};
} Membro;


void *memst(void *s, char c, size_t n) {
	for (int i = 0; i < n; i++) {
		*((char *) (s + i)) = c;
	}
	return s;	// Analogo ao memset de <string.h>
}

int strin(char * in, int size, FILE * f) {		// Recebe strings usando fgets e remove caracteres indesejados
	fgets(in, size, f);
	int tamn = 0;
	while (in[tamn] != '\0') {
		if (in[tamn] == '\n' || in[tamn] == '\r') {
			in[tamn] = '\0';
			break;
		}
		tamn++;
	}
	return tamn;
}



int Preenche(Membro * pss, short int uid) {		
	// Pede informações do usuário para criar um novo membro
	// id unico deve ser gerenciado externamente
	// retorna 0 se nome nao for fornecido, 1 caso contrario

	pss->uid = uid;
	pss->valid = 1;
	pss->size = sizeof(pss->uid) + sizeof(pss->size) + sizeof(pss->valid);


	char input[16] = {0};			// Variavél usada para receber e avaliar entrada do usuario
	unsigned char chce = 0;			// Variavel usada para saber se o usuario quer adicionar mais disciplinas / turmas
	int disci;						// Variavel para armazenar o codigo da disciplina / numero da sala a ser adicionado à grade
	int dia = 0, hora = 0;			// dia escolhido pelo usuario [1 <-> 5]; horario escolhido pelo usuario [1 <-> 18]


	printf("Nome: ");
	pss->tamn = strin(pss->nome, 64, stdin);
	pss->size += sizeof(char)*(pss->tamn) + sizeof(pss->tamn);

	if(pss->tamn < 1) {
		return 0;
	}

	printf("[ 0 ] Aluno // [ 1 ] Professor: ");
	fgets(input, 16, stdin);
	sscanf(input, "%d", (int *) &pss->tipo);
	pss->tipo %= 2;

	pss->size += sizeof(char);		// ~= tipo

	if (pss->tipo == Alun) {
		pss->discs = 0;
		pss->size += sizeof(pss->discs);
		
		memst(pss->grade, 0, sizeof(int)*5*18);		// Limpa a grade ( 0 => vazio)

		printf("Matrícula: ");
		fgets(input, 16, stdin);
		sscanf(input, "%ld", &pss->matr);
		pss->size += sizeof(pss->matr);

		printf("Período: ");
		fgets(input, 16, stdin);
		sscanf(input, "%hhd", &pss->peri);
		pss->size += sizeof(pss->peri);

		printf("Deseja adicionar uma disciplina à grade? [ S ] / [ N ]: ");
		fgets(input, 16, stdin);
		sscanf(input, "%c", &chce);
		chce %= 2;
		


		while (chce) {
			printf("Insira o código da disciplina: ");
			fgets(input, 16, stdin);
			sscanf(input, "%d", &disci);

			while (1) {
				do {
					printf("Insira um dia da semana [ 1 <-> 5 ]: ");
					fgets(input, 16, stdin);
				} while(sscanf(input, "%d", &dia) < 1);


				if (dia <= 0 || dia > 5) {			// caso o dia escolhido for invalido, a inserção é cancelada
					break;
				}

				while (1) {
					do {
						printf("Insira um horário [ 1 <-> 18 ]: ");
						fgets(input, 16, stdin);
					} while(sscanf(input, "%d", &hora) < 1);


					if (hora <= 0 || hora > 18) {	// caso o horario escolhido for invalido, a inserção é cancelada
						break;
					}


					if (pss->grade[dia-1][hora-1] != 0) {		// sobrescrever uma disciplina com outra não altera a quantidade de disciplinas totais
						pss->discs--;							// operação será revertida, exceto quando o valor novo for 0 (*)
					}

					pss->grade[dia-1][hora-1] = disci;

					

					if (disci != 0) {							// (*)
						pss->discs++;
					}


				}
			}

			printf("Deseja inserir outra disciplina [ S ] / [ N ]: ");
			fgets(input, 16, stdin);
			if (sscanf(input, "%c", &chce) < 1) {
				chce = 0;
			}
			chce %= 2;		// Na tabela ascii, S/s são valores impares e N/n são pares
		}

		pss->size += sizeof(int)*2*pss->discs;		// No arquivo, cada entrada não nula da grade é acompanhada de um inteiro correpondente à sua posição
	}

	else if (pss->tipo == Prof) {
		memst(pss->aulas, 0, sizeof(int)*5*18);		// Limpa a grade ( 0 => vazio)
		pss->turms = 0;
		pss->size += sizeof(pss->turms);

		printf("Nº Identificação: ");
		fgets(input, 16, stdin);
		sscanf(input, "%d", &pss->id);
		pss->size += sizeof(pss->id);

		printf("Salário: ");
		fgets(input, 16, stdin);
		sscanf(input, "%f", &pss->slro);
		pss->size += sizeof(pss->slro);

		printf("Deseja adicionar uma aula [ S ] / [ N ]: ");
		fgets(input, 16, stdin);
		sscanf(input, "%c", &chce);
		chce %= 2;

		while (chce) {
			printf("Insira a sala: ");
			fgets(input, 16, stdin);
			sscanf(input, "%d", &disci);

			while (1) {
				do {
					printf("Insira um dia da semana [ 1 <-> 5 ]: ");
					fgets(input, 16, stdin);
				} while(sscanf(input, "%d", &dia) < 1);

				if (dia <= 0 || dia > 5) {				// caso o dia escolhido for invalido, a inserção é cancelada
					//chce = 0;
					break;
				}

				while (1) {
					do {
						printf("Insira um horário [ 1 <-> 18 ]: ");
						fgets(input, 16, stdin);
					} while(sscanf(input, "%d", &hora) < 1);

					if (hora <= 0 || hora > 18) {			// caso o horario escolhido for invalido, a inserção é cancelada
						//chce = 0;
						break;
					}

					if (pss->aulas[dia-1][hora-1] != 0) {		// (*)
						pss->turms--;
					}

					pss->aulas[dia-1][hora-1] = disci;

					if (disci != 0) {						//(*)
						pss->turms++;
					}
				}
			}

			printf("Deseja adicionar outra aula [ S ] / [ N ]: ");
			fgets(input, 16, stdin);
			if (sscanf(input, "%c", &chce) < 1) {
				chce = 0;
			}
			chce %= 2;				// Na tabela ascii, S/s são valores impares e N/n são pares
		}
		pss->size += sizeof(int)*2*pss->turms;			// No arquivo, cada entrada não nula da grade é acompanhada de um inteiro correpondente à sua posição
	}


	return 1;
}

void Exibe(Membro * pss) {
	printf("%d\n", pss->uid);
	printf("%hhd\n", pss->valid);
	printf("%d\n", pss->size);


	printf("%d\n", pss->tipo);
	printf("%s\n", pss->nome);
	printf("%hhd\n", pss->tamn);

	switch (pss->tipo) {
	case Alun: 
		printf("%ld\n", pss->matr);
		printf("%hhd\n", pss->peri);

		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 18; j++) {
				printf("%d ", pss->grade[i][j]);
			}
			printf("\n");
		}

		printf("%hhd\n", pss->discs);
		break;

	case Prof:
		printf("%d\n", pss->id);
		printf("%.2f\n", pss->slro);

		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 18; j++) {
				printf("%d ", pss->aulas[i][j]);
			}
			printf("\n");
		}

		printf("%hhd\n", pss->turms);
		break;
	default:
		break;
	}
}

void wAlun(Membro * pss, FILE * f) {		// escreve dados referentes a aluno no arquivo
	fwrite(&pss->matr, sizeof(long int), 1, f);
	fwrite(&pss->peri, sizeof(char), 1, f);

	fwrite(&pss->discs, sizeof(char), 1, f);

	for (int i = 0; i < 5*18; i++) {
		if ( *( ((int *)(pss->grade)) + i ) != 0 ) {
			fwrite(&i, sizeof(int), 1, f);
			fwrite( (((int *)(pss->grade)) + i), sizeof(int), 1, f);
		}
	}	
}

void wProf(Membro * pss, FILE * f) {		// escreve dados referentes a professor no arquivo
	fwrite(&pss->id, sizeof(int), 1, f);
	fwrite(&pss->slro, sizeof(float), 1, f);

	fwrite(&pss->turms, sizeof(char), 1, f);

	for (int i = 0; i < 5*18; i++) {
		if ( *( ((int *)(pss->aulas)) + i ) != 0 ) {
			fwrite(&i, sizeof(int), 1, f);
			fwrite( (((int *)(pss->aulas)) + i), sizeof(int), 1, f);
		}
	}
}

void Armazena(Membro * pss) {			// escreve os dados no arquivo
	FILE * f = fopen("dados.2.bin", "a");

	fwrite(&pss->uid, sizeof(int), 1, f);
	fwrite(&pss->valid, sizeof(int), 1, f);
	fwrite(&pss->size, sizeof(int), 1, f);

	fwrite((char *) &pss->tipo, sizeof(char), 1, f);
	fwrite(&pss->tamn, sizeof(char), 1, f);
	fwrite(pss->nome, sizeof(char), pss->tamn, f);

	if (pss->tipo == Alun) {
		wAlun(pss, f);
	}
	else {
		wProf(pss, f);
	}

	fclose(f);
}

Membro Le() {			// Lê e retorna um membro do arquivo   //!! inacabada !!
	Membro pss;
	FILE * f = fopen("dados.2.bin", "r");
	while (1) {
		fread(&pss.uid, sizeof(int), 1, f);
		fread(&pss.valid, sizeof(int), 1, f);
		if (pss.valid < 1) {
			break;
		}
		fread(&pss.size, sizeof(int), 1, f);

		fread(&pss.tipo, sizeof(char), 1, f);
		fread(&pss.tamn, sizeof(char), 1, f);
		fread(pss.nome, sizeof(char), pss.tamn, f);
		if (pss.tipo == Alun) {
			memst(pss.grade, 0, sizeof(int)*5*18);
			fread(&pss.matr, sizeof(long int), 1, f);
			fread(&pss.peri, sizeof(char), 1, f);
			fread(&pss.discs, sizeof(char), 1, f);
			for (int k = 0; k < pss.discs; k++) {
				int i = 0;
				fread(&i, sizeof(int), 1, f);
				fread((int *) (pss.grade) + i, sizeof(int), 1, f);
			}
		}
		else {
			memst(pss.aulas, 0, sizeof(int)*5*18);
			fread(&pss.id, sizeof(int), 1, f);
			fread(&pss.slro, sizeof(float), 1, f);
			fread(&pss.turms, sizeof(char), 1, f);
			for (int k = 0; k < pss.turms; k++) {
				int i = 0;
				fread(&i, sizeof(int), 1, f);
				fread((int *) (pss.aulas) + i, sizeof(int), 1, f);
			}
		}
		Exibe(&pss);
	}
	fclose(f);
	return pss;
}


int main(void) {
	//data * banco = NULL;
	//data * u = NULL;
	int cont = 0;			//"id unico" temporario

	Membro pss1;

	char input[3];

	//Le(&banco, &u);

	while (1) {

		if(Preenche(&pss1, cont)) {

			//Insere(&banco, &u, pss1);

			printf("\n");

			Exibe(&pss1);

			Armazena(&pss1);

			cont++;
		}

		printf("Sair? [ S ] / [ N ]: ");
		fgets(input, 3, stdin);
		sscanf(input, "%c", input);
		input[0] %= 2;

		if (input[0]) {
			return 0;
		}
	}
}