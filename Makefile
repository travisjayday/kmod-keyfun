SOURCE=src
TARGET=keyfun
OUTPUT=build

SOURCES=$(wildcard $(SOURCE)/*.c)

# Depends on bin/include bin/*.c and bin/Makefile
all: $(subst $(SOURCE),$(OUTPUT),$(SOURCES)) $(OUTPUT)/Makefile
	make -C /lib/modules/$(shell uname -r)/build CPPFLAGS="-I$(PWD)/$(INCLUDE)" M=$(PWD)/$(OUTPUT) modules

# Create a symlink from src to bin
$(OUTPUT)/%: $(SOURCE)/%
	ln -s ../$< $@

# Generate a Makefile with the needed obj-m and mymodule-objs set
$(OUTPUT)/Makefile:
	echo "\
        obj-m += $(TARGET).o                                                                    \n\
        $(TARGET)-objs := $(subst $(TARGET).o,, $(subst .c,.o,$(subst $(SOURCE)/,,$(SOURCES)))) \
    " > $@

install: 
	-lsmod | grep "keyfun"; res=$$?; \
	if [ $$res -eq 0 ] ; then \
		sudo rmmod keyfun; \
	fi
	sudo insmod build/keyfun.ko
	@echo ""
	@echo "Loaded keyfun module!"

clean:
	rm -rf $(OUTPUT)
	mkdir $(OUTPUT)

