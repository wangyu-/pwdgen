cc_cross=/home/wangyu/Desktop/arm-2014.05/bin/arm-none-linux-gnueabi-g++
cc_local=g++
#cc_local=/opt/cross/x86_64-linux-musl/bin/x86_64-linux-musl-g++
#cc_mips34kc=/toolchains/OpenWrt-SDK-ar71xx-for-linux-x86_64-gcc-4.8-linaro_uClibc-0.9.33.2/staging_dir/toolchain-mips_34kc_gcc-4.8-linaro_uClibc-0.9.33.2/bin/mips-openwrt-linux-g++
cc_mips24kc_be=/toolchains/lede-sdk-17.01.2-ar71xx-generic_gcc-5.4.0_musl-1.1.16.Linux-x86_64/staging_dir/toolchain-mips_24kc_gcc-5.4.0_musl-1.1.16/bin/mips-openwrt-linux-musl-g++
cc_mips24kc_le=/toolchains/lede-sdk-17.01.2-ramips-mt7621_gcc-5.4.0_musl-1.1.16.Linux-x86_64/staging_dir/toolchain-mipsel_24kc_gcc-5.4.0_musl-1.1.16/bin/mipsel-openwrt-linux-musl-g++
#cc_arm= /toolchains/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabi/bin/arm-linux-gnueabi-g++ -march=armv6 -marm 
cc_arm= /toolchains/arm-2014.05/bin/arm-none-linux-gnueabi-g++
#cc_arm=/toolchains/lede-sdk-17.01.2-brcm2708-bcm2708_gcc-5.4.0_musl-1.1.16_eabi.Linux-x86_64/staging_dir/toolchain-arm_arm1176jzf-s+vfp_gcc-5.4.0_musl-1.1.16_eabi/bin/arm-openwrt-linux-muslgnueabi-g++
#cc_bcm2708=/home/wangyu/raspberry/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-g++ 
FLAGS= -std=c++11 -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -Wno-missing-field-initializers ${OPT}

SOURCES=main.cpp lib/md5.cpp lib/pbkdf2-sha1.cpp lib/pbkdf2-sha256.cpp

NAME=pwdgen
TARGETS=amd64 arm mips24kc_be x86 mips24kc_le
TAR=${NAME}_binaries.tar.gz `echo ${TARGETS}|sed -r 's/([^ ]+)/pwdgen_\1/g'` version.txt

all:git_version
	rm -f ${NAME}
	${cc_local}   -o ${NAME}          -I. ${SOURCES} ${FLAGS} -lrt -ggdb -static -O3

mingw:git_version
	rm -f ${NAME}
	${cc_local}   -o ${NAME}          -I. ${SOURCES} ${FLAGS}  -ggdb -static -O3

fast: git_version
	rm -f ${NAME}
	${cc_local}   -o ${NAME}          -I. ${SOURCES} ${FLAGS} -lrt -ggdb
debug: git_version
	rm -f ${NAME}
	${cc_local}   -o ${NAME}          -I. ${SOURCES} ${FLAGS} -lrt -Wformat-nonliteral -D MY_DEBUG 
debug2: git_version
	rm -f ${NAME}
	${cc_local}   -o ${NAME}          -I. ${SOURCES} ${FLAGS} -lrt -Wformat-nonliteral -ggdb

dynamic: git_version
	${cc_local}   -o ${NAME}_$@          -I. ${SOURCES} ${FLAGS} -lrt -O3

mips24kc_be: git_version
	${cc_mips24kc_be}  -o ${NAME}_$@   -I. ${SOURCES} ${FLAGS} -lrt -lgcc_eh -static -O3

mips24kc_le: git_version
	${cc_mips24kc_le}  -o ${NAME}_$@   -I. ${SOURCES} ${FLAGS} -lrt -lgcc_eh -static -O3

amd64:git_version
	${cc_local}   -o ${NAME}_$@    -I. ${SOURCES} ${FLAGS} -lrt -static -O3

amd64_perf:git_version
	${cc_local}   -o ${NAME}_$@    -I. ${SOURCES} ${FLAGS} -lrt -static -O0 -fno-omit-frame-pointer -g

x86:git_version
	${cc_local}   -o ${NAME}_$@      -I. ${SOURCES} ${FLAGS} -lrt -static -O3 -m32
arm:git_version
	${cc_arm}   -o ${NAME}_$@      -I. ${SOURCES} ${FLAGS} -lrt -static -O3

arm_perf:git_version
	${cc_arm}   -o ${NAME}_$@      -I. ${SOURCES} ${FLAGS} -lrt -static -mapcs-frame -fno-omit-frame-pointer -g -O0 -lgcc_eh

cross:git_version
	${cc_cross}   -o ${NAME}_cross    -I. ${SOURCES} ${FLAGS} -lrt -O3

cross2:git_version
	${cc_cross}   -o ${NAME}_cross    -I. ${SOURCES} ${FLAGS} -lrt -static -lgcc_eh -O3   

cross3:git_version
	${cc_cross}   -o ${NAME}_cross    -I. ${SOURCES} ${FLAGS} -lrt -static -O3

release: ${TARGETS} 
	cp git_version.h version.txt
	tar -zcvf ${TAR}

clean:	
	rm -f ${TAR}
	rm -f pwdgen pwdgen_cross pwdgen_cmake pwdgen_dynamic
	rm -f git_version.h

git_version:
	    echo "const char *gitversion = \"$(shell git rev-parse HEAD)\";" > git_version.h
	
