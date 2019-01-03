# SPEED

 * Introduction
 * Publication
 * Requirements
 * Installation
 * Configuration
 * Compiling
 * Usage
 * Applications
 * Maintainers

# INTRODUCTION

SPEED is a secure computation deduplication library for SGX-assisted applications.
To be continued.

# PUBLICATION

To be continued.

# REQUIREMENTS

Recommended Environment: Ubuntu 16.04 LTS with gcc version 4.8.4. Intel® Software Guard Extensions enable.

This software requires the following libraries:

 * [Intel SGX](https://software.intel.com/en-us/sgx)
 * [libz](https://zlib.net/)
 * [pcre](https://www.pcre.org/)
 * [Crypto++](https://www.cryptopp.com/)
 * [Redis](https://redis.io/)

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

* Redis installation

```shell
cd /data/download
wget http://download.redis.io/releases/redis-3.2.0.tar.gz
tar zxvf redis-3.2.0.tar.gz
cd redis-3.2.0
make && make install

cd /data/download
git clone https://github.com/luca3m/redis3m
cd redis3m
cmake .
make && make install
```


# CONFIGURATION

 Configure the environment

	Add the libraries paths to $LD_LIBRARY_PATH.

	```shell
	* export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
	```

	Start redis server.
	```shell
	redis-server
	```

# COMPILING

 Compile SPEED:

```shell
 * git clone https://github.com/CongGroup/SPEED.git
 * cd SPEED
 * cd speed_client && make
```

# USAGE

 Speed support generic API-level runtime deduplication of redundant in-enclave computations

 When we use some function without speed, it maybe like this.
 
```shell
string compress_file(const string& file_context);

string output = compress_file(file_context); 
```

 With SPEED library we can dedup pcre computation like this.
 
```shell
Deduplicable<string, const string&> 
		dedup_func("libz", "1.12.11", "compress_file", compress_file);

string output = dedup_func(file_context); 
```

 We provide four application as samples in path SPEED/speed_client
 
# APPLICATION

After compiling with speed, you can find a program named appDedup in SPEED/speed_client, it includes four applications.

```shell
~/SPEED/speed_client$ ./appDedup 
Usage : dedupApplication <applicationID> <Path> <Count> <Dedup>
       <applicationID> : 1:BackupFolder, 2:MiddleboxIDS, 3:MapreduceBow, 4:PicFolderDiff.
       <Path>          : The input data path.
       <Count>         : The input data count.
       <Dedup>         : 0: disable deduplication  1: enable deduplication.

  Applications 1. BackupFolder: use libz to compress the folder <Path>.
               2. MiddleboxIDS: do IDS for pcap file <Path>, with <Count> packages.
               3. MapreduceBow: use mapreduce to compute bow for <Count> files in <Path>.
               4. PicFeature: use sift to compute pic feature in folder <Path>.

```
* BackupFolder

	This application will compress all files in the <Path> folder, it will print the process time.

	The test data is in the speed/speed_client/data/compress

	you can run this application without speed like this
	```shell
	./appDedup 1 ./data/compress/file-3M 0 0
	```
	enable speed like this
	```shell
	./appDedup 1 ./data/compress/file-3M 0 1
	```

	We also have 1M Size folder and 2M Size folder, you can try it by yourself.

* MiddleboxIDS

	This application will read a local pcap files' packets, and do the IDS middlebox functions, it will print the process time.

	We use two big datasets in our papers, [m57](https://digitalcorpora.org/corpora/network-packet-dumps) and [4SICS](https://www.netresec.com/?page=PCAP4SICS). 

	It is too big to put to GitHub, we provide a small pcap file for the test run.

	The test data is in the speed/speed_client/data/patternMatching

	you can run this application without speed like this
	```shell
	./appDedup 2 data/patternMatching/tcp300.pcap 100 0
	```
	then, enable speed like this
	```shell
	./appDedup 2 data/patternMatching/tcp300.pcap 100 1
	```

* MapreduceBow

	This application can compute some text files' bow value with our sgx-mapreduce engine.

	We use [CommonCrawl](http://commoncrawl.org/) web page to test our program in the paper, and we use 5000 words as the dictionary in words5000.cpp. 

	It is too big to put to GitHub, we provide a small dataset for a test run. We use 36 files named 3M01, 3M02...3M36 in the folder 3M-36files. It will print the mapper count and the reducer count and the process time.

	The test data is in the speed/speed_client/data/bow/3M-36files

	you can run this application without speed like this
	```shell
	./appDedup 3 ./data/bow/3M-36files/3M 36 0
	```
	then, enable speed like this
	```shell
	./appDedup 3 ./data/bow/3M-36files/3M 36 1
	```
	We also have 1M Size folder and 2M Size folder, you can try it by yourself.

* PicFeature

	This application will compute all pictures sift feature in the <Path> folder, it will print the process time.

	The test data is in the speed/speed_client/data/compress

	you can run this application without speed like this
	```shell
	./appDedup 4 ./data/sift/pic-500kb 0 0
	```
	then, enable speed like this
	```shell
	./appDedup 4 ./data/sift/pic-500kb 0 1
	```

	We also have a 1M Size folder and 1.5M Size Pic, you can try it by yourself.



# MAINTAINER

  - Helei Cui, City University of Hong Kong, heleicui2-c@my.cityu.edu.hk
  - Huayi Duan, City University of Hong Kong, hduan2-c@my.cityu.edu.hk
  - Mengyu Yao, City University of Hong Kong, mengycs@gmail.com
