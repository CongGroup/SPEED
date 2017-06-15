#!/bin/bash
 
flags=-avz
filter=(--exclude="*.sln" --exclude="*.VC.*" --exclude="*bin/" --exclude="*obj/" --exclude="*.o" --exclude="*Debug/" --exclude="*vcxproj*" --exclude=".git*" --exclude="env" --exclude="*.sh")

local_dir=./
remote_dir=comdedup/

remote_A=192.168.1.2
remote_B=192.168.1.5

rsync "$flags" "${filter[@]}" "$local_dir" conggroup@"$remote_A":"$remote_dir"
rsync "$flags" "${filter[@]}" "$local_dir" conggroup@"$remote_B":"$remote_dir"