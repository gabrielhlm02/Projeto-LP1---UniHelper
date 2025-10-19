#include <stdio.h>
#include <stdlib.h>

#define TAM_NOME 64
#define DIAS 5
#define TURNOS 18
#define TAM_INPUT 16
#define TAM_REGISTRO 441

#define ARQUIVO_DADOS "dados.3.bin"

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
	char nome[64];					// Nome da pessoa
	enum {Alun, Prof} tipo;			// ~tipo~ do dado 

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

int strin(char * in, int size, FILE * f);
void *memst(void *s, char c, size_t n);

size_t writerb(const void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t wAlun(Membro * pss, FILE * f);
size_t wProf(Membro * pss, FILE * f);
int Armazena(Membro * pss);



int main(void) {
	Menu();

	return 0;
}

void Menu(void) {
	int opcao;
	char input[TAM_INPUT];

	int cont = 0;
	Membro pss1;

	system("clear");
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

		if (fgets(input, sizeof(input), stdin) == NULL)
			fprintf(stderr, "Entrada inválida\n");

		if (sscanf(input, "%d", &opcao) != 1)
			printf("Entrada inválida\n");

		switch (opcao) 
		{
			case 1: 
				Preenche(&pss1, cont);
				Armazena(&pss1);
				cont++;
				break;
			//case 2: editar_registro(); break;
			case 3: 
				RemoverRegistro(BuscarRegistroId());
				break;
			case 4: 
				Le(BuscarRegistroId());
				break;
			//case 5: buscar_registro_por_nome(); break;
			case 6:
				ListarTodosRegistros();
				break;
			case 7: return (void) 0;
			default:
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



int Preenche(Membro * pss, short int uid) {		
	char input[16] = {0};
						
	printf("\nNome: ");
	if(strin(pss->nome, 64, stdin) < 1) {
		return 0;
	}

	printf("[ 0 ] Aluno // [ 1 ] Professor: ");
	fgets(input, 16, stdin);
	sscanf(input, "%d", (int *) &pss->tipo);
	pss->tipo %= 2; //* qualquer valor pode recair como 0 ou 1

	if (pss->tipo == Alun)
		PreencheAluno(pss,uid);

	else if (pss->tipo == Prof) 
		PreencheProfessor(pss,uid);

	printf("Exibir o último registro criado [ S ] / [ N ]: ");
	fgets(input, 16, stdin);
	if (input[0] %= 2)
		Exibe(pss);

	return 1;
}

int PreencheAluno(Membro * pss, short int uid)
{
	pss->uid = uid;
	pss->valido = 1;

	char input[16] = {0};			// Variavél usada para receber e avaliar entrada do usuario
	unsigned char chce = 0;			// Variavel usada para saber se o usuario quer adicionar mais disciplinas / turmas
	int elemento;					// Variavel para armazenar o codigo da disciplina / numero da sala a ser adicionado à grade
	int dia = 0, hora = 0;			// dia escolhido pelo usuario [1 <-> 5]; horario escolhido pelo usuario [1 <-> 18]
						
	memst(pss->dados.aluno.grade, 0, sizeof(int)*5*18);		// Limpa a grade ( 0 => vazio)
	pss->dados.aluno.discs = 0;

	printf("Matrícula: ");
	fgets(input, 16, stdin);
	sscanf(input, "%ld", &pss->dados.aluno.matr);

	printf("Período: ");
	fgets(input, 16, stdin);
	sscanf(input, "%hhd", &pss->dados.aluno.peri);

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
		fgets(input, 16, stdin);
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

	char input[16] = {0};			// Variavél usada para receber e avaliar entrada do usuario
	unsigned char chce = 0;			// Variavel usada para saber se o usuario quer adicionar mais disciplinas / turmas
	int elemento;					// Variavel para armazenar o codigo da disciplina / numero da sala a ser adicionado à grade
	int dia = 0, hora = 0;			// dia escolhido pelo usuario [1 <-> 5]; horario escolhido pelo usuario [1 <-> 18]

	memst(pss->dados.professor.aulas, 0, sizeof(int)*5*18);		// Limpa a grade ( 0 => vazio)
	pss->dados.professor.turms = 0;

	printf("Nº Identificação: ");
	fgets(input, 16, stdin);
	sscanf(input, "%d", &pss->dados.professor.id);

	printf("Salário: ");
	fgets(input, 16, stdin);
	sscanf(input, "%f", &pss->dados.professor.slro);

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
		fgets(input, 16, stdin);
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
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 18; j++) {
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
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 18; j++) {
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

//pelo fato de usar const void *ptr, n'ao deveria converter antes para outro tipo, i.g. (int *) ptr
size_t writerb(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
	fwrite(ptr, size, nmemb, stream);

	return size*nmemb;
	//printf("tamanho do membro copiado: %lu\n", size);
	// pelo que vi num livro, o recomendado seria fazer:
	//printf("tamanho do membro copiado: %lu\n", (long unsigned) size); //considerando C89
	//printf("tamanho do membro copiado: %zu\n", size); // para C99
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
	

	printf("quantidade de bytes escritos %zu", byt);

	return byt;
}

int Armazena(Membro * pss) {			// escreve os dados no arquivo
	//int msiz = sizeof(short int) + (sizeof(char)*68) + sizeof(long int) + (sizeof(int)*5*18);	// quantidade de bytes do pior caso (aluno)
	//por que 68 char, não seria 67? 

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

	size_final += (size_aluno > size_professor) ? size_aluno : size_professor; // 441 bytes
	printf("size_final: %zu\n",size_final);

	//int byt = 0;				// contagem de bytes escritos
	size_t byt = 0;				// contagem de bytes escritos

	FILE * f = fopen("dados.3.bin", "a");

	byt += writerb(&pss->uid, sizeof(pss->uid), 1, f);
	byt += writerb(&pss->valido, sizeof(pss->valido), 1, f);
	//byt += writerb(&pss->tipo, sizeof(char), 1, f); //*** NO MEU COMPUTADOR, sizeof(tipo) = 4
	byt += writerb(&pss->tipo, sizeof(int), 1, f); //*** NO MEU COMPUTADOR, sizeof(tipo) = 4
	byt += writerb(pss->nome, sizeof(pss->nome), 1, f);

	/*
	byt += fwrite(&pss->uid, sizeof(pss->uid), 1, f);
	byt += fwrite(&pss->valido, sizeof(pss->valido), 1, f);
	byt += fwrite(&pss->tipo, sizeof(char), 1, f);
	byt += fwrite(pss->nome, sizeof(pss->nome), 1, f);
	*/

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

	fclose(f);

	return (int) byt;
}

Membro Le(long posicao_registro) 
{
	Membro pss;

	FILE * f = fopen(ARQUIVO_DADOS, "rb");
	fseek(f, posicao_registro, SEEK_SET);

	fread(&pss.uid, sizeof(short int), 1, f);
	fread(&pss.valido, sizeof(char), 1, f);
	fread(&pss.tipo, sizeof(int), 1, f);
	fread(pss.nome, sizeof(char), 64, f);

	if (pss.tipo == Alun) {
		//memst(pss.dados.aluno.grade, 0, sizeof(int)*5*18);
		fread(&pss.dados.aluno.matr, sizeof(long int), 1, f);
		fread(&pss.dados.aluno.peri, sizeof(char), 1, f);
		fread(&pss.dados.aluno.discs, sizeof(char), 1, f);
		fread(pss.dados.aluno.grade, sizeof(pss.dados.aluno.grade), 1, f);
		/* não entendi essa parte, VOLTAR AQUI
		for (int k = 0; k < pss.dados.aluno.discs; k++) {
			int i = 0;
			fread(&i, sizeof(int), 1, f);
			fread((int *) (pss.dados.aluno.grade) + i, sizeof(int), 1, f);
		}
		*/
	}
	else {
		//memst(pss.dados.professor.aulas, 0, sizeof(int)*5*18);
		fread(&pss.dados.professor.id, sizeof(int), 1, f);
		fread(&pss.dados.professor.slro, sizeof(float), 1, f);
		fread(&pss.dados.professor.turms, sizeof(char), 1, f);
		fread(pss.dados.professor.aulas, sizeof(pss.dados.professor.aulas), 1, f);
		/* não entendi essa parte, VOLTAR AQUI
		for (int k = 0; k < pss.dados.professor.turms; k++) {
			int i = 0;
			fread(&i, sizeof(int), 1, f);
			fread((int *) (pss.dados.professor.aulas) + i, sizeof(int), 1, f);
		}
		*/
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
	fgets(input, TAM_INPUT, stdin);
	sscanf(input, "%hd", &id_unico);

	long posicao_registro = id_unico * 441L; //considerando uid em [0, inf) inter. Z
	
	fseek(file_pointer, 0L, SEEK_END);
	long end_file_pos = ftell(file_pointer);

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
	char zeros[TAM_REGISTRO] = {0};

	fseek(file_pointer, posicao_registro, SEEK_SET);
	fwrite(zeros, sizeof(char), TAM_REGISTRO, file_pointer);

	fclose(file_pointer);
}

void ListarTodosRegistros()
{
	FILE * file_pointer = fopen(ARQUIVO_DADOS, "rb+");

	fseek(file_pointer, 0L, SEEK_END);
	long end_file_pos = ftell(file_pointer);

	rewind(file_pointer);
	long cur_file_pos = ftell(file_pointer);

	for(; cur_file_pos < end_file_pos; cur_file_pos += 441L)
	{
		Le(cur_file_pos);
	}

}
