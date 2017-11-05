#!/bin/bash 

 pushd build
 gnome-terminal -x sh -c 'padsp ./main ; exec /bin/bash -i'
 popd
