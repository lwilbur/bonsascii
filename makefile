OUTPUT = bonsascii

.PHONY : clean test

$(OUTPUT) : bonsai.c
	gcc -lncurses -o $(OUTPUT) bonsai.c

test : $(OUTPUT)
	./$(OUTPUT)

clean :
	rm -rf $(OUTPUT)