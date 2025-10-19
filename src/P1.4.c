#include <stdio.h>
#include <stdlib.h>

#define TAM_NOME 64
#define DIAS 5
#define TURNOS 18
#define TAM_INPUT 16
#define TAM_REGISTRO 438L

#define ARQUIVO_DADOS "dados.4.bin"

typedef struct {					// Tipo = Aluno
	long int matr;			// Matrícula
	char peri;				// Período
	char discs;				// Quantidade de disciplinas registradas
	int grade[5][18];		// Grade de horários; primerio índice indica dia da semana [seg <-> sex]; segundo índice indica turno [m1 <-> t3 <-> n6]; 
} Dados_aluno;

typedef struct {					// Tipo = Professor
	int id;					// Número de identificação do docente
	float slro;				// Salário
	char turms;				// Quantidade de turmas 
	int aulas[5][18]; 		// Grade das aulas por dia e horario; elementos são as salas em que ocorrerão as aulas
} Dados_professor;

typedef struct {
	short int uid;					// ID único
	char valido;					// Indicador de remoção
	//enum {Alun, Prof} tipo;
	enum __attribute__ ((__packed__)) {Alun, Prof} tipo;			// ~tipo~ do dado 
	char nome[64];					// Nome da pessoa

	/*
	enum {
		ALUNO = 0, 
		PROF = 1
	} tipo;
	*/

	union {
		Dados_aluno aluno;
		Dados_professor professor;
	} dados;
} Membro;



void Menu(void);
int Preenche(Membro * pss, short int uid);
int PreencheAluno(Membro * pss, short int uid);
int PreencheProfessor(Membro * pss, short int uid);

long BuscarRegistroId();

void RemoverRegistro(long posicao_registro);
Membro Le(long posicao_registro);
void Exibe(Membro * pss);
void ListarTodosRegistros();

void ListaCompacto();
int LeUIDeNome(FILE * f, short *uid, char *valido, char nome[TAM_NOME]);
short BuscaPorNome();
int prefixo(char *str1, char *str2);

int strin(char * in, int size, FILE * f);
void *memst(void *s, char c, size_t n);
char SouN(char * in, int size, FILE * f);
char *strncpy(char *dest, const char *src, size_t n);
short UltUID(FILE * f);

