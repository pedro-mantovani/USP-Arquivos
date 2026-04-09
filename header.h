#ifndef HEADER_H
    #define HEADER_H

    #define header_tam 17
    typedef struct header Header;

    Header* criar_header();
    Header* bin_to_header(FILE* fp);
    void header_to_bin(FILE* fp, const Header* head);

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

    void head_print(Header* h);
    void head_free(Header** head);


#endif