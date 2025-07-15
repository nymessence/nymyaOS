VERSION=$(grep ^VERSION= version.conf | cut -d= -f2)

# make sure the .debs are built
make deb

# install both AMD64 packagest
sudo dpkg -i nymya-core-userland_${VERSION}_amd64.deb \
             nymya-core-kernel_${VERSION}_amd64.deb

# fix any missing deps, if needed
sudo apt-get install -f
