#include <stdio.h>

typedef struct Memb {
	short int uid;					// ID único
	char valido;					// Indicador de remoção

	enum {Alun, Prof} tipo;			// ~tipo~ do dado 
	
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

int writerb(const void *ptr, size_t size, size_t nmemb, FILE *stream) {				// variação de byt += writerb para retornar quantidade de bytes escritos
	printf("%lu\n", size);
	fwrite(ptr, size, nmemb, stream);
	return (int) size*nmemb;
}

void *memst(void *s, char c, size_t n) {
	for (int i = 0; i < n; i++) {
		*((char *) (s + i)) = c;
	}
	return s;	// Analogo ao memset de <string.h>
}

int strin(char * in, int size, FILE * f) {		// Recebe strings usando fgets e remove caracteres indesejados
	fgets(in, size, f);
	int tamn = 0;
	while (tamn < size && in[tamn] != '\0') {
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
	pss->valido = 1;


	char input[16] = {0};			// Variavél usada para receber e avaliar entrada do usuario
	unsigned char chce = 0;			// Variavel usada para saber se o usuario quer adicionar mais disciplinas / turmas
	int elemento;					// Variavel para armazenar o codigo da disciplina / numero da sala a ser adicionado à grade
	int dia = 0, hora = 0;			// dia escolhido pelo usuario [1 <-> 5]; horario escolhido pelo usuario [1 <-> 18]


	printf("Nome: ");
	if(strin(pss->nome, 64, stdin) < 1) {
		return 0;
	}

	printf("[ 0 ] Aluno // [ 1 ] Professor: ");
	fgets(input, 16, stdin);
	sscanf(input, "%d", (int *) &pss->tipo);
	pss->tipo %= 2;

	if (pss->tipo == Alun) {
		pss->discs = 0;
		
		memst(pss->grade, 0, sizeof(int)*5*18);		// Limpa a grade ( 0 => vazio)

		printf("Matrícula: ");
		fgets(input, 16, stdin);
		sscanf(input, "%ld", &pss->matr);

		printf("Período: ");
		fgets(input, 16, stdin);
		sscanf(input, "%hhd", &pss->peri);

		printf("Deseja adicionar uma disciplina à grade? [ S ] / [ N ]: ");
		fgets(input, 16, stdin);
		sscanf(input, "%c", &chce);
		chce %= 2;				// Na tabela ascii, S/s são valores impares e N/n são pares


		while (chce) {
			printf("Insira o código da disciplina: ");
			fgets(input, 16, stdin);
			sscanf(input, "%d", &elemento);

			while (1) {
				do {
					printf("Insira um dia da semana [ 1 <-> 5 ]: ");
					fgets(input, 16, stdin);
				} while(sscanf(input, "%d", &dia) < 1);


				if (dia < 1 || dia > 5) {			// caso o dia escolhido for invalido, a inserção é cancelada
					break;
				}

				while (1) {
					do {
						printf("Insira um horário [ 1 <-> 18 ]: ");
						fgets(input, 16, stdin);
					} while(sscanf(input, "%d", &hora) < 1);


					if (hora < 1 || hora > 18) {	// caso o horario escolhido for invalido, a inserção é cancelada
						break;
					}


					if (pss->grade[dia-1][hora-1] != 0) {		// sobrescrever uma disciplina com outra não altera a quantidade de disciplinas totais
						pss->discs--;							// operação será revertida, exceto quando o valor novo for 0 (*)
					}

					pss->grade[dia-1][hora-1] = elemento;

					

					if (elemento != 0) {							// (*)
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

	}

	else if (pss->tipo == Prof) {
		memst(pss->aulas, 0, sizeof(int)*5*18);		// Limpa a grade ( 0 => vazio)
		pss->turms = 0;

		printf("Nº Identificação: ");
		fgets(input, 16, stdin);
		sscanf(input, "%d", &pss->id);

		printf("Salário: ");
		fgets(input, 16, stdin);
		sscanf(input, "%f", &pss->slro);

		printf("Deseja adicionar uma aula [ S ] / [ N ]: ");
		fgets(input, 16, stdin);
		sscanf(input, "%c", &chce);
		chce %= 2;						// Na tabela ascii, S/s são valores impares e N/n são pares

		while (chce) {
			printf("Insira a sala: ");
			fgets(input, 16, stdin);
			sscanf(input, "%d", &elemento);

			while (1) {
				do {
					printf("Insira um dia da semana [ 1 <-> 5 ]: ");
					fgets(input, 16, stdin);
				} while(sscanf(input, "%d", &dia) < 1);

				if (dia < 1 || dia > 5) {				// caso o dia escolhido for invalido, a inserção é cancelada
					break;
				}

				while (1) {
					do {
						printf("Insira um horário [ 1 <-> 18 ]: ");
						fgets(input, 16, stdin);
					} while(sscanf(input, "%d", &hora) < 1);

					if (hora < 1 || hora > 18) {			// caso o horario escolhido for invalido, a inserção é cancelada
						break;
					}

					if (pss->aulas[dia-1][hora-1] != 0) {		// (*)
						pss->turms--;
					}

					pss->aulas[dia-1][hora-1] = elemento;

					if (elemento != 0) {						//(*)
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
	}
	return 1;
}

void Exibe(Membro * pss) {
	printf("%hd\n", pss->uid);
	printf("%hhd\n", pss->valido);

	printf("%d\n", pss->tipo);
	printf("%s\n", pss->nome);

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

int wAlun(Membro * pss, FILE * f) {		// escreve dados referentes a aluno no arquivo
	int byt = 0;				// contagem de bytes escritos

	byt += writerb(&pss->matr, sizeof(pss->matr), 1, f);
	byt += writerb(&pss->peri, sizeof(pss->peri), 1, f);

	byt += writerb(&pss->discs, sizeof(pss->discs), 1, f);

	byt += writerb(pss->grade, sizeof(pss->grade), 1, f);

	return byt;
}

int wProf(Membro * pss, FILE * f) {		// escreve dados referentes a professor no arquivo
	int byt = 0;				// contagem de bytes escritos

	byt += writerb(&pss->id, sizeof(pss->id), 1, f);
	byt += writerb(&pss->slro, sizeof(pss->slro), 1, f);

	byt += writerb(&pss->turms, sizeof(pss->turms), 1, f);

	byt += writerb(pss->aulas, sizeof(pss->aulas), 1, f);

	return byt;
}

int Armazena(Membro * pss) {			// escreve os dados no arquivo
	int msiz = sizeof(short int) + (sizeof(char)*68) + sizeof(long int) + (sizeof(int)*5*18);	// quantidade de bytes do pior caso (aluno)
	int byt = 0;				// contagem de bytes escritos

	FILE * f = fopen("dados.3.bin", "a");

	byt += writerb(&pss->uid, sizeof(pss->uid), 1, f);
	byt += writerb(&pss->valido, sizeof(pss->valido), 1, f);

	byt += writerb(&pss->tipo, sizeof(char), 1, f);

	byt += writerb(pss->nome, sizeof(pss->nome), 1, f);

	if (pss->tipo == Alun) {
		byt += wAlun(pss, f);
	}
	else {
		byt += wProf(pss, f);
	}

	printf("%d %d\n", msiz, byt);

	if (sizeof(Membro) > byt) {
		fwrite(pss, sizeof(char), msiz - byt, f);
	}

	fclose(f);

	return byt;
}

Membro Le() {			// Lê e retorna um membro do arquivo   //!! inacabada !!
	Membro pss;
	FILE * f = fopen("dados.3.bin", "r");
	while (1) {
		fread(&pss.uid, sizeof(short int), 1, f);
		fread(&pss.valido, sizeof(char), 1, f);
		if (pss.valido < 1) {
			break;
		}

		fread(&pss.tipo, sizeof(int), 1, f);

		fread(pss.nome, sizeof(char), 64, f);
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