#include <assert.h>
#include "./tipos.h"

int main(void) {
	Menu();
	return 0;
}

void Menu(void) {
	char input[TAM_INPUT];
	int opcao = 0;
	Membro pessoa1;

	FILE * f = fopen(ARQUIVO_DADOS, "ab+");
	int cont = UltUID(f) + 1;
	fclose(f);

	system("clear||cls");
	printf("\n"
	"╔═══════════════════════════════════════════════════════════════════════════╗\n"
	"║                                                                           ║\n"
	"║ ██    ██ ███    ██ ██     ██   ██ ███████ ██      ██████  ███████ ██████  ║\n"
	"║ ██    ██ ████   ██ ██     ██   ██ ██      ██      ██   ██ ██      ██   ██ ║\n"
	"║ ██    ██ ██ ██  ██ ██     ███████ █████   ██      ██████  █████   ██████  ║\n"
	"║ ██    ██ ██  ██ ██ ██     ██   ██ ██      ██      ██      ██      ██   ██ ║\n"
	"║  ██████  ██   ████ ██     ██   ██ ███████ ███████ ██      ███████ ██   ██ ║\n"
	"║                                                                           ║\n"
	"╚═══════════════════════════════════════════════════════════════════════════╝\n");
	for(;;)
	{	

		printf("\n"
		"╔═══════════════════════════════════════════════════════════════════════════╗\n"
		"║                                                                           ║\n"
		"║ [1] Criar registro                                                        ║\n"
		"║ [2] Editar registro                                                       ║\n"
		"║ [3] Remover registro                                                      ║\n"
		"║ [4] Buscar registro por ID                                                ║\n"
		"║ [5] Buscar registro por nome                                              ║\n"
		"║ [6] Listar todos os registros                                             ║\n"
		"║ [7] Sair                                                                  ║\n"
		"║                                                                           ║\n"
		"╚═══════════════════════════════════════════════════════════════════════════╝\n"
		"                                                                             \n"
		" Escolha uma opção: ");
		strin(input, TAM_INPUT, stdin);
		sscanf(input, "%d", &opcao);
		system("clear||cls");

		switch (opcao) 
		{
			case 1: 
				criar_registro(&pessoa1); break;

			case 2: 
				editar_registro_menu(&pessoa1); break;

			case 3: 
				RemoverRegistro(); break;

			case 4: 
				printf("\n"
				"╔═══════════════════════════════════════════════════════════════════════════╗\n"
				"║ Opção [4]: buscar registro por ID                                         ║\n"
				"╚═══════════════════════════════════════════════════════════════════════════╝\n");
				pessoa1 = Le(BuscarRegistroId()); 

				if (pessoa1.valido == 1)
					Exibe(&pessoa1);
				else
				{
					printf("\n"
					"╔═══════════════════════════════════════════════════════════════════════════╗\n"
					"  O registro de id %d foi removido                                           \n"
					"╚═══════════════════════════════════════════════════════════════════════════╝\n", pessoa1.uid);
				}
				break;

			case 5: 
				printf("\n"
				"╔═══════════════════════════════════════════════════════════════════════════╗\n"
				"║ Opção [5]: buscar registro por nome                                       ║\n"
				"╚═══════════════════════════════════════════════════════════════════════════╝\n");
				char busca[TAM_NOME];
				printf("\n Qual nome deseja buscar: ");
				strin(busca, TAM_NOME, stdin);

				for(long pos = BuscaPorNome(busca, 0); pos != -1; pos = BuscaPorNome(busca, pos+TAM_REGISTRO)) {
					pessoa1 = Le(pos);

					if (pessoa1.valido == 1)
						Exibe(&pessoa1);
					else
					{
						printf("\n"
						"╔═══════════════════════════════════════════════════════════════════════════╗\n"
						"  O registro de id %d foi removido                                           \n"
						"╚═══════════════════════════════════════════════════════════════════════════╝\n", pessoa1.uid);
					}
				}
				break;

			case 6:
				ListarTodosRegistros(); break;

			case 7:
				return (void) 0;

			default:
				fprintf(stderr, " Opção inválida: %s\n", input);
				break;
		}
	}
}