size_t writerb(const void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t wAlun(Membro * pss, FILE * f);
size_t wProf(Membro * pss, FILE * f);
int Armazena(Membro * pss, FILE * f);

void AlunEdit(Membro * pss, char * inpt, int sze);
void ProfEdit(Membro * pss, char * inpt, int sze);
int EditarRegistro(Membro * pss);


int main(void) {
	//Membro pss;
	//printf("%zu", sizeof(pss.tipo));
	Menu();

	return 0;
}

void Menu(void) {
	int opcao;
	char input[TAM_INPUT];

	FILE * f = fopen(ARQUIVO_DADOS, "rb");
	int cont = UltUID(f) + 1;
	fclose(f);

	Membro pss1;

	system("clear||cls");

	for(;;)
	{	
		
		printf(
		"\n"
		"██    ██ ███    ██ ██     ██   ██ ███████ ██      ██████  ███████ ██████ \n"
		"██    ██ ████   ██ ██     ██   ██ ██      ██      ██   ██ ██      ██   ██\n"
		"██    ██ ██ ██  ██ ██     ███████ █████   ██      ██████  █████   ██████ \n"
		"██    ██ ██  ██ ██ ██     ██   ██ ██      ██      ██      ██      ██   ██\n"
		" ██████  ██   ████ ██     ██   ██ ███████ ███████ ██      ███████ ██   ██\n\n"
		"1 Criar novo registro\n"
		"2 Editar registro\n"
		"3 Remover registro\n"
		"4 Buscar registro por ID\n"
		"5 Buscar registro por nome\n"
		"6 Listar todos os registros\n"
		"7 Sair\n\n"
		"Escolha uma opção: "
		);

		if (fgets(input, sizeof(input), stdin) == NULL) {
			system("clear||cls");
			fprintf(stderr, "Entrada inválida\n");
			continue;
		}

		if (sscanf(input, "%d", &opcao) != 1) {
			system("clear||cls");
			printf("Entrada inválida\n");
			continue;
		}

		switch (opcao) 
		{
			case 1: 
				printf("%hd", cont);
				if (Preenche(&pss1, cont)) {

					FILE * f = fopen(ARQUIVO_DADOS, "ab");
					Armazena(&pss1, f);
					fclose(f);

					cont++;
				}
				break;

			case 2: 
				ListaCompacto();

				long memb_pointer = BuscarRegistroId();

				while (memb_pointer == -1) {
					memb_pointer = BuscarRegistroId();
				}

				if (memb_pointer == -2) {
					break;
				}

				pss1 = Le(memb_pointer);

				if (pss1.valido) {
					if(!EditarRegistro(&pss1)) {
						break;
					}
				}
				else {
					printf("Deseja restaurar entrada removida? [ S ] / [ N ]");
					if (SouN(input, TAM_INPUT, stdin)) {
						if(!Preenche(&pss1, memb_pointer/TAM_REGISTRO)) {
							printf("Ação interrompida.");
							break;
						}
					}
					else {
						break;
					}
				}
				system("clear||cls");
				FILE * f = fopen(ARQUIVO_DADOS, "rb+");
				fseek(f, memb_pointer, SEEK_SET);
				Armazena(&pss1, f);
				fclose(f);

				break;

			case 3: 
				RemoverRegistro(BuscarRegistroId());
				break;
			case 4: 
				pss1 = Le(BuscarRegistroId());
				break;
			case 5: 
				pss1 = Le(BuscaPorNome());
				break;
			case 6:
				system("clear||cls");
				ListarTodosRegistros();
				break;
			case 7: return (void) 0;
			default:
				system("clear||cls");
				fprintf(stderr, "Opção inválida\n");
				break;
		}
	}
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


char SouN(char * in, int size, FILE * f) {
	char opt;
	if (fgets(in, size, f) == NULL || sscanf(in, "%c", &opt) != 1) {
		return 0;
	}
	return opt % 2;
}



int Preenche(Membro * pss, short int uid) {		
	char input[TAM_INPUT] = {0};
						
	printf("\nNome: ");
	if(strin(pss->nome, TAM_NOME, stdin) < 1) {
		return 0;
	}

	//printf("%s", pss->nome);

	printf("[ 0 ] Aluno // [ 1 ] Professor: ");
	fgets(input, TAM_INPUT, stdin);
	sscanf(input, "%hhd", (char *) &pss->tipo);
	pss->tipo %= 2; //* qualquer valor pode recair como 0 ou 1

	if (pss->tipo == Alun)
		PreencheAluno(pss,uid);

	else if (pss->tipo == Prof) 
		PreencheProfessor(pss,uid);

	printf("Exibir o último registro criado [ S ] / [ N ]: ");
	fgets(input, TAM_INPUT, stdin);
	if (input[0] %= 2)
		Exibe(pss);

	return 1;
}

int PreencheAluno(Membro * pss, short int uid)
{
	pss->uid = uid;
	pss->valido = 1;

	char input[TAM_INPUT] = {0};			// Variavél usada para receber e avaliar entrada do usuario
	unsigned char chce = 0;			// Variavel usada para saber se o usuario quer adicionar mais disciplinas / turmas
	int elemento;					// Variavel para armazenar o codigo da disciplina / numero da sala a ser adicionado à grade
	int dia = 0, hora = 0;			// dia escolhido pelo usuario [1 <-> 5]; horario escolhido pelo usuario [1 <-> 18]
						
	memst(pss->dados.aluno.grade, 0, sizeof(int)*DIAS*TURNOS);		// Limpa a grade ( 0 => vazio)
	pss->dados.aluno.discs = 0;

	printf("Matrícula: ");
	fgets(input, TAM_INPUT, stdin);
	sscanf(input, "%ld", &pss->dados.aluno.matr);

	printf("Período: ");
	fgets(input, TAM_INPUT, stdin);
	sscanf(input, "%hhd", &pss->dados.aluno.peri);

	printf("Deseja adicionar uma disciplina à grade? [ S ] / [ N ]: ");
	fgets(input, TAM_INPUT, stdin);
	sscanf(input, "%c", &chce);
	chce %= 2;				// Na tabela ascii, S/s são valores impares e N/n são pares


	while (chce) {
		printf("Insira o código da disciplina: ");
		fgets(input, TAM_INPUT, stdin);
		sscanf(input, "%d", &elemento);

		while (1) {
			do {
				printf("Insira um dia da semana [ 1 <-> 5 ]: ");
				fgets(input, TAM_INPUT, stdin);
			} while(sscanf(input, "%d", &dia) < 1);


			if (dia < 1 || dia > DIAS) {			// caso o dia escolhido for invalido, a inserção é cancelada
				break;
			}

			while (1) {
				do {
					printf("Insira um horário [ 1 <-> 18 ]: ");
					fgets(input, TAM_INPUT, stdin);
				} while(sscanf(input, "%d", &hora) < 1);


				if (hora < 1 || hora > TURNOS) {	// caso o horario escolhido for invalido, a inserção é cancelada
					break;
				}


				if (pss->dados.aluno.grade[dia-1][hora-1] != 0) {		// sobrescrever uma disciplina com outra não altera a quantidade de disciplinas totais
					pss->dados.aluno.discs--;							// operação será revertida, exceto quando o valor novo for 0 (*)
				}

				pss->dados.aluno.grade[dia-1][hora-1] = elemento;


				if (elemento != 0) {							// (*)
					pss->dados.aluno.discs++;
				}
			}
		}

		printf("Deseja inserir outra disciplina [ S ] / [ N ]: ");
		fgets(input, TAM_INPUT, stdin);
		if (sscanf(input, "%c", &chce) < 1) {
			chce = 0;
		}
		chce %= 2;		// Na tabela ascii, S/s são valores impares e N/n são pares
	}
}

int PreencheProfessor(Membro * pss, short int uid)
{
	pss->uid = uid;
	pss->valido = 1;

	char input[TAM_INPUT] = {0};			// Variavél usada para receber e avaliar entrada do usuario
	unsigned char chce = 0;			// Variavel usada para saber se o usuario quer adicionar mais disciplinas / turmas
	int elemento;					// Variavel para armazenar o codigo da disciplina / numero da sala a ser adicionado à grade
	int dia = 0, hora = 0;			// dia escolhido pelo usuario [1 <-> 5]; horario escolhido pelo usuario [1 <-> 18]

	memst(pss->dados.professor.aulas, 0, sizeof(int)*DIAS*TURNOS);		// Limpa a grade ( 0 => vazio)
	pss->dados.professor.turms = 0;

	printf("Nº Identificação: ");
	fgets(input, TAM_INPUT, stdin);
	sscanf(input, "%d", &pss->dados.professor.id);

	printf("Salário: ");
	fgets(input, TAM_INPUT, stdin);
	sscanf(input, "%f", &pss->dados.professor.slro);

	printf("Deseja adicionar uma aula [ S ] / [ N ]: ");
	fgets(input, TAM_INPUT, stdin);
	sscanf(input, "%c", &chce);
	chce %= 2;						// Na tabela ascii, S/s são valores impares e N/n são pares

	while (chce) {
		printf("Insira a sala: ");
		fgets(input, TAM_INPUT, stdin);
		sscanf(input, "%d", &elemento);

		while (1) {
			do {
				printf("Insira um dia da semana [ 1 <-> 5 ]: ");
				fgets(input, TAM_INPUT, stdin);
			} while(sscanf(input, "%d", &dia) < 1);

			if (dia < 1 || dia > DIAS) {				// caso o dia escolhido for invalido, a inserção é cancelada
				break;
			}

			while (1) {
				do {
					printf("Insira um horário [ 1 <-> 18 ]: ");
					fgets(input, TAM_INPUT, stdin);
				} while(sscanf(input, "%d", &hora) < 1);

				if (hora < 1 || hora > TURNOS) {			// caso o horario escolhido for invalido, a inserção é cancelada
					break;
				}

				if (pss->dados.professor.aulas[dia-1][hora-1] != 0) {		// (*)
					pss->dados.professor.turms--;
				}

				pss->dados.professor.aulas[dia-1][hora-1] = elemento;

				if (elemento != 0) {						//(*)
					pss->dados.professor.turms++;
				}
			}
		}
		printf("Deseja adicionar outra aula [ S ] / [ N ]: ");
		fgets(input, TAM_INPUT, stdin);
		if (sscanf(input, "%c", &chce) < 1) {
			chce = 0;
		}
		chce %= 2;				// Na tabela ascii, S/s são valores impares e N/n são pares
	}
}



void Exibe(Membro * pss) {
	printf("uid: %hd\n", pss->uid);
	printf("valido: %hhd\n", pss->valido);

	printf("tipo: %d\n", pss->tipo);
	printf("nome: %s\n", pss->nome);

	switch (pss->tipo) {
	case Alun: 
		printf("matrícula: %ld\n", pss->dados.aluno.matr);
		printf("período: %hhd\n", pss->dados.aluno.peri);

		printf("\ngrade:\n");
		for (int i = 0; i < DIAS; i++) {
			for (int j = 0; j < TURNOS; j++) {
				printf("%5d ", pss->dados.aluno.grade[i][j]);
			}
			printf("\n");
		}

		printf("disciplinas: %hhd\n", pss->dados.aluno.discs);
		break;

	case Prof:
		printf("id: %d\n", pss->dados.professor.id);
		printf("salário: %.2f\n", pss->dados.professor.slro);

		printf("\naulas:\n");
		for (int i = 0; i < DIAS; i++) {
			for (int j = 0; j < TURNOS; j++) {
				printf("%5d ", pss->dados.professor.aulas[i][j]);
			}
			printf("\n");
		}

		printf("turmas: %hhd\n", pss->dados.professor.turms);
		break;
	default:
		break;
	}
}



void *memst(void *s, char c, size_t n) {
	for (int i = 0; i < n; i++) {
		*((char *) (s + i)) = c;
	}
	return s;	// Analogo ao memset de <string.h>
}

char *strncpy(char *dest, const char *src, size_t n) { 			// inplementação de strncpy encontrada no manual
	size_t i;
	for (i = 0; i < n && src[i] != '\0'; i++) {
		dest[i] = src[i];
	}
	for (; i < n; i++) {
		dest[i] = '\0';
	}
	return dest;
}

//pelo fato de usar const void *ptr, n'ao deveria converter antes para outro tipo, i.g. (int *) ptr
//[g]nao, typecasting é válido para constantes. só copiei os atributos do fwrite[g]
size_t writerb(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
	fwrite(ptr, size, nmemb, stream);

	return size*nmemb;
	//printf("tamanho do membro copiado: %lu\n", size);
	// pelo que vi num livro, o recomendado seria fazer:
	//printf("tamanho do membro copiado: %lu\n", (long unsigned) size); //considerando C89
	//printf("tamanho do membro copiado: %zu\n", size); // para C99
	//[g]👍[g]
}

size_t wAlun(Membro * pss, FILE * f) {		// escreve dados referentes a aluno no arquivo
	//int byt = 0;				// contagem de bytes escritos
	size_t byt = 0;				// contagem de bytes escritos

	/*
	byt += fwrite(&pss->dados.aluno.matr, sizeof(pss->dados.aluno.matr), 1, f);
	byt += fwrite(&pss->dados.aluno.peri, sizeof(pss->dados.aluno.peri), 1, f);
	byt += fwrite(&pss->dados.aluno.discs, sizeof(pss->dados.aluno.discs), 1, f);
	byt += fwrite(pss->dados.aluno.grade, sizeof(pss->dados.aluno.grade), 1, f);
	*/

	byt += writerb(&pss->dados.aluno.matr, sizeof(pss->dados.aluno.matr), 1, f);
	byt += writerb(&pss->dados.aluno.peri, sizeof(pss->dados.aluno.peri), 1, f);
	byt += writerb(&pss->dados.aluno.discs, sizeof(pss->dados.aluno.discs), 1, f);
	byt += writerb(pss->dados.aluno.grade, sizeof(pss->dados.aluno.grade), 1, f);

	printf("quantidade de bytes escritos [alun]: %zu\n", byt);

	return byt;
}

size_t wProf(Membro * pss, FILE * f) {		// escreve dados referentes a professor no arquivo
	//int byt = 0;				// contagem de bytes escritos
	size_t byt = 0;				//size_t fwrite(const void * restrict ptr,size_t size, size_t nmemb,FILE * restrict stream);

/*
	byt += fwrite(&pss->dados.professor.id, sizeof(pss->dados.professor.id), 1, f);
	byt += fwrite(&pss->dados.professor.slro, sizeof(pss->dados.professor.slro), 1, f);
	byt += fwrite(&pss->dados.professor.turms, sizeof(pss->dados.professor.turms), 1, f);
	byt += fwrite(pss->dados.professor.aulas, sizeof(pss->dados.professor.aulas), 1, f);
*/

	byt += writerb(&pss->dados.professor.id, sizeof(pss->dados.professor.id), 1, f);
	byt += writerb(&pss->dados.professor.slro, sizeof(pss->dados.professor.slro), 1, f);
	byt += writerb(&pss->dados.professor.turms, sizeof(pss->dados.professor.turms), 1, f);
	byt += writerb(pss->dados.professor.aulas, sizeof(pss->dados.professor.aulas), 1, f);
	

	printf("quantidade de bytes escritos [prof]: %zu\n", byt);

	return byt;
}

int Armazena(Membro * pss, FILE * f) {			// escreve os dados no arquivo
	//int msiz = sizeof(short int) + (sizeof(char)*68) + sizeof(long int) + (sizeof(int)*5*18);	// quantidade de bytes do pior caso (aluno)
	//por que 68 char, não seria 67? 

	//[g¡] a intenção era tratar o enum tipo como um char (por isso sizeof(char) na escrita do tipo).
	// __attribute__ ((__packed__)) adicionado ao struct reduz o tamanho do enum para 1 byte (nesse caso). 
	//sizeof(pss->tipo) recomendado caso tamanho mude de novo.[!g]

	/*
	size_t size_final = 0;
	size_final += sizeof(short int);		// uid
	size_final += sizeof(char);			// valido
	size_final += 64 * sizeof(char);		// nome
	size_final += sizeof(int);			// enum tipo (poderia fazer um cast para char)

	size_t size_aluno = 0;
	size_aluno += sizeof(long int);			// matr
	size_aluno += sizeof(char);			// peri
	size_aluno += sizeof(char);			// discs
	size_aluno += sizeof(int) * 5 * 18;		// grade

	size_t size_professor = 0;
	size_professor += sizeof(int);			// id
	size_professor += sizeof(float);		// slro
	size_professor += sizeof(char);			// turms
	size_professor += sizeof(int) * 5 * 18;		// aulas
	*/

	//[g¿] usar sizeof(elemento) [?g]
	size_t size_final = 0;
	size_final += sizeof(pss->uid);		// uid
	size_final += sizeof(pss->valido);			// valido
	size_final += sizeof(pss->tipo);			// enum tipo (poderia fazer um cast para char)
	size_final += sizeof(pss->nome);		// nome

	size_t size_aluno = 0;
	size_aluno += sizeof(pss->dados.aluno.matr);			// matr
	size_aluno += sizeof(pss->dados.aluno.peri);			// peri
	size_aluno += sizeof(pss->dados.aluno.discs);			// discs
	size_aluno += sizeof(pss->dados.aluno.grade);		// grade

	size_t size_professor = 0;
	size_professor += sizeof(pss->dados.professor.id);			// id
	size_professor += sizeof(pss->dados.professor.slro);		// slro
	size_professor += sizeof(pss->dados.professor.turms);			// turms
	size_professor += sizeof(pss->dados.professor.aulas);		// aulas

	size_final += (size_aluno > size_professor) ? size_aluno : size_professor; // 441 bytes
	printf("size_final: %zu\n",size_final);

	//int byt = 0;				// contagem de bytes escritos
	size_t byt = 0;				// contagem de bytes escritos

	//FILE * f = fopen(ARQUIVO_DADOS, "ab");

	byt += writerb(&pss->uid, sizeof(pss->uid), 1, f);
	byt += writerb(&pss->valido, sizeof(pss->valido), 1, f);

	//byt += writerb(&pss->tipo, sizeof(char), 1, f); //*** NO MEU COMPUTADOR, sizeof(tipo) = 4
	//byt += writerb(&pss->tipo, sizeof(int), 1, f); //*** NO MEU COMPUTADOR, sizeof(tipo) = 4
	byt += writerb(&pss->tipo, sizeof(pss->tipo), 1, f); //[g] com __attribute__ ((__packed__)) , sizeof(tipo) = 1 [g]

	byt += writerb(pss->nome, sizeof(pss->nome), 1, f);

	/*
	byt += fwrite(&pss->uid, sizeof(pss->uid), 1, f);
	byt += fwrite(&pss->valido, sizeof(pss->valido), 1, f);
	byt += fwrite(&pss->tipo, sizeof(char), 1, f);
	byt += fwrite(pss->nome, sizeof(pss->nome), 1, f);
	*/

	printf("quantidade de bytes escritos [comum]: %zu\n", byt);

	if (pss->tipo == Alun) {
		byt += wAlun(pss, f);
	}
	else {
		byt += wProf(pss, f);
	}

	printf("size_final: %zu\nsize real: %zu\n", size_final, byt);

	/*
	if (sizeof(Membro) > byt) {
		fwrite(pss, sizeof(char), size_final - byt, f);
	}
	*/

	if (size_final > byt)
	{
		size_t fill = size_final - byt;
		for (size_t i = 0; i < fill; i++)
		{
			fputc(0, f);
		}
	}

	//fclose(f);

	return (int) byt;
}

Membro Le(long posicao_registro) 
{
	Membro pss;

	FILE * f = fopen(ARQUIVO_DADOS, "rb");
	fseek(f, posicao_registro, SEEK_SET);

	fread(&pss.uid, sizeof(pss.uid), 1, f);
	fread(&pss.valido, sizeof(pss.valido), 1, f);
	fread(&pss.tipo, sizeof(pss.tipo), 1, f);
	fread(pss.nome, sizeof(pss.nome), 1, f);

	if (pss.tipo == Alun) {
		//memst(pss.dados.aluno.grade, 0, sizeof(int)*5*18);
		fread(&pss.dados.aluno.matr, sizeof(pss.dados.aluno.matr), 1, f);
		fread(&pss.dados.aluno.peri, sizeof(pss.dados.aluno.peri), 1, f);
		fread(&pss.dados.aluno.discs, sizeof(pss.dados.aluno.discs), 1, f);
		fread(pss.dados.aluno.grade, sizeof(pss.dados.aluno.grade), 1, f);
		/* não entendi essa parte, VOLTAR AQUI
		for (int k = 0; k < pss.dados.aluno.discs; k++) {
			int i = 0;
			fread(&i, sizeof(int), 1, f);
			fread((int *) (pss.dados.aluno.grade) + i, sizeof(int), 1, f);
		}
		*/
		// [g‼] ^ relacionada a estrutura antiga do arquivo, quando grade era armazenada por indice + entrada [‼g]
	}
	else {
		//memst(pss.dados.professor.aulas, 0, sizeof(int)*5*18);
		fread(&pss.dados.professor.id, sizeof(pss.dados.professor.id), 1, f);
		fread(&pss.dados.professor.slro, sizeof(pss.dados.professor.slro), 1, f);
		fread(&pss.dados.professor.turms, sizeof(pss.dados.professor.turms), 1, f);
		fread(pss.dados.professor.aulas, sizeof(pss.dados.professor.aulas), 1, f);
		/* não entendi essa parte, VOLTAR AQUI
		for (int k = 0; k < pss.dados.professor.turms; k++) {
			int i = 0;
			fread(&i, sizeof(int), 1, f);
			fread((int *) (pss.dados.professor.aulas) + i, sizeof(int), 1, f);
		}
		*/
		// [g‼] ^ relacionada a estrutura antiga do arquivo, quando grade era armazenada por indice + entrada [‼g]
	}

	if (pss.valido == 1)
		Exibe(&pss);

	fclose(f);
	return pss;
}

long BuscarRegistroId()
{
	FILE * file_pointer = fopen(ARQUIVO_DADOS, "rb");

	char input[TAM_INPUT] = {0};
	short int id_unico;

	printf("Selecione o valor do id único (uid): ");
	if (fgets(input, TAM_INPUT, stdin) == NULL || sscanf(input, "%hd", &id_unico) != 1 || id_unico < 0) {
		return -2;
	}

	long posicao_registro = id_unico * TAM_REGISTRO; //considerando uid em [0, inf) inter. Z
	
	fseek(file_pointer, 0L, SEEK_END);
	long end_file_pos = ftell(file_pointer);

	printf("%lu\n", end_file_pos);

	if (posicao_registro >= end_file_pos) {
		fprintf(stderr, "Registro com o id único %hd não existe\n", id_unico);
		fclose(file_pointer);
		return -1;
	}

	fclose(file_pointer);
	return posicao_registro;
}

void RemoverRegistro(long posicao_registro)
{
	FILE * file_pointer = fopen(ARQUIVO_DADOS, "rb+");
	
	short uid;
	fseek(file_pointer, posicao_registro, SEEK_SET);
	fread(&uid, sizeof(uid), 1, file_pointer);

	char zeros[TAM_REGISTRO] = {0};

	fseek(file_pointer, posicao_registro, SEEK_SET);
	fwrite(zeros, sizeof(char), TAM_REGISTRO, file_pointer); 

	fseek(file_pointer, posicao_registro, SEEK_SET);
	fwrite(&uid, sizeof(uid), 1, file_pointer);

	fclose(file_pointer);
}

void ListarTodosRegistros()
{
	FILE * file_pointer = fopen(ARQUIVO_DADOS, "rb+");

	fseek(file_pointer, 0L, SEEK_END);
	long end_file_pos = ftell(file_pointer);

	rewind(file_pointer);
	long cur_file_pos = ftell(file_pointer);

	for(; cur_file_pos < end_file_pos; cur_file_pos += TAM_REGISTRO)
	{
		Le(cur_file_pos);
	}

}

void ListaCompacto() {
	short uid;
	char valido;
	char nome[64];

	FILE * file_pointer = fopen(ARQUIVO_DADOS, "rb");

	fseek(file_pointer, 0l, SEEK_END);
	long end_file_pos = ftell(file_pointer);

	rewind(file_pointer);
	long cur_file_pos = ftell(file_pointer);

	for (; cur_file_pos < end_file_pos; cur_file_pos += TAM_REGISTRO) {
		fseek(file_pointer, cur_file_pos, SEEK_SET);
		
		if (LeUIDeNome(file_pointer, &uid, &valido, nome)) {
			strncpy(nome, "**Removido**", 64);
			uid = cur_file_pos / TAM_REGISTRO;
		}

		printf("[ %hd ] %s\n", uid, nome);
	}
}


int LeUIDeNome(FILE * f, short *uid, char *valido, char nome[TAM_NOME]) {
	if (uid != NULL) {
		fread(uid, sizeof(*uid), 1, f);
	}
	else {
		fseek(f, sizeof(short), SEEK_CUR);	
	}

	if (valido != NULL) {
		fread(valido, sizeof(*valido), 1, f);
	}
	else {
		fseek(f, sizeof(char), SEEK_CUR);	
	}

	fseek(f, sizeof(char), SEEK_CUR);
	fread(nome, sizeof(char), TAM_NOME, f);

	if (valido != NULL && *valido) {
		return 0;
	}
	else {
		return 1;
	}
}


short BuscaPorNome() {
	char busca[TAM_NOME];
	printf("Qual nome deseja buscar: ");
	if(strin(busca, TAM_NOME, stdin) < 1) {
		printf("Nome inválido!\n");
		return -1;
	}

	char nome[64] = {0};
	long cur_pos = 0;
	FILE * f = fopen(ARQUIVO_DADOS, "rb");
	fseek(f, 0l, SEEK_END);
	long end_pos = ftell(f);
	rewind(f);

	while (cur_pos < end_pos) {
		fseek(f, cur_pos, SEEK_SET);
		LeUIDeNome(f, NULL, NULL, nome);

		printf("%s\n", nome);
		printf("%d\n", prefixo(busca, nome));

		if (prefixo(busca, nome)) {
			return cur_pos;
		}
		cur_pos += TAM_REGISTRO;
	}

	printf("Nome não encontrado!\n");
	return -1;
}

int prefixo(char *str1, char *str2) {
	//printf("%s\n", str1);
	//printf("%s\n", str2);
	for (int i = 0; str1[i] != '\0'; i++) {
		if (str1[i] != str2[i]) {
			return 0;
		}
	}
	return 1;
}

int GradEdit(int tipo, int grade[DIAS][TURNOS], char * count) {
	int elemento;
	char input[TAM_INPUT];
	unsigned char chce = 0;
	int dia = 0, hora = 0;

	while (1) {
		printf("O que deseja fazer: \n");
		printf("[ 1 ] Inserir\n");
		printf("[ 2 ] Remover\n");
		printf("[ 3 ] Voltar\n");

		fgets(input, TAM_INPUT, stdin);
		sscanf(input, "%hhd", &chce);

		if (chce == 2) {
			elemento = 0;
		}
		else if (chce != 1) {
			return 0;
		}

		while (1) {
			if (chce == 1) {
				if (tipo == Alun) {
					printf("Insira a disciplina: ");
				}
				else {
					printf("Insira a sala: ");	
				}
				
				fgets(input, 16, stdin);
				sscanf(input, "%d", &elemento);
			}

			while (1) {
				do {
					printf("Insira um dia da semana [ 1 <-> 5 ]: ");
					fgets(input, TAM_INPUT, stdin);
				} while(sscanf(input, "%d", &dia) < 1);

				if (dia < 1 || dia > DIAS) {				// caso o dia escolhido for invalido, a inserção é cancelada
					break;
				}

				while (1) {
					do {
						printf("Insira um horário [ 1 <-> 18 ]: ");
						fgets(input, TAM_INPUT, stdin);
					} while(sscanf(input, "%d", &hora) < 1);

					if (hora < 1 || hora > TURNOS) {			// caso o horario escolhido for invalido, a inserção é cancelada
						break;
					}

					if (grade[dia-1][hora-1] != 0) {		// (*)
						(*count)--;
						//printf("%hhd", *count);
					}

					grade[dia-1][hora-1] = elemento;

					if (elemento != 0) {						//(*)
						(*count)++;
						//printf("%hhd", *count);
					}
				}
			}

			printf("Deseja alterar outro elemento [ S ] / [ N ]: ");
			fgets(input, TAM_INPUT, stdin);
			if (sscanf(input, "%c", &chce) < 1) {
				chce = 0;
			}
			chce %= 2;				// Na tabela ascii, S/s são valores impares e N/n são pares

			if (!chce) {
				break;
			}
		}
	}
}

void AlunEdit(Membro * pss, char * inpt, int sze) {
	switch(inpt[0]) {
	case 1:
		printf("Insira o novo nome: ");
		strin(pss->nome, 64, stdin);
		break;
	case 2:
		printf("Insira a nova matrícula: ");
		fgets(inpt, sze, stdin);
		sscanf(inpt, "%ld", &pss->dados.aluno.matr);
		break;
	case 3:
		printf("Insira o novo período: ");
		fgets(inpt, sze, stdin);
		sscanf(inpt, "%hhd", &pss->dados.aluno.peri);
		break;
	case 4:
		GradEdit(pss->tipo, pss->dados.aluno.grade, &pss->dados.aluno.discs);
		break;
	default:
		printf("Opção inválida!\n");
	}
}

void ProfEdit(Membro * pss, char * inpt, int sze) {
	switch(inpt[0]) {
	case 1:
		printf("Insira o novo nome: ");
		strin(pss->nome, 64, stdin);
		break;
	case 2:
		printf("Insira a nova identificação: ");
		fgets(inpt, sze, stdin);
		sscanf(inpt, "%d", &pss->dados.professor.id);
		break;
	case 3:
		printf("Insira o novo salário: ");
		fgets(inpt, sze, stdin);
		sscanf(inpt, "%f", &pss->dados.professor.slro);
		break;
	case 4:
		GradEdit(pss->tipo, pss->dados.professor.aulas, &pss->dados.professor.turms);
		break;
	default:
		printf("Opção inválida!\n");
	}
}

int EditarRegistro(Membro * pss) {
	char input[TAM_INPUT];

	printf("[ 1 ] Nome\n");

	switch(pss->tipo) {
	case Alun:
		printf("[ 2 ] Matrícula\n");
		printf("[ 3 ] Período\n");
		printf("[ 4 ] Grade\n");
		break;
	case Prof:
		printf("[ 2 ] Identificação\n");
		printf("[ 3 ] Salário\n");
		printf("[ 4 ] Aulas\n");
		break;
	}

	printf("[ 5 ] Voltar\n");

	printf("Qual dado deseja editar: ");

	fgets(input, TAM_INPUT, stdin);
	sscanf(input, "%hhd", &input[0]);

	if (input[0] == 5) {
		return 0;
	}

	switch(pss->tipo) {
	case Alun:
		AlunEdit(pss, input, TAM_INPUT);
		break;
	case Prof:
		ProfEdit(pss, input, TAM_INPUT);
	}

	return 1;
}

short UltUID(FILE * f) {
	short uid;
	fseek(f, -(TAM_REGISTRO), SEEK_END);
	printf("%lu", ftell(f));
	fread(&uid, sizeof(uid), 1, f);
	return uid;
}