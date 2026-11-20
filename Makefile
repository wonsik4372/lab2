# lab2 전체 빌드: make / make clean

SUBDIRS = file process ipc dbus myshell

 

all:

    @for d in $(SUBDIRS); do $(MAKE) -C $$d || exit 1; done

    cd matrix_rs && cargo build --release

 

clean:

    @for d in $(SUBDIRS); do $(MAKE) -C $$d clean; done

    cd matrix_rs && cargo clean


