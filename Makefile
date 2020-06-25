#工程名称
TARGET			= linux-model

#设置编译器
#CC				= arm-linux-gnueabihf-gcc
CC				= gcc

#获取当前工作目录
TOP=.

#设置源文件后缀，c或cpp
EXT				= c

#设置源文件搜索路径
VPATH			+= $(TOP)/app:$(TOP)/user:$(TOP)/hardware

#设置中间目标文件目录
OBJ_DIR			= $(TOP)/obj

#设定头文件包含目录
INC_FLAGS 		+= -I $(TOP)/app
INC_FLAGS 		+= -I $(TOP)/user
INC_FLAGS 		+= -I $(TOP)/hardware

#编译选项
CFLAGS 			+= -W -Wall -g -O0 -std=gnu11

#链接选项
LFLAGS 			+= -pthread

#固定源文件添加
C_SRC			+= $(shell find $(TOP)/user -name '*.$(EXT)')
C_SRC			+= $(shell find $(TOP)/app -name '*.$(EXT)')

#自定义源文件添加
C_SRC			+= $(TOP)/hardware/uart.c

#中间目标文件
#C_OBJ			+= $(C_SRC:%.$(EXT)=%.o)
C_SRC_NODIR		= $(notdir $(C_SRC))
C_OBJ 			= $(patsubst %.$(EXT), $(OBJ_DIR)/%.o,$(C_SRC_NODIR))

#依赖文件
C_DEP			= $(patsubst %.$(EXT), $(OBJ_DIR)/%.d,$(C_SRC_NODIR))

.PHONY: all clean rebuild test ctags

all:$(C_OBJ)
	@echo "linking object to $(TARGET).elf"
	@$(CC) $(C_OBJ) -o $(TARGET).elf $(LFLAGS)

$(OBJ_DIR)/%.o:%.$(EXT)
	@mkdir -p obj
	@echo "building $<"
	@$(CC) -c $(CFLAGS) $(INC_FLAGS) -o $@ $<

-include $(C_DEP)
$(OBJ_DIR)/%.d:%.$(EXT)
	@mkdir -p obj
	@echo "making $@"
	@set -e;rm -f $@;$(CC) -MM $(CFLAGS) $(INC_FLAGS) $< > $@.$$$$;sed 's,\($*\)\.o[ :]*,$(OBJ_DIR)/\1.o $(OBJ_DIR)/\1.d:,g' < $@.$$$$ > $@;rm -f $@.$$$$

clean:
	-rm -f $(shell find ./ -name '*.o')
	-rm -f $(shell find ./ -name '*.d')
	-rm -f $(shell find ./ -name '*.elf')

rebuild: clean all

ctags:
	@ctags -R *
	@echo "making tags file"
