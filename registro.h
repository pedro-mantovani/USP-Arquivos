#ifndef REGISTRO_H
    #define REGISTRO_H

    typedef struct registro Registro;
    typedef struct header Header;

    Registro* criar_registro();
    Header* criar_header();

    Registro* ler_registro(FILE* f);
    Registro* bin_to_reg(FILE* fp);

    // Funções de get dos registros
    char reg_get_removido(Registro* reg);
    int reg_get_proximo(Registro* reg);
    int reg_get_codEstacao(Registro* reg);
    int reg_get_codLinha(Registro* reg);
    int reg_get_codProxEstacao(Registro* reg);
    int reg_get_distProxEstacao(Registro* reg);
    int reg_get_codLinhaIntegra(Registro* reg);
    int reg_get_codEstIntegra(Registro* reg);
    int reg_get_tamNomeEstacao(Registro* reg);
    char* reg_get_nomeEstacao(Registro* reg);
    int reg_get_tamNomeLinha(Registro* reg);
    char* reg_get_nomeLinha(Registro* reg);

    // Funções de set dos registros
    void remover_registro(Registro* reg);
    void reg_set_proximo(Registro* reg, int proximo);
    void reg_set_codEstacao(Registro* reg, int codEstacao);
    void reg_set_codLinha(Registro* reg, int codLinha);
    void reg_set_codProxEstacao(Registro* reg, int codProxEstacao);
    void reg_set_distProxEstacao(Registro* reg, int distProxEstacao);
    void reg_set_codLinhaIntegra(Registro* reg, int codLinhaIntegra);
    void reg_set_codEstIntegra(Registro* reg, int codEstIntegra);
    void reg_set_tamNomeEstacao(Registro* reg, int tamNomeEstacao);
    void reg_set_nomeEstacao(Registro* reg, char* nomeEstacao);
    void reg_set_tamNomeLinha(Registro* reg, int tamNomeLinha);
    void reg_set_nomeLinha(Registro* reg, char* nomeLinha);
    void reg_set_removido(Registro* reg, char removido);

    // Funções de get do header
    char header_get_status(const Header* h);
    int header_get_topo(const Header* h);
    int header_get_proxRRN(const Header* h);
    int header_get_nroEstacoes(const Header* h);
    int header_get_nroParesEstacao(const Header* h);

    // Funções de set do header
    void header_set_status(Header* h, char status);
    void header_set_topo(Header* h, int topo);
    void header_set_proxRRN(Header* h, int proxRRN);
    void header_set_nroEstacoes(Header* h, int nroEstacoes);
    void header_set_nroParesEstacao(Header* h, int nroParesEstacao);

    void print_reg(Registro* reg);

    void reg_free(Registro** reg);
    void head_free(Header** head);

#endif