all: clean clean_prx signed

clean: clean_prx
	make -f Makefile.signed clean

clean_prx:
	make -C downgrade_ctrl -f Makefile clean
	make -C downgrade660_ctrl -f Makefile clean
	
signed: clean_prx mk_downgrade_ctrl
	make -f Makefile.signed
	
hbl: clean_prx mk_downgrade_ctrl
	make -f Makefile.hbl
	
remove_dir:
	-rmdir /S /Q RELEASE

create_dir:
	-mkdir RELEASE
	-mkdir RELEASE\PSP
	-mkdir RELEASE\PSP\GAME
	-mkdir RELEASE\PSP\GAME\Downgrader
	-mkdir RELEASE\SRC
	
copy_src:
	cp -dpR downgrade_ctrl\.. RELEASE\SRC
	rmdir /S /Q RELEASE\SRC\RELEASE
	
mk_downgrade_ctrl:
	make -C downgrade_ctrl -f Makefile
	make -C downgrade660_ctrl -f Makefile

signed_release: clean clean_prx remove_dir create_dir copy_src signed
	cp EBOOT.PBP RELEASE\PSP\GAME\Downgrader\EBOOT.PBP
	
hbl_release: clean clean_prx remove_dir create_dir copy_src hbl
	cp EBOOT.PBP RELEASE\PSP\GAME\Downgrader\EBOOT.PBP