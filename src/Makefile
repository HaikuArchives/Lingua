MACHINE=$(shell uname -m)
ifeq ($(MACHINE), BePC)
	CPU = x86
else
	CPU = ppc
endif

OBJ_DIR	:= obj.$(CPU)
NAME = Lingua
RSRCS := Lingua.rsrc
LIBS= -lbe -lnet -lnetapi -ltracker -ltranslation
CFLAGS =
LDFLAGS =

#CFLAGS += -O0 -g
CFLAGS += -O3

SRCS := \
	AboutWindow.o \
	DeskbarView.o \
	DictApplication.o \
	DictWindow.o \
	DownloadItem.o \
	UpdateWindow.o \
	Preferences.o \
	PrefWindow.o \
	Utilities.o \

ifeq ($(CPU), x86)
	CC = gcc
	LD = gcc
	LIBS += -lstdc++.r4
	INCLUDES = -I./
	CFLAGS += -Wall -W -Wno-multichar -Wno-ctor-dtor-privacy 
	LDFLAGS += -Xlinker -soname=_APP_
else
ifeq ($(CPU), ppc)
	CC = mwcc
	LD = mwldppc
	BELIBFILES=
	LIBS += -lmslcpp_4_0
	INCLUDES = -I./ -i-
	CFLAGS += -w off
	LDFLAGS +=	-export pragma \
				-init _init_routine_ \
				-term _term_routine_ \
				-l /boot/develop/lib/ppc/ \
				-msg nodup \
				-lroot \
				/boot/develop/lib/ppc/glue-noinit.a \
				/boot/develop/lib/ppc/init_term_dyn.o \
				/boot/develop/lib/ppc/start_dyn.o
endif
endif

LDFLAGS += $(LIBS)

OBJS = 	$(addprefix $(OBJ_DIR)/, $(foreach file, $(SRCS), $(file)))
SRC_PATHS += $(sort $(foreach file, $(SRCS), $(dir $(file))))

$(NAME) : $(OBJ_DIR) $(OBJS) $(RSRCS)
		$(LD) -o $@ $(OBJS) $(LDFLAGS)
		xres -o "$(NAME)" $(RSRCS)
		mimeset -f $@

$(OBJ_DIR) ::
	@[ -d $(OBJ_DIR) ] || mkdir $(OBJ_DIR) > /dev/null 2>&1

$(OBJ_DIR)/%.o : %.cpp
	$(CC) -c $< $(INCLUDES) $(CFLAGS) -o $@

clean :
	rm -f $(NAME) $(OBJ_DIR)/*.o
