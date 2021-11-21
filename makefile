all:mpv-ipc
mpv-ipc:mpvc.c
	$(CC) -l cjson $^ -o $@
