#!/bin/bash 

 pushd build
 gnome-terminal -x sh -c './main ; exec /bin/bash -i'
 popd