int strin(char * input, int size, FILE * f) {

	// garantir que a entrada não seja EOF
	while (fgets(input, size, f) == NULL)
	{
		if (feof(stdin)) clearerr(stdin);
		fprintf(stderr, "\nTente novamente: ");
	}
	int tamanho = 0;

	// remover \n, \r\n ou \r da string que armazena
	// a entrada do usuário
	while (tamanho < size && input[tamanho] != '\0') {
		if (input[tamanho] == '\n' || input[tamanho] == '\r') {
			input[tamanho] = '\0';
			break;
		}
		tamanho++;
	}

	// garantir que a string que armazena a entrada 
	// do usuário tenha ao menos um caractere
	if(tamanho < 1)
	{
		fprintf(stderr, " Tente novamente: ");
		tamanho = strin(input, size, f);
	}
	return tamanho;
}



void criar_registro(Membro * pessoa)
{
	printf("\n"
	"╔═══════════════════════════════════════════════════════════════════════════╗\n"
	"║ Opção [1]: criar registro                                                 ║\n"
	"╚═══════════════════════════════════════════════════════════════════════════╝\n");
	ListaCompacto();

	char input[TAM_INPUT];	

	// UltUID retorna o valor do uid do último registro
	FILE * f = fopen(ARQUIVO_DADOS, "ab+");
	short proximo_uid = UltUID(f) + 1;

	printf("\n UID do próximo registro: %hd", proximo_uid);
	if (Preenche(pessoa, proximo_uid) == 0)
	{
		Armazena(pessoa, f);
	}

	fclose(f);

	printf("\n Exibir o registro criado [1: S] :|: [0: N]: ");
	strin(input, TAM_INPUT, stdin);

	if (input[0] %= 2) 
	{
		Exibe(pessoa);
	}
}
int Preenche(Membro * pss, short int uid) {		
	char input[TAM_INPUT] = {0};

	pss->uid = uid;
	pss->valido = 1;
						
	printf("\n Nome: ");
	strin(pss->nome, TAM_NOME, stdin);

	printf(" Tipo [1:  PROFESSOR] :|: [0:  ALUNO]: ");
	strin(input, TAM_INPUT, stdin);
	sscanf(input, "%hhd", (char *) &pss->tipo);
	pss->tipo %= 2; 

	if (pss->tipo == Alun)
	{
		if (PreencheAluno(pss,uid) != 0)
		{
			return 1;
		}
	}
	else 
	{
		if (PreencheProfessor(pss,uid) != 0)
		{
			return 1;
		}
	}

	return 0; 
}
int PreencheAluno(Membro * pss, short int uid)
{
	char input[TAM_INPUT] = {0};
	unsigned char adicionar_disciplina = 1;	
	int elemento;		
	int dia = 0, hora = 0;
						
	memst(pss->dados.aluno.grade, 0, sizeof(int)*DIAS*TURNOS);
	pss->dados.aluno.discs = 0;

	printf(" Matrícula: ");
	strin(input, TAM_INPUT, stdin);
	sscanf(input, "%ld", &pss->dados.aluno.matr);

	printf(" Período: ");
	strin(input, TAM_INPUT, stdin);
	sscanf(input, "%hhd", &pss->dados.aluno.peri);

	while (adicionar_disciplina)
	{
		printf(" Adicionar disciplina à grade [1: S ] :|: [0: N ]: ");
		strin(input, TAM_INPUT, stdin);
		sscanf(input, "%c", &adicionar_disciplina);
		adicionar_disciplina %= 2;
		
		if (adicionar_disciplina)
		{
			printf(" Insira o código da disciplina: ");
			strin(input, TAM_INPUT, stdin);
			sscanf(input, "%d", &elemento);

			while(1)
			{
				do {
					printf(" Insira um dia [ 1 <-> 5 ]: ");
					strin(input, TAM_INPUT, stdin);
				} while(sscanf(input, "%d", &dia) < 1);
				if (dia < 1 || dia > DIAS) {
					break;
				}

				while (1) {
					do {
						printf(" Insira um horário [ 1 <-> 18 ]: ");
						strin(input, TAM_INPUT, stdin);
					} while(sscanf(input, "%d", &hora) < 1);

					if (hora < 1 || hora > TURNOS) {
						break;
					}
					
					if (pss->dados.aluno.grade[dia-1][hora-1] != 0) {
						pss->dados.aluno.discs--;		
					}
					pss->dados.aluno.grade[dia-1][hora-1] = elemento;

					if (elemento != 0) {	
						pss->dados.aluno.discs++;
					}
				}
			}
		}
		else
			break;
	}
	return 0;
}
int PreencheProfessor(Membro * pss, short int uid)
{
	char input[TAM_INPUT] = {0};
	unsigned char chce = 0;			// Variavel usada para saber se o usuario quer adicionar mais disciplinas / turmas
	int elemento;					// Variavel para armazenar o codigo da disciplina / numero da sala a ser adicionado à grade
	int dia = 0, hora = 0;	

	memst(pss->dados.professor.aulas, 0, sizeof(int)*DIAS*TURNOS);		// Limpa a grade ( 0 => vazio)
	pss->dados.professor.turms = 0;

	printf(" Nº Identificação: ");
	strin(input, TAM_INPUT, stdin);
	sscanf(input, "%d", &pss->dados.professor.id);

	printf(" Salário: ");
	strin(input, TAM_INPUT, stdin);
	sscanf(input, "%f", &pss->dados.professor.slro);

	printf(" Deseja adicionar uma aula [ S ] / [ N ]: ");
	strin(input, TAM_INPUT, stdin);
	sscanf(input, "%c", &chce);
	chce %= 2;

	while (chce) {
		printf(" Insira a sala: ");
		strin(input, TAM_INPUT, stdin);
		sscanf(input, "%d", &elemento);

		while (1) {
			do {
				printf(" Insira um dia da semana [ 1 <-> 5 ]: ");
				strin(input, TAM_INPUT, stdin);
			} while(sscanf(input, "%d", &dia) < 1);

			if (dia < 1 || dia > DIAS) {				// caso o dia escolhido for invalido, a inserção é cancelada
				break;
			}

			while (1) {
				do {
					printf(" Insira um horário [ 1 <-> 18 ]: ");
					strin(input, TAM_INPUT, stdin);
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
		printf(" Deseja adicionar outra aula [ S ] / [ N ]: ");
		strin(input, TAM_INPUT, stdin);
		if (sscanf(input, "%c", &chce) < 1) {
			chce = 0;
		}
		chce %= 2;
	}
	return 0;
}
int Armazena(Membro * pss, FILE * f) {			// escreve os dados no arquivo
	size_t size_final = 0;
	size_final += sizeof(pss->uid);
	size_final += sizeof(pss->valido);
	size_final += sizeof(pss->tipo);
	size_final += sizeof(pss->nome);

	size_t size_aluno = 0;
	size_aluno += sizeof(pss->dados.aluno.matr);
	size_aluno += sizeof(pss->dados.aluno.peri);
	size_aluno += sizeof(pss->dados.aluno.discs);
	size_aluno += sizeof(pss->dados.aluno.grade);

	size_t size_professor = 0;
	size_professor += sizeof(pss->dados.professor.id);
	size_professor += sizeof(pss->dados.professor.slro);
	size_professor += sizeof(pss->dados.professor.turms);
	size_professor += sizeof(pss->dados.professor.aulas);

	size_final += (size_aluno > size_professor) ? size_aluno : size_professor; 
	//printf("\n tamanho efetivo do registro em bytes: %zu\n",size_final);


	size_t byt = 0;				// contagem de bytes escritos

	byt += writerb(&pss->uid, sizeof(pss->uid), 1, f);
	byt += writerb(&pss->valido, sizeof(pss->valido), 1, f);
	byt += writerb(&pss->tipo, sizeof(pss->tipo), 1, f);
 	//[g] com __attribute__ ((__packed__)) , sizeof(tipo) = 1 [g]
	byt += writerb(pss->nome, sizeof(pss->nome), 1, f);
	//printf(" quantidade de bytes escritos [comum]: %zu\n", byt);

	if (pss->tipo == Alun) {
		byt += wAlun(pss, f);
	}
	else {
		byt += wProf(pss, f);
	}
		
	//printf(" quantidade de bytes escritos [total]: %zu\n", byt);

	if (size_final > byt)
	{
		size_t fill = size_final - byt;
		for (size_t i = 0; i < fill; i++)
		{
			fputc(0, f);
		}
	}

	return (int) byt;
}
size_t writerb(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
	fwrite(ptr, size, nmemb, stream);
	return size*nmemb;
}

size_t wAlun(Membro * pss, FILE * f) {		// escreve dados referentes a aluno no arquivo
	size_t byt = 0;

	byt += writerb(&pss->dados.aluno.matr, sizeof(pss->dados.aluno.matr), 1, f);
	byt += writerb(&pss->dados.aluno.peri, sizeof(pss->dados.aluno.peri), 1, f);
	byt += writerb(&pss->dados.aluno.discs, sizeof(pss->dados.aluno.discs), 1, f);
	byt += writerb(pss->dados.aluno.grade, sizeof(pss->dados.aluno.grade), 1, f);
	//printf(" quantidade de bytes escritos [alun]: %zu\n", byt);

	return byt;
}

size_t wProf(Membro * pss, FILE * f) {		// escreve dados referentes a professor no arquivo
	size_t byt = 0;

	byt += writerb(&pss->dados.professor.id, sizeof(pss->dados.professor.id), 1, f);
	byt += writerb(&pss->dados.professor.slro, sizeof(pss->dados.professor.slro), 1, f);
	byt += writerb(&pss->dados.professor.turms, sizeof(pss->dados.professor.turms), 1, f);
	byt += writerb(pss->dados.professor.aulas, sizeof(pss->dados.professor.aulas), 1, f);
	//printf(" quantidade de bytes escritos [prof]: %zu\n", byt);

	return byt;
}



void editar_registro_menu(Membro * pessoa)
{
	//system("clear||cls"); 
	printf("\n"
	"╔═══════════════════════════════════════════════════════════════════════════╗\n"
	"║ Opção [2]: editar registro                                                ║\n"
	"╚═══════════════════════════════════════════════════════════════════════════╝\n");
	ListaCompacto();

	// o usuário fornece um UID;
	// a função busca o registro associado,
	// e retorna a posição de início de leitura
	long memb_pointer = BuscarRegistroId();
	system("clear||cls");

	// a função recebe a posição de início de um registro,
	// e armazena os membros desse registro na variável apontada;
	*pessoa = Le(memb_pointer);

	// se o status de validação associado ao registro for válido, 
	// então os membros do registro apontado são exibidos
	if (pessoa->valido == 1)
	{
		printf(" Dados atuais do registro:");
		Exibe(pessoa);

		// quando EditarRegistro() retorna 0,
		// a opção 5 (voltar) foi escolhida
		if(EditarRegistro(pessoa) == 0) {
			return (void) 0;
		}
	}
	else { 
		char input[TAM_INPUT];
		printf(" Restaurar entrada removida [1: S] :|: [0: N ]: ");
		if (SouN(input, TAM_INPUT, stdin)) {
			// retorno 0 representa a execução normal de Preenche()
			if(Preenche(pessoa, memb_pointer/TAM_REGISTRO) != 0) {
				printf(" Ação interrompida.");
				return (void) 0;
			}
		}
		else {
			return (void) 0;
		}
	}

	system("clear||cls");
	FILE * f = fopen(ARQUIVO_DADOS, "rb+");
	fseek(f, memb_pointer, SEEK_SET);
	Armazena(pessoa, f);
	fclose(f);
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
	//printf(" end_file_pos: %ld\n", end_file_pos);

	if (end_file_pos == 0)
	{
		printf(" A base de dados está vazia!\n\n");
	}
	else
	{
		printf("\n Lista de registros:\n");
		for (; cur_file_pos < end_file_pos; cur_file_pos += TAM_REGISTRO) {
			fseek(file_pointer, cur_file_pos, SEEK_SET);
			
			if (LeUIDeNome(file_pointer, &uid, &valido, nome)) {
				strncpy(nome, "**Removido**", 64);
				uid = cur_file_pos / TAM_REGISTRO;
			}

			printf(" [ %hd ] %s\n", uid, nome);
		}
	}

}
long BuscarRegistroId()
{
	FILE * file_pointer = fopen(ARQUIVO_DADOS, "rb");

	char input[TAM_INPUT] = {0};
	short int id_unico;

	printf("\n Selecione o id único (UID): ");
	strin(input, TAM_INPUT, stdin);
	if (sscanf(input, "%hd", &id_unico) != 1 || id_unico < 0) {
		return -2;
	}

	long posicao_registro = id_unico * TAM_REGISTRO;
	
	fseek(file_pointer, 0L, SEEK_END);
	long end_file_pos = ftell(file_pointer);
	//printf(" end_file_pos: %lu\n", end_file_pos);

	if (posicao_registro >= end_file_pos) {
		fprintf(stderr, " Registro com o id único %hd inexistente\n", id_unico);
		fclose(file_pointer);
		return -1;
	}

	fclose(file_pointer);
	return posicao_registro;
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
		fread(&pss.dados.aluno.matr, sizeof(pss.dados.aluno.matr), 1, f);
		fread(&pss.dados.aluno.peri, sizeof(pss.dados.aluno.peri), 1, f);
		fread(&pss.dados.aluno.discs, sizeof(pss.dados.aluno.discs), 1, f);
		fread(pss.dados.aluno.grade, sizeof(pss.dados.aluno.grade), 1, f);
	}
	else {
		fread(&pss.dados.professor.id, sizeof(pss.dados.professor.id), 1, f);
		fread(&pss.dados.professor.slro, sizeof(pss.dados.professor.slro), 1, f);
		fread(&pss.dados.professor.turms, sizeof(pss.dados.professor.turms), 1, f);
		fread(pss.dados.professor.aulas, sizeof(pss.dados.professor.aulas), 1, f);
	}

	fclose(f);
	return pss;
}
void Exibe(Membro * pss) {
	printf("\n"
	"╔═══════════════════════════════════════════════════════════════════════════╗\n\n");
	printf("%-20s %hd\n", " id único:", pss->uid);
	printf("%-20s %hhd\n"," válido:", pss->valido);
	printf("%-19s %d\n",  " tipo:", pss->tipo);
	printf("%-19s %s\n",  " nome:", pss->nome);

	switch (pss->tipo) {
	case Alun: 
		printf("%-20s %ld\n", " matrícula:", pss->dados.aluno.matr);
		printf("%-20s %hhd\n"," período:", pss->dados.aluno.peri);
		printf("%-19s %hhd\n", " disciplinas", pss->dados.aluno.discs);
		printf("\n"
		"╚═══════════════════════════════════════════════════════════════════════════╝\n");

		printf("\n grade:\n");
		for (int i = 0; i < DIAS; i++) {
			for (int j = 0; j < TURNOS; j++) {
				printf(" %-4d", pss->dados.aluno.grade[i][j]);
			}
			printf("\n");
		}
		printf("\n");
		break;

	case Prof:
		printf("%-21s %d\n"," identificação", pss->dados.professor.id);
		printf("%-20s %.2f\n", " salário", pss->dados.professor.slro);
		printf("%-19s %hhd\n", " turmas", pss->dados.professor.turms);
		printf("\n"
		"╚═══════════════════════════════════════════════════════════════════════════╝\n");

		printf("\n aulas:\n");
		for (int i = 0; i < DIAS; i++) {
			for (int j = 0; j < TURNOS; j++) {
				printf(" %-4d", pss->dados.professor.aulas[i][j]);
			}
			printf("\n");
		}
		printf("\n");
		break;
	default:
		break;

	}
}
int EditarRegistro(Membro * pss) {
	char input[TAM_INPUT];

	printf("\n"
	"╔═══════════════════════════════════════════════════════════════════════════╗\n"
	"║ Campo de edição de registro                                               ║\n"
	"╠═══════════════════════════════════════════════════════════════════════════╣\n\n");
	printf(" [ 1 ] Nome\n");

	switch(pss->tipo) {
	case Alun:
		printf(" [ 2 ] Matrícula:\n");
		printf(" [ 3 ] Período\n");
		printf(" [ 4 ] Grade\n");
		break;
	case Prof:
		printf(" [ 2 ] Identificação\n");
		printf(" [ 3 ] Salário\n");
		printf(" [ 4 ] Aulas\n");
		break;
	}
	printf(" [ 5 ] Voltar\n");
	printf("\n"
	"╚═══════════════════════════════════════════════════════════════════════════╝\n");

	printf("\n Qual dado deseja editar: ");
	strin(input, TAM_INPUT, stdin);
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
void AlunEdit(Membro * pss, char * inpt, int sze) {
	switch(inpt[0]) {
	case 1:
		printf(" Insira o novo nome: ");
		strin(pss->nome, 64, stdin);
		break;
	case 2:
		printf(" Insira a nova matrícula: ");
		strin(inpt, sze, stdin);
		sscanf(inpt, "%ld", &pss->dados.aluno.matr);
		break;
	case 3:
		printf(" Insira o novo período: ");
		strin(inpt, sze, stdin);
		sscanf(inpt, "%hhd", &pss->dados.aluno.peri);
		break;
	case 4:
		GradEdit(pss->tipo, pss->dados.aluno.grade, &pss->dados.aluno.discs);
		break;
	default:
		printf(" Opção inválida!\n");
	}
}
void ProfEdit(Membro * pss, char * inpt, int sze) {
	switch(inpt[0]) {
	case 1:
		printf(" Insira o novo nome: ");
		strin(pss->nome, 64, stdin);
		break;
	case 2:
		printf(" Insira a nova identificação: ");
		strin(inpt, sze, stdin);
		sscanf(inpt, "%d", &pss->dados.professor.id);
		break;
	case 3:
		printf(" Insira o novo salário: ");
		strin(inpt, sze, stdin);
		sscanf(inpt, "%f", &pss->dados.professor.slro);
		break;
	case 4:
		GradEdit(pss->tipo, pss->dados.professor.aulas, &pss->dados.professor.turms);
		break;
	default:
		printf(" Opção inválida!\n");
	}
}
int GradEdit(int tipo, int grade[DIAS][TURNOS], char * count) {
	int elemento;
	char input[TAM_INPUT];
	unsigned char chce = 0;
	int dia = 0, hora = 0;

	while (1) {
		printf("\n"
		"╔═══════════════════════════════════════════════════════════════════════════╗\n\n");
		printf(" [ 1 ] Inserir\n");
		printf(" [ 2 ] Remover\n");
		printf(" [ 3 ] Voltar\n");
		printf("\n"
		"╚═══════════════════════════════════════════════════════════════════════════╝\n");
		printf("\n O que deseja fazer: ");

		strin(input, TAM_INPUT, stdin);
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
					printf(" Insira a disciplina: ");
				}
				else {
					printf(" Insira a sala: ");	
				}
				
				strin(input, TAM_INPUT, stdin);
				sscanf(input, "%d", &elemento);
			}

			while (1) {
				do {
					printf(" Insira um dia da semana [ 1 <-> 5 ]: ");
					strin(input, TAM_INPUT, stdin);
				} while(sscanf(input, "%d", &dia) < 1);

				if (dia < 1 || dia > DIAS) {				// caso o dia escolhido for invalido, a inserção é cancelada
					break;
				}

				while (1) {
					do {
						printf(" Insira um horário [ 1 <-> 18 ]: ");
						strin(input, TAM_INPUT, stdin);
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

			printf(" Alterar outro elemento [ S ] / [ N ]: ");
			strin(input, TAM_INPUT, stdin);
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



char SouN(char * in, int size, FILE * f) {
	char opt;
	if (fgets(in, size, f) == NULL || sscanf(in, "%c", &opt) != 1) {
		return 0;
	}
	return opt % 2;
}
char *strncpy(char *dest, const char *src, size_t n) { 			// implementação de strncpy encontrada no manual
	size_t i;
	for (i = 0; i < n && src[i] != '\0'; i++) {
		dest[i] = src[i];
	}
	for (; i < n; i++) {
		dest[i] = '\0';
	}
	return dest;
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

void *memst(void *s, char c, size_t n) { // Análogo ao memset de <string.h>
	for (int i = 0; i < n; i++) {
		*((char *) (s + i)) = c;
	}
	return s;	
}


void RemoverRegistro()
{
	printf("\n"
	"╔═══════════════════════════════════════════════════════════════════════════╗\n"
	"║ Opção [3]: remover registro                                               ║\n"
	"╚═══════════════════════════════════════════════════════════════════════════╝\n");
	
	// usuário fornece o UID à função, a função
	// retorna a posição de início do registro associado
	long posicao_registro = BuscarRegistroId();
	FILE * file_pointer = fopen(ARQUIVO_DADOS, "rb+");
	
	// armazena o UID antes de remover
	short uid;
	fseek(file_pointer, posicao_registro, SEEK_SET);
	fread(&uid, sizeof(uid), 1, file_pointer);

	char zeros[TAM_REGISTRO] = {0};

	// remove o registro (preenche com zeros)
	fseek(file_pointer, posicao_registro, SEEK_SET);
	fwrite(zeros, sizeof(char), TAM_REGISTRO, file_pointer); 

	// restaura apenas o UID do registro
	fseek(file_pointer, posicao_registro, SEEK_SET);
	fwrite(&uid, sizeof(uid), 1, file_pointer);

	fclose(file_pointer);
}

void ListarTodosRegistros()
{
	printf("\n"
	"╔═══════════════════════════════════════════════════════════════════════════╗\n"
	"║ Opção [6]: listar todos os registros                                      ║\n"
	"╚═══════════════════════════════════════════════════════════════════════════╝\n\n");

	FILE * file_pointer = fopen(ARQUIVO_DADOS, "rb+");

	fseek(file_pointer, 0L, SEEK_END);
	long end_file_pos = ftell(file_pointer);

	rewind(file_pointer);
	long cur_file_pos = ftell(file_pointer);

	for(; cur_file_pos < end_file_pos; cur_file_pos += TAM_REGISTRO)
	{
		Membro aux = Le(cur_file_pos);
		if (aux.valido == 1)
		{
			Exibe(&aux);
		}
		else
		{
			printf("\n"
			"╔═══════════════════════════════════════════════════════════════════════════╗\n"
			"  O registro de id %d foi removido                                           \n"
			"╚═══════════════════════════════════════════════════════════════════════════╝\n", aux.uid);
		}
	}
}


long BuscaPorNome(char busca[], long pos_ini) {

	char nome[64] = {0};
	long cur_pos = pos_ini;
	FILE * f = fopen(ARQUIVO_DADOS, "rb");
	fseek(f, 0l, SEEK_END);
	long end_pos = ftell(f);
	if (pos_ini >= end_pos) {
		return -1;
	}
	rewind(f);

	while (cur_pos < end_pos) {
		fseek(f, cur_pos, SEEK_SET);
		LeUIDeNome(f, NULL, NULL, nome);

		//printf("%s\n", nome);
		//printf("%d\n", prefixo(busca, nome));

		if (prefixo(busca, nome)) {
			return cur_pos;
		}
		cur_pos += TAM_REGISTRO;
	}

	if (pos_ini == 0){
		printf(" Nome não encontrado!\n");
	}
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

short UltUID(FILE * f) {
	fseek(f, 0l, SEEK_END);
	if(ftell(f) == 0) {
		return -1;
	}
	short uid;
	fseek(f, -(TAM_REGISTRO), SEEK_END);
	//printf("pos: %lu\n", ftell(f));
	fread(&uid, sizeof(uid), 1, f);
	return uid;
}
