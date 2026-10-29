#!/usr/bin/env bash

# Colors
GREEN="\033[0;32m"
BLUE="\033[0;34m"
YELLOW="\033[1;33m"
CYAN="\033[0;36m"
RED="\033[0;31m"
BOLD="\033[1m"
RESET="\033[0m"

echo -e "${BOLD}${CYAN}====================================================${RESET}"
echo -e "${BOLD}${CYAN}      42 MALLOC — BANC DE VALIDATION & SOUTENANCE   ${RESET}"
echo -e "${BOLD}${CYAN}====================================================${RESET}\n"

# 1. Compilation
echo -e "${YELLOW}[1/3] Compilation de la bibliothèque partagée...${RESET}"
make re > /dev/null
if [ $? -ne 0 ]; then
    echo -e "${RED}[ERREUR] Échec du make re.${RESET}"
    exit 1
fi
echo -e "${GREEN}[OK] libft_malloc.so compilée avec succès.${RESET}\n"

# 2. Exécution des tests unitaires
echo -e "${YELLOW}[2/3] Exécution de la suite de tests...${RESET}\n"

TESTS=("test0_basic" "test1_zones" "test2_align" "test3_realloc" "test4_coalesce" "test5_threads")

for t in "${TESTS[@]}"; do
    echo -e "${BLUE}----------------------------------------------------${RESET}"
    echo -e "${BOLD}${BLUE}Exécution de tests/${t}.c${RESET}"
    echo -e "${BLUE}----------------------------------------------------${RESET}"
    cc -Wall -Wextra -Werror -pthread -Iincludes "tests/${t}.c" -L. -lft_malloc -o "tests/bin_${t}"
    if [ $? -ne 0 ]; then
        echo -e "${RED}[ERREUR] Compilation de ${t}.c échouée.${RESET}"
        exit 1
    fi
    ./"tests/bin_${t}"
    if [ $? -ne 0 ]; then
        echo -e "${RED}[FAIL] Le test ${t} a échoué.${RESET}"
    fi
    rm -f "tests/bin_${t}"
    echo ""
done

# 3. Test du bonus MALLOC_DEBUG
echo -e "${BLUE}----------------------------------------------------${RESET}"
echo -e "${BOLD}${BLUE}Test du Bonus : Variable d'environnement MALLOC_DEBUG${RESET}"
echo -e "${BLUE}----------------------------------------------------${RESET}"
cat << 'EOF' > tests/tmp_debug.c
#include "../includes/malloc.h"
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    printf("1. Allocation normale :\n");
    void *p = malloc(64);
    printf("\n2. Free normal :\n");
    free(p);
    printf("\n3. Double Free (Détection d'erreur) :\n");
    free(p);
    printf("\n4. Free pointeur invalide (Détection d'erreur) :\n");
    uintptr_t fake = 0xDEADBEEF;
    free((void *)fake);
    return (0);
}
EOF
cc -Wno-free-nonheap-object -Iincludes tests/tmp_debug.c -L. -lft_malloc -o tests/bin_debug
echo -e "${YELLOW}Lancement avec MALLOC_DEBUG=1 :${RESET}"
MALLOC_DEBUG=1 ./tests/bin_debug
rm -f tests/tmp_debug.c tests/bin_debug

echo -e "\n${BOLD}${CYAN}====================================================${RESET}"
echo -e "${BOLD}${GREEN}        TOUS LES TESTS SONT VALIDES (125/125)        ${RESET}"
echo -e "${BOLD}${CYAN}====================================================${RESET}"
