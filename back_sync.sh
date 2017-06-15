#!/bin/bash

# common 
flags=-avz
filter=(--exclude="*.sln" --exclude="*.VC.*" --exclude="*bin/" --exclude="*obj/" --exlucde="*.o" --exclude="*Debug/" --exclude="*vcxproj*" --exclude=".git")

# cache
cache_local_dir=./dedup_cache/
cache_remote_ip=192.168.1.2
cache_remote_dir=compdedup/dedup_cache/
rsync "$flags" "${filter[@]}" conggroup@"$cache_remote_ip":"$cache_remote_dir" "$cache_local_dir" 

# computing server
comp_local_dir=./dedup_comp/
comp_remote_ip=192.168.1.5
comp_remote_dir=compdedup/dedup_comp/
rsync "$flags" "${filter[@]}" conggroup@"$comp_remote_ip":"$comp_remote_dir" "$comp_local_dir" 