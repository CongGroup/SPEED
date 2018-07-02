#!/bin/bash
 
flags=-avz
filter=(--exclude="*.sln" --exclude="*.VC.*" --exclude="*bin/" --exclude="*obj/" --exclude="*.o" --exclude="*Debug/" --exclude="*vcxproj*" --exclude=".git*" --exclude="env" --exclude="*.sh" --exclude=".config*")

local_dir=./
remote_dir=speed/

remote_A=192.168.1.2
remote_B=192.168.1.15

rsync "$flags" "${filter[@]}" conggroup@"$remote_B":"$remote_dir" "$local_dir"




