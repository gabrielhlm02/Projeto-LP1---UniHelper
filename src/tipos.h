#include <stdio.h>
#include <stdlib.h>

#define TAM_NOME 64
#define DIAS 5
#define TURNOS 18
#define TAM_INPUT 16
#define TAM_REGISTRO 438L

#define ARQUIVO_DADOS "dados.bin"

typedef struct {
	long int matr;
	char peri;
	char discs;
	int grade[5][18];
} Dados_aluno;

typedef struct {
	int id;	
	float slro;
	char turms;
	int aulas[5][18]; 
} Dados_professor;

typedef struct {
	short int uid;
	char valido;
	enum __attribute__ ((__packed__)) {Alun, Prof} tipo;
	char nome[64];

	union {
		Dados_aluno aluno;
		Dados_professor professor;
	} dados;
} Membro;

void Menu(void);
void criar_registro(Membro * pessoa);
int Preenche(Membro * pss, short int uid);
int PreencheAluno(Membro * pss, short int uid);
int PreencheProfessor(Membro * pss, short int uid);
void editar_registro_menu(Membro * pessoa);

long BuscarRegistroId();

void RemoverRegistro();
Membro Le(long posicao_registro);
void Exibe(Membro * pss);
void ListarTodosRegistros();

void ListaCompacto();
int LeUIDeNome(FILE * f, short *uid, char *valido, char nome[TAM_NOME]);
long BuscaPorNome(char busca[], long pos_ini);
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
int GradEdit(int tipo, int grade[DIAS][TURNOS], char * count);




