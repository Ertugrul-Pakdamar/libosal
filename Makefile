NAME   = libosal.a
CC     = cc
CFLAGS = -Wall -Wextra -Werror -Iinclude

PLATFORM = posix

SRC = $(PLATFORM)/osal_$(PLATFORM).c
OBJ = build/osal_$(PLATFORM).o

default: build $(NAME)

$(NAME):
	@cc -c $(CFLAGS) $(SRC)
	@mv *.o build/
	@ar rcs $(NAME) $(OBJ)
	@echo "Library $(NAME) created successfully."

build:
	@mkdir -p build

clean:
	@rm -f $(OBJ)
	@echo "Cleaned osal."

clean-all: clean
	@rm -f $(NAME)
	@rm -rf build
	@echo "All cleaned osal."

re: clean-all default

.PHONY: default clean clean-all re
