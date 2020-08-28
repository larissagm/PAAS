#detecta se o sistema é de 32 ou 64 bits
N_BITS = $(shell getconf LONG_BIT)
ifeq ($(N_BITS),32)
   BITS_OPTION = -m32
else
   BITS_OPTION = -m64
endif

####diretorios com as libs do solver
ORTOOLSDIR = /opt/or-tools

ORTOOLSLIBDIR = /opt/or-tools/lib

#### define o compilador
CPPC = g++
#############################

#### opcoes de compilacao e includes
#APP_OPTS => -DONLY_ILS = Execita apenas o ILS, -DORT_CHECKER = Usa o coin com o OrTools como solver
APP_OPTS =
CCOPT = $(BITS_OPTION) $(APP_OPTS) -O3 -fPIC -fexceptions -DNDEBUG -DIL_STD -std=c++11 -DARCH_K8 -Wno-deprecated -DUSE_CBC -DUSE_CLP -DUSE_BOP -DUSE_GLOP
# CONCERTINCDIR = $(CONCERTDIR)/include
# CPLEXINCDIR   = $(CPLEXDIR)/include
ORTOOLSINCDIR = $(ORTOOLSDIR)/include
CCFLAGS = $(CCOPT) -I$(ORTOOLSINCDIR)
#############################

#### flags do linker
CCLNFLAGS = -lm -lpthread -L$(ORTOOLSLIBDIR) -lz -lrt -Llib -Llib64 -lprotobuf -lglog -lgflags -lCbcSolver -lCbc -lOsiCbc -lCgl -lClpSolver -lClp -lOsiClp -lOsi -lCoinUtils -lortools
#############################

#### diretorios com os source files e com os objs files
SRCDIR = src
OBJDIR = obj
#############################

#### lista de todos os srcs e todos os objs
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))
#############################

#### regra principal, gera o executavel
paas: $(OBJS) 
	@echo  "\033[31m \nLinking all objects files: \033[0m"
	$(CPPC) $(BITS_OPTION) $(OBJS) -o $@ $(CCLNFLAGS)
############################

#inclui os arquivos de dependencias
-include $(OBJS:.o=.d)

#regra para cada arquivo objeto: compila e gera o arquivo de dependencias do arquivo objeto
#cada arquivo objeto depende do .c e dos headers (informacao dos header esta no arquivo de dependencias gerado pelo compiler)
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo  "\033[31m \nCompiling $<: \033[0m"
	$(CPPC) $(CCFLAGS) -c $< -o $@
	@echo  "\033[32m \ncreating $< dependency file: \033[0m"
	$(CPPC) -std=c++0x  -MM $< > $(basename $@).d
	@mv -f $(basename $@).d $(basename $@).d.tmp #proximas tres linhas colocam o diretorio no arquivo de dependencias (g++ nao coloca, surprisingly!)
	@sed -e 's|.*:|$(basename $@).o:|' < $(basename $@).d.tmp > $(basename $@).d
	@rm -f $(basename $@).d.tmp

#delete objetos e arquivos de dependencia
clean:
	@echo "\033[31mcleaning obj directory \033[0m"
	@rm paas -f $(OBJDIR)/*.o $(OBJDIR)/*.d


rebuild: clean paas
