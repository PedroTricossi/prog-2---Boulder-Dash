CFLAGS = -Wall -g -std=gnu11 # gerar "warnings" detalhados e infos de depuração   
LDLIBS = -lm -pedantic `pkg-config --cflags --libs allegro-5 allegro_dialog-5 allegro_ttf-5 allegro_image-5 allegro_dialog-5 allegro_audio-5 allegro_acodec-5 `
CC = gcc

objs = main.o boulder_dash.o
# regra default (primeira regra)
all: mosaico
	$(CC) $(CFLAGS) -o boulder_dash $(objs) $(LDLIBS)

# regras de ligacao
mosaico: $(objs)

# regras de compilação
main.o: main.c boulder_dash.h
boulder_dash.o: boulder_dash.c boulder_dash.h

# remove arquivos temporários
clean:
	-rm -f $(objs) *~

# remove tudo o que não for o código-fonte
purge: clean
	-rm -f mosaico
