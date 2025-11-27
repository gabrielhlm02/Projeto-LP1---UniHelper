#include <stdio.h>
#include <stdlib.h>

#define TAM_NOME 64
#define DIAS 5
#define TURNOS 18
#define TAM_INPUT 16
#define TAM_REGISTRO 438L

#define ARQUIVO_DADOS "dados.bin"

typedef struct {		// Tipo = Aluno
	long int matr;		// Matrícula
	char peri;			// Período
	char discs;			// Quantidade de disciplinas registradas
	int grade[5][18];	// Grade de horários [i][j]; i -> dias da semana (seg <-> sex); j -> horarios (m1 <-> t3 <-> n6); Elementos são códigos de disciplinas
} Dados_aluno;

typedef struct {		// Tipo = Professor
	int id;				// Número de identificação do docente
	float slro;			// Salário
	char turms;			// Quantidade de turmas
	int aulas[5][18]; 	// Grade das aulas por dia e horário; elementos são as salas em que ocorrerão as aulas
} Dados_professor;

typedef struct {
	short int uid;		// ID Único / UID (gerado automaticamente)
	char valido;		// Indicador de remoção [0 => removido / 1 => válido]
	enum __attribute__ ((__packed__)) {Alun, Prof} tipo;		// ~tipo~ do dado [Alun == Aluno / Prof == Professor]
	char nome[64];		// Nome do membro

	union {
		Dados_aluno aluno;
		Dados_professor professor;
	} dados;
} Membro;

void Menu(void);		// Gerencia menu principal; interface de acesso às outras funções
void criar_registro(Membro * pessoa);		// interface de criação de registros; preenche um registro novo e armazena no banco de dados
int Preenche(Membro * pss, short int uid);	// Preenche dados comuns aos tipos e chama função especifica do tipo; retorna 0
int PreencheAluno(Membro * pss, short int uid);		// Preenche dados relacionados ao tipo alun; retorna 0
int PreencheProfessor(Membro * pss, short int uid);		// Preenche dados relacionados ao tipo prof; retorna 0
void editar_registro_menu(Membro * pessoa);		// interface de atualização de registro; edita dados comuns e chama funções especificas

long BuscarRegistroId();		// Percorre o arquivo '.bin' até encontrar um ID Único correspondente; retorna a posição de membro no arquivo

void RemoverRegistro();			// Preenche todos os campos de um registro coom '0's, com excessão do UID
Membro Le(long posicao_registro);		// Copia os dados de um membro armazenados no banco de dados; retorna um membro com os dados copiados;
void Exibe(Membro * pss);		// 'printf's para cada dado de um membro (de acordo com o tipo)
void ListarTodosRegistros();	// Le e exibe os dados de todos os membros armazenados

void ListaCompacto();		// Le e exibe apenas o ID Único e o nome de um membro
int LeUIDeNome(FILE * f, short *uid, char *valido, char nome[TAM_NOME]);		// Versão reduzida da função 'Le()' para leitura UID, nome e validade; retorna se o dado foi removido ou não; 1 <-> removido; 0 <=> valido
long BuscaPorNome(char busca[], long pos_ini);		// Partindo da posição 'pos_ini', encontra o primeiro membro cujo nome é prefixado pela string 'busca[]'; retorna a posição do membro no arquivo
int prefixo(char *str1, char *str2);		// Usada por 'BuscaPorNome()'; checa se str1 esta contida no inicio de str2; 1 <-> contem; 0 <-> não contem;

int strin(char * in, int size, FILE * f);		// 'fgets' com formatação de string e tratamento de erros uteis para a aplicação; retorna qtd de bytes escritos
void *memst(void *s, char c, size_t n);			// partindo de 's', preenche 'n' bytes com 'c'
char SouN(char * in, int size, FILE * f);		// Função para converter entrada do usuario em "booleano"; entradas pares (como a letra "n") são falsas; impares (como "s") são verdadeiras
char *strncpy(char *dest, const char *src, size_t n);		// copia 'n' bytes de 'src' para 'dest' implementação de strncpy encontrada no manual;
short UltUID(FILE * f);			// retorna o id do último membro inserido no arquivo

size_t writerb(const void *ptr, size_t size, size_t nmemb, FILE *stream);		// Variação do fwrite, retornando a quantidade de bytes escritos
size_t wAlun(Membro * pss, FILE * f);		// Escreve os dados correspondentes a um aluno no arquivo e retorna a qtd de bytes escritos
size_t wProf(Membro * pss, FILE * f);		// Escreve os dados correspondentes a um professor no arquivo e retorna a qtd de bytes escritos
int Armazena(Membro * pss, FILE * f);		// Escreve os dados comuns a ambos os tipos e chama funções especificas; retorna a qtd de bytes escritos

void AlunEdit(Membro * pss, char * inpt, int sze);		// Interface +  Altera os dados de um aluno na memória
void ProfEdit(Membro * pss, char * inpt, int sze);		// Interface +  Altera os dados de um professor na memória
int EditarRegistro(Membro * pss);		// Altara dados comuns aos tipos, na memória; retorna 1 caso ocorreu edição; 0 caso o usuário a cancelou
int GradEdit(int tipo, int grade[DIAS][TURNOS], char * count);		// interface de edição de grade; retorna 0 se o usuário cancelar edição




