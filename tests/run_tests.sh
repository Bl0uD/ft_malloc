#!/usr/bin/env bash

# Export pour que le loader dynamique trouve la bibliothèque partagée sur Linux et macOS
export LD_LIBRARY_PATH=".:$LD_LIBRARY_PATH"
export DYLD_LIBRARY_PATH=".:$DYLD_LIBRARY_PATH"

if [ -z "$HOSTTYPE" ]; then
    HOSTTYPE="$(uname -m)_$(uname -s)"
fi

# Colors
GREEN="\033[0;32m"
BLUE="\033[0;34m"
YELLOW="\033[1;33m"
CYAN="\033[0;36m"
RED="\033[0;31m"
BOLD="\033[1m"
RESET="\033[0m"

SCORE=0
MAX_SCORE=125
MANDATORY_FAILED=0

echo -e "${BOLD}${CYAN}====================================================${RESET}"
echo -e "${BOLD}${CYAN}      42 MALLOC — BANC DE VALIDATION & SOUTENANCE   ${RESET}"
echo -e "${BOLD}${CYAN}====================================================${RESET}\n"

# 1. Compilation
echo -e "${YELLOW}[1/3] Compilation de la bibliothèque partagée...${RESET}"
make re > /dev/null
if [ $? -ne 0 ]; then
    echo -e "${RED}[ERREUR] Échec du make re.${RESET}"
    echo -e "${BOLD}${RED}NOTE FINALE : 0 / ${MAX_SCORE}${RESET}"
    exit 1
fi
echo -e "${GREEN}[OK] libft_malloc_${HOSTTYPE}.so compilée avec succès.${RESET}\n"

# 2. Exécution des tests unitaires
echo -e "${YELLOW}[2/3] Exécution de la suite de tests...${RESET}\n"

run_test() {
    local name="$1"
    local file="$2"
    local points="$3"
    local is_bonus="$4"

    echo -e "${BLUE}----------------------------------------------------${RESET}"
    echo -e "${BOLD}${BLUE}Exécution de tests/${file}.c (${points} pts)${RESET}"
    echo -e "${BLUE}----------------------------------------------------${RESET}"
    
    cc -Wall -Wextra -Werror -pthread -Iincludes "tests/${file}.c" -L. -lft_malloc -Wl,-rpath,. -o "tests/bin_${file}"
    if [ $? -ne 0 ]; then
        echo -e "${RED}[ERREUR] Compilation de ${file}.c échouée.${RESET}"
        if [ "$is_bonus" = "false" ]; then MANDATORY_FAILED=1; fi
        rm -f "tests/bin_${file}"
        return 1
    fi

    ./"tests/bin_${file}"
    local status=$?
    rm -f "tests/bin_${file}"
    echo ""

    if [ $status -eq 0 ]; then
        echo -e "${GREEN}[OK] ${name} réussi (+${points} pts)${RESET}\n"
        SCORE=$((SCORE + points))
        return 0
    else
        echo -e "${RED}[FAIL] ${name} a échoué (0/${points} pts)${RESET}\n"
        if [ "$is_bonus" = "false" ]; then MANDATORY_FAILED=1; fi
        return 1
    fi
}

# Mandatory tests (100 pts)
run_test "Test 0 (Basic Allocations & show_alloc_mem)" "test0_basic" 25 "false"
run_test "Test 1 (Saturation des Zones ≥ 100 allocs)" "test1_zones" 25 "false"
run_test "Test 2 (Alignement strict 16 octets)" "test2_align" 25 "false"
run_test "Test 3 (Realloc In-Place & Relocation)" "test3_realloc" 25 "false"

# Bonus tests (25 pts)
run_test "Test 4 (Coalescing & show_alloc_mem_ex)" "test4_coalesce" 10 "true"
run_test "Test 5 (Multi-Threading 8 Threads)" "test5_threads" 8 "true"

# 3. Test du bonus MALLOC_DEBUG (7 pts)
echo -e "${BLUE}----------------------------------------------------${RESET}"
echo -e "${BOLD}${BLUE}Test du Bonus : Variable d'environnement MALLOC_DEBUG (7 pts)${RESET}"
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
cc -Wno-free-nonheap-object -Iincludes tests/tmp_debug.c -L. -lft_malloc -Wl,-rpath,. -o tests/bin_debug
echo -e "${YELLOW}Lancement avec MALLOC_DEBUG=1 :${RESET}"
MALLOC_DEBUG=1 ./"tests/bin_debug"
DEBUG_STATUS=$?
rm -f tests/tmp_debug.c tests/bin_debug
echo ""

if [ $DEBUG_STATUS -eq 0 ]; then
    echo -e "${GREEN}[OK] Bonus MALLOC_DEBUG validé (+7 pts)${RESET}\n"
    SCORE=$((SCORE + 7))
else
    echo -e "${RED}[FAIL] Bonus MALLOC_DEBUG a échoué (0/7 pts)${RESET}\n"
fi

# Application de la règle 42 sur les bonus
if [ $MANDATORY_FAILED -ne 0 ]; then
    echo -e "${RED}⚠️  Règle 42 : La partie obligatoire n'étant pas parfaite, les bonus sont annulés.${RESET}"
    if [ $SCORE -gt 100 ]; then
        SCORE=100
    fi
fi

# Affichage du bilan dynamique
echo -e "${BOLD}${CYAN}====================================================${RESET}"
if [ $SCORE -eq 125 ]; then
    echo -e "${BOLD}${GREEN}   NOTE FINALE DYNAMIQUE : ${SCORE} / ${MAX_SCORE} (SANS-FAUTE TOTAL)   ${RESET}"
elif [ $SCORE -ge 100 ]; then
    echo -e "${BOLD}${GREEN}   NOTE FINALE DYNAMIQUE : ${SCORE} / ${MAX_SCORE} (MANDATORY VALIDÉ)   ${RESET}"
else
    echo -e "${BOLD}${RED}   NOTE FINALE DYNAMIQUE : ${SCORE} / ${MAX_SCORE} (ÉCHEC PROJET)   ${RESET}"
fi
echo -e "${BOLD}${CYAN}====================================================${RESET}"
