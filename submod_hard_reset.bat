pushd src\ccwrap
git fetch origin
git reset --hard origin/master
popd
call submod_update.bat
