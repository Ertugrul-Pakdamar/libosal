# =============================================================================
# libosal — Operating System Abstraction Layer
# =============================================================================
#
#  Targets:
#
#    help      Show this message                      (default)
#    all       Build libosal.a for the configured platform
#    examples  Build all programs in examples/
#    clean     Remove object files and example binaries
#    fclean    clean + remove libosal.a
#    re        fclean + all
#
#  Platform selection (default: posix):
#
#    make all PLATFORM=posix        POSIX / Linux / macOS
#    make all PLATFORM=freertos     FreeRTOS  (provide freertos/osal_freertos.c)
#
# =============================================================================

CC       := cc
AR       := ar
ARFLAGS  := rcs

NAME     := libosal.a
PLATFORM ?= posix
CFLAGS   := -Wall -Wextra -Werror -Iinclude

SRC := $(PLATFORM)/osal_$(PLATFORM).c
OBJ := build/osal_$(PLATFORM).o

RESET  := \033[0m
BOLD   := \033[1m
DIM    := \033[2m
GREEN  := \033[32m
CYAN   := \033[36m
YELLOW := \033[33m

# =============================================================================

.DEFAULT_GOAL := help

.PHONY: help
help:
	@printf "\n$(BOLD)libosal$(RESET) — available targets\n\n"
	@printf "  $(CYAN)%-14s$(RESET) %s\n" "make all"      "Build $(NAME)  [PLATFORM=$(PLATFORM)]"
	@printf "  $(CYAN)%-14s$(RESET) %s\n" "make examples" "Build all programs in examples/"
	@printf "  $(CYAN)%-14s$(RESET) %s\n" "make clean"    "Remove object files and example binaries"
	@printf "  $(CYAN)%-14s$(RESET) %s\n" "make fclean"   "clean + remove $(NAME)"
	@printf "  $(CYAN)%-14s$(RESET) %s\n" "make re"       "fclean + all"
	@printf "\n  $(DIM)Porting: implement posix/osal_posix.c for your target platform$(RESET)\n\n"

## all: Build libosal.a
.PHONY: all
all: $(NAME)

build/osal_$(PLATFORM).o: $(PLATFORM)/osal_$(PLATFORM).c | build
	@printf "  $(DIM)CC$(RESET)  $<\n"
	@$(CC) $(CFLAGS) -c $< -o $@

build:
	@mkdir -p build

$(NAME): $(OBJ)
	@$(AR) $(ARFLAGS) $@ $^
	@printf "$(GREEN)$(BOLD)✓ $(NAME)$(RESET)  [platform: $(YELLOW)$(PLATFORM)$(RESET)]\n"

# =============================================================================
# Examples
# =============================================================================

EXAMPLES_SRC := $(wildcard examples/*.c)
EXAMPLES_BIN := $(EXAMPLES_SRC:examples/%.c=examples/bin/%)
EXAMPLES_CFLAGS := $(CFLAGS) -lpthread

## examples: Build all programs in examples/
.PHONY: examples
examples: $(NAME) $(EXAMPLES_BIN)

examples/bin/%: examples/%.c $(NAME) | examples/bin
	@printf "  $(DIM)CC$(RESET)  $<\n"
	@$(CC) $(CFLAGS) -o $@ $< $(NAME) -lpthread
	@printf "$(GREEN)✓ $@$(RESET)\n"

examples/bin:
	@mkdir -p examples/bin

# =============================================================================
# Cleanup
# =============================================================================

## clean: Remove object files and example binaries
.PHONY: clean
clean:
	@rm -rf examples/bin $(OBJ)
	@printf "$(DIM)libosal: cleaned$(RESET)\n"

## fclean: clean + remove libosal.a
.PHONY: fclean
fclean: clean
	@rm -f $(NAME)

## re: fclean + all
.PHONY: re
re: fclean all
