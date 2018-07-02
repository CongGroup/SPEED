# SPEED

 * Introduction
 * Publication
 * Requirements
 * Installation
 * Configuration
 * Compiling
 * Usage
 * Extend
 * Maintainers

# INTRODUCTION

SPEED is a secure computation deduplication library for SGX-assisted applications.
To be continued.

# PUBLICATION

To be continued.

# REQUIREMENTS

Recommended Environment: Ubuntu 16.04 LTS with gcc version 4.8.4. Intel® Software Guard Extensions enable.

This software requires the following libraries:

 * Intel SGX (https://software.intel.com/en-us/sgx)
 * libz (https://zlib.net/)
 * pcre (https://www.pcre.org/)
 * Crypto++ (https://www.cryptopp.com/)

# INSTALLATION

* Environment setup:

```shell
 * sudo apt-get update
 * sudo apt-get install -y lrzsz gcc g++ libssl-dev libgmp-dev subversion make cmake libboost-dev libboost-test-dev libboost-program-options-dev libboost-system-dev libboost-filesystem-dev libevent-dev automake libtool flex bison pkg-config ssh openssh-server rsync python-software-properties libglib2.0-dev git libmsgpack-dev libboost-thread-dev libboost-date-time-dev libhiredis-dev build-essential libboost-regex-dev gdb
```

* Crypto++ installation:

You can find the [version 5.6.5](https://www.cryptopp.com/cryptopp565.zip) on crypto++ website.

```shell
wget https://github.com/weidai11/cryptopp/archive/CRYPTOPP_5_6_5.tar.gz
tar -zxf CRYPTOPP_5_6_5.tar.gz
cd cryptopp-CRYPTOPP_5_6_5
make libcryptopp.a libcryptopp.so 
sudo make install PREFIX=/usr/local
```

* SGX installation:

You need to enable SGX follow the [Intel_SGX_Installation_Guide_Linux](https://download.01.org/intel-sgx/linux-2.1/docs/Intel_SGX_Installation_Guide_Linux_2.1_Open_Source.pdf)


# CONFIGURATION

 Configure the environment

	Add the libraries paths to $LD_LIBRARY_PATH.

	```shell
	* export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
	```

# COMPILING

 Compile SPEED:

```shell
 * git clone https://github.com/CongGroup/SPEED.git
 * cd SPEED
 * cd speed_core && make
```

# USAGE

 Speed support generic API-level runtime deduplication of redundant in-enclave computations

 When we use pcre without speed, it maybe like this.
 
```shell
int rc = pcre_exec(pcre_engine, 0, // pcre engine
			str_buffer, str_len, // query string
			0, // starting offset
			0, // options
			pcre_output_buffer, output_buffer_len); 
```

 With SPEED library we can dedup pcre computation like this.
 
```shell
Deduplicable<int, const pcre *, const pcre_extra *, const char*, int, int, int, int *, int> 
		dedup_pcre("libpcre", "10.23", "pcre_exec");
int rc = dedup_pcre(pcre_engine, 0, // pcre engine
				str_buffer, str_len, // query string
				0, // starting offset
				0, // options
				pcre_output_buffer, output_buffer_len); 
```

 We provide pcre and libz as samples in path SPEED/speed_sample
 
# EXTEND

You can extend speed to support other library, just three steps.

For example, there is a library "SomeLib" have a function "LongTimeCall".

1. You need to create a new class extend class "FunctionDB".
2. You need to implement "LongTimeCall" of the new class.
 * 2.1 Use marco DEDUP_FUNCTION_INIT, then write function input to input_buffer.
 * 2.2 Use marco DEDUP_FUNCTION_QUERY, then write function output to output_buffer.
 * 2.3 Use marco DEDUP_FUNCTION_UPDATE, then return the function.
3. Finally, you need to update FunctionDBFactory.h to add "Somelib" and it`s version info.
 
# MAINTAINER

  - Helei Cui, City University of Hong Kong, heleicui2-c@my.cityu.edu.hk
  - Huayi Duan, City University of Hong Kong, hduan2-c@my.cityu.edu.hk
  - Mengyu Yao, City University of Hong Kong, mengycs@gmail.com
