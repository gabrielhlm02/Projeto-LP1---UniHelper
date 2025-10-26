### UNIHELPER - SISTEMA DE GERENCIAMENTO ACADÊMICO

&nbsp;
#### **Domínio da Aplicação**

Consiste em um sistema de gerenciamento acadêmico para a administração de informações de alunos e docentes. O UniHelper permite o cadastro, edição, consulta e remoção de registros acadêmicos e gestão de grades horárias.

---

&nbsp;
#### **Informações Gerenciadas**

1. **Registro Principal**
    ```c
    typedef struct {
        short int uid;
        char valido;
        enum __attribute__ ((__packed__)) {
            Alun,
            Prof
        } tipo;
        char nome[64];

        union {
            Dados_aluno aluno;
            Dados_professor professor;
        } dados;
    } Membro;
    ```
    identificador único para cada registro\
    status de validação\
    tipo (aluno ou professor)\
    nome

1. **Registro do aluno**
    ```c
    typedef struct {
        long int matr;
        char peri;
        char discs;
        int grade[5][18];
    } Dados_aluno;
    ```
    matrícula\
    período\
    quantidade de disciplinas\
    grade horária (matriz 5×18)

1. **Registro do professor**
    ```c
    typedef struct {
        int id;	
        float slro;
        char turms;
        int aulas[5][18];
    } Dados_professor;
    ```
    identificação\
    salário\
    quantidade de turmas\
    grade de aulas (Matriz 5×18)

---

&nbsp;
#### **Operações desenvolvidas**

1. Criação de Registros\
    cadastro de novos alunos e professores\
    preenchimento de dados pessoais e acadêmicos\
    definição de grades horárias

1. Edição de Registros\
    modificação de informações pessoais\
    atualização de grades horárias\
    adição/remoção de disciplinas (alunos) ou turmas (professores)\
    restauração de registros previamente removidos

1. Remoção de Registros\
    exclusão lógica de registros (marcado como inválido)

1. Busca e Consulta\
    Busca por ID único\
    Busca por nome (uso de prefixo)

1. Visualização\
    exibição detalhada de registros individuais\
    listagem completa: exibição de todos os registros ativos

1. Persistência de Dados\
    armazenamento em arquivo binário (dados.bin)\
    estrutura de registro fixa (438 bytes por entrada)
